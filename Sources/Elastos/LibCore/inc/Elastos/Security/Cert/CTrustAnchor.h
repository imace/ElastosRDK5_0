
#ifndef __CTRUSTANCHOR_H__
#define __CTRUSTANCHOR_H__

#include "_CTrustAnchor.h"

using Elastosx::Security::Auth::X500::IX500Principal;
using Elastos::Security::IPublicKey;

namespace Elastos {
namespace Security {
namespace Cert {

CarClass(CTrustAnchor)
{
public:
    CARAPI GetNameConstraints(
        /* [out, callee] */ ArrayOf<Byte> **nameConstraints);

    CARAPI GetTrustedCert(
        /* [out] */ IX509Certificate **cert);

    CARAPI GetCA(
        /* [out] */ IX500Principal **ca);

    CARAPI GetCAName(
        /* [out] */ String *caName);

    CARAPI GetCAPublicKey(
        /* [out] */ IPublicKey **key);

    CARAPI ToString(
        /* [out] */ String *str);

    CARAPI constructor(
        /* [in] */ IX509Certificate *trustedCert,
        /* [in] */ ArrayOf<Byte> *nameConstraints);

    CARAPI constructor(
        /* [in] */ const String& caName,
        /* [in] */ IPublicKey *caPublicKey,
        /* [in] */ ArrayOf<Byte> *nameConstraints);

    CARAPI constructor(
        /* [in] */ IX500Principal *caPrincipal,
        /* [in] */ IPublicKey *caPublicKey,
        /* [in] */ ArrayOf<Byte> *nameConstraints);

private:
    // Decodes and checks NameConstraints structure.
    // Throws IllegalArgumentException if NameConstraints
    // encoding is invalid.
    CARAPI ProcessNameConstraints();

private:
    // Most trusted CA as a X500Principal
    AutoPtr<IX500Principal> mCaPrincipal;
    // Most trusted CA name
    String mCaName;
    // Most trusted CA public key
    AutoPtr<IPublicKey> mCaPublicKey;
    // Most trusted CA certificate
    AutoPtr<IX509Certificate> mTrustedCert;
    // Name constraints extension
    AutoPtr<ArrayOf<Byte> > mNameConstraints;
};

}
}
}

#endif // __CTRUSTANCHOR_H__
