
#include "ZipOutputStream.h"
#include "CZipFile.h"
#include "CByteArrayOutputStream.h"
#include "CZipEntry.h"
#include "CCRC32.h"
#include "CDeflater.h"
#include "CSystem.h"
#include "Arrays.h"
#include <elastos/utility/etl/Algorithm.h>

using Elastos::Core::ISystem;
using Elastos::IO::CByteArrayOutputStream;

namespace Elastos {
namespace Utility {
namespace Zip {

const Int32 ZipOutputStream::ZIPLocalHeaderVersionNeeded;

CAR_INTERFACE_IMPL(ZipOutputStream, DeflaterOutputStream, IZipOutputStream)

ZipOutputStream::ZipOutputStream()
    : mCompressMethod(IDeflater::DEFLATED)
    , mCompressLevel(IDeflater::DEFAULT_COMPRESSION)
    , mOffset(0)
    , mCurOffset(0)
    , mNameLength(0)
{
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&mCDir);
    CCRC32::New((ICRC32**)&mCrc);
}

ZipOutputStream::~ZipOutputStream()
{
    mEntries.Clear();
}

/**
 * Constructs a new {@code ZipOutputStream} with the specified output
 * stream.
 *
 * @param p1
 *            the {@code OutputStream} to write the data to.
 */
ECode ZipOutputStream::constructor(
    /* [in] */ IOutputStream* p1)
{
    AutoPtr<CDeflater> deflater;
    CDeflater::NewByFriend(IDeflater::DEFAULT_COMPRESSION, TRUE, (CDeflater**)&deflater);
    return DeflaterOutputStream::constructor(p1, (IDeflater*)deflater.Get());
}

ECode ZipOutputStream::Close()
{
    // don't call super.close() because that calls finish() conditionally
    if (mOut != NULL) {
        FAIL_RETURN(Finish());
        FAIL_RETURN(mDef->End());
        FAIL_RETURN(mOut->Close());
        mOut = NULL;
    }
    return NOERROR;
}

ECode ZipOutputStream::CloseEntry()
{
    FAIL_RETURN(CheckClosed());
    if (mCurrentEntry == NULL) {
        return NOERROR;
    }

    CZipEntry* ze = (CZipEntry*)mCurrentEntry.Get();
    Int32 method;
    ze->GetMethod(&method);
    if (method == IZipOutputStream::DEFLATED) {
        FAIL_RETURN(DeflaterOutputStream::Finish());
    }

    // Verify values for STORED types
    if (method == IZipOutputStream::STORED) {
        Int64 value;
        IChecksum::Probe(mCrc)->GetValue(&value);
        if (value != ze->mCrc) {
//            throw new ZipException("CRC mismatch");
            return E_ZIP_EXCEPTION;
        }
        if (ze->mSize != ((CCRC32*)mCrc.Get())->mTbytes) {
//           throw new ZipException("Size mismatch");
            return E_ZIP_EXCEPTION;
        }
    }
    mCurOffset = IZipConstants::LOCHDR;

    // Write the DataDescriptor
    if (method != IZipOutputStream::STORED) {
        mCurOffset += IZipConstants::EXTHDR;
        WriteInt64(mOut, IZipConstants::EXTSIG);
        IChecksum::Probe(mCrc)->GetValue(&ze->mCrc);
        WriteInt64(mOut, ze->mCrc);
        Int32 ival;
        mDef->GetTotalOut(&ival);
        ze->mCompressedSize = ival;
        WriteInt64(mOut, ze->mCompressedSize);
        mDef->GetTotalIn(&ival);
        ze->mSize = ival;
        WriteInt64(mOut, ze->mSize);
    }
    // Update the CentralDirectory
    // http://www.pkware.com/documents/casestudies/APPNOTE.TXT
    Int32 flags = method == IZipOutputStream::STORED ? 0 : CZipFile::GPBF_DATA_DESCRIPTOR_FLAG;
    // Since gingerbread, we always set the UTF-8 flag on individual files.
    // Some tools insist that the central directory also have the UTF-8 flag.
    // http://code.google.com/p/android/issues/detail?id=20214
    flags |= CZipFile::GPBF_UTF8_FLAG;

    IOutputStream* os = IOutputStream::Probe(mCDir);
    WriteInt64(os, IZipConstants::CENSIG);
    WriteInt16(os, ZIPLocalHeaderVersionNeeded); // Version created
    WriteInt16(os, ZIPLocalHeaderVersionNeeded); // Version to extract
    WriteInt16(os, flags);
    WriteInt16(os, method);
    WriteInt16(os, ze->mTime);
    WriteInt16(os, ze->mModDate);
    Int64 checksum;
    IChecksum::Probe(mCrc)->GetValue(&checksum);
    WriteInt64(os, checksum);
    if (method == IZipOutputStream::DEFLATED) {
        Int32 value;
        mDef->GetTotalOut(&value);
        mCurOffset += WriteInt64(os, value);
        mDef->GetTotalIn(&value);
        WriteInt64(os, value);
    }
    else {
        mCurOffset += WriteInt64(os, ((CCRC32*)mCrc.Get())->mTbytes);
        WriteInt64(os, ((CCRC32*)mCrc.Get())->mTbytes);
    }
    mCurOffset += WriteInt16(os, mNameLength);
    if (ze->mExtra != NULL) {
        mCurOffset += WriteInt16(os, ze->mExtra->GetLength());
    }
    else {
        WriteInt16(os, 0);
    }
    String c;
    ze->GetComment(&c);
    if (!c.IsNull()) {
        WriteInt16(os, c.GetLength());
    }
    else {
        WriteInt16(os, 0);
    }
    WriteInt16(os, 0); // Disk Start
    WriteInt16(os, 0); // Internal File Attributes
    WriteInt64(os, 0); // External File Attributes
    WriteInt64(os, mOffset);
    os->Write(mNameBytes);
    mNameBytes = NULL;
    if (ze->mExtra != NULL) {
        os->Write(ze->mExtra);
    }
    mOffset +=mCurOffset;
    if (!c.IsNull()) {
        AutoPtr<ArrayOf<Byte> > bytes = c.GetBytes();
        os->Write(bytes, 0, bytes->GetLength());
    }
    mCurrentEntry = NULL;
    IChecksum::Probe(mCrc)->Reset();
    mDef->Reset();
    mDone = FALSE;
    return NOERROR;
}

ECode ZipOutputStream::Finish()
{
    // TODO: is there a bug here? why not checkClosed?
    if (mOut == NULL) {
//        throw new IOException("Stream is closed");
        return E_IO_EXCEPTION;
    }
    if (mCDir == NULL) {
        return NOERROR;
    }
    if (mEntries.IsEmpty()) {
//        throw new ZipException("No entries");
        return E_ZIP_EXCEPTION;
    }
    if (mCurrentEntry != NULL) {
        FAIL_RETURN(CloseEntry());
    }

    IOutputStream* os = IOutputStream::Probe(mCDir);
    Int32 cdirSize;
    mCDir->GetSize(&cdirSize);
    // Write Central Dir End
    WriteInt64(os, IZipConstants::ENDSIG);
    WriteInt16(os, 0); // Disk Number
    WriteInt16(os, 0); // Start Disk
    WriteInt16(os, mEntries.GetSize()); // Number of entries
    WriteInt16(os, mEntries.GetSize()); // Number of entries
    WriteInt64(os, cdirSize); // Size of central dir
    WriteInt64(os, mOffset); // Offset of central dir
    if (!mComment.IsNull()) {
        AutoPtr<ArrayOf<Byte> > bytes = mComment.GetBytes();
        WriteInt16(os, bytes->GetLength());
        os->Write(bytes, 0, bytes->GetLength());
    }
    else {
        WriteInt16(os, 0);
    }
    // Write the central dir
    AutoPtr<ArrayOf<Byte> > bytes;
    mCDir->ToByteArray((ArrayOf<Byte> **)&bytes);
    mOut->Write(bytes);
    mCDir = NULL;
    return NOERROR;
}

ECode ZipOutputStream::PutNextEntry(
    /* [in] */ IZipEntry* _ze)
{
    CZipEntry* ze = (CZipEntry*)_ze;
    if (mCurrentEntry != NULL) {
        FAIL_RETURN(CloseEntry());
    }
    Int32 method;
    ze->GetMethod(&method);
    if (method == IZipOutputStream::STORED ||
        (mCompressMethod == IZipOutputStream::STORED && method == -1)) {
        if (ze->mCrc == -1) {
//            throw new ZipException("CRC mismatch");
            return E_ZIP_EXCEPTION;
        }
        if (ze->mSize == -1 && ze->mCompressedSize == -1) {
//            throw new ZipException("Size mismatch");
            return E_ZIP_EXCEPTION;
        }
        if (ze->mSize != ze->mCompressedSize && ze->mCompressedSize != -1 && ze->mSize != -1) {
//            throw new ZipException("Size mismatch");
            return E_ZIP_EXCEPTION;
        }
    }
    FAIL_RETURN(CheckClosed());
    HashSet<String>::Iterator it = Find(mEntries.Begin(), mEntries.End(), ze->mName);
    if (it != mEntries.End()) {
//        throw new ZipException("Entry already exists: " + ze.name);
        return E_ZIP_EXCEPTION;
    }
    if (mEntries.GetSize() == 64*1024-1) {
        // TODO: support Zip64.
        return E_ZIP_EXCEPTION;
//        throw new ZipException("Too many entries for the zip file format's 16-bit entry count");
    }
    mNameLength = GetUtf8Count(ze->mName);
    if (mNameLength > 0xffff) {
//        throw new IllegalArgumentException("Name too long: " + nameLength + " UTF-8 bytes");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mDef->SetLevel(mCompressLevel);
    mCurrentEntry = _ze;
    mEntries.Insert(ze->mName);
    mCurrentEntry->GetMethod(&method);
    if (method == -1) {
        mCurrentEntry->SetMethod(mCompressMethod);
    }
    // Local file header.
    // http://www.pkware.com/documents/casestudies/APPNOTE.TXT
    Int32 flags = method == IZipOutputStream::STORED ? 0 : CZipFile::GPBF_DATA_DESCRIPTOR_FLAG;
    // Java always outputs UTF-8 filenames. (Before Java 7, the RI didn't set this flag and used
    // modified UTF-8. From Java 7, it sets this flag and uses normal UTF-8.)
    flags |= CZipFile::GPBF_UTF8_FLAG;
    IOutputStream* os = IOutputStream::Probe(mCDir);
    WriteInt64(os, IZipConstants::LOCSIG); // Entry header
    WriteInt16(os, ZIPLocalHeaderVersionNeeded); // Extraction version
    WriteInt16(os, flags);
    WriteInt16(os, method);
    Int64 time;
    mCurrentEntry->GetTime(&time);
    if (time == -1) {
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 millis;
        system->GetCurrentTimeMillis(&millis);
        mCurrentEntry->SetTime(millis);
    }
    WriteInt16(os, ze->mTime);
    WriteInt16(os, ze->mModDate);

    if (method == IZipOutputStream::STORED) {
        if (ze->mSize == -1) {
            ze->mSize = ze->mCompressedSize;
        }
        else if (ze->mCompressedSize == -1) {
            ze->mCompressedSize = ze->mSize;
        }
        WriteInt64(os, ze->mCrc);
        WriteInt64(os, ze->mSize);
        WriteInt64(os, ze->mSize);
    }
    else {
        WriteInt64(os, 0);
        WriteInt64(os, 0);
        WriteInt64(os, 0);
    }
    WriteInt16(mOut, mNameLength);
    if (ze->mExtra != NULL) {
        WriteInt16(os, ze->mExtra->GetLength());
    }
    else {
        WriteInt16(os, 0);
    }
    mNameBytes = ToUTF8Bytes(ze->mName, mNameLength);
    os->Write(mNameBytes);
    if (ze->mExtra != NULL) {
        os->Write(ze->mExtra);
    }
    return NOERROR;
}

ECode ZipOutputStream::SetComment(
    /* [in] */ const String& comment)
{
    if (!comment.IsNull() && comment.GetLength() > 0xFFFF) {
    //        throw new IllegalArgumentException("Comment too long: " + comment.length() + " characters");
         return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mComment = comment;
    return NOERROR;
}

ECode ZipOutputStream::SetLevel(
    /* [in] */ Int32 level)
{
    if (level < IDeflater::DEFAULT_COMPRESSION
            || level > IDeflater::BEST_COMPRESSION) {
//        throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mCompressLevel = level;
    return NOERROR;
}

ECode ZipOutputStream::SetMethod(
    /* [in] */ Int32 method)
{
    if (method != IZipOutputStream::STORED && method != IZipOutputStream::DEFLATED) {
//        throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mCompressMethod = method;
    return NOERROR;
}

Int64 ZipOutputStream::WriteInt64(
    /* [in] */ IOutputStream* os,
    /* [in] */ Int64 i)
{
    // Write out the long value as an unsigned int
    os->Write((Int32)(i & 0xFF));
    os->Write((Int32)(i >> 8) & 0xFF);
    os->Write((Int32)(i >> 16) & 0xFF);
    os->Write((Int32)(i >> 24) & 0xFF);
    return i;
}

Int32 ZipOutputStream::WriteInt16(
    /* [in] */ IOutputStream* os,
    /* [in] */ Int32 i)
{
    os->Write(i & 0xFF);
    os->Write((i >> 8) & 0xFF);
    return i;

}

ECode ZipOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount)
{
    VALIDATE_NOT_NULL(buffer)
    FAIL_RETURN(Arrays::CheckOffsetAndCount(buffer->GetLength(), offset, byteCount))

    if (mCurrentEntry == NULL) {
//        throw new ZipException("No active entry");
        return E_ZIP_EXCEPTION;
    }

    Int32 method;
    mCurrentEntry->GetMethod(&method);
    if (method == IZipOutputStream::STORED) {
        FAIL_RETURN(mOut->Write(buffer, offset, byteCount));
    }
    else {
        FAIL_RETURN(DeflaterOutputStream::Write(buffer, offset, byteCount));
    }
    return IChecksum::Probe(mCrc)->Update(buffer, offset, byteCount);
}

Int32 ZipOutputStream::GetUtf8Count(
    /* [in] */ const String& value)
{
    return value.GetLength();
}

AutoPtr<ArrayOf<Byte> > ZipOutputStream::ToUTF8Bytes(
    /* [in] */ const String& value,
    /* [in] */ Int32 length)
{
    Int32 byteLength = value.ToByteIndex(length);
    assert(value.GetByteLength() >= byteLength);
    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(length);
    memcpy(result->GetPayload(), value.string(), byteLength);
    return result;
}

ECode ZipOutputStream::CheckClosed()
{
    if (mCDir == NULL) {
//        throw new IOException("Stream is closed");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}


} // namespace Zip
} // namespace Utility
} // namespace Elastos
