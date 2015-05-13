
#ifndef __CSUBJECTPUBLICKEYINFOHELPER_H__
#define __CSUBJECTPUBLICKEYINFOHELPER_H__

#include "_CSubjectPublicKeyInfoHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CSubjectPublicKeyInfoHelper)
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

#endif // __CSUBJECTPUBLICKEYINFOHELPER_H__
