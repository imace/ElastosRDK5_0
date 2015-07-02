#include "CoreUtils.h"
#include "CBoolean.h"
#include "CByte.h"
#include "CChar32.h"
#include "CInteger16.h"
#include "CInteger32.h"
#include "CInteger64.h"
#include "CFloat.h"
#include "CDouble.h"
#include "CStringWrapper.h"

namespace Elastos {
namespace Core {

AutoPtr<IByte> CoreUtils::ConvertByte(
    /* [in] */ Byte value)
{
    AutoPtr<CByte> obj;
    CByte::NewByFriend(value, (CByte**)&obj);
    return (IByte*)obj.Get();
}

AutoPtr<IChar32> CoreUtils::ConvertChar32(
    /* [in] */ Char32 value)
{
    AutoPtr<CChar32> obj;
    CChar32::NewByFriend(value, (CChar32**)&obj);
    return (IChar32*)obj.Get();
}

AutoPtr<IBoolean> CoreUtils::Convert(
    /* [in] */ Boolean value)
{
    AutoPtr<CBoolean> obj;
    CBoolean::NewByFriend(value, (CBoolean**)&obj);
    return (IBoolean*)obj.Get();
}

AutoPtr<IInteger16> CoreUtils::Convert(
    /* [in] */ Int16 value)
{
    AutoPtr<CInteger16> obj;
    CInteger16::NewByFriend(value, (CInteger16**)&obj);
    return (IInteger16*)obj.Get();
}

AutoPtr<IInteger32> CoreUtils::Convert(
    /* [in] */ Int32 value)
{
    AutoPtr<CInteger32> obj;
    CInteger32::NewByFriend(value, (CInteger32**)&obj);
    return (IInteger32*)obj.Get();
}

AutoPtr<IInteger64> CoreUtils::Convert(
    /* [in] */ Int64 value)
{
    AutoPtr<CInteger64> obj;
    CInteger64::NewByFriend(value, (CInteger64**)&obj);
    return (IInteger64*)obj.Get();
}

AutoPtr<IFloat> CoreUtils::Convert(
    /* [in] */ Float value)
{
    AutoPtr<CFloat> obj;
    CFloat::NewByFriend(value, (CFloat**)&obj);
    return (IFloat*)obj.Get();
}

AutoPtr<IDouble> CoreUtils::Convert(
    /* [in] */ Double value)
{
    AutoPtr<CDouble> obj;
    CDouble::NewByFriend(value, (CDouble**)&obj);
    return (IDouble*)obj.Get();
}

AutoPtr<ICharSequence> CoreUtils::Convert(
    /* [in] */ const String & value)
{
    AutoPtr<CStringWrapper> obj;
    CStringWrapper::NewByFriend(value, (CStringWrapper**)&obj);
    return (ICharSequence*)obj.Get();
}

} // namespace Core
} // namespace Elastos