
#ifndef __CORADDRESSHELPER_H__
#define __CORADDRESSHELPER_H__

#include "_CORAddressHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CORAddressHelper)
{
public:
    CARAPI GetASN1(
        /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Sequence ** ppAsn1);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __CORADDRESSHELPER_H__
