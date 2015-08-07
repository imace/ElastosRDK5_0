
#include "SocketInputBuffer.h"
#include <elastos/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

SocketInputBuffer::SocketInputBuffer(
    /* [in] */ ISocket* socket,
    /* [in] */ Int32 buffersize,
    /* [in] */ IHttpParams* params)
    : AbstractSessionInputBuffer()
{
    if (socket == NULL) {
        Logger::E("SocketInputBuffer", "Socket may not be null");
        assert(0);
        // throw new IllegalArgumentException("Socket may not be null");
    }
    mSocket = socket;
    // BEGIN android-changed
    // Workaround for http://b/3514259. We take 'buffersize' as a hint in
    // the weakest sense, and always use an 8KiB heap buffer and leave the
    // kernel buffer size alone, trusting the system to have set a
    // network-appropriate default.
    AutoPtr<IInputStream> in;
    socket->GetInputStream((IInputStream**)&in);
    Init(in, 8192, params);
    // END android-changed
}

ECode SocketInputBuffer::IsDataAvailable(
    /* [in] */ Int32 timeout,
    /* [out] */ Boolean* isAvailable)
{
    VALIDATE_NOT_NULL(isAvailable)
    Boolean result = HasBufferedData();
    if (!result) {
        Int32 oldtimeout;
        mSocket->GetSoTimeout(&oldtimeout);
        // try {
        mSocket->SetSoTimeout(timeout);
        FillBuffer();
        result = HasBufferedData();
        // } catch (InterruptedIOException e) {
        //     if (!(e instanceof SocketTimeoutException)) {
        //         throw e;
        //     }
        // } finally {
        //     socket.setSoTimeout(oldtimeout);
        // }
        socket->SetSoTimeout(oldtimeout);
    }
    *isAvailable = result;
    return NOERROR;
}

ECode SocketInputBuffer::IsStale(
    /* [out] */ Boolean* isStale)
{
    VALIDATE_NOT_NULL(isStale)
    if (HasBufferedData()) {
        *isStale = FALSE;
        return NOERROR;
    }
    Int32 oldtimeout;
    mSocket->GetSoTimeout(&oldtimeout);
    // try {
    mSocket->SetSoTimeout(1);
    *isStale = FillBuffer() == -1;
    // } catch (SocketTimeoutException e) {
    //     return false; // the connection is not stale; hooray
    // } catch (IOException e) {
    //     return true; // the connection is stale, the read or soTimeout failed.
    // } finally {
    //     this.socket.setSoTimeout(oldTimeout);
    // }
    socket->SetSoTimeout(oldtimeout);
    return NOERROR;
}

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org