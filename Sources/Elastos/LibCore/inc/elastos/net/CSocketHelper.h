
#ifndef __ELASTOS_NET_CSOCKETHELPER_H__
#define __ELASTOS_NET_CSOCKETHELPER_H__

#include "_ELASTOS_NET_CSocketHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Net {

CarClass(CSocketHelper)
    , public Singleton
    , public ISocketHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI SetSocketImplFactory(
        /* [in] */ ISocketImplFactory* fac);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CSOCKETHELPER_H__
