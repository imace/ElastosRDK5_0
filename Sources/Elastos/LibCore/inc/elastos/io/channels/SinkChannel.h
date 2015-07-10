#ifndef __ELASTOS_IO_CHANNELS_SINKCHANNEL_H__
#define __ELASTOS_IO_CHANNELS_SINKCHANNEL_H__

#include "AbstractSelectableChannel.h"

using Elastos::IO::Channels::Spi::ISelectorProvider;
using Elastos::IO::Channels::Spi::AbstractSelectableChannel;

namespace Elastos {
namespace IO {
namespace Channels {

class SinkChannel
    : public AbstractSelectableChannel
    , public IGatheringByteChannel
{
public:
    CAR_INTERFACE_DECL()

    CARAPI ValidOps(
        /* [out] */ Int32* ret);

protected:
    SinkChannel(
        /* [in] */ ISelectorProvider* provider);
};

} // namespace Channels
} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CHANNELS_SINKCHANNEL_H__
