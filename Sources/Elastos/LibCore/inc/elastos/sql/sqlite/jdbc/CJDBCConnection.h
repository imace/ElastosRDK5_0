
#ifndef __CJDBCCONNECTION_H__
#define __CJDBCCONNECTION_H__

#include "_Elastos_Sql_SQLite_JDBC_CJDBCConnection.h"

using Elastos::Utility::IObjectStringMap;
using Elastos::Utility::IProperties;
using Elastos::Sql::ISQLWarning;
using Elastos::Sql::IStatement;
using Elastos::Sql::IDatabaseMetaData;
using Elastos::Sql::ICallableStatement;
using Elastos::Sql::IPreparedStatement;
using Elastos::Sql::ISavePoint;
using Elastos::Sql::IClob;
using Elastos::Sql::IBlob;
using Elastos::Sql::INClob;
using Elastos::Sql::ISQLXML;
using Elastos::Sql::IPreparedStatement;

namespace Elastos {
namespace Sql {
namespace SQLite {
namespace JDBC{

CarClass(CJDBCConnection)
    , public Object
    , public IJDBCConnection
    , public IConnection
    , public IWrapper
    , public IBusyHandler
{
public:
    CAR_OBJECT_DECL();

    CARAPI ClearWarnings();

    CARAPI Close();

    CARAPI Commit();

    CARAPI CreateStatement(
        /* [out] */ IStatement ** ppStatement);

    CARAPI CreateStatement(
        /* [in] */ Int32 resultSetType,
        /* [in] */ Int32 resultSetConcurrency,
        /* [out] */ IStatement ** ppStatement);

    CARAPI CreateStatement(
        /* [in] */ Int32 resultSetType,
        /* [in] */ Int32 resultSetConcurrency,
        /* [in] */ Int32 resultSetHoldability,
        /* [out] */ IStatement ** ppStatement);

    CARAPI GetAutoCommit(
        /* [out] */ Boolean * pAuto);

    CARAPI GetCatalog(
        /* [out] */ String * pCatelog);

    CARAPI GetHoldability(
        /* [out] */ Int32 * pHoldability);

    CARAPI GetMetaData(
        /* [out] */ IDatabaseMetaData ** ppMetadata);

    CARAPI GetTransactionIsolation(
        /* [out] */ Int32 * pValue);

    CARAPI GetTypeMap(
        /* [out] */ IObjectStringMap ** typemap);

    CARAPI GetWarnings(
        /* [out] */ ISQLWarning ** ppWarning);

    CARAPI IsClosed(
        /* [out] */ Boolean * pIsClosed);

    CARAPI IsReadOnly(
        /* [out] */ Boolean * pIsReadOnly);

    CARAPI NativeSQL(
        /* [in] */ const String& sql,
        /* [out] */ String * pNsql);

    CARAPI PrepareCall(
        /* [in] */ const String& sql,
        /* [out] */ ICallableStatement ** ppCallableStatement);

    CARAPI PrepareCall(
        /* [in] */ const String& sql,
        /* [in] */ Int32 resultSetType,
        /* [in] */ Int32 resultSetConcurrency,
        /* [out] */ ICallableStatement ** ppCallableStatement);

    CARAPI PrepareCall(
        /* [in] */ const String& sql,
        /* [in] */ Int32 resultSetType,
        /* [in] */ Int32 resultSetConcurrency,
        /* [in] */ Int32 resultSetHoldability,
        /* [out] */ ICallableStatement ** ppCallableStatement);

    CARAPI PrepareStatement(
        /* [in] */ const String& sql,
        /* [out] */ IPreparedStatement ** ppPreparedStatement);

    CARAPI PrepareStatement(
        /* [in] */ const String& sql,
        /* [in] */ Int32 autoGeneratedKeys,
        /* [out] */ IPreparedStatement ** ppPreparedStatement);

    CARAPI PrepareStatement(
        /* [in] */ const String& sql,
        /* [in] */ const ArrayOf<Int32> & columnIndexes,
        /* [out] */ IPreparedStatement ** ppPreparedStatement);

    CARAPI PrepareStatement(
        /* [in] */ const String& sql,
        /* [in] */ Int32 resultSetType,
        /* [in] */ Int32 resultSetConcurrency,
        /* [out] */ IPreparedStatement ** ppStatement);

    CARAPI PrepareStatement(
        /* [in] */ const String& sql,
        /* [in] */ Int32 resultSetType,
        /* [in] */ Int32 resultSetConcurrency,
        /* [in] */ Int32 resultSetHoldability,
        /* [out] */ IPreparedStatement ** ppPreparedStatement);

    CARAPI ReleaseSavepoint(
        /* [in] */ ISavePoint * pSavepoint);

    CARAPI Rollback();

    CARAPI Rollback(
        /* [in] */ ISavePoint * pSavepoint);

    CARAPI SetAutoCommit(
        /* [in] */ Boolean autoCommit);

    CARAPI SetCatalog(
        /* [in] */ const String& catalog);

    CARAPI SetHoldability(
        /* [in] */ Int32 holdability);

    CARAPI SetReadOnly(
        /* [in] */ Boolean readOnly);

    CARAPI SetSavepoint(
        /* [out] */ ISavePoint ** ppSavepoint);

    CARAPI SetSavepoint(
        /* [in] */ const String& name,
        /* [out] */ ISavePoint ** ppSavepoint);

    CARAPI SetTransactionIsolation(
        /* [in] */ Int32 level);

    CARAPI SetTypeMap(
        /* [in] */ IObjectStringMap * typemap);

    CARAPI CreateClob(
        /* [out] */ IClob ** ppClob);

    CARAPI CreateBlob(
        /* [out] */ Elastos::Sql::IBlob ** ppBlob);

    CARAPI CreateNClob(
        /* [out] */ INClob ** ppNclob);

    CARAPI CreateSQLXML(
        /* [out] */ ISQLXML ** ppSqlXML);

    CARAPI IsValid(
        /* [in] */ Int32 timeout,
        /* [out] */ Boolean * pValid);

    CARAPI SetClientInfo(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    CARAPI GetClientInfo(
        /* [out] */ IProperties** properties);

    CARAPI Busy(
        /* [in] */ const String& table,
        /* [in] */ Int32 count,
        /* [out] */ Boolean * pValue);

    CARAPI constructor(
        /* [in] */ const String& url,
        /* [in] */ const String& enc,
        /* [in] */ const String& pwd,
        /* [in] */ const String& drep,
        /* [in] */ const String& vfs);

    CARAPI GetSQLiteDatabase(
        /* [out] */ IDatabase** ppDb);

    CARAPI PrepareStatement(
        /* [in] */ const String& sql,
        /* [in] */ const ArrayOf<String>& columnNames,
        /* [out] */ IPreparedStatement** preparedStatement);

    CARAPI GetClientInfo(
        /* [in] */ const String& name,
        /* [out] */ String* info);

    CARAPI IsWrapperFor(
        /* [in] */ IInterface * iface,
        /* [out] */ Boolean * value);

    CARAPI SetClientInfo(
        /* [in] */ IProperties* properties);
    CARAPI CreateArrayOf(
        /* [in] */ const String& typeName,
        /* [in] */ const ArrayOf<IInterface * >& elements,
        /* [out] */ IArray ** iarray);

    CARAPI CreateStruct(
        /* [in] */ const String& typeName,
        /* [in] */ const ArrayOf<IInterface * >& attributes,
        /* [out] */ IStruct ** istruct);

    CARAPI Unwrap(
        /* [in] */ PInterface iface,
        /* [out] */ IInterface ** oface);

    CARAPI_(Boolean) Busy3(
        /* [in] */ IDatabase* db,
        /* [in] */ Int32 count);

private:
    ECode Busy0(
        /* [in] */ IDatabase* db,
        /* [in] */ Int32 count,
        /* [out] */ Boolean* value);

    ECode Open(
        /* [in] */ Boolean readonly,
        /* [out] */ IDatabase** db);

public:
    AutoPtr<IDatabase> mDb;
    Int64 timeout;
    String mVfs;
    String mEnc;
    Boolean mUseJulian;
    String mUrl;
    Boolean mIntrans;
    Boolean mAutocommit;

private:
    Int64 mT0;
    String mDbfile;
    Boolean mReadonly;
    Int32 mTrmode;
};

} // namespace JDBC
} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __CJDBCCONNECTION_H__
