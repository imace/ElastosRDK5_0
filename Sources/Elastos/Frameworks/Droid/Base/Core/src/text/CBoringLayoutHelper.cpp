
#include "text/CBoringLayoutHelper.h"
#include "text/CBoringLayout.h"

namespace Elastos {
namespace Droid {
namespace Text {

ILAYOUTHELPER_METHODS_IMPL(CBoringLayoutHelper, CBoringLayout, CBoringLayout)

CARAPI CBoringLayoutHelper::Make(
    /*[in]*/ ICharSequence* source,
    /*[in]*/ ITextPaint* paint,
    /*[in]*/ Int32 outerwidth,
    /*[in]*/ LayoutAlignment align,
    /*[in]*/ Float spacingmult,
    /*[in]*/ Float spacingadd,
    /*[in]*/ IBoringLayoutMetrics* metrics,
    /*[in]*/ Boolean includepad,
    /*[out]*/ IBoringLayout** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<IBoringLayout> bl = CBoringLayout::Make(source, paint, outerwidth, align, spacingmult, spacingadd, metrics, includepad);
    *ret = bl;
    INTERFACE_ADDREF(*ret);
    return NOERROR;
}

CARAPI CBoringLayoutHelper::MakeEx(
    /*[in]*/ ICharSequence* source,
    /*[in]*/ ITextPaint* paint,
    /*[in]*/ Int32 outerwidth,
    /*[in]*/ LayoutAlignment align,
    /*[in]*/ Float spacingmult,
    /*[in]*/ Float spacingadd,
    /*[in]*/ IBoringLayoutMetrics* metrics,
    /*[in]*/ Boolean includepad,
    /*[in]*/ TextUtilsTruncateAt ellipsize,
    /*[in]*/ Int32 ellipsizedWidth,
    /*[out]*/ IBoringLayout** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<IBoringLayout> bl = CBoringLayout::Make(source, paint, outerwidth, align, spacingmult, spacingadd, metrics, includepad, ellipsize, ellipsizedWidth);
    *ret = bl;
    INTERFACE_ADDREF(*ret);
    return NOERROR;
}

CARAPI CBoringLayoutHelper::IsBoring(
    /*[in]*/ ICharSequence* text,
    /*[in]*/ ITextPaint* paint,
    /*[out]*/ IBoringLayoutMetrics** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<IBoringLayoutMetrics> blm = CBoringLayout::IsBoring(text, paint);
    *ret = blm;
    INTERFACE_ADDREF(*ret);
    return NOERROR;
}

CARAPI CBoringLayoutHelper::IsBoringEx(
    /*[in]*/ ICharSequence* text,
    /*[in]*/ ITextPaint* paint,
    /*[in]*/ ITextDirectionHeuristic* textDir,
    /*[out]*/ IBoringLayoutMetrics** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<IBoringLayoutMetrics> blm = CBoringLayout::IsBoring(text, paint, textDir);
    *ret = blm;
    INTERFACE_ADDREF(*ret);
    return NOERROR;
}

CARAPI CBoringLayoutHelper::IsBoringEx2(
    /*[in]*/ ICharSequence* text,
    /*[in]*/ ITextPaint* paint,
    /*[in]*/ IBoringLayoutMetrics* metrics,
    /*[out]*/ IBoringLayoutMetrics** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<IBoringLayoutMetrics> blm = CBoringLayout::IsBoring(text, paint, metrics);
    *ret = blm;
    INTERFACE_ADDREF(*ret);
    return NOERROR;
}

CARAPI CBoringLayoutHelper::IsBoringEx3(
    /*[in]*/ ICharSequence* text,
    /*[in]*/ ITextPaint* paint,
    /*[in]*/ ITextDirectionHeuristic* textDir,
    /*[in]*/ IBoringLayoutMetrics* metrics,
    /*[out]*/ IBoringLayoutMetrics** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<IBoringLayoutMetrics> blm = CBoringLayout::IsBoring(text, paint, textDir, metrics);
    *ret = blm;
    INTERFACE_ADDREF(*ret);
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos