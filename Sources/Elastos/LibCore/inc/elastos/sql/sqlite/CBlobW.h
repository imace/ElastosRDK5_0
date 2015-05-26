
#ifndef __CBLOBW_H__
#define __CBLOBW_H__

#include "_Elastos_Sql_SQLite_CBlobW.h"
#include "CBlob.h"
#include <OutputStream.h>

using Elastos::IO::OutputStream;

namespace Elastos {
namespace Sql {
namespace SQLite {

CarClass(CBlobW)
    , public OutputStream
    , public IBlobW
{
public:
    CAR_OBJECT_DECL();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CBlobW();

    CARAPI Write(
        /* [in] */ Int32 oneByte);

    CARAPI WriteBytes(
        /* [in] */ const ArrayOf<Byte>& buffer);

    CARAPI WriteBytes(
        /* [in] */ const ArrayOf<Byte>& b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len);

    CARAPI Close();

    CARAPI constructor(
        /* [in] */ Elastos::Sql::SQLite::IBlob* blob);

private:
    AutoPtr<CBlob> mBlob;
    Int32 mPos;
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __CBLOBW_H__
