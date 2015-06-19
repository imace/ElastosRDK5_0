
#include "CStmt.h"
#include "sqlitejni.h"

namespace Elastos {
namespace Sql {
namespace SQLite {

CAR_OBJECT_IMPL(CStmt);

CAR_INTERFACE_IMPL(CStmt, Object, IStmt);

CStmt::CStmt()
{
    mHandle = 0;
    isinit = FALSE;
    mError_code = 0;
}

Boolean CStmt::Internal_init()
{
    if (!isinit) {
        isinit = TRUE;
    }
    return isinit;
}

Boolean CStmt::isinit = Internal_init();

ECode CStmt::Prepare(
    /* [out] */ Boolean* value)
{
#if HAVE_SQLITE3
    hvm *v = (hvm *)mHandle;
    void *svm = 0;
    char *tail;
    Int32 ret;

    if (v && v->vm) {
        sqlite3_finalize((sqlite3_stmt *) v->vm);
        v->vm = 0;
    }
    if (v && v->h && v->h->sqlite) {
    if (!v->tail) {
        *value = FALSE;
    }
#if HAVE_SQLITE3_PREPARE16_V2
    ret = sqlite3_prepare16_v2((sqlite3 *) v->h->sqlite,
                   v->tail, -1, (sqlite3_stmt **) &svm,
                   (const void **) &tail);
#else
    ret = sqlite3_prepare16((sqlite3 *) v->h->sqlite,
                v->tail, -1, (sqlite3_stmt **) &svm,
                (const void **) &tail);
#endif
    if (ret != SQLITE_OK) {
        if (svm) {
        sqlite3_finalize((sqlite3_stmt *) svm);
        svm = 0;
        }
    }
    if (ret != SQLITE_OK) {
        const char *err = sqlite3_errmsg((sqlite3*)v->h->sqlite);

        mError_code = (Int32)err;
        v->tail = 0;
        *value = FALSE;
        return E_SQL_EXCEPTION;
    }
    if (!svm) {
        v->tail = 0;
        *value = FALSE;
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
    v->vm = svm;
    v->tail = (char *) tail;
    v->hh.row1 = 1;
    *value = TRUE;
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::Step(
    /* [out] */ Boolean* value)
{
#if HAVE_SQLITE3 && HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
        Int32 ret = 0;

        ret = sqlite3_step((sqlite3_stmt *) v->vm);
        if (ret == SQLITE_ROW) {
            *value = TRUE;
            return NOERROR;
        }
        if (ret != SQLITE_DONE) {
            const char *err = sqlite3_errmsg((sqlite3*)v->h->sqlite);

            mError_code = (Int32)err;
            return E_SQL_EXCEPTION;
        }
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    *value = FALSE;
    return NOERROR;
}

ECode CStmt::Close()
{
#if HAVE_SQLITE3 && HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;
    if (v && v->vm && v->h) {
        Int32 ret = 0;
        ret = sqlite3_finalize((sqlite3_stmt *) v->vm);
        v->vm = 0;
        if (ret != SQLITE_OK) {
            const char  *err = sqlite3_errmsg((sqlite3*)v->h->sqlite);

            mError_code = (Int32)err;
            return E_SQL_EXCEPTION;
        }
        return NOERROR;
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif

    return NOERROR;
}

ECode CStmt::Reset()
{
#if HAVE_SQLITE3 && HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
        sqlite3_reset((sqlite3_stmt *) v->vm);
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::ClearBindings()
{
#if HAVE_SQLITE3 && HAVE_SQLITE3_CLEAR_BINDINGS
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
        sqlite3_clear_bindings((sqlite3_stmt *) v->vm);
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::Bind(
    /* [in] */ Int32 pos,
    /* [in] */ Int32 value)
{
#if HAVE_SQLITE3 && HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
    Int32 npar = sqlite3_bind_parameter_count((sqlite3_stmt *) v->vm);
    Int32 ret = 0;

    if (pos < 1 || pos > npar) {
        return E_SQL_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    ret = sqlite3_bind_int((sqlite3_stmt *) v->vm, pos, value);
    if (ret != SQLITE_OK) {
        mError_code = ret;
        return E_SQL_EXCEPTION;
    }
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
        return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::Bind(
    /* [in] */ Int32 pos,
    /* [in] */ Int64 value)
{
#if HAVE_SQLITE3 && HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
    Int32 npar = sqlite3_bind_parameter_count((sqlite3_stmt *) v->vm);
    Int32 ret = 0;

    if (pos < 1 || pos > npar) {
        return E_SQL_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    ret = sqlite3_bind_int64((sqlite3_stmt *) v->vm, pos, value);
    if (ret != SQLITE_OK) {
        mError_code = ret;
        return E_SQL_EXCEPTION;
    }
    } else {
    return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::Bind(
    /* [in] */ Int32 pos,
    /* [in] */ Double value)
{
#if HAVE_SQLITE3 && HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
    Int32 npar = sqlite3_bind_parameter_count((sqlite3_stmt *) v->vm);
    Int32 ret = 0;

    if (pos < 1 || pos > npar) {
        return E_SQL_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    ret = sqlite3_bind_double((sqlite3_stmt *) v->vm, pos, value);
    if (ret != SQLITE_OK) {
        mError_code = ret;
        return E_SQL_EXCEPTION;
    }
    } else {
    return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::Bind(
    /* [in] */ Int32 pos,
    /* [in] */ const ArrayOf<Byte>& value)
{
#if HAVE_SQLITE3 && HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
        Int32 npar = sqlite3_bind_parameter_count((sqlite3_stmt *) v->vm);
        Int32 ret = 0;
        Int32 len = 0;

        if (pos < 1 || pos > npar) {
            return E_SQL_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        if (value.GetPayload()) {
            len = value.GetLength();
            if (len > 0) {
                ret = sqlite3_bind_blob((sqlite3_stmt *) v->vm,
                            pos, value.GetPayload(), len, sqlite3_free);
            } else {
                ret = sqlite3_bind_blob((sqlite3_stmt *) v->vm,
                            pos, "", 0, SQLITE_STATIC);
            }
        } else {
            ret = sqlite3_bind_null((sqlite3_stmt *) v->vm, pos);
        }
        if (ret != SQLITE_OK) {
            mError_code = ret;
            return E_SQL_EXCEPTION;
        }
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::Bind(
    /* [in] */ Int32 pos,
    /* [in] */ const String& value)
{
#if HAVE_SQLITE3 && HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
        Int32 npar = sqlite3_bind_parameter_count((sqlite3_stmt *) v->vm);
        Int32 ret = 0;
        UInt32 len = 0, count = 0;

        if (pos < 1 || pos > npar) {
            return E_SQL_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (!value.IsNull()) {
            len = value.GetByteLength();
            if (len > 0) {
                ret = sqlite3_bind_text((sqlite3_stmt *) v->vm,
                            pos, value.string(), len, sqlite3_free);
            } else {
                ret = sqlite3_bind_text((sqlite3_stmt *) v->vm, pos, "", 0,
                          SQLITE_STATIC);
            }
        } else {
            ret = sqlite3_bind_null((sqlite3_stmt *) v->vm, pos);
        }
        if (ret != SQLITE_OK) {
            mError_code = ret ;
            return E_SQL_EXCEPTION;
        }
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::Bind(
    /* [in] */ Int32 pos)
{
#if HAVE_SQLITE3 && HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
    Int32 npar = sqlite3_bind_parameter_count((sqlite3_stmt *) v->vm);
    Int32 ret = 0;

    if (pos < 1 || pos > npar) {
        return E_SQL_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    ret = sqlite3_bind_null((sqlite3_stmt *) v->vm, pos);
    if (ret != SQLITE_OK) {
        mError_code = ret;
        return E_SQL_EXCEPTION;
    }
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::BindZeroblob(
    /* [in] */ Int32 pos,
    /* [in] */ Int32 length)
{
#if HAVE_SQLITE3 && HAVE_SQLITE3_BIND_ZEROBLOB
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
    Int32 npar = sqlite3_bind_parameter_count((sqlite3_stmt *) v->vm);
    Int32 ret = 0;

    if (pos < 1 || pos > npar) {
        return E_SQL_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    ret = sqlite3_bind_zeroblob((sqlite3_stmt *) v->vm, pos, len);
    if (ret != SQLITE_OK) {
        mError_code = ret;
        return E_SQL_EXCEPTION;
    }
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::BindParameterCount(
    /* [out] */ Int32* count)
{
#if HAVE_SQLITE3 && HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
        *count = sqlite3_bind_parameter_count((sqlite3_stmt *) v->vm);
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::BindParameterName(
    /* [in] */ Int32 pos,
    /* [out] */ String* str)
{
#if HAVE_SQLITE3 && HAVE_SQLITE3_BIND_PARAMETER_NAME
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
        Int32 npar = sqlite3_bind_parameter_count((sqlite3_stmt *) v->vm);
        const char *name = NULL;

        if (pos < 1 || pos > npar) {
            *str = String(NULL) ;
            return E_SQL_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        name = sqlite3_bind_parameter_name((sqlite3_stmt *) v->vm, pos);
        if (name) {
            *str = String(name);
        }
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::BindParameterIndex(
    /* [in] */ const String& name,
    /* [out] */ Int32* index)
{
#if HAVE_SQLITE3 && HAVE_SQLITE3_BIND_PARAMETER_INDEX
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
        Int32 pos = 0;
        const char *n = NULL;
        n = name.string();
        pos = sqlite3_bind_parameter_index((sqlite3_stmt *) v->vm, n);
        *index = pos;
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::ColumnInt(
    /* [in] */ Int32 col,
    /* [out] */ Int32* value)
{
#if HAVE_SQLITE3 && HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
        Int32 ncol = sqlite3_data_count((sqlite3_stmt *) v->vm);

        if (col < 0 || col >= ncol) {
            return E_SQL_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        *value = sqlite3_column_int((sqlite3_stmt *) v->vm, col);
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::ColumnLong(
    /* [in] */ Int32 col,
    /* [out] */ Int64* value)
{
#if HAVE_SQLITE3 && HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
        Int32 ncol = sqlite3_data_count((sqlite3_stmt *) v->vm);

        if (col < 0 || col >= ncol) {
            return E_SQL_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        *value = sqlite3_column_int64((sqlite3_stmt *) v->vm, col);
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::ColumnDouble(
    /* [in] */ Int32 col,
    /* [out] */ Double* value)
{
#if HAVE_SQLITE3 && HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
        Int32 ncol = sqlite3_data_count((sqlite3_stmt *) v->vm);

        if (col < 0 || col >= ncol) {
            return E_SQL_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        *value = sqlite3_column_double((sqlite3_stmt *) v->vm, col);

    } else {
        return E_SQL_SQLITE_THROWEX_EXCEPTION;
    }
#else
    return E_SQL_SQLITE_THROWEX_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::ColumnBytes(
    /* [in] */ Int32 col,
    /* [out,callee] */ ArrayOf<unsigned char>** array)
{
#if HAVE_SQLITE3 && HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
        Int32 ncol = sqlite3_data_count((sqlite3_stmt *) v->vm);
        Int32 nbytes = 0;
        const unsigned char *data = NULL;

        if (col < 0 || col >= ncol) {
            *array = NULL;
            return E_SQL_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        data = (const unsigned char *)sqlite3_column_blob((sqlite3_stmt *) v->vm, col);
        if (data) {
            nbytes = sqlite3_column_bytes((sqlite3_stmt *) v->vm, col);
        } else {
            *array = NULL;
            return E_SQL_EXCEPTION;
        }
        AutoPtr<ArrayOf<unsigned char> > outchar = ArrayOf<unsigned char>::Alloc(nbytes);
        outchar->Copy(data,nbytes);
        *array = outchar;
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::ColumnString(
    /* [in] */ Int32 col,
    /* [out] */ String* str)
{
#if HAVE_SQLITE3 && HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
        Int32 ncol = sqlite3_data_count((sqlite3_stmt *) v->vm);
        Int32 nbytes = 0;
        const char *data = NULL;

        if (col < 0 || col >= ncol) {
            return E_SQL_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        data = (const char *)sqlite3_column_text((sqlite3_stmt *) v->vm, col);
        if (data) {
            nbytes = sqlite3_column_bytes((sqlite3_stmt *) v->vm, col);
        } else {
            *str = String(NULL);
            return E_SQL_EXCEPTION;
        }
        *str = String((char *)data);
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::ColumnType(
    /* [in] */ Int32 col,
    /* [out] */ Int32* type)
{
#if HAVE_SQLITE3 && HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
        Int32 ncol = sqlite3_data_count((sqlite3_stmt *) v->vm);

        if (col < 0 || col >= ncol) {
            *type = 0;
            return E_SQL_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        *type = sqlite3_column_type((sqlite3_stmt *) v->vm, col);
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::ColumnCount(
    /* [out] */ Int32* count)
{
#if HAVE_SQLITE3 && HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
        *count = sqlite3_column_count((sqlite3_stmt *) v->vm);
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::Column(
    /* [in] */ Int32 col,
    /* [out] */ IInterface** obj)
{
    Int32  type = 0;
    ColumnType(col,&type);
    switch (type) {
        case SQLITE_INTEGER:
            {
                Int64 value = 0;
                ColumnLong(col,&value);
                *obj = (IInterface *)&value; // android-changed: performance
            }
            break;
        case SQLITE_FLOAT:
            {
                Double value = 0.0;
                ColumnDouble(col,&value);
                *obj = (IInterface *)&value;
            }
            break;
        case SQLITE_BLOB:
            {
                AutoPtr<ArrayOf<Byte> > value;
                ColumnBytes(col,(ArrayOf<unsigned char>**)&value);
                *obj = (IInterface *)&value;
            }
            break;
        case SQLITE3_TEXT:
            {
                String value;
                ColumnString(col,&value);
                *obj = (IInterface *)&value;
            }
            break;
    }
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CStmt::ColumnTableName(
    /* [in] */ Int32 col,
    /* [out] */ String* str)
{
#if HAVE_SQLITE3 && HAVE_SQLITE3_COLUMN_TABLE_NAME16
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
        Int32 ncol = sqlite3_column_count((sqlite3_stmt *) v->vm);
        const char *strchar = NULL;

        if (col < 0 || col >= ncol) {
            *str = String(NULL);
            return E_SQL_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        strchar = (const char *)sqlite3_column_database_name((sqlite3_stmt *) v->vm, col);
        if (strchar) {
            *str = String(strchar);
        }
    } else {
        return E_SQL_ILLEGAL_ARGUMENT_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::ColumnDatabaseName(
    /* [in] */ Int32 col,
    /* [out] */ String* str)
{
#if HAVE_SQLITE3 && HAVE_SQLITE3_COLUMN_DATABASE_NAME16
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
        Int32 ncol = sqlite3_column_count((sqlite3_stmt *) v->vm);
        const char *strchar;

        if (col < 0 || col >= ncol) {
            *str = String(NULL);
            return E_SQL_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        strchar = sqlite3_column_database_name((sqlite3_stmt *) v->vm, col);
        if (strchar) {
            *str = String(strchar);
        }
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::ColumnDecltype(
    /* [in] */ Int32 col,
    /* [out] */ String* str)
{
#if HAVE_SQLITE3 && HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
        Int32 ncol = sqlite3_column_count((sqlite3_stmt *) v->vm);
        const char *strchar;

        if (col < 0 || col >= ncol) {
            *str = String(NULL);
            return E_SQL_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        strchar = (const char *)sqlite3_column_decltype((sqlite3_stmt *) v->vm, col);
        if (strchar) {
            *str = String((char *)strchar);
        }
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::ColumnOriginName(
    /* [in] */ Int32 col,
    /* [out] */ String* str)
{
#if HAVE_SQLITE3 && HAVE_SQLITE3_COLUMN_ORIGIN_NAME16
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
        Int32 ncol = sqlite3_column_count((sqlite3_stmt *) v->vm);
        const char *strchar;

        if (col < 0 || col >= ncol) {
            *str = String(NULL);
            return E_SQL_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        strchar = sqlite3_column_origin_name((sqlite3_stmt *) v->vm, col);
        if (strchar) {
            *str = String(strchar));
        }
    } else {
        return E_SQL_NULL_POINTER_EXCEPTION;
    }
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CStmt::Status(
    /* [in] */ Int32 op,
    /* [in] */ Boolean flg,
    /* [out] */ Int32* value)
{
    Int32 count = 0;
#if HAVE_SQLITE3 && HAVE_SQLITE3_STMT_STATUS
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
    count = sqlite3_stmt_status((sqlite3_stmt *) v->vm, op,
                    flg == TRUE);
    }
#endif
    *value = count;
    return NOERROR;
}

ECode CStmt::Finalize()
{
#if HAVE_SQLITE3 && HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;

    if (v) {
    if (v->h) {
        handle *h = v->h;
        hvm *vv, **vvp;

        vvp = &h->vms;
        vv = *vvp;
        while (vv) {
        if (vv == v) {
            *vvp = vv->next;
            break;
        }
        vvp = &vv->next;
        vv = *vvp;
        }
    }
    if (v->vm) {
        sqlite3_finalize((sqlite3_stmt *) v->vm);
    }
    v->vm = 0;
    free(v);
    mHandle = 0;
    }
#endif
    return NOERROR;
}

} // namespace SQLite
} // namespace Sql
} // namespace Elastos
