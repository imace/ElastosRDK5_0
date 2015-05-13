#ifndef __READONLYDIRECTBYTEBUFFER_H__
#define __READONLYDIRECTBYTEBUFFER_H__

#include "DirectByteBuffer.h"
#include "MemoryBlock.h"

namespace Elastos {
namespace IO {

/**
 * DirectByteBuffer, ReadWriteDirectByteBuffer and ReadOnlyDirectByteBuffer
 * compose the implementation of platform memory based byte buffers.
 * <p>
 * ReadOnlyDirectByteBuffer extends DirectByteBuffer with all the write methods
 * throwing read only exception.
 * </p>
 * <p>
 * This class is marked final for runtime performance.
 * </p>
 *
 */
class ReadOnlyDirectByteBuffer
    : public IByteBuffer
    , public DirectByteBuffer
{
public:
    ReadOnlyDirectByteBuffer(
        /* [in] */ MemoryBlock* block,
        /* [in] */ Int32 capacity,
        /* [in] */ Int32 offset);

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    static CARAPI_(AutoPtr<ReadOnlyDirectByteBuffer>) Copy(
        /* [in] */ DirectByteBuffer* other,
        /* [in] */ Int32 markOfOther);

    CARAPI GetPrimitiveArray(
        /* [out] */ Handle32* arrayHandle);

    CARAPI GetArray(
        /* [out, callee] */ ArrayOf<Byte>** array);

    CARAPI GetElementSizeShift(
        /* [out] */ Int32* elementSizeShift);

    CARAPI GetEffectiveDirectAddress(
        /* [out] */ Int32* effectiveDirectAddress);

    CARAPI GetArrayOffset(
        /* [out] */ Int32* offset);

    CARAPI AsCharBuffer(
        /* [out] */ ICharBuffer** buffer);

    CARAPI AsDoubleBuffer(
        /* [out] */ IDoubleBuffer** buffer);

    CARAPI AsFloatBuffer(
        /* [out] */ IFloatBuffer** buffer);

    CARAPI AsInt16Buffer(
        /* [out] */ IInt16Buffer** buffer);

    CARAPI AsInt32Buffer(
        /* [out] */ IInt32Buffer** buffer);

    CARAPI AsInt64Buffer(
        /* [out] */ IInt64Buffer** buffer);

    CARAPI AsReadOnlyBuffer(
        /* [out] */ IByteBuffer** buffer);

    CARAPI Compact();

    CARAPI CompareTo(
        /* [in] */ IByteBuffer* otherBuffer,
        /* [out] */ Int32* result);

    CARAPI Duplicate(
        /* [out] */ IByteBuffer** buffer);

    CARAPI GetByte(
        /* [out] */ Byte* value);

    CARAPI GetByteEx(
        /* [in] */ Int32 index,
        /* [out] */ Byte* value);

    CARAPI GetBytes(
        /* [out] */ ArrayOf<Byte>* dst);

    CARAPI GetBytesEx(
        /* [out] */ ArrayOf<Byte>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 byteCount);

    CARAPI GetChar(
        /* [out] */ Char32* value);

    CARAPI GetCharEx(
        /* [in] */ Int32 index,
        /* [out] */ Char32* value);

    CARAPI GetDouble(
        /* [out] */ Double* value);

    CARAPI GetDoubleEx(
        /* [in] */ Int32 index,
        /* [out] */ Double* value);

    CARAPI GetFloat(
        /* [out] */ Float* value);

    CARAPI GetFloatEx(
        /* [in] */ Int32 index,
        /* [out] */ Float* value);

    CARAPI GetInt32(
        /* [out] */ Int32* value);

    CARAPI GetInt32Ex(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    CARAPI GetInt64(
        /* [out] */ Int64* value);

    CARAPI GetInt64Ex(
        /* [in] */ Int32 index,
        /* [out] */ Int64* value);

    CARAPI GetInt16(
        /* [out] */ Int16* value);

    CARAPI GetInt16Ex(
        /* [in] */ Int32 index,
        /* [out] */ Int16* value);

    CARAPI GetOrder(
       /* [out] */ ByteOrder* byteOrder);

    CARAPI SetOrder(
        /* [in] */ ByteOrder byteOrder);

    CARAPI ProtectedArray(
        /* [out, callee] */ ArrayOf<Byte>** array);

    CARAPI ProtectedArrayOffset(
        /* [out] */ Int32* offset);

    CARAPI ProtectedHasArray(
        /* [out] */ Boolean* hasArray);

    CARAPI PutByte(
        /* [in] */ Byte b);

    CARAPI PutByteEx(
        /* [in] */ Int32 index,
        /* [in] */ Byte b);

    CARAPI PutBytes(
        /* [in] */ const ArrayOf<Byte>& src);

    CARAPI PutBytesEx(
        /* [in] */ const ArrayOf<Byte>& src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 byteCount);

    CARAPI PutByteBuffer(
        /* [in] */ IByteBuffer* src);

    CARAPI PutChar(
        /* [in] */ Char32 value);

    CARAPI PutCharEx(
        /* [in] */ Int32 index,
        /* [in] */ Char32 value);

    CARAPI PutDouble(
        /* [in] */ Double value);

    CARAPI PutDoubleEx(
        /* [in] */ Int32 index,
        /* [in] */ Double value);

    CARAPI PutFloat(
        /* [in] */ Float value);

    CARAPI PutFloatEx(
        /* [in] */ Int32 index,
        /* [in] */ Float value);

    CARAPI PutInt16(
        /* [in] */ Int16 value);

    CARAPI PutInt16Ex(
        /* [in] */ Int32 index,
        /* [in] */ Int16 value);

    CARAPI PutInt32(
        /* [in] */ Int32 value);

    CARAPI PutInt32Ex(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    CARAPI PutInt64(
        /* [in] */ Int64 value);

    CARAPI PutInt64Ex(
        /* [in] */ Int32 index,
        /* [in] */ Int64 value);

    CARAPI Slice(
       /* [out] */ IByteBuffer** buffer);

    CARAPI GetCapacity(
        /* [out] */ Int32* cap);

    CARAPI Clear();

    CARAPI Flip();

    CARAPI HasArray(
        /* [out] */ Boolean* hasArray);

    CARAPI HasRemaining(
        /* [out] */ Boolean* hasRemaining);

    CARAPI IsDirect(
        /* [out] */ Boolean* isDirect);

    CARAPI IsReadOnly(
        /* [out] */ Boolean* isReadOnly);

    CARAPI GetLimit(
        /* [out] */ Int32* limit);

    CARAPI SetLimit(
        /* [in] */ Int32 newLimit);

    CARAPI Mark();

    CARAPI GetPosition(
        /* [out] */ Int32* pos);

    CARAPI SetPosition(
        /* [in] */ Int32 newPosition);

    CARAPI GetRemaining(
        /* [out] */ Int32* remaining);

    CARAPI Reset();

    CARAPI Rewind();

    virtual CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* isEquals);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    AutoPtr<ArrayOf<Byte> > mArrayTemp;
};

} // namespace IO
} // namespace Elastos

#endif // __READONLYDIRECTBYTEBUFFER_H__
