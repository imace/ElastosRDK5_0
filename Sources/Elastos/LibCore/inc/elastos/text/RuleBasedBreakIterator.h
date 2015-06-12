#ifndef __ELASTOS_TEXT_RULEBASEDBREAKITERATOR_H__
#define __ELASTOS_TEXT_RULEBASEDBREAKITERATOR_H__

#include "Elastos.CoreLibrary_server.h"
#include <elastos.h>
#include "BreakIterator.h"

using Elastos::Text::ICharacterIterator;

namespace Elastos {
namespace Text {

class RuleBasedBreakIterator : public BreakIterator
{
public:

    CARAPI Init(
        /* [in] */ INativeBreakIterator* iterator);

    ~RuleBasedBreakIterator();

    //@Override
    CARAPI Current(
        /* [out] */ Int32* currentValue);

    //@Override
    CARAPI First(
        /* [out] */ Int32* firstValue);

    //@Override
    CARAPI Following(
        /* [in] */ Int32 offset,
        /* [out] */ Int32* followingValue);

    //@Override
    CARAPI GetText(
        /* [out] */ ICharacterIterator** text);

    //@Override
    CARAPI Last(
        /* [out] */ Int32* lastValue);

    //@Override
    CARAPI GetNext(
        /* [out] */ Int32* nextValue);

    //@Override
    CARAPI GetNext(
        /* [in] */ Int32 n,
        /* [out] */ Int32* nextValue);

    //@Override
    CARAPI Previous(
        /* [out] */ Int32* previousValue);

    CARAPI SetText(
        /* [in] */ const String& newText);

    //@Override
    CARAPI SetText(
        /* [in] */ ICharacterIterator* newText);

    //@Override
    CARAPI IsBoundary(
        /* [in] */ Int32 offset,
        /* [out] */ Boolean* isBoundary);

    //@Override
    CARAPI Preceding(
        /* [in] */ Int32 offset,
        /* [out] */ Int32* precedingValue);
    //@Override
    CARAPI Clone(
        /* [out] */ IInterface ** outface);

private:

    CARAPI ValidateOffset(
        /* [in] */ Int32 offset);
};

} // namespace Text
} // namespace Elastos

#endif //__ELASTOS_TEXT_RULEBASEDBREAKITERATOR_H__
