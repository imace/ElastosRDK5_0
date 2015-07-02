#ifndef __ELASTOS_IO_DIRECTBYTEBUFFER_H__
#define __ELASTOS_IO_DIRECTBYTEBUFFER_H__

#include "MappedByteBuffer.h"
#include "MemoryBlock.h"

namespace Elastos {
namespace IO {

extern "C" const InterfaceID EIID_DirectByteBuffer;

/**
 * DirectByteBuffer, ReadWriteDirectByteBuffer and ReadOnlyDirectByteBuffer
 * compose the implementation of platform memory based byte buffers.
 * <p>
 * DirectByteBuffer implements all the shared readonly methods and is extended
 * by the other two classes.
 * </p>
 * <p>
 * All methods are marked final for runtime performance.
 * </p>
 *
 */
class DirectByteBuffer
    : public MappedByteBuffer
{
    friend class ReadWriteDirectByteBuffer;
    friend class ReadOnlyDirectByteBuffer;

public:
    CARAPI Get(
        /* [out] */ ArrayOf<Byte>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 byteCount);

    CARAPI GetChars(
        /* [out] */ ArrayOf<Char32>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 charCount);

    CARAPI GetDoubles(
        /* [out] */ ArrayOf<Double>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 doubleCount);

    CARAPI GetFloats(
        /* [out] */ ArrayOf<Float>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 floatCount);

    CARAPI GetInt32s(
        /* [out] */ ArrayOf<Int32>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 int32Count);

    CARAPI GetInt64s(
        /* [out] */ ArrayOf<Int64>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 int64Count);

    CARAPI GetInt16s(
        /* [out] */ ArrayOf<Int16>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 int16Count);

    CARAPI Get(
        /* [out] */ Byte* value);

    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Byte* value);

    CARAPI GetChar(
        /* [out] */ Char32* value);

    CARAPI GetChar(
        /* [in] */ Int32 index,
        /* [out] */ Char32* value);

    CARAPI GetDouble(
        /* [out] */ Double* value);

    CARAPI GetDouble(
        /* [in] */ Int32 index,
        /* [out] */ Double* value);

    CARAPI GetFloat(
        /* [out] */ Float* value);

    CARAPI GetFloat(
        /* [in] */ Int32 index,
        /* [out] */ Float* value);

    CARAPI GetInt32(
        /* [out] */ Int32* value);

    CARAPI GetInt32(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    CARAPI GetInt64(
        /* [out] */ Int64* value);

    CARAPI GetInt64(
        /* [in] */ Int32 index,
        /* [out] */ Int64* value);

    CARAPI GetInt16(
        /* [out] */ Int16* value);

    CARAPI GetInt16(
        /* [in] */ Int32 index,
        /* [out] */ Int16* value);

    CARAPI IsDirect(
        /* [out] */ Boolean* isDirect);

    CARAPI Free();

    CARAPI ProtectedArray(
        /* [out, callee] */ ArrayOf<Byte>** array);

    CARAPI ProtectedArrayOffset(
        /* [out] */ Int32* offset);

    CARAPI ProtectedHasArray(
        /* [out] */ Boolean* hasArray);

protected:
    DirectByteBuffer(
        /* [in] */ Int64 address,
        /* [in] */ Int32 capacity);

    DirectByteBuffer(
        /* [in] */ MemoryBlock* block,
        /* [in] */ Int32 capacity,
        /* [in] */ Int32 offset,
        /* [in] */ Boolean isReadOnly,
        /* [in] */ FileChannelMapMode mapMode);

protected:
    // This is the offset into {@code Buffer.block} at which this buffer logically starts.
    // TODO: rewrite this so we set 'block' to an OffsetMemoryBlock?
    Int32 mOffset;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_DIRECTBYTEBUFFER_H__
