
#include "CSerializableEntity.h"
#include <elastos/Logger.h>

using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::CObjectOutputStream;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

CAR_OBJECT_IMPL(CSerializableEntity)

ECode CSerializableEntity::CreateBytes(
    /* [in] */ ISerializable* ser)
{
    AutoPtr<IByteArrayOutputStream> baos;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&baos);
    AutoPtr<IObjectOutputStream> out;
    CObjectOutputStream::New(IOutputStream::Probe(baos), (IObjectOutputStream**)&out);
    out->WriteObject(IObject::Probe(ser));
    IFlush::Probe(out)->Flush();
    return baos->ToByteArray((ArrayOf<Byte>**)&mObjSer);
}

ECode CSerializableEntity::GetContent(
    /* [out] */ IInputStream** inputStream)
{
    VALIDATE_NOT_NULL(inputStream)
    if (mObjSer == NULL) {
        CreateBytes(mObjRef);
    }
    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(mObjSer, (IByteArrayInputStream**)&stream);
    *inputStream = IInputStream::Probe(stream);
    REFCOUNT_ADD(*inputStream)
    return NOERROR;
}

ECode CSerializableEntity::GetContentLength(
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length)
    if (mObjSer ==  NULL) {
        *length = -1;
    }
    else {
        *length = mObjSer->GetLength();
    }
    return NOERROR;
}

ECode CSerializableEntity::IsRepeatable(
    /* [out] */ Boolean* isRepeatable)
{
    VALIDATE_NOT_NULL(isRepeatable)
    *isRepeatable = TRUE;
    return NOERROR;
}

ECode CSerializableEntity::IsStreaming(
    /* [out] */ Boolean* isStreaming)
{
    VALIDATE_NOT_NULL(isStreaming)
    *isStreaming = mObjSer == NULL;
    return NOERROR;
}

ECode CSerializableEntity::WriteTo(
    /* [in] */ IOutputStream* outstream)
{
    if (outstream == NULL) {
        Logger::E("CSerializableEntity", "Output stream may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mObjSer == NULL) {
        AutoPtr<IObjectOutputStream> out;
        CObjectOutputStream::New(outstream, (IObjectOutputStream**)&out);
        out->WriteObject(IObject::Probe(mObjRef));
        IFlush::Probe(out)->Flush();
    }
    else {
        outstream->Write(mObjSer);
        IFlush::Probe(outstream)->Flush();
    }
    return NOERROR;
}

ECode CSerializableEntity::constructor(
    /* [in] */ ISerializable* ser,
    /* [in] */ Boolean bufferize)
{
    if (ser == NULL) {
        Logger::E("CSerializableEntity", "Source object may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (bufferize) {
        return CreateBytes(ser);
    }
    else {
        mObjRef = ser;
    }
    return NOERROR;
}

} // namespace Cookie
} // namespace Http
} // namespace Apache
} // namespace Org