
#include "database/sqlite/SQLiteDatabase.h"
#include "database/sqlite/SQLiteCustomFunction.h"
#include "database/sqlite/SQLiteDatabaseConfiguration.h"
#include "database/sqlite/SQLiteGlobal.h"
#include "database/sqlite/SQLiteSession.h"
#include "database/sqlite/CSQLiteStatement.h"
#include "database/sqlite/SQLiteQueryBuilder.h"
#include "database/sqlite/SQLiteDirectCursorDriver.h"
#include "database/CDefaultDatabaseErrorHandler.h"
#include "database/DatabaseUtils.h"
#include "os/SystemProperties.h"
#include "os/SystemClock.h"
#include "os/Looper.h"
#include "text/TextUtils.h"
#include <elastos/StringUtils.h>
#include <elastos/Slogger.h>
#include <elastos/Logger.h>
#include <sqlite3_android.h>
#include <sqlite3.h>

using Elastos::Core::StringUtils;
using Elastos::Core::CObjectContainer;
using Elastos::Core::CStringWrapper;
using Elastos::IO::CFile;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::CObjectStringMap;
using Elastos::IO::EIID_ICloseable;
using Elastos::IO::EIID_IFileFilter;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Database::CDefaultDatabaseErrorHandler;
using Elastos::Droid::Database::DatabaseUtils;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

#define UTF16_STORAGE 0
#define SQLITE_SOFT_HEAP_LIMIT (4 * 1024 * 1024)
#define ANDROID_TABLE "android_metadata"

// 98c6f42a-1ffb-4aa9-a739-b8d8943a1eaf
extern "C" const InterfaceID EIID_SQLiteDatabase =
        { 0x98c6f42a, 0x1ffb, 0x4aa9, { 0xa7, 0x39, 0xb8, 0xd8, 0x94, 0x3a, 0x1a, 0xaf } };

void ReleaseSqliteSessionAfter(void* param) {
    SQLiteSession* sqliteSession = (SQLiteSession*)param;
    sqliteSession->Release();
    sqliteSession = NULL;
}

SQLiteDatabase::MyFileFilter::MyFileFilter(
    /* [in] */ const String& prefix)
    : mPrefix(prefix)
{}

CAR_INTERFACE_IMPL(SQLiteDatabase::MyFileFilter, IFileFilter)

ECode SQLiteDatabase::MyFileFilter::Accept(
    /* [in] */ IFile* candidate,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    String name;
    candidate->GetName(&name);
    *succeeded = name.StartWith(mPrefix);
    return NOERROR;
}


const String SQLiteDatabase::TAG("SQLiteDatabase");
const Int32 SQLiteDatabase::EVENT_DB_CORRUPT;
static AutoPtr< ArrayOf<String> > InitConflictValues()
{
    AutoPtr< ArrayOf<String> > values = ArrayOf<String>::Alloc(6);
    (*values)[0] = String("");
    (*values)[1] = String(" OR ROLLBACK ");
    (*values)[2] = String(" OR ABORT ");
    (*values)[3] = String(" OR FAIL ");
    (*values)[4] = String(" OR IGNORE ");
    (*values)[5] = String(" OR REPLACE ");
    return values;
}
const AutoPtr< ArrayOf<String> > SQLiteDatabase::CONFLICT_VALUES = InitConflictValues();
const Int32 SQLiteDatabase::OPEN_READ_MASK;
HashMap<AutoPtr<SQLiteDatabase>, AutoPtr<IInterface> > SQLiteDatabase::sActiveDatabases;
Mutex SQLiteDatabase::sActiveDatabasesLock;

SQLiteDatabase::SQLiteDatabase(
    /* [in] */ const String& path,
    /* [in] */ Int32 openFlags,
    /* [in] */ ISQLiteDatabaseCursorFactory* cursorFactory,
    /* [in] */ IDatabaseErrorHandler* errorHandler)
    : mCursorFactory(cursorFactory)
    , mHasAttachedDbsLocked(FALSE)
    , mKeyThreadSessionInitialized(FALSE)
{
    if (errorHandler != NULL) {
        mErrorHandler = errorHandler;
    }
    else {
        CDefaultDatabaseErrorHandler::New((IDatabaseErrorHandler**)&mErrorHandler);
    }
    mConfigurationLocked = new SQLiteDatabaseConfiguration(path, openFlags);
}

PInterface SQLiteDatabase::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(ISQLiteDatabase*)this;
    }
    else if (riid == EIID_ISQLiteDatabase) {
        return (ISQLiteDatabase*)this;
    }
    else if (riid == EIID_ISQLiteClosable) {
        return (ISQLiteClosable*)this;
    }
    else if (riid == EIID_ICloseable) {
        return (ICloseable*)this;
    }
    else if (riid == EIID_SQLiteDatabase) {
        return reinterpret_cast<PInterface>(this);
    }
    else {
        return NULL;
    }
}

UInt32 SQLiteDatabase::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 SQLiteDatabase::Release()
{
    return ElRefBase::Release();
}

ECode SQLiteDatabase::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    VALIDATE_NOT_NULL(pIID)

    if (pObject == (IInterface*)(ISQLiteDatabase*)this) {
        *pIID = EIID_ISQLiteDatabase;
        return NOERROR;
    }
    else if (pObject == (IInterface*)(ISQLiteClosable*)this) {
        *pIID = EIID_ISQLiteClosable;
        return NOERROR;
    }
    else if (pObject == (IInterface*)(ICloseable*)this) {
        *pIID = EIID_ICloseable;
        return NOERROR;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
}

SQLiteDatabase::~SQLiteDatabase()
{
    // try {
    Dispose(TRUE);
    pthread_key_delete(mKeyThreadSession);
    // } finally {
    //     super.finalize();
    // }
}

void SQLiteDatabase::OnAllReferencesReleased()
{
    Dispose(FALSE);
}

void SQLiteDatabase::Dispose(
    /* [in] */ Boolean finalized)
{
    AutoPtr<SQLiteConnectionPool> pool;
    {
        Mutex::Autolock lock(mLock);
        // if (mCloseGuardLocked != NULL) {
        //     if (finalized) {
        //         mCloseGuardLocked.warnIfOpen();
        //     }
        //     mCloseGuardLocked.close();
        // }

        pool = mConnectionPoolLocked;
        mConnectionPoolLocked = NULL;
    }

    if (!finalized) {
        {
            Mutex::Autolock lock(sActiveDatabasesLock);
            sActiveDatabases.Erase(this);
        }
        if (pool != NULL) {
            pool->Close();
        }
    }
}

Int32 SQLiteDatabase::ReleaseMemory()
{
    return SQLiteGlobal::ReleaseMemory();
}

ECode SQLiteDatabase::SetLockingEnabled(
    /* [in] */ Boolean lockingEnabled)
{
    return NOERROR;
}

String SQLiteDatabase::GetLabel()
{
    Mutex::Autolock lock(mLock);
    return mConfigurationLocked->mLabel;
}

ECode SQLiteDatabase::OnCorruption()
{
    //EventLog.writeEvent(EVENT_DB_CORRUPT, getLabel());
    return mErrorHandler->OnCorruption(THIS_PROBE(ISQLiteDatabase));
}

ECode SQLiteDatabase::GetThreadSession(
    /* [out] */ SQLiteSession** session)
{
    VALIDATE_NOT_NULL(session);
    if (!mKeyThreadSessionInitialized) {
        pthread_key_create(&mKeyThreadSession, ReleaseSqliteSessionAfter);
        mKeyThreadSessionInitialized = TRUE;
    }
    AutoPtr<SQLiteSession> sqliteSession = (SQLiteSession*)pthread_getspecific(mKeyThreadSession);
    if (sqliteSession == NULL) {
        FAIL_RETURN(CreateSession((SQLiteSession**)&sqliteSession));
        pthread_setspecific(mKeyThreadSession, sqliteSession.Get());
        sqliteSession->AddRef();
    }
    *session = sqliteSession;
    INTERFACE_ADDREF(*session);
    return NOERROR;
}

ECode SQLiteDatabase::CreateSession(
    /* [out] */ SQLiteSession** session)
{
    AutoPtr<SQLiteConnectionPool> pool;
    {
        Mutex::Autolock lock(mLock);
        FAIL_RETURN(ThrowIfNotOpenLocked());
        pool = mConnectionPoolLocked;
    }
    AutoPtr<SQLiteSession> s = new SQLiteSession(pool);
    *session = s;
    INTERFACE_ADDREF(*session);
    return  NOERROR;
}

Int32 SQLiteDatabase::GetThreadDefaultConnectionFlags(
    /* [in] */ Boolean readOnly)
{
    Int32 flags = readOnly ? SQLiteConnectionPool::CONNECTION_FLAG_READ_ONLY :
            SQLiteConnectionPool::CONNECTION_FLAG_PRIMARY_CONNECTION_AFFINITY;
    if (IsMainThread()) {
        flags |= SQLiteConnectionPool::CONNECTION_FLAG_INTERACTIVE;
    }
    return flags;
}

Boolean SQLiteDatabase::IsMainThread()
{
    // FIXME: There should be a better way to do this.
    // Would also be nice to have something that would work across Binder calls.
    AutoPtr<ILooper> looper = Looper::MyLooper();
    return looper != NULL && looper == Looper::GetMainLooper();
}

ECode SQLiteDatabase::BeginTransaction()
{
    BeginTransaction(NULL/* transactionStatusCallback */, TRUE);
    return NOERROR;
}

ECode SQLiteDatabase::BeginTransactionNonExclusive()
{
    return BeginTransaction(NULL/* transactionStatusCallback */, FALSE);
}

ECode SQLiteDatabase::BeginTransactionWithListener(
    /* [in] */ ISQLiteTransactionListener* transactionListener)
{
    return BeginTransaction(transactionListener, TRUE);
}

ECode SQLiteDatabase::BeginTransactionWithListenerNonExclusive(
    /* [in] */ ISQLiteTransactionListener* transactionListener)
{
    return BeginTransaction(transactionListener, FALSE);
}

ECode SQLiteDatabase::BeginTransaction(
    /* [in] */ ISQLiteTransactionListener* transactionListener,
    /* [in] */ Boolean exclusive)
{
    AcquireReference();
    // try {
    AutoPtr<SQLiteSession> session;
    ECode ec = GetThreadSession((SQLiteSession**)&session);
    if (FAILED(ec)) {
        ReleaseReference();
        return ec;
    }
    session->BeginTransaction(
            exclusive ? SQLiteSession::TRANSACTION_MODE_EXCLUSIVE :
                    SQLiteSession::TRANSACTION_MODE_IMMEDIATE,
            transactionListener,
            GetThreadDefaultConnectionFlags(FALSE/*readOnly*/), NULL);
    // } finally {
    return ReleaseReference();
    // }
}

ECode SQLiteDatabase::EndTransaction()
{
    AcquireReference();
    // try {
    AutoPtr<SQLiteSession> session;
    ECode ec = GetThreadSession((SQLiteSession**)&session);
    if (FAILED(ec)) {
        ReleaseReference();
        return ec;
    }
    session->EndTransaction(NULL);
    // } finally {
    return ReleaseReference();
    // }
}

ECode SQLiteDatabase::SetTransactionSuccessful()
{
    AcquireReference();
    // try {
    AutoPtr<SQLiteSession> session;
    ECode ec = GetThreadSession((SQLiteSession**)&session);
    if (FAILED(ec)) {
        ReleaseReference();
        return ec;
    }
    session->SetTransactionSuccessful();
    // } finally {
    return ReleaseReference();
    // }
}

ECode SQLiteDatabase::InTransaction(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AcquireReference();
    //try {
    AutoPtr<SQLiteSession> session;
    ECode ec = GetThreadSession((SQLiteSession**)&session);
    if (FAILED(ec)) {
        ReleaseReference();
        return ec;
    }
    *result = session->HasTransaction();
    //} finally {
    return ReleaseReference();
    //}
}

ECode SQLiteDatabase::IsDbLockedByCurrentThread(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AcquireReference();
    //try {
    AutoPtr<SQLiteSession> session;
    ECode ec = GetThreadSession((SQLiteSession**)&session);
    if (FAILED(ec)) {
        ReleaseReference();
        return ec;
    }
    *result = session->HasConnection();
    //} finally {
    return ReleaseReference();
    //}
}

ECode SQLiteDatabase::IsDbLockedByOtherThreads(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode SQLiteDatabase::YieldIfContended(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return YieldIfContendedHelper(FALSE /* do not check yielding */, -1 /* sleepAfterYieldDelay */, result);
}

ECode SQLiteDatabase::YieldIfContendedSafely(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return YieldIfContendedHelper(TRUE /* check yielding */, -1 /* sleepAfterYieldDelay*/, result);
}

ECode SQLiteDatabase::YieldIfContendedSafelyEx(
    /* [in] */ Int64 sleepAfterYieldDelay,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return YieldIfContendedHelper(TRUE /* check yielding */, sleepAfterYieldDelay, result);
}

ECode SQLiteDatabase::YieldIfContendedHelper(
    /* [in] */ Boolean throwIfUnsafe,
    /* [in] */ Int64 sleepAfterYieldDelay,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AcquireReference();
    //try {
    AutoPtr<SQLiteSession> session;
    ECode ec = GetThreadSession((SQLiteSession**)&session);
    if (FAILED(ec)) {
        ReleaseReference();
        return ec;
    }
    ec = session->YieldTransaction(sleepAfterYieldDelay, throwIfUnsafe, NULL, result);
    //} finally {
    ReleaseReference();
    //}
    return ec;
}

ECode SQLiteDatabase::GetSyncedTables(
    /* [out] */ IObjectStringMap** tables)
{
    VALIDATE_NOT_NULL(tables);
    return CObjectStringMap::New(tables);
}

ECode SQLiteDatabase::OpenDatabase(
    /* [in] */ const String& path,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [in] */ Int32 flags,
    /* [out] */ ISQLiteDatabase** db)
{
    return OpenDatabaseEx(path, factory, flags, NULL, db);
}

ECode SQLiteDatabase::OpenDatabaseEx(
    /* [in] */ const String& path,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [in] */ Int32 flags,
    /* [in] */ IDatabaseErrorHandler* errorHandler,
    /* [out] */ ISQLiteDatabase** db)
{
    VALIDATE_NOT_NULL(db);
    AutoPtr<SQLiteDatabase> sqliteDatabase = new SQLiteDatabase(path, flags, factory, errorHandler);
    *db = ISQLiteDatabase::Probe(sqliteDatabase);
    INTERFACE_ADDREF(*db);
    return sqliteDatabase->Open();
}

ECode SQLiteDatabase::OpenOrCreateDatabase(
    /* [in] */ IFile* file,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [out] */ ISQLiteDatabase** db)
{
    VALIDATE_NOT_NULL(db);
    String path;
    file->GetPath(&path);
    return OpenOrCreateDatabaseEx(path, factory, db);
}

ECode SQLiteDatabase::OpenOrCreateDatabaseEx(
    /* [in] */ const String& path,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [out] */ ISQLiteDatabase** db)
{
    VALIDATE_NOT_NULL(db);
    return OpenDatabase(path, factory, CREATE_IF_NECESSARY, db);
}

ECode SQLiteDatabase::OpenOrCreateDatabaseEx2(
    /* [in] */ const String& path,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [in] */ IDatabaseErrorHandler* errorHandler,
    /* [out] */ ISQLiteDatabase** db)
{
    VALIDATE_NOT_NULL(db);
    return OpenDatabaseEx(path, factory, CREATE_IF_NECESSARY, errorHandler, db);
}

ECode SQLiteDatabase::DeleteDatabase(
    /* [in] */ IFile* file,
    /* [out] */ Boolean* _result)
{
    VALIDATE_NOT_NULL(_result)

    if (file == NULL) {
        // throw new IllegalArgumentException("file must not be null");
        Slogger::E(TAG, "file must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean deleted = FALSE;
    Boolean result;
    file->Delete(&result);
    deleted |= result;
    String path;
    file->GetPath(&path);

    AutoPtr<IFile> file1, file2, file3;
    CFile::New(path + String("-journal"), (IFile**)&file1);
    CFile::New(path + String("-shm"), (IFile**)&file2);
    CFile::New(path + String("-wal"), (IFile**)&file3);
    file1->Delete(&result);
    deleted |= result;
    file2->Delete(&result);
    deleted |= result;
    file3->Delete(&result);
    deleted |= result;

    AutoPtr<IFile> dir;
    file->GetParentFile((IFile**)&dir);
    if (dir != NULL) {
        String name;
        file->GetName(&name);
        String prefix = name + String("-mj");
        AutoPtr<IFileFilter> filter = (IFileFilter*)new MyFileFilter(prefix);
        AutoPtr< ArrayOf<IFile*> > fileList;
        dir->ListFiles((ArrayOf<IFile*>**)&fileList);
        for (Int32 i = 0; i < fileList->GetLength(); ++i) {
            AutoPtr<IFile> masterJournal = (*fileList)[i];
            masterJournal->Delete(&result);
            deleted |= result;
        }
    }
    *_result = deleted;
    return NOERROR;
}

ECode SQLiteDatabase::ReopenReadWrite()
{
    Mutex::Autolock lock(mLock);
    FAIL_RETURN(ThrowIfNotOpenLocked())

    if (!IsReadOnlyLocked()) {
        return NOERROR; // nothing to do
    }

    // Reopen the database in read-write mode.
    Int32 oldOpenFlags = mConfigurationLocked->mOpenFlags;
    mConfigurationLocked->mOpenFlags = (mConfigurationLocked->mOpenFlags & ~OPEN_READ_MASK)
            | OPEN_READWRITE;
    //try {
    ECode ec = mConnectionPoolLocked->Reconfigure(mConfigurationLocked);
    //} catch (RuntimeException ex) {
    if (FAILED(ec)) {
        mConfigurationLocked->mOpenFlags = oldOpenFlags;
    }
    //}
    return ec;
}

ECode SQLiteDatabase::Open()
{
    //try {
    //try {
    ECode ec = OpenInner();
    //} catch (SQLiteDatabaseCorruptException ex) {
    if (ec == (ECode)E_SQLITE_DATABASE_CORRUPT_EXCEPTION) {
        OnCorruption();
        ec = OpenInner();
    }
    else if (ec == (ECode)E_SQLITE_EXCEPTION) {
        Slogger::E(TAG, "Failed to open database '%s'.0x%08x", GetLabel().string(), ec);
        Close();
        return ec;
    }
    //}
    //} catch (SQLiteException ex) {
    //}
    return NOERROR;
}

ECode SQLiteDatabase::OpenInner()
{
    {
        Mutex::Autolock lock(mLock);

        assert(mConnectionPoolLocked == NULL);
        FAIL_RETURN(SQLiteConnectionPool::Open(mConfigurationLocked, (SQLiteConnectionPool**)&mConnectionPoolLocked));
        // mCloseGuardLocked.Open("close");
    }

    Mutex::Autolock lock(sActiveDatabasesLock);
    sActiveDatabases[this] = NULL;
    return NOERROR;
}

ECode SQLiteDatabase::Create(
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [out] */ ISQLiteDatabase** db)
{
    VALIDATE_NOT_NULL(db);
    // This is a magic string with special meaning for SQLite.
    return OpenDatabase(String(":memory:"), factory, CREATE_IF_NECESSARY, db);
}

ECode SQLiteDatabase::AddCustomFunction(
    /* [in] */ const String& name,
    /* [in] */ Int32 numArgs,
    /* [in] */ ICustomFunction* function)
{
    // Create wrapper (also validates arguments).
    AutoPtr<SQLiteCustomFunction> wrapper = new SQLiteCustomFunction(name, numArgs, function);

    Mutex::Autolock lock(mLock);
    FAIL_RETURN(ThrowIfNotOpenLocked())

    mConfigurationLocked->mCustomFunctions.PushBack(wrapper);
    //try {
    ECode ec = mConnectionPoolLocked->Reconfigure(mConfigurationLocked);
    //} catch (RuntimeException ex) {
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        mConfigurationLocked->mCustomFunctions.Remove(wrapper);
    }
    //}
    return ec;
}

ECode SQLiteDatabase::GetVersion(
    /* [out] */ Int32* version)
{
    VALIDATE_NOT_NULL(version);
    Int64 value = DatabaseUtils::Int64ForQuery(THIS_PROBE(ISQLiteDatabase), String("PRAGMA user_version;"), NULL);
    *version = (Int32)value;
    return NOERROR;
}

ECode SQLiteDatabase::SetVersion(
    /* [in] */ Int32 version)
{
    String sql = String("PRAGMA user_version = ") + StringUtils::Int32ToString(version);
    return ExecSQL(sql);
}

ECode SQLiteDatabase::GetMaximumSize(
    /* [out] */ Int64* maxSize)
{
    VALIDATE_NOT_NULL(maxSize);

    Int64 pageCount = DatabaseUtils::Int64ForQuery(THIS_PROBE(ISQLiteDatabase), String("PRAGMA max_page_count;"), NULL);
    Int64 pageSize;
    GetPageSize(&pageSize);
    *maxSize = pageCount * pageSize;
    return NOERROR;
}

ECode SQLiteDatabase::SetMaximumSize(
    /* [in] */ Int64 numBytes,
    /* [out] */ Int64* maxSize)
{
    VALIDATE_NOT_NULL(maxSize);

    Int64 pageSize;
    GetPageSize(&pageSize);
    Int64 numPages = numBytes / pageSize;
    // If numBytes isn't a multiple of pageSize, bump up a page
    if ((numBytes % pageSize) != 0) {
        numPages++;
    }
    Int64 newPageCount = DatabaseUtils::Int64ForQuery(THIS_PROBE(ISQLiteDatabase),
            String("PRAGMA max_page_count = ") + StringUtils::Int64ToString(numPages), NULL);
    *maxSize = newPageCount * pageSize;
    return NOERROR;
}

ECode SQLiteDatabase::GetPageSize(
    /* [out] */ Int64* pageSize)
{
    VALIDATE_NOT_NULL(pageSize);
    *pageSize = DatabaseUtils::Int64ForQuery(THIS_PROBE(ISQLiteDatabase), String("PRAGMA page_size;"), NULL);
    return NOERROR;
}

ECode SQLiteDatabase::SetPageSize(
    /* [in] */ Int64 numBytes)
{
    return ExecSQL(String("PRAGMA page_size = ") + StringUtils::Int64ToString(numBytes));
}

ECode SQLiteDatabase::MarkTableSyncable(
    /* [in] */ const String& table,
    /* [in] */ const String& deletedTable)
{
    return NOERROR;
}

ECode SQLiteDatabase::MarkTableSyncableEx(
    /* [in] */ const String& table,
    /* [in] */ const String& foreignKey,
    /* [in] */ const String& updateTable)
{
    return NOERROR;
}

ECode SQLiteDatabase::FindEditTable(
    /* [in] */ const String& tables,
    /* [out] */ String* editable)
{
    VALIDATE_NOT_NULL(editable);

    if (!TextUtils::IsEmpty(tables)) {
        // find the first word terminated by either a space or a comma
        Int32 spacepos = tables.IndexOf(' ');
        Int32 commapos = tables.IndexOf(',');

        if (spacepos > 0 && (spacepos < commapos || commapos < 0)) {
            *editable = tables.Substring(0, spacepos);
            return NOERROR;
        }
        else if (commapos > 0 && (commapos < spacepos || spacepos < 0) ) {
            *editable = tables.Substring(0, commapos);
            return NOERROR;
        }
        *editable = tables;
        return NOERROR;
    }
    else {
        // throw new IllegalStateException("Invalid tables");
        Slogger::E(TAG, "Invalid tables");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

ECode SQLiteDatabase::CompileStatement(
    /* [in] */ const String& sql,
    /* [out] */ ISQLiteStatement** stmt)
{
    VALIDATE_NOT_NULL(stmt)
    AcquireReference();
    //try {
    ECode ec = CSQLiteStatement::New(THIS_PROBE(ISQLiteDatabase), sql, NULL, stmt);
    //} finally {
    SQLiteClosable::ReleaseReference();
    //}
    return ec;
}

ECode SQLiteDatabase::Query(
    /* [in] */ Boolean distinct,
    /* [in] */ const String& table,
    /* [in] */ ArrayOf<String>* columns,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& groupBy,
    /* [in] */ const String& having,
    /* [in] */ const String& orderBy,
    /* [in] */ const String& limit,
    /* [out] */ ICursor** cursor)
{
    return QueryWithFactory(NULL, distinct, table, columns, selection, selectionArgs,
            groupBy, having, orderBy, limit, cursor);
}

ECode SQLiteDatabase::QueryEx(
    /* [in] */ Boolean distinct,
    /* [in] */ const String& table,
    /* [in] */ ArrayOf<String>* columns,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& groupBy,
    /* [in] */ const String& having,
    /* [in] */ const String& orderBy,
    /* [in] */ const String& limit,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ ICursor** cursor)
{
    return QueryWithFactoryEx(NULL, distinct, table, columns, selection, selectionArgs,
                groupBy, having, orderBy, limit, cancellationSignal, cursor);
}

ECode SQLiteDatabase::QueryWithFactory(
    /* [in] */ ISQLiteDatabaseCursorFactory* cursorFactory,
    /* [in] */ Boolean distinct,
    /* [in] */ const String& table,
    /* [in] */ ArrayOf<String>* columns,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& groupBy,
    /* [in] */ const String& having,
    /* [in] */ const String& orderBy,
    /* [in] */ const String& limit,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor)
    *cursor = NULL;

    AcquireReference();
    String sql, editTable;
    ECode ec = SQLiteQueryBuilder::BuildQueryString(distinct, table, columns, selection, groupBy, having, orderBy, limit, &sql);
    if (FAILED(ec)) {
        goto fail;
    }
    ec = FindEditTable(table, &editTable);
    if (FAILED(ec)) {
        goto fail;
    }
    ec = RawQueryWithFactory(cursorFactory, sql, selectionArgs, editTable, cursor);

fail:
    ReleaseReference();
    return ec;
}

ECode SQLiteDatabase::QueryWithFactoryEx(
    /* [in] */ ISQLiteDatabaseCursorFactory* cursorFactory,
    /* [in] */ Boolean distinct,
    /* [in] */ const String& table,
    /* [in] */ ArrayOf<String>* columns,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& groupBy,
    /* [in] */ const String& having,
    /* [in] */ const String& orderBy,
    /* [in] */ const String& limit,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor)
    *cursor = NULL;
    AcquireReference();
    // try {
    String sql, editTable;
    ECode ec = SQLiteQueryBuilder::BuildQueryString(
            distinct, table, columns, selection, groupBy, having, orderBy, limit, &sql);
    if (FAILED(ec)) {
        goto fail;
    }
    ec = FindEditTable(table, &editTable);
    if (FAILED(ec)) {
        goto fail;
    }
    ec = RawQueryWithFactoryEx(cursorFactory, sql, selectionArgs, editTable, cancellationSignal, cursor);
    // } finally {
    //     releaseReference();
    // }
fail:
    ReleaseReference();
    return ec;
}

ECode SQLiteDatabase::QueryEx2(
    /* [in] */ const String& table,
    /* [in] */ ArrayOf<String>* columns,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& groupBy,
    /* [in] */ const String& having,
    /* [in] */ const String& orderBy,
    /* [out] */ ICursor** cursor)
{
    return Query(FALSE, table, columns, selection, selectionArgs, groupBy,
            having, orderBy, String(NULL) /* limit */, cursor);
}

ECode SQLiteDatabase::QueryEx3(
    /* [in] */ const String& table,
    /* [in] */ ArrayOf<String>* columns,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& groupBy,
    /* [in] */ const String& having,
    /* [in] */ const String& orderBy,
    /* [in] */ const String& limit,
    /* [out] */ ICursor** cursor)
{
    return Query(FALSE, table, columns, selection, selectionArgs, groupBy,
            having, orderBy, limit, cursor);
}

ECode SQLiteDatabase::RawQuery(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ ICursor** cursor)
{
    return RawQueryWithFactory(NULL, sql, selectionArgs, String(NULL), cursor);
}

ECode SQLiteDatabase::RawQueryEx(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ ICursor** cursor)
{
    return RawQueryWithFactoryEx(NULL, sql, selectionArgs, String(NULL), cancellationSignal, cursor);
}

ECode SQLiteDatabase::RawQueryWithFactory(
    /* [in] */ ISQLiteDatabaseCursorFactory* cursorFactory,
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& editTable,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor)
    *cursor = NULL;
    AcquireReference();
    // try {
    AutoPtr<ISQLiteCursorDriver> driver = new SQLiteDirectCursorDriver(THIS_PROBE(ISQLiteDatabase), sql, editTable, NULL);
    ECode ec = driver->Query(cursorFactory != NULL ? cursorFactory : mCursorFactory.Get(), selectionArgs, cursor);
    // } finally {
    ReleaseReference();
    // }
    return ec;
}

ECode SQLiteDatabase::RawQueryWithFactoryEx(
    /* [in] */ ISQLiteDatabaseCursorFactory* cursorFactory,
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& editTable,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor)
    *cursor = NULL;

    AcquireReference();
    // try {
    AutoPtr<ISQLiteCursorDriver> driver = new SQLiteDirectCursorDriver(THIS_PROBE(ISQLiteDatabase), sql, editTable,
            cancellationSignal);
    ECode ec = driver->Query(cursorFactory != NULL ? cursorFactory : mCursorFactory.Get(), selectionArgs, cursor);
    // } finally {
    ReleaseReference();
    // }
    return ec;
}

ECode SQLiteDatabase::Insert(
    /* [in] */ const String& table,
    /* [in] */ const String& nullColumnHack,
    /* [in] */ IContentValues* values,
    /* [out] */ Int64* rowId)
{
    VALIDATE_NOT_NULL(rowId);
    //try {
    ECode ec = InsertWithOnConflict(table, nullColumnHack, values, CONFLICT_NONE, rowId);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Error inserting %p, 0x%08x", values, ec);
        *rowId = -1;
    }
    // } catch (SQLException e) {
    //     Log.e(TAG, "Error inserting " + values, e);
    //     return -1;
    // }
    return ec;
}

ECode SQLiteDatabase::InsertOrThrow(
    /* [in] */ const String& table,
    /* [in] */ const String& nullColumnHack,
    /* [in] */ IContentValues* values,
    /* [out] */ Int64* rowId)
{
    return InsertWithOnConflict(table, nullColumnHack, values, CONFLICT_NONE, rowId);
}

ECode SQLiteDatabase::Replace(
    /* [in] */ const String& table,
    /* [in] */ const String& nullColumnHack,
    /* [in] */ IContentValues* initialValues,
    /* [out] */ Int64* rowId)
{
    VALIDATE_NOT_NULL(rowId);
    // try {
    ECode ec = InsertWithOnConflict(table, nullColumnHack, initialValues, CONFLICT_REPLACE, rowId);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Error inserting %p, 0x%08x", initialValues, ec);
        *rowId = -1;
    }
    // } catch (SQLException e) {
    //     Log.e(TAG, "Error inserting " + initialValues, e);
    //     return -1;
    // }
    return ec;
}

ECode SQLiteDatabase::ReplaceOrThrow(
    /* [in] */ const String& table,
    /* [in] */ const String& nullColumnHack,
    /* [in] */ IContentValues* initialValues,
    /* [out] */ Int64* rowId)
{
    return InsertWithOnConflict(table, nullColumnHack, initialValues, CONFLICT_REPLACE, rowId);
}

ECode SQLiteDatabase::InsertWithOnConflict(
    /* [in] */ const String& table,
    /* [in] */ const String& nullColumnHack,
    /* [in] */ IContentValues* initialValues,
    /* [in] */ Int32 conflictAlgorithm,
    /* [out] */ Int64* rowId)
{
    VALIDATE_NOT_NULL(rowId);
    *rowId = -1;

    AcquireReference();
    // try {
    StringBuilder sql("INSERT");
    sql.AppendString((*CONFLICT_VALUES)[conflictAlgorithm]);
    sql.AppendCStr(" INTO ");
    sql.AppendString(table);
    sql.AppendCStr("(");

    AutoPtr< ArrayOf<IInterface*> > bindArgs;
    Int32 size = (initialValues != NULL && (initialValues->GetSize(&size), size > 0))
            ? (initialValues->GetSize(&size), size) : 0;
    if (size > 0) {
        bindArgs = ArrayOf<IInterface*>::Alloc(size);
        Int32 i = 0;
        AutoPtr< ArrayOf<String> > colNames;
        initialValues->KeySet((ArrayOf<String>**)&colNames);
        for (Int32 j = 0; j < colNames->GetLength(); j++) {
            sql.AppendCStr((i > 0) ? "," : "");
            sql += (*colNames)[i];
            AutoPtr<IInterface> temp;
            initialValues->Get((*colNames)[i], (IInterface**)&temp);
            bindArgs->Set(i++, temp);
        }
        sql.AppendCStr(")");
        sql.AppendCStr(" VALUES (");
            //begin from this
        for (i = 0; i < size; i++) {
            sql.AppendCStr((i > 0) ? ",?" : "?");
        }
    }
    else {
        sql.AppendString(nullColumnHack + String(") VALUES (NULL"));
    }
    sql.AppendCStr(")");

    AutoPtr<ISQLiteStatement> statement;
    ECode ec = CSQLiteStatement::New(THIS_PROBE(ISQLiteDatabase), sql.ToString(), bindArgs,
            (ISQLiteStatement**)&statement);
    if (FAILED(ec)) {
        goto fail;
    }
    // try {
    ec = statement->ExecuteInsert(rowId);
    // } finally {
    statement->Close();
    // }
    // } finally {
fail:
    ReleaseReference();
    // }
    return ec;
}

ECode SQLiteDatabase::Delete(
    /* [in] */ const String& table,
    /* [in] */ const String& whereClause,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    AcquireReference();
    // try {
    AutoPtr<ISQLiteStatement> statement;
    StringBuilder sb("DELETE FROM ");
    sb.AppendString(table);
    sb.AppendString(!TextUtils::IsEmpty(whereClause) ? String(" WHERE ") + whereClause : String(""));

    AutoPtr< ArrayOf<IInterface*> > bindArgs;
    if (whereArgs != NULL && whereArgs->GetLength() > 0) {
        bindArgs = ArrayOf<IInterface*>::Alloc(whereArgs->GetLength());
        for(Int32 i = 0; i < whereArgs->GetLength(); i++) {
            AutoPtr<ICharSequence> cs;
            CStringWrapper::New((*whereArgs)[i], (ICharSequence**)&cs);
            bindArgs->Set(i, cs);
        }
    }

    ECode ec = CSQLiteStatement::New(THIS_PROBE(ISQLiteDatabase), sb.ToString(), bindArgs, (ISQLiteStatement**)&statement);
    if (FAILED(ec)) {
        goto fail;
    }
    // try {
    ec = statement->ExecuteUpdateDelete(value);
    // } finally {
    statement->Close();
    // }
    // } finally {
fail:
    ReleaseReference();
    // }
    return ec;
}

ECode SQLiteDatabase::Update(
    /* [in] */ const String& table,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& whereClause,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [out] */ Int32* value)
{
    return UpdateWithOnConflict(table, values, whereClause, whereArgs, CONFLICT_NONE, value);
}

ECode SQLiteDatabase::UpdateWithOnConflict(
    /* [in] */ const String& table,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& whereClause,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [in] */ Int32 conflictAlgorithm,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    Int32 size;
    if (values == NULL || (values->GetSize(&size), size == 0)) {
        //throw new IllegalArgumentException("Empty values");
        Slogger::E(TAG, "Empty values");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AcquireReference();
    // try {
    StringBuilder sql("UPDATE ");
    sql.AppendString((*CONFLICT_VALUES)[conflictAlgorithm]);
    sql.AppendString(table);
    sql.AppendCStr(" SET ");

    // move all bind args to one array
    Int32 setValuesSize;
    values->GetSize(&setValuesSize);
    Int32 bindArgsSize = (whereArgs == NULL) ? setValuesSize : (setValuesSize + whereArgs->GetLength());
    AutoPtr< ArrayOf<IInterface*> > bindArgs = ArrayOf<IInterface*>::Alloc(bindArgsSize);
    Int32 i = 0;
    AutoPtr< ArrayOf<String> > colNames;
    values->KeySet((ArrayOf<String>**)&colNames);
    for (Int32 j = 0; j < colNames->GetLength(); j++) {
        String colName = (*colNames)[i];
        sql.AppendCStr((i > 0) ? "," : "");
        sql.AppendString(colName);
        AutoPtr<IInterface> temp;
        values->Get(colName, (IInterface**)&temp);
        bindArgs->Set(i++, temp);
        sql.AppendCStr("=?");
    }
    if (whereArgs != NULL) {
        for (i = setValuesSize; i < bindArgsSize; i++) {
            AutoPtr<ICharSequence> cs;
            CStringWrapper::New((*whereArgs)[i - setValuesSize], (ICharSequence**)&cs);
            bindArgs->Set(i, cs);
        }
    }
    if (!TextUtils::IsEmpty(whereClause)) {
        sql.AppendCStr(" WHERE ");
        sql.AppendString(whereClause);
    }

    AutoPtr<ISQLiteStatement> statement;
    ECode ec = CSQLiteStatement::New(THIS_PROBE(ISQLiteDatabase), sql.ToString(), bindArgs, (ISQLiteStatement**)&statement);
    if (FAILED(ec)) {
        goto fail;
    }
    //try {
    ec = statement->ExecuteUpdateDelete(value);
    //} finally {
    statement->Close();
    //}
    //} finally {
fail:
    ReleaseReference();
    //}
    return ec;
}

ECode SQLiteDatabase::ExecSQL(
    /* [in] */ const String& sql)
{
    return ExecuteSql(sql, NULL);
}

ECode SQLiteDatabase::ExecSQLEx(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs)
{
    if (bindArgs == NULL) {
        // throw new IllegalArgumentException("Empty bindArgs");
        Slogger::E(TAG, "Empty bindArgs");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return ExecuteSql(sql, bindArgs);
}

ECode SQLiteDatabase::ExecuteSql(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs)
{
    AcquireReference();
    //try {
    Int32 type = DatabaseUtils::GetSqlStatementType(sql);
    if (type == DatabaseUtils_STATEMENT_ATTACH) {
        Boolean disableWal = FALSE;
        {
            Mutex::Autolock lock(mLock);
            if (!mHasAttachedDbsLocked) {
                mHasAttachedDbsLocked = TRUE;
                disableWal = TRUE;
            }
        }
        if (disableWal) {
            DisableWriteAheadLogging();
        }
    }

    AutoPtr<ISQLiteStatement> statement;
    ECode ec = CSQLiteStatement::New(THIS_PROBE(ISQLiteDatabase), sql, bindArgs, (ISQLiteStatement**)&statement);
    if (FAILED(ec)) {
        goto fail;
    }
    //try {
    Int32 result;
    ec = statement->ExecuteUpdateDelete(&result);
    //} finally {
    statement->Close();
    //}
    //} finally {
fail:
    ReleaseReference();
    //}
    return ec;
}

ECode SQLiteDatabase::IsReadOnly(
    /* [out] */ Boolean* isReadOnly)
{
    VALIDATE_NOT_NULL(isReadOnly);
    Mutex::Autolock lock(mLock);
    *isReadOnly = IsReadOnlyLocked();
    return NOERROR;
}

Boolean SQLiteDatabase::IsReadOnlyLocked()
{
    Mutex::Autolock lock(mLock);
    return (mConfigurationLocked->mOpenFlags & OPEN_READ_MASK) == OPEN_READONLY;
}

ECode SQLiteDatabase::IsInMemoryDatabase(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Mutex::Autolock lock(mLock);
    *result = mConfigurationLocked->IsInMemoryDb();
    return NOERROR;
}

ECode SQLiteDatabase::IsOpen(
    /* [out] */ Boolean* isOpen)
{
    VALIDATE_NOT_NULL(isOpen);
    Mutex::Autolock lock(mLock);
    *isOpen = mConnectionPoolLocked != NULL;
    return NOERROR;
}

ECode SQLiteDatabase::NeedUpgrade(
    /* [in] */ Int32 newVersion,
    /* [out] */ Boolean* needed)
{
    VALIDATE_NOT_NULL(needed);
    Int32 version;
    GetVersion(&version);
    *needed = newVersion > version;
    return NOERROR;
}

ECode SQLiteDatabase::GetPath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);
    Mutex::Autolock lock(mLock);
    *path = mConfigurationLocked->mPath;
    return NOERROR;
}

ECode SQLiteDatabase::SetLocale(
    /* [in] */ ILocale* locale)
{
    if (locale == NULL) {
        //throw new IllegalArgumentException("locale must not be null.");
        Slogger::E(TAG, "locale must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Mutex::Autolock lock(mLock);
    FAIL_RETURN(ThrowIfNotOpenLocked())

    AutoPtr<ILocale> oldLocale = mConfigurationLocked->mLocale;
    mConfigurationLocked->mLocale = locale;
    //try {
    ECode ec = mConnectionPoolLocked->Reconfigure(mConfigurationLocked);
    //} catch (RuntimeException ex) {
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        mConfigurationLocked->mLocale = oldLocale;
    }
    //}
    return ec;
}

ECode SQLiteDatabase::SetMaxSqlCacheSize(
    /* [in] */ Int32 cacheSize)
{
    if (cacheSize > MAX_SQL_CACHE_SIZE || cacheSize < 0) {
        //throw new IllegalStateException(
        //        "expected value between 0 and " + MAX_SQL_CACHE_SIZE);
        Slogger::E(TAG, "expected value between 0 and %d", MAX_SQL_CACHE_SIZE);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Mutex::Autolock lock(mLock);
    FAIL_RETURN(ThrowIfNotOpenLocked())

    Int32 oldMaxSqlCacheSize = mConfigurationLocked->mMaxSqlCacheSize;
    mConfigurationLocked->mMaxSqlCacheSize = cacheSize;
    //try {
    ECode ec = mConnectionPoolLocked->Reconfigure(mConfigurationLocked);
    //} catch (RuntimeException ex) {
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        mConfigurationLocked->mMaxSqlCacheSize = oldMaxSqlCacheSize;
        //throw ex;
    }
    //}
    return ec;
}

ECode SQLiteDatabase::SetForeignKeyConstraintsEnabled(
    /* [in] */ Boolean enable)
{
    Mutex::Autolock lock(mLock);
    FAIL_RETURN(ThrowIfNotOpenLocked())

    if (mConfigurationLocked->mForeignKeyConstraintsEnabled == enable) {
        return NOERROR;
    }

    mConfigurationLocked->mForeignKeyConstraintsEnabled = enable;
    //try {
    ECode ec = mConnectionPoolLocked->Reconfigure(mConfigurationLocked);
    //} catch (RuntimeException ex) {
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        mConfigurationLocked->mForeignKeyConstraintsEnabled = !enable;
        //throw ex;
    }
    //}
    return ec;
}

ECode SQLiteDatabase::EnableWriteAheadLogging(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Mutex::Autolock lock(mLock);
    FAIL_RETURN(ThrowIfNotOpenLocked())

    if ((mConfigurationLocked->mOpenFlags & ENABLE_WRITE_AHEAD_LOGGING) != 0) {
        *result = TRUE;
        return NOERROR;
    }

    if (IsReadOnlyLocked()) {
        // WAL doesn't make sense for readonly-databases.
        // TODO: True, but connection pooling does still make sense...
        *result = FALSE;
        return NOERROR;
    }

    if (mConfigurationLocked->IsInMemoryDb()) {
        //Log.i(TAG, "can't enable WAL for memory databases.");
        Slogger::I(TAG, "can't enable WAL for memory databases.");
        *result = FALSE;
        return NOERROR;
    }

    // make sure this database has NO attached databases because sqlite's write-ahead-logging
    // doesn't work for databases with attached databases
    if (mHasAttachedDbsLocked) {
        if (Logger::IsLoggable(TAG, FALSE/*Logger::DEBUG*/)) {
           Logger::D(TAG, "this database: %s has attached databases. can't  enable WAL.", mConfigurationLocked->mLabel.string());
        }
        *result = FALSE;
        return NOERROR;
    }

    mConfigurationLocked->mOpenFlags |= ENABLE_WRITE_AHEAD_LOGGING;
    //try {
    ECode ec = mConnectionPoolLocked->Reconfigure(mConfigurationLocked);
    //} catch (RuntimeException ex) {
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        mConfigurationLocked->mOpenFlags &= ~ENABLE_WRITE_AHEAD_LOGGING;
        *result = FALSE;
        return ec;
    }
    //}
    *result = TRUE;
    return ec;
}

ECode SQLiteDatabase::DisableWriteAheadLogging()
{
    Mutex::Autolock lock(mLock);
    FAIL_RETURN(ThrowIfNotOpenLocked())

    if ((mConfigurationLocked->mOpenFlags & ENABLE_WRITE_AHEAD_LOGGING) == 0) {
        return NOERROR;
    }

    mConfigurationLocked->mOpenFlags &= ~ENABLE_WRITE_AHEAD_LOGGING;
    //try {
    ECode ec = mConnectionPoolLocked->Reconfigure(mConfigurationLocked);
    //} catch (RuntimeException ex) {
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        mConfigurationLocked->mOpenFlags |= ENABLE_WRITE_AHEAD_LOGGING;
    }
    //}
    return ec;
}

ECode SQLiteDatabase::IsWriteAheadLoggingEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Mutex::Autolock lock(mLock);
    FAIL_RETURN(ThrowIfNotOpenLocked())
    *result = (mConfigurationLocked->mOpenFlags & ENABLE_WRITE_AHEAD_LOGGING) != 0;
    return NOERROR;
}

AutoPtr< List<AutoPtr<SQLiteDebug::DbStats> > > SQLiteDatabase::GetDbStats()
{
    AutoPtr< List<AutoPtr<SQLiteDebug::DbStats> > > dbStatsList = new List<AutoPtr<SQLiteDebug::DbStats> >();
    AutoPtr<List<AutoPtr<SQLiteDatabase> > > database = GetActiveDatabases();
    List<AutoPtr<SQLiteDatabase> >::Iterator it;
    for (it = database->Begin(); it != database->End(); ++it) {
        (*it)->CollectDbStats(dbStatsList);
    }
    return dbStatsList;
}

void SQLiteDatabase::CollectDbStats(
    /* [in] */ List<AutoPtr<SQLiteDebug::DbStats> >* dbStatsList)
{
    Mutex::Autolock lock(mLock);
    if (mConnectionPoolLocked != NULL) {
        mConnectionPoolLocked->CollectDbStats(dbStatsList);
    }
}

AutoPtr<List<AutoPtr<SQLiteDatabase> > > SQLiteDatabase::GetActiveDatabases()
{
    AutoPtr<List<AutoPtr<SQLiteDatabase> > > databases = new List<AutoPtr<SQLiteDatabase> >();
    Mutex::Autolock lock(sActiveDatabasesLock);
    HashMap<AutoPtr<SQLiteDatabase>, AutoPtr<IInterface> >::Iterator it;
    for (it = sActiveDatabases.Begin(); it != sActiveDatabases.End(); ++it) {
        databases->PushBack(it->mFirst);
    }
    return databases;
}

void SQLiteDatabase::DumpAll(
    /* [in] */ IPrinter* printer,
    /* [in] */ Boolean verbose)
{
    AutoPtr<List<AutoPtr<SQLiteDatabase> > > database = GetActiveDatabases();
    List<AutoPtr<SQLiteDatabase> >::Iterator it;
    for (it = database->Begin(); it != database->End(); ++it) {
        (*it)->Dump(printer, verbose);
    }
    delete database;
}

void SQLiteDatabase::Dump(
    /* [in] */ IPrinter* printer,
    /* [in] */ Boolean verbose)
{
    Mutex::Autolock lock(mLock);
    if (mConnectionPoolLocked != NULL) {
        printer->Println(String(""));
        mConnectionPoolLocked->Dump(printer, verbose);
    }
}

ECode SQLiteDatabase::GetAttachedDbs(
    /* [out] */ IObjectStringMap** dbs)
{
    VALIDATE_NOT_NULL(dbs)
    *dbs = NULL;

    AutoPtr<IObjectStringMap> attachedDbs;
    CObjectStringMap::New((IObjectStringMap**)&attachedDbs);
    {
        Mutex::Autolock lock(mLock);
        if (mConnectionPoolLocked == NULL) {
            // not open
            return NOERROR;
        }

        if (!mHasAttachedDbsLocked) {
            // No attached databases.
            // There is a small window where attached databases exist but this flag is not
            // set yet.  This can occur when this thread is in a race condition with another
            // thread that is executing the SQL statement: "attach database <blah> as <foo>"
            // If this thread is NOT ok with such a race condition (and thus possibly not
            // receivethe entire list of attached databases), then the caller should ensure
            // that no thread is executing any SQL statements while a thread is calling this
            // method.  Typically, this method is called when 'adb bugreport' is done or the
            // caller wants to collect stats on the database and all its attached databases.
            //attachedDbs.add(new Pair<String, String>("main", mConfigurationLocked.path));
            AutoPtr<ICharSequence> cs;
            CStringWrapper::New(mConfigurationLocked->mPath, (ICharSequence**)&cs);
            attachedDbs->Put(String("main"), cs);
            *dbs = attachedDbs;
            INTERFACE_ADDREF(*dbs);
            return NOERROR;
        }

        AcquireReference();
    }

    //try {
    // has attached databases. query sqlite to get the list of attached databases.
    AutoPtr<ICursor> c;
    //try {
    ECode ec = RawQuery(String("pragma database_list;"), NULL, (ICursor**)&c);
    if (FAILED(ec)) {
        goto fail;
    }
    Boolean result;
    while (c->MoveToNext(&result), result) {
        // sqlite returns a row for each database in the returned list of databases.
        //   in each row,
        //       1st column is the database name such as main, or the database
        //                              name specified on the "ATTACH" command
        //       2nd column is the database file path.
        String str1,str2;
        c->GetString(1, &str1);
        c->GetString(2, &str2);
        AutoPtr<ICharSequence> cs;
        CStringWrapper::New(str2, (ICharSequence**)&cs);
        attachedDbs->Put(str1, cs);
    }
    //} finally {
    if (c != NULL) {
        c->Close();
    }
fail:
    //}
    //return attachedDbs;
    //} finally {
    ReleaseReference();
    //}
    *dbs = attachedDbs;
    INTERFACE_ADDREF(*dbs);
    return ec;
}

ECode SQLiteDatabase::IsDatabaseIntegrityOk(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AcquireReference();
    //try {
    AutoPtr<IObjectStringMap> attachedDbs;
    //try {
    ECode ec = GetAttachedDbs((IObjectStringMap**)&attachedDbs);
    if (ec == (ECode)E_SQLITE_EXCEPTION) {
        // can't get attachedDb list. do integrity check on the main database
        attachedDbs = NULL;
        CObjectStringMap::New((IObjectStringMap**)&attachedDbs);
        String path;
        GetPath(&path);
        AutoPtr<ICharSequence> cs;
        CStringWrapper::New(path, (ICharSequence**)&cs);
        attachedDbs->Put(String("main"), cs);
    }
    if (attachedDbs == NULL) {
        //throw new IllegalStateException("databaselist for: " + getPath() + " couldn't " +
        //        "be retrieved. probably because the database is closed");
        String path;
        GetPath(&path);
        Slogger::E(TAG, "databaselist for: %s couldn't be retrieved. probably because the database is closed", path.string());
        ReleaseReference();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    //} catch // can't get attachedDb list. do integrity check on the main database
        // attachedDbs = new ArrayList<Pair<String, String>>();
        // attachedDbs.add(new Pair<String, String>("main", getPath()));(SQLiteException e) {
    //}

    AutoPtr< ArrayOf<String> > keys;
    attachedDbs->GetKeys((ArrayOf<String>**)&keys);
    for (Int32 i = 0; i < keys->GetLength(); i++) {
        AutoPtr<ISQLiteStatement> prog;
        // try {
        StringBuilder sb("PRAGMA ");
        sb.AppendString((*keys)[i]);
        sb.AppendCStr(".integrity_check(1);");
        if (FAILED(CompileStatement(sb.ToString(), (ISQLiteStatement**)&prog))) {
            if (prog != NULL) prog->Close();
            continue;
        }
        String rslt;
        if (FAILED(prog->SimpleQueryForString(&rslt))) {
            if (prog != NULL) prog->Close();
            continue;
        }
        if (!rslt.EqualsIgnoreCase("ok")) {
            AutoPtr<ICharSequence> cs;
            attachedDbs->Get((*keys)[i], (IInterface**)&cs);
            String value;
            cs->ToString(&value);
            // integrity_checker failed on main or attached databases
            Slogger::E(TAG, "PRAGMA integrity_check on %s returned: %s", value.string(), rslt.string());
            *result = FALSE;
            if (prog != NULL) prog->Close();
            ReleaseReference();
            return NOERROR;
        }
        //} finally {
        if (prog != NULL) prog->Close();
        //}
    }
    //} finally {
    ReleaseReference();
    //}
    return NOERROR;
}

ECode SQLiteDatabase::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    String path;
    GetPath(&path);
    *str = String("SQLiteDatabase: ") + path;
    return NOERROR;
}

ECode SQLiteDatabase::ThrowIfNotOpenLocked()
{
    if (mConnectionPoolLocked == NULL) {
        //throw new IllegalStateException("The database '" + mConfigurationLocked.label
        //        + "' is not open.");
        Slogger::E(TAG, "The database '%s' is not open.", mConfigurationLocked->mLabel.string());
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode SQLiteDatabase::AcquireReference()
{
    return SQLiteClosable::AcquireReference();
}

ECode SQLiteDatabase::ReleaseReference()
{
    return SQLiteClosable::ReleaseReference();
}

ECode SQLiteDatabase::ReleaseReferenceFromContainer()
{
    return SQLiteClosable::ReleaseReferenceFromContainer();
}

ECode SQLiteDatabase::Close()
{
    return SQLiteClosable::Close();
}

} //Sqlite
} //Database
} //Droid
} //Elastos
