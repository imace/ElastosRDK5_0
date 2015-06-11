
#include "FileOutputStream.h"
#include "CFile.h"
// #include "IoUtils.h"
#include "OsConstants.h"
#include "CLibcore.h"
#include "CIoBridge.h"
#include "CFileDescriptor.h"

using Elastos::Droid::System::OsConstants;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IOs;
using Libcore::IO::IIoBridge;
using Libcore::IO::CIoBridge;


namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(FileOutputStream, OutputStream, IFileOutputStream)

FileOutputStream::FileOutputStream()
{}

FileOutputStream::~FileOutputStream()
{
    // try {
    CloseInner();
    // } finally {
    //     try {
    //         super.finalize();
    //     } catch (Throwable t) {
    //         // for consistency with the RI, we must override Object.finalize() to
    //         // remove the 'throws Throwable' clause.
    //         throw new AssertionError(t);
    //     }
    // }
}

ECode FileOutputStream::constructor(
    /* [in] */ IFile* file)
{
    return constructor(file, FALSE);
}

ECode FileOutputStream::constructor(
    /* [in] */ IFile* file,
    /* [in] */ Boolean append)
{
    if (file == NULL) {
        // throw new NullPointerException("file == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    // AutoPtr<COsConstants> osConstans;
    // COsConstants::AcquireSingletonByFriend((COsConstants**)&osConstans);
    // Int32 m1, m2, m3, m4;
    // osConstans->GetOsConstant(String("O_WRONLY"), &m1);
    // osConstans->GetOsConstant(String("O_CREAT"), &m2);
    // osConstans->GetOsConstant(String("O_APPEND"), &m3);
    // osConstans->GetOsConstant(String("O_TRUNC"), &m4);
    // mMode = m1 | m2 | (append ? m3 : m4);
    // String path;
    // file->GetPath(&path);
    // CFileDescriptor::NewByFriend((CFileDescriptor**)&mFd);
    // AutoPtr<CIoBridge> ioBridge;
    // CIoBridge::AcquireSingletonByFriend((CIoBridge**)&ioBridge);
    // Int32 fd;
    // // FAIL_RETURN(IoUtils::Libcore2IoECode(ioBridge->Open(path, mMode, &fd)));
    // mFd->SetDescriptor(fd);
    // mShouldClose = TRUE;
    return NOERROR;
}

ECode FileOutputStream::constructor(
    /* [in] */ IFileDescriptor* fd)
{
    if (fd == NULL) {
//        throw new NullPointerException("fd == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mFd = (CFileDescriptor*)fd;
    mShouldClose = FALSE;
    // AutoPtr<COsConstants> osConstans;
    // COsConstants::AcquireSingletonByFriend((COsConstants**)&osConstans);
    // osConstans->GetOsConstant(String("O_WRONLY"), &mMode);
    // this.channel = NioUtils.newFileChannel(this, fd, mode);
    return NOERROR;
}

ECode FileOutputStream::constructor(
    /* [in] */ const String& fileName)
{
    return constructor(fileName, FALSE);
}

ECode FileOutputStream::constructor(
    /* [in] */ const String& fileName,
    /* [in] */ Boolean append)
{
    AutoPtr<CFile> file;
    CFile::NewByFriend(fileName, (CFile**)&file);
    return constructor((IFile*)file.Get(), append);
}

ECode FileOutputStream::Close()
{
    return CloseInner();
}

ECode FileOutputStream::GetChannel(
    /* [out] */ IFileChannel** channel)
{
    VALIDATE_NOT_NULL(channel)

    Object::Autolock lock(this);

    // if (channel == null) {
    //     channel = NioUtils.newFileChannel(this, fd, mode);
    // }
    *channel = mChannel;
    REFCOUNT_ADD(*channel);
    return NOERROR;
}

ECode FileOutputStream::GetFD(
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd)

    *fd = mFd;
    REFCOUNT_ADD(*fd);
    return NOERROR;
}

ECode FileOutputStream::Write(
    /* [in] */ Int32 oneByte)
{
    ArrayOf_<Byte, 1> bytes;
    (bytes)[0] = (Byte)oneByte;
    return Write(bytes, 0, 1);
}

ECode FileOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount)
{
    Int32 fd;
    mFd->GetDescriptor(&fd);
    AutoPtr<IIoBridge> ioBridge;
    CIoBridge::AcquireSingleton((IIoBridge**)&ioBridge);
    // return IoUtils::Libcore2IoECode(ioBridge->Write(fd,
    //         buffer, byteOffset, byteCount));
}

ECode FileOutputStream::CloseInner()
{
    Object::Autolock lock(this);

    if (mShouldClose) {
        AutoPtr<IIoBridge> ioBridge;
        CIoBridge::AcquireSingleton((IIoBridge**)&ioBridge);
        return ioBridge->CloseAndSignalBlockedThreads(mFd);
    }
    else {
        // An owned fd has been invalidated by IoUtils.close, but
        // we need to explicitly stop using an unowned fd (http://b/4361076).
        mFd = NULL;
        return CFileDescriptor::NewByFriend((CFileDescriptor**)&mFd);
    }
}

} // namespace IO
} // namespace Elastos
