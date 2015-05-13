#ifndef __CEDITTEXT_H_
#define __CEDITTEXT_H_

#include "_CEditText.h"
#include "widget/EditText.h"
#include "widget/TextViewMacro.h"

namespace Elastos {
namespace Droid{
namespace Widget{

CarClass(CEditText) , public EditText
{
public:
    IVIEW_METHODS_DECL()
    ITEXTVIEW_METHODS_DECL()
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

    CARAPI SetSelection(
        /* [in] */ Int32 start,
        /* [in] */ Int32 stop);

    CARAPI SetSelectionEx(
        /* [in] */ Int32 index);

    CARAPI SelectAll();

    CARAPI ExtendSelection(
        /* [in] */ Int32 index);

    CARAPI OnPreDraw(
        /* [out] */ Boolean* result);

};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __CEDITTEXT_H_
