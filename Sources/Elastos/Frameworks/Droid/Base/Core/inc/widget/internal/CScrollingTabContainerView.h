#ifndef __CSCROLLINGTABCONTAINERVIEW_H_
#define __CSCROLLINGTABCONTAINERVIEW_H_

#include "_CScrollingTabContainerView.h"
#include "view/ViewMacro.h"
#include "widget/FrameLayoutMacro.h"
#include "widget/HorizontalScrollViewMacro.h"
#include "widget/internal/ScrollingTabContainerView.h"

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

CarClass(CScrollingTabContainerView), public ScrollingTabContainerView
{
public:
    IVIEW_METHODS_DECL()
    IVIEWGROUP_METHODS_DECL()
    IVIEWPARENT_METHODS_DECL()
    IVIEWMANAGER_METHODS_DECL()
    IFRAMELAYOUT_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()
    IHORIZONTALSCROLLVIEW_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    CARAPI SetAllowCollapse(
        /* [in] */ Boolean allowCollapse);

    CARAPI SetTabSelected(
        /* [in] */ Int32 position);

    CARAPI SetContentHeight(
        /* [in] */ Int32 contentHeight);

    CARAPI AnimateToVisibility(
        /* [in] */ Int32 visibility);

    CARAPI AnimateToTab(
        /* [in] */ Int32 position);

    CARAPI AddTab(
        /* [in] */ IActionBarTab* tab,
        /* [in] */ Boolean setSelected);

    CARAPI AddTabEx(
        /* [in] */ IActionBarTab* tab,
        /* [in] */ Int32 position,
        /* [in] */ Boolean setSelected);

    CARAPI UpdateTab(
        /* [in] */ Int32 position);

    CARAPI RemoveTabAt(
        /* [in] */ Int32 position);

    CARAPI RemoveAllTabs();
};

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__CSCROLLINGTABCONTAINERVIEW_H_
