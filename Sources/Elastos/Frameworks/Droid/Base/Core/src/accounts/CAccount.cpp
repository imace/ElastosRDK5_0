
#include "ext/frameworkdef.h"
#include "accounts/CAccount.h"
#include "text/TextUtils.h"
#include <elastos/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Core::ICharSequence;
using Elastos::Core::CStringWrapper;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Accounts {

const String CAccount::TAG("Account");

ECode CAccount::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode CAccount::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode CAccount::GetType(
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mType;
    return NOERROR;
}

ECode CAccount::SetType(
    /* [in] */ const String& type)
{
    mType = type;
    return NOERROR;
}

ECode CAccount::ToString(
    /* [out] */ String* s)
{
    VALIDATE_NOT_NULL(s);
    *s = String("Account {name=") + mName + String(", type=") + mType + String("}");
    return  NOERROR;
}

ECode CAccount::Equals(
    /* [in] */ IInterface *obj,
    /* [out] */ Boolean *equal)
{
    VALIDATE_NOT_NULL(equal)
    if (THIS_PROBE(IInterface) == obj) {
        *equal = TRUE;
        return NOERROR;
    }
    if (!IAccount::Probe(obj)) {
        *equal = FALSE;
        return NOERROR;
    }
    const AutoPtr<CAccount> other = (CAccount*)IAccount::Probe(obj);
    Boolean equal1, equal2;
    *equal = mName.Equals(other->mName) && mType.Equals(other->mType);
    return NOERROR;
}

ECode CAccount::GetHashCode(
    /* [out] */ Int32 *hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    Int32 result = 17;
    result = 31 * result + mName.GetHashCode();
    result = 31 * result + mType.GetHashCode();
    *hashCode = result;
    return NOERROR;
}

ECode CAccount::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mName);
    dest->WriteString(mType);
    return NOERROR;
}

ECode CAccount::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mName);
    source->ReadString(&mType);
    return NOERROR;
}

ECode CAccount::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& type)
{
    AutoPtr<ICharSequence> csq;
    CStringWrapper::New(name, (ICharSequence**)&csq);
    if (TextUtils::IsEmpty(csq)) {
        Slogger::E(TAG, "the name must not be empty: %s", (const char*)name);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("the name must not be empty: " + name);
    }

    csq = NULL;
    CStringWrapper::New(type, (ICharSequence**)&csq);
    if (TextUtils::IsEmpty(csq)) {
        Slogger::E(TAG, "the type must not be empty: %s", (const char*)type);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("the type must not be empty: " + type);
    }
    mName = name;
    mType = type;
    return NOERROR;
}

ECode CAccount::constructor()
{
    return NOERROR;
}

}  //namespace Accounts
}  //namespace Droid
}  //namespace Elastos
