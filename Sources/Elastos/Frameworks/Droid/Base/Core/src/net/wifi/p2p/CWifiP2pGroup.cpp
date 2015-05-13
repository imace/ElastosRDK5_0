
#include "CWifiP2pGroup.h"
#include <Elastos.Core.h>
#include <elastos/Slogger.h>
#include <elastos/StringUtils.h>
#include <elastos/StringBuilder.h>
#include "CWifiP2pDevice.h"

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Utility::Regex::EIID_IMatchResult;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Wifi {
namespace P2p {

static AutoPtr<IPattern> InitPattern()
{
    AutoPtr<IPattern> pattern;
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    helper->Compile(
        String("ssid=\"(.+)\" freq=(\\d+) (?:psk=)?([0-9a-fA-F]{64})?(?:passphrase=)?(?:\"(.{0,63})\")? go_dev_addr=((?:[0-9a-f]{2}:){5}[0-9a-f]{2}) ?(\\[PERSISTENT\\])?"),
        (IPattern**)&pattern);
    return pattern;
}

AutoPtr<IPattern> CWifiP2pGroup::mGroupStartedPattern = InitPattern();

CWifiP2pGroup::CWifiP2pGroup()
    : mNetId(0)
    , mIsGroupOwner(FALSE)
{}

ECode CWifiP2pGroup::constructor()
{
    return NOERROR;
}

ECode CWifiP2pGroup::constructor(
    /* [in] */ const String& supplicantEvent)
{
    AutoPtr<ArrayOf<String> > tokens;
    FAIL_RETURN(StringUtils::Split(supplicantEvent, String(" "), (ArrayOf<String>**)&tokens));

    if (tokens == NULL || tokens->GetLength() < 3) {
        Slogger::E("WifiP2pGroup", "E_ILLEGAL_ARGUMENT_EXCEPTION: Malformed supplicant event %s", supplicantEvent.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if ((*tokens)[0].StartWith("P2P-GROUP")) {
        mInterface = (*tokens)[1];
        mIsGroupOwner = (*tokens)[2].Equals("GO");

        AutoPtr<IMatcher> match;
        FAIL_RETURN(mGroupStartedPattern->Matcher(supplicantEvent, (IMatcher**)&match));

        Boolean result;
        FAIL_RETURN(match->Find(&result));
        if (!result) {
            return NOERROR;
        }

        FAIL_RETURN(match->GroupEx(1, &mNetworkName));
        //freq and psk are unused right now
        //int freq = Integer.parseInt(match.group(2));
        //String psk = match.group(3);
        FAIL_RETURN(match->GroupEx(4, &mPassphrase));

        String temp;
        FAIL_RETURN(match->GroupEx(5, &temp));
        FAIL_RETURN(CWifiP2pDevice::New(temp, (IWifiP2pDevice**)&mOwner));

        FAIL_RETURN(match->GroupEx(6, &temp));
        if (!temp.IsNull()) {
            mNetId = IWifiP2pGroup::PERSISTENT_NET_ID;
        }
        else {
            mNetId = IWifiP2pGroup::TEMPORARY_NET_ID;
        }
    }
    else if ((*tokens)[0].Equals("P2P-INVITATION-RECEIVED")) {
        String sa;
        mNetId = IWifiP2pGroup::PERSISTENT_NET_ID;
        for (Int32 i = 0; i < tokens->GetLength(); ++i) {
            AutoPtr<ArrayOf<String> > nameValue;
            FAIL_RETURN(StringUtils::Split((*tokens)[i], String("="), (ArrayOf<String>**)&nameValue));
            if (nameValue == NULL || nameValue->GetLength() != 2) continue;

            if ((*nameValue)[0].Equals("sa")) {
                sa = (*nameValue)[1];

                // set source address into the client list.
                AutoPtr<IWifiP2pDevice> dev;
                FAIL_RETURN(CWifiP2pDevice::New((IWifiP2pDevice**)&dev));
                FAIL_RETURN(dev->SetDeviceAddress((*nameValue)[1]));
                mClients.PushBack(dev);
                continue;
            }

            if ((*nameValue)[0].Equals("go_dev_addr")) {
                mOwner = NULL;
                FAIL_RETURN(CWifiP2pDevice::New((*nameValue)[1], (IWifiP2pDevice**)&mOwner));
                continue;
            }

            if ((*nameValue)[0].Equals("persistent")) {
                mOwner = NULL;
                FAIL_RETURN(CWifiP2pDevice::New(sa, (IWifiP2pDevice**)&mOwner));
                mNetId = StringUtils::ParseInt32((*nameValue)[1]);
                continue;
            }
        }
    }
    else {
        Slogger::E("WifiP2pGroup", "E_ILLEGAL_ARGUMENT_EXCEPTION: Malformed supplicant event %s", supplicantEvent.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode CWifiP2pGroup::constructor(
    /* [in] */ IWifiP2pGroup* source)
{
    if (source != NULL) {
        FAIL_RETURN(source->GetNetworkName(&mNetworkName));
        AutoPtr<IWifiP2pDevice> temp;
        FAIL_RETURN(source->GetOwner((IWifiP2pDevice**)&temp));
        FAIL_RETURN(CWifiP2pDevice::New(temp, (IWifiP2pDevice**)&mOwner));
        FAIL_RETURN(source->IsGroupOwner(&mIsGroupOwner));

        AutoPtr<ArrayOf<IWifiP2pDevice*> > list;
        FAIL_RETURN(source->GetClientList((ArrayOf<IWifiP2pDevice*>**)&list));

        if (list != NULL) {
            for (Int32 i = 0; i < list->GetLength(); ++i) {
                AutoPtr<IWifiP2pDevice> dev = (*list)[i];
                mClients.PushBack(dev);
            }
        }

        FAIL_RETURN(source->GetPassphrase(&mPassphrase));
        FAIL_RETURN(source->GetInterface(&mInterface));
        FAIL_RETURN(source->GetNetworkId(&mNetId));
    }

    return NOERROR;
}

ECode CWifiP2pGroup::SetNetworkName(
    /* [in] */ const String& networkName)
{
    mNetworkName = networkName;

    return NOERROR;
}

ECode CWifiP2pGroup::GetNetworkName(
    /* [out] */ String* networkName)
{
    VALIDATE_NOT_NULL(networkName);

    *networkName = mNetworkName;

    return NOERROR;
}

ECode CWifiP2pGroup::SetIsGroupOwner(
    /* [in] */ Boolean isGo)
{
    mIsGroupOwner = isGo;

    return NOERROR;
}

ECode CWifiP2pGroup::IsGroupOwner(
    /* [out] */ Boolean* isGroupOwner)
{
    VALIDATE_NOT_NULL(isGroupOwner);

    *isGroupOwner = mIsGroupOwner;

    return NOERROR;
}

ECode CWifiP2pGroup::SetOwner(
    /* [in] */ IWifiP2pDevice* device)
{
    mOwner = device;

    return NOERROR;
}

ECode CWifiP2pGroup::GetOwner(
    /* [out] */ IWifiP2pDevice** device)
{
    VALIDATE_NOT_NULL(device);

    *device = mOwner;
    INTERFACE_ADDREF(*device);
    return NOERROR;
}

ECode CWifiP2pGroup::AddClient(
    /* [in] */ const String& address)
{
    AutoPtr<IWifiP2pDevice> device;
    FAIL_RETURN(CWifiP2pDevice::New(address, (IWifiP2pDevice**)&device));

    return AddClientEx(device);
}

ECode CWifiP2pGroup::AddClientEx(
    /* [in] */ IWifiP2pDevice* device)
{
    Boolean ret;
    List<AutoPtr<IWifiP2pDevice> >::Iterator it;
    for (it = mClients.Begin(); it != mClients.End(); ++it) {
        FAIL_RETURN(device->Equals(*it, &ret));
        if (ret) return NOERROR;
    }

    mClients.PushBack(device);

    return NOERROR;
}

ECode CWifiP2pGroup::RemoveClient(
    /* [in] */ const String& address,
    /* [out] */ Boolean* ret)
{

    AutoPtr<IWifiP2pDevice> device;
    FAIL_RETURN(CWifiP2pDevice::New(address, (IWifiP2pDevice**)&device));

    return RemoveClientEx(device, ret);
}

ECode CWifiP2pGroup::RemoveClientEx(
    /* [in] */ IWifiP2pDevice* device,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = FALSE;

    Boolean equal;
    List<AutoPtr<IWifiP2pDevice> >::Iterator it;
    for (it = mClients.Begin(); it != mClients.End(); ++it) {
        FAIL_RETURN(device->Equals(*it, &equal));
        if (equal) {
            mClients.Erase(it);
            *ret = TRUE;
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode CWifiP2pGroup::IsClientListEmpty(
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);

    *ret = mClients.IsEmpty();

    return NOERROR;
}

ECode CWifiP2pGroup::Contains(
    /* [in] */ IWifiP2pDevice* device,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = FALSE;

    Boolean equal;
    FAIL_RETURN(mOwner->Equals(device, &equal));
    if (equal) {
        *ret = TRUE;
        return NOERROR;
    }

    List<AutoPtr<IWifiP2pDevice> >::Iterator it;
    for (it = mClients.Begin(); it != mClients.End(); ++it) {
        FAIL_RETURN(device->Equals(*it, &equal));
        if (equal) {
            *ret = TRUE;
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode CWifiP2pGroup::GetClientList(
    /* [out, callee] */ ArrayOf<IWifiP2pDevice*>** list)
{
    VALIDATE_NOT_NULL(list);
    *list = NULL;

    AutoPtr<ArrayOf<IWifiP2pDevice*> > array = ArrayOf<IWifiP2pDevice*>::Alloc(mClients.GetSize());
    if (array == NULL) return E_OUT_OF_MEMORY;

    List<AutoPtr<IWifiP2pDevice> >::Iterator it;
    Int32 i = 0;
    for (it = mClients.Begin(); it != mClients.End(); ++it, ++i) {
        array->Set(i, *it);
    }

    *list = array;
    INTERFACE_ADDREF(*list);

    return NOERROR;
}

ECode CWifiP2pGroup::SetPassphrase(
    /* [in] */ const String& passphrase)
{
    mPassphrase = passphrase;

    return NOERROR;
}

ECode CWifiP2pGroup::GetPassphrase(
    /* [out] */ String* passphrase)
{
    VALIDATE_NOT_NULL(passphrase);

    *passphrase = mPassphrase;

    return NOERROR;
}

ECode CWifiP2pGroup::SetInterface(
    /* [in] */ const String& intf)
{
    mInterface = intf;

    return NOERROR;
}

ECode CWifiP2pGroup::GetInterface(
    /* [out] */ String* interfaceName)
{
    VALIDATE_NOT_NULL(interfaceName);

    *interfaceName = mInterface;

    return NOERROR;
}

ECode CWifiP2pGroup::GetNetworkId(
    /* [out] */ Int32* networkId)
{
    VALIDATE_NOT_NULL(networkId);

    *networkId = mNetId;

    return NOERROR;
}

ECode CWifiP2pGroup::SetNetworkId(
    /* [in] */ Int32 netId)
{
    mNetId = netId;

    return NOERROR;
}

ECode CWifiP2pGroup::ToString(
    /* [out] */ String * string)
{
    VALIDATE_NOT_NULL(string);

    StringBuilder sb;

    sb += "network: ";
    sb += mNetworkName;

    sb += "\n isGO: ";
    sb += mIsGroupOwner;

    sb += "\n GO: ";
    String temp;
    mOwner->ToString(&temp);
    sb += temp;

    List<AutoPtr<IWifiP2pDevice> >::Iterator it;
    for (it = mClients.Begin(); it != mClients.End(); ++it) {
        sb += "\n Client: ";
        (*it)->ToString(&temp);
        sb += temp;
    }

    sb += "\n interface: ";
    sb += mInterface;
    sb += "\n networkId: ";
    sb += mNetId;

    *string = sb.ToString();
    return NOERROR;
}

ECode CWifiP2pGroup::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    FAIL_RETURN(source->ReadString(&mNetworkName));
    FAIL_RETURN(source->ReadInterfacePtr((Handle32*)&mOwner));
    Byte b;
    FAIL_RETURN(source->ReadByte(&b));
    mIsGroupOwner = (b == 1);

    Int32 size = 0;
    FAIL_RETURN(source->ReadInt32(&size));
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IWifiP2pDevice> device;
        FAIL_RETURN(source->ReadInterfacePtr((Handle32*)&device));
        mClients.PushBack(device);
    }

    FAIL_RETURN(source->ReadString(&mPassphrase));
    FAIL_RETURN(source->ReadString(&mInterface));
    FAIL_RETURN(source->ReadInt32(&mNetId));

    return NOERROR;
}

ECode CWifiP2pGroup::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteString(mNetworkName));
    FAIL_RETURN(dest->WriteInterfacePtr(mOwner.Get()));
    FAIL_RETURN(dest->WriteByte(mIsGroupOwner? 1: 0));

    FAIL_RETURN(dest->WriteInt32(mClients.GetSize()));
    List<AutoPtr<IWifiP2pDevice> >::Iterator it;
    for (it = mClients.Begin(); it != mClients.End(); ++it) {
        FAIL_RETURN(dest->WriteInterfacePtr((*it).Get()));
    }

    FAIL_RETURN(dest->WriteString(mPassphrase));
    FAIL_RETURN(dest->WriteString(mInterface));
    FAIL_RETURN(dest->WriteInt32(mNetId));

    return NOERROR;
}


}
}
}
}
}
