
#include "CSystem.h"
#include <stdlib.h>
#include "Elastos.Core_server.h"
#include <elastos/StringBuilder.h>
#include "CStringWrapper.h"
#include "CHashMap.h"
#include "CProperties.h"
#include "CICUHelper.h"
#include "CSelectorProviderHelper.h"
#include "CFileDescriptor.h"
#include "CFileInputStream.h"
#include "CFileOutputStream.h"
#include "CBufferedInputStream.h"
#include "CPrintStream.h"

#include <cutils/log.h>
#include <unistd.h>
#include <sys/utsname.h>
#include "openssl/opensslv.h"
#include "zlib.h"

using Elastos::Core::CStringWrapper;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CProperties;
using Elastos::IO::CFileDescriptor;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::CPrintStream;
using Elastos::IO::EIID_IInputStream;
using Elastos::IO::EIID_IOutputStream;
using Elastos::IO::EIID_IPrintStream;
using Elastos::IO::Channels::Spi::ISelectorProvider;
using Elastos::IO::Channels::Spi::ISelectorProviderHelper;
using Elastos::IO::Channels::Spi::CSelectorProviderHelper;
using Libcore::ICU::CICUHelper;
using Libcore::ICU::IICUHelper;

namespace Elastos {
namespace Core {

String CSystem::sLineSeparator;
AutoPtr<IProperties> CSystem::sSystemProperties;
AutoPtr<IInputStream> CSystem::mIn;
AutoPtr<IPrintStream> CSystem::mOut;
AutoPtr<IPrintStream> CSystem::mErr;

ECode CSystem::constructor()
{
    AutoPtr<CFileInputStream> input;
    CFileInputStream::NewByFriend(CFileDescriptor::IN, (CFileInputStream**)&input);
    AutoPtr<CBufferedInputStream> bi;
    CBufferedInputStream::NewByFriend((IInputStream*)input->Probe(EIID_IInputStream), (CBufferedInputStream**)&bi);
    mIn = (IInputStream*)bi->Probe(EIID_IInputStream);

    AutoPtr<CFileOutputStream> output;
    CFileOutputStream::NewByFriend(CFileDescriptor::OUT, (CFileOutputStream**)&output);
    AutoPtr<CPrintStream> outputPs;
    CPrintStream::NewByFriend((IOutputStream*)output->Probe(EIID_IOutputStream), (CPrintStream**)&outputPs);
    mOut = (IPrintStream*)outputPs->Probe(EIID_IPrintStream);

    AutoPtr<CFileOutputStream> err;
    CFileOutputStream::NewByFriend(CFileDescriptor::ERR, (CFileOutputStream**)&err);
    AutoPtr<CPrintStream> errPs;
    CPrintStream::NewByFriend((IOutputStream*)err->Probe(EIID_IOutputStream), (CPrintStream**)&errPs);
    //if (errPs)
    {
        mErr = (IPrintStream*)errPs->Probe(EIID_IPrintStream);
    }
    return NOERROR;
}

PInterface CSystem::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_ISystem) {
        return (ISystem*)this;
    }
    else return Object::Probe(riid);
}

UInt32 CSystem::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CSystem::Release()
{
    return ElRefBase::Release();
}

ECode CSystem::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);
    if (object == (IInterface*)(ISystem*)this) {
        *iid = EIID_ISystem;
        return NOERROR;
    }
    else return Object::GetInterfaceID(object, iid);
}

ECode CSystem::SetIn(
    /* [in] */ IInputStream* newIn)
{
    mIn = newIn;
    return NOERROR;
}

ECode CSystem::GetIn(
    /* [out] */ IInputStream** input)
{
    VALIDATE_NOT_NULL(input);
    *input = mIn;
    INTERFACE_ADDREF(*input);
    return NOERROR;
}

ECode CSystem::SetOut(
    /* [in] */ IPrintStream* newOut)
{
    mOut = newOut;
    return NOERROR;
}

ECode CSystem::GetOut(
    /* [out] */ IPrintStream** output)
{
    VALIDATE_NOT_NULL(output);
    *output = mOut;
    INTERFACE_ADDREF(*output);
    return NOERROR;
}

ECode CSystem::SetErr(
    /* [in] */ IPrintStream* newErr)
{
    mErr = newErr;
    return NOERROR;
}

ECode CSystem::GetErr(
    /* [out] */ IPrintStream** err)
{
    VALIDATE_NOT_NULL(err);
    *err = mErr;
    INTERFACE_ADDREF(*err);
    return NOERROR;
}

ECode CSystem::GetCurrentTimeMillis(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);

    // we don't support the clocks here.
    struct timeval t;
    t.tv_sec = t.tv_usec = 0;
    gettimeofday(&t, NULL);
    *value = ((Int64)t.tv_sec * 1000 + (Int64)t.tv_usec / 1000.0);
    return NOERROR;
}

ECode CSystem::GetNanoTime(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);

    struct timespec ts;

    ts.tv_sec = ts.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    *value = (Int64)ts.tv_sec*1000000000LL + ts.tv_nsec;
    return NOERROR;
}

ECode CSystem::Exit(
    /* [in] */ Int32 code)
{
    //Runtime.getRuntime().exit(code);
    return NOERROR;
}

ECode CSystem::GC()
{
    //Runtime.getRuntime().gc();
    return NOERROR;
}

ECode CSystem::IdentityHashCode(
    /* [in] */ IInterface* anObject,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = (Int32)anObject;
    return NOERROR;
}

ECode CSystem::GetLineSeparator(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    if (sLineSeparator.IsNull()) {
        GetProperty(String("line.separator"), &sLineSeparator);
    }
    *value = sLineSeparator;
    return NOERROR;
}

ECode CSystem::GetEnv(
    /* [in] */ const String& name,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = GetEnvByName(name);
    return NOERROR;
}

ECode CSystem::GetEnvEx(
    /* [in] */ const String& name,
    /* [in] */ const String& defaultValue,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    *value = GetEnvByName(name);
    if ((*value).IsNull()) {
        *value = defaultValue;
    }

    return NOERROR;
}

String CSystem::GetEnvByName(
    /* [in] */ const String& name)
{
    if (name.IsNull()) {
        ALOGD("Failed to CSystem::GetEnv(), name == null!");
        return String(NULL);
    }

    return String(getenv(name.string()));
}

ECode CSystem::GetEnvs(
    /* [out] */ IMap** map)
{
    VALIDATE_NOT_NULL(map);

    AutoPtr<CHashMap> hashMap;
    CHashMap::NewByFriend((CHashMap**)&hashMap);
    char ** p = environ;
    Int32 index = 0;
    String entry, key, value;
    while (p && *p) {
        entry = String(*p);
        index = entry.IndexOf('=');
        if (index != -1) {
            key = entry.Substring(0, index);
            value = entry.Substring(index + 1);

            AutoPtr<CStringWrapper> ko, vo;
            CStringWrapper::NewByFriend(key, (CStringWrapper**)&ko);
            CStringWrapper::NewByFriend(value, (CStringWrapper**)&vo);
            hashMap->Put(ko->Probe(EIID_IInterface), vo->Probe(EIID_IInterface), NULL);
        }
        ++p;
    }

    *map = (IMap*)hashMap.Get();
    INTERFACE_ADDREF(*map);
    return NOERROR;
}

ECode CSystem::InheritedChannel(
    /* [out] */ IChannel** value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<CSelectorProviderHelper> helper;
    CSelectorProviderHelper::AcquireSingletonByFriend((CSelectorProviderHelper**)&helper);
    AutoPtr<ISelectorProvider> provider;
    helper->GetProvider((ISelectorProvider**)&provider);
    provider->InheritedChannel((IChannel**)&value);
    return NOERROR;
}

ECode CSystem::GetProperties(
    /* [out] */ IProperties** value)
{
    VALIDATE_NOT_NULL(value);

    if (sSystemProperties == NULL) {
        InitSystemProperties();
    }
    *value = sSystemProperties;
    INTERFACE_ADDREF(*value);
    return NOERROR;
}

ECode CSystem::InitSystemProperties()
{
    // VMRuntime runtime = VMRuntime.getRuntime();
    AutoPtr<IProperties> p;
    CProperties::New((IProperties**)&p);

    String projectUrl("http://www.android.com/");
    String projectName("The Android Project");

    // p->SetProperty(String("java.boot.class.path"), runtime.bootClassPath(), NULL);
    // p->SetProperty(String("java.class.path"), runtime.classPath(), NULL);

    // None of these four are meaningful on Android, but these keys are guaranteed
    // to be present for System.getProperty. For java.class.version, we use the maximum
    // class file version that dx currently supports.
    p->SetProperty(String("java.class.version"), String("50.0"), NULL);
    p->SetProperty(String("java.compiler"), String(""), NULL);
    p->SetProperty(String("java.ext.dirs"), String(""), NULL);
    p->SetProperty(String("java.version"), String("0"), NULL);

    String tmp;
    GetEnvEx(String("JAVA_HOME"), String("/system"), &tmp);
    p->SetProperty(String("java.home"), tmp, NULL);

    p->SetProperty(String("java.io.tmpdir"), String("/tmp"), NULL);
    GetEnv(String("LD_LIBRARY_PATH"), &tmp);
    p->SetProperty(String("java.library.path"), tmp, NULL);

    p->SetProperty(String("java.specification.name"), String("Dalvik Core Library"), NULL);
    p->SetProperty(String("java.specification.vendor"), projectName, NULL);
    p->SetProperty(String("java.specification.version"), String("0.9"), NULL);

    p->SetProperty(String("java.vendor"), projectName, NULL);
    p->SetProperty(String("java.vendor.url"), projectUrl, NULL);
    p->SetProperty(String("java.vm.name"), String("Dalvik"), NULL);
    p->SetProperty(String("java.vm.specification.name"), String("Dalvik Virtual Machine Specification"), NULL);
    p->SetProperty(String("java.vm.specification.vendor"), projectName, NULL);
    p->SetProperty(String("java.vm.specification.version"), String("0.9"), NULL);
    p->SetProperty(String("java.vm.vendor"), projectName, NULL);
    // p->SetProperty(String("java.vm.version"), runtime.vmVersion(), NULL);

    p->SetProperty(String("file.separator"), String("/"), NULL);
    p->SetProperty(String("line.separator"), String("\n"), NULL);
    p->SetProperty(String("path.separator"), String(":"), NULL);

    p->SetProperty(String("java.runtime.name"), String("Android Runtime"), NULL);
    p->SetProperty(String("java.runtime.version"), String("0.9"), NULL);
    p->SetProperty(String("java.vm.vendor.url"), projectUrl, NULL);

    p->SetProperty(String("file.encoding"), String("UTF-8"), NULL);
    p->SetProperty(String("user.language"), String("en"), NULL);
    p->SetProperty(String("user.region"), String("US"), NULL);

    GetEnvEx(String("HOME"), String(""), &tmp);
    p->SetProperty(String("user.home"), tmp, NULL);
    GetEnvEx(String("USER"), String(""), &tmp);
    p->SetProperty(String("user.name"), tmp, NULL);

    struct utsname info;
    if (uname(&info) == 0) {
        p->SetProperty(String("os.arch"), String(info.machine), NULL);
        p->SetProperty(String("os.name"), String(info.sysname), NULL);
        p->SetProperty(String("os.version"), String(info.release), NULL);
    }

    AutoPtr<CICUHelper> icuHelper;
    CICUHelper::AcquireSingletonByFriend((CICUHelper**)&icuHelper);
    String icuVersion, unicodeVersion;
    icuHelper->GetIcuVersion(&icuVersion);
    icuHelper->GetUnicodeVersion(&unicodeVersion);
    // Undocumented Android-only properties.
    p->SetProperty(String("android.icu.library.version"), icuVersion, NULL);
    p->SetProperty(String("android.icu.unicode.version"), unicodeVersion, NULL);
    // TODO: it would be nice to have this but currently it causes circularity.
    // p->SetProperty(String("android.tzdata.version"), ZoneInfoDB.getVersion(), NULL);
    ParsePropertyAssignments(p, SpecialProperties());

    // Override built-in properties with settings from the command line.
    //TODO parsePropertyAssignments(p, runtime.properties());

    CSystem::sSystemProperties = p;
    return NOERROR;
}

ECode CSystem::GetProperty(
    /* [in] */ const String& propertyName,
    /* [out] */ String* value)
{
    return GetPropertyEx(propertyName, String(NULL), value);
}

ECode CSystem::GetPropertyEx(
    /* [in] */ const String& prop,
    /* [in] */ const String& defaultValue,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = String(NULL);

    if (prop.IsNullOrEmpty()) {
        ALOGD("Failed to GetProperty! parameter prop is null or empty!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IProperties> properties;
    GetProperties((IProperties**)&properties);
    properties->GetPropertyEx(prop, defaultValue, value);
    if ((*value).IsNull()) {
        *value = defaultValue;
    }
    return NOERROR;
}

ECode CSystem::SetProperty(
    /* [in] */ const String& prop,
    /* [in] */ const String& value,
    /* [out] */ String* oldValue)
{
    VALIDATE_NOT_NULL(value);
    *oldValue = String(NULL);

    if (prop.IsNullOrEmpty()) {
        ALOGD("Failed to SetProperty! parameter prop is null or empty!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IProperties> properties;
    GetProperties((IProperties**)&properties);
    properties->SetProperty(prop, value, oldValue);
    return NOERROR;
}

ECode CSystem::ClearProperty(
    /* [in] */ const String& key,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = String(NULL);

    if (key.IsNullOrEmpty()) {
        ALOGD("Failed to ClearProperty! parameter key is null or empty!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IProperties> properties;
    GetProperties((IProperties**)&properties);
    AutoPtr<IInterface> obj;
    AutoPtr<ICharSequence> keyObj;
    CStringWrapper::New(key, (ICharSequence**)&keyObj);
    IMap::Probe(properties)->Remove(keyObj, (IInterface**)&obj);
    if (obj) {
        ICharSequence* seq = ICharSequence::Probe(obj);
        if (seq) {
            seq->ToString(value);
        }
    }

    return NOERROR;
}

ECode CSystem::Load(
    /* [in] */ const String& pathName)
{
    //Runtime.getRuntime().load(pathName, VMStack.getCallingClassLoader());
    return NOERROR;
}

ECode CSystem::LoadLibrary(
    /* [in] */ const String& libName)
{
    //Runtime.getRuntime().loadLibrary(libName, VMStack.getCallingClassLoader());
    return NOERROR;
}

ECode CSystem::MapLibraryName(
    /* [in] */ const String& userLibName,
    /* [out] */ String* value)
 {
    VALIDATE_NOT_NULL(value);
    *value = String(NULL);
    return NOERROR;
 }

ECode CSystem::RunFinalization()
{
    //Runtime.getRuntime().runFinalization();
    return NOERROR;
}

ECode CSystem::RunFinalizersOnExit(
    /* [in] */ Boolean flag)
{
    //Runtime.runFinalizersOnExit(flag);
    return NOERROR;
}

ECode CSystem::SetProperties(
    /* [in] */ IProperties* properties)
{
    sSystemProperties = properties;
    return NOERROR;
}

AutoPtr<ArrayOf<String> > CSystem::SpecialProperties()
{
    char path[PATH_MAX];
    getcwd(path, sizeof(path));

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(3);
    StringBuilder sb(64);
    sb += "user.dir=";
    sb += path;
    array->Set(0, sb.ToString());

    sb.Reset();
    sb += "android.zlib.version=";
    sb += ZLIB_VERSION;
    array->Set(1, sb.ToString());

    sb.Reset();
    sb += "android.openssl.version=";
    sb += OPENSSL_VERSION_TEXT;
    array->Set(2, sb.ToString());

    return array;
}

ECode CSystem::ParsePropertyAssignments(
    /* [in] */ IProperties* p,
    /* [in] */ ArrayOf<String>* assignments)
{
    if (assignments == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String key, value, assignment;
    for (Int32 i = 0; i < assignments->GetLength(); ++i) {
        assignment = (*assignments)[i];
        Int32 split = assignment.IndexOf('=');
        key = assignment.Substring(0, split);
        value = assignment.Substring(split + 1);
        p->SetProperty(key, value, NULL);
    }
    return NOERROR;
}


} // namespace Core
} // namespace Elastos
