#ifndef __CIMAGEBUTTON_H_
#define __CIMAGEBUTTON_H_

#include "_CImageButton.h"
#include "ImageButton.h"
#include "view/ViewMacro.h"
#include "view/ViewGroupLayoutParamsMacro.h"

namespace Elastos{
namespace Droid{
namespace Widget{

CarClass(CImageButton), public Elastos::Droid::Widget::ImageButton
{
public:

    IVIEW_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI GetAdjustViewBounds(
        /* [out] */ Boolean* res);

    CARAPI SetAdjustViewBounds(
        /* [in] */ Boolean adjustViewBounds);

    CARAPI GetMaxWidth(
        /* [out] */ Int32* width);

    CARAPI SetMaxWidth(
        /* [in] */ Int32 maxWidth);

    CARAPI GetMaxHeight(
        /* [out] */ Int32* height);

    CARAPI SetMaxHeight(
        /* [in] */ Int32 maxHeight);

    CARAPI GetDrawable(
        /* [out] */ IDrawable** drawable);

    CARAPI SetImageResource(
        /* [in] */ Int32 resId);

    CARAPI SetImageURI(
        /* [in] */ IUri* uri);

    CARAPI SetImageDrawable(
        /* [in] */ IDrawable* drawable);

    CARAPI SetImageBitmap(
        /* [in] */ IBitmap* bm);

    CARAPI SetImageState(
        /* [in] */ ArrayOf<Int32>* state,
        /* [in] */ Boolean mg);

    CARAPI SetImageLevel(
        /* [in] */ Int32 level);

    CARAPI SetScaleType(
        /* [in] */ ImageViewScaleType scaleType);

    CARAPI GetScaleType(
        /* [out] */ ImageViewScaleType* scaleType);

    CARAPI GetImageMatrix(
        /* [out] */ IMatrix** martix);

    CARAPI SetImageMatrix(
        /* [in] */ IMatrix* matrix);

    CARAPI GetCropToPadding(
        /* [out] */ Boolean* padding);

    CARAPI SetCropToPadding(
        /* [in] */ Boolean cropToPadding);

    CARAPI SetBaseline(
        /* [in] */ Int32 baseline);

    CARAPI SetBaselineAlignBottom(
        /* [in] */ Boolean aligned);

    CARAPI GetBaselineAlignBottom(
        /* [out] */ Boolean* aligned);

    CARAPI SetColorFilter(
        /* [in] */ Int32 color);

    CARAPI SetColorFilterEx(
        /* [in] */ Int32 color,
        /* [in] */ PorterDuffMode mode);

    CARAPI ClearColorFilter();

    CARAPI GetColorFilter(
        /* [out] */ IColorFilter** filter);

    CARAPI SetColorFilterEx2(
        /* [in] */ IColorFilter* cf);

    CARAPI GetImageAlpha(
        /* [out] */ Int32* alpha);

    CARAPI SetImageAlpha(
        /* [in] */ Int32 alpha);

    CARAPI SetAlphaEx(
        /* [in] */ Int32 alpha);
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__CIMAGEBUTTON_H_
