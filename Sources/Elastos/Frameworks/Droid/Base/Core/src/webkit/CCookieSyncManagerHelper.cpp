
#include "ext/frameworkext.h"
#include "webkit/CookieSyncManager.h"
#include "webkit/CCookieSyncManagerHelper.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

ECode CCookieSyncManagerHelper::GetInstance(
    /* [out] */ ICookieSyncManager** instance)
{
    Mutex::Autolock lock(_m_syncLock);
    VALIDATE_NOT_NULL(instance);
    CheckInstanceIsCreated();
    *instance = sRef;
    INTERFACE_ADDREF(*instance);
    return NOERROR;
}

ECode CCookieSyncManagerHelper::CreateInstance(
    /* [in] */ IContext* context,
    /* [out] */ ICookieSyncManager** instance)
{
    Mutex::Autolock lock(_m_syncLock);
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(instance);

    if (sRef == NULL) {
        sRef = new CookieSyncManager(context);
    }
    *instance = sRef;
    INTERFACE_ADDREF(*instance);

    return NOERROR;
}

ECode CCookieSyncManagerHelper::CheckInstanceIsCreated()
{
    if (sRef == NULL) {
        return E_NOT_IMPLEMENTED;
    }
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos