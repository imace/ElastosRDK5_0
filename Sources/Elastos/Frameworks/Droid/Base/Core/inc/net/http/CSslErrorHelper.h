
#ifndef __CSSLERRORHELPER_H__
#define __CSSLERRORHELPER_H__

#include "_CSslErrorHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CarClass(CSslErrorHelper)
{
public:
    /**
     * Creates an SslError object from a chromium error code.
     * @param error The chromium error code
     * @param certificate The associated SSL certificate
     * @param url The associated URL.
     * @hide  chromium error codes only available inside the framework
     */
    CARAPI SslErrorFromChromiumErrorCode(
        /* [in] */ Int32 error,
        /* [in] */ ISslCertificate* cert,
        /* [in] */ const String& url,
        /* [out] */ ISslError** sslError);

};

}
}
}
}

#endif // __CSSLERRORHELPER_H__
