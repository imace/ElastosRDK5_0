#ifndef __ELASTOS_TEXT_CHOICEFORMAT_H__
#define __ELASTOS_TEXT_CHOICEFORMAT_H__

#include "Elastos.CoreLibrary_server.h"
#include "NumberFormat.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Core::IStringBuffer;
using Elastos::Text::IFieldPosition;
using Elastos::Text::IParsePosition;

namespace Elastos {
namespace Text {

class ChoiceFormat : public NumberFormat
{
public:
    virtual ~ChoiceFormat();

    CARAPI constructor(
        /* [in] */ const ArrayOf<Double>& limits,
        /* [in] */ const ArrayOf<String>& formats);

    CARAPI constructor(
        /* [in] */ const String& tem);

    virtual CARAPI ApplyPattern(
        /* [in] */ const String& tem);

    CARAPI Clone(
        /* [out] */ IInterface ** outface);

    CARAPI Equals(
        /* [in] */ IInterface * object,
        /* [out] */ Boolean * value);

    CARAPI FormatDouble(
        /* [in] */ Double value,
        /* [in] */ IStringBuffer * inbuffer,
        /* [in] */ IFieldPosition * field,
        /* [out] */ IStringBuffer ** outbuffer);

    CARAPI FormatInt64(
        /* [in] */ Int64 value,
        /* [in] */ IStringBuffer * inbuffer,
        /* [in] */ IFieldPosition * field ,
        /* [out] */ IStringBuffer ** outbuffer);

    CARAPI GetHashCode(
        /* [out] */ Int32 * value);

    virtual CARAPI GetFormats(
        /* [out, callee] */ ArrayOf<IInterface*>** arrayOfFormattedString);

    CARAPI GetChoiceFormats(
        /* [out, callee] */ ArrayOf<String>** formats);

    virtual CARAPI GetLimits(
        /* [out, callee] */ ArrayOf<Double>** arrayOfDoubles);

    static CARAPI_(Double) NextDouble(
        /* [in] */ Double value);

    static CARAPI_(Double) NextDouble(
        /* [in] */ Double value,
        /* [in] */ Boolean increment);

    //@Override
    CARAPI Parse(
        /* [in] */ const String& string,
        /* [in] */ IParsePosition* position,
        /* [out] */ INumber** value);

    static CARAPI_(Double) PreviousDouble(
        /* [in] */ Double value);

    virtual CARAPI SetChoices(
        /* [in] */ const ArrayOf<Double>& limits,
        /* [in] */ const ArrayOf<String>& formats);

    virtual CARAPI ToPattern(
        /* [out] */ String* pattern);

private:
    CARAPI_(Int32) SkipWhitespace(
        /* [in] */ const String& string,
        /* [in] */ Int32 index);

public:
    AutoPtr<ArrayOf<Double> > mChoiceLimits;

    AutoPtr<ArrayOf<String> > mChoiceFormats;
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CHOICEFORMAT_H__
