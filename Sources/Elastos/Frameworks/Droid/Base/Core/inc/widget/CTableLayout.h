#ifndef __CTABLELAYOUT_H_
#define __CTABLELAYOUT_H_

#include "_CTableLayout.h"
#include "view/ViewMacro.h"
#include "view/ViewGroupLayoutParamsMacro.h"
#include "widget/LinearLayoutMacro.h"
#include "widget/TableLayout.h"

namespace Elastos{
namespace Droid{
namespace Widget{

CarClass(CTableLayout), public TableLayout
{
public:
    IVIEW_METHODS_DECL()
    IVIEWGROUP_METHODS_DECL()
    IVIEWPARENT_METHODS_DECL()
    IVIEWMANAGER_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()
    ILINEARLAYOUT_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * <p>Indicates whether all columns are shrinkable or not.</p>
     *
     * @return true if all columns are shrinkable, false otherwise
     */
    CARAPI IsShrinkAllColumns(
        /* [out] */ Boolean* shrinkAllColumns);

    /**
     * <p>Convenience method to mark all columns as shrinkable.</p>
     *
     * @param shrinkAllColumns true to mark all columns shrinkable
     *
     * @attr ref android.R.styleable#TableLayout_shrinkColumns
     */
    CARAPI SetShrinkAllColumns(
        /* [in] */ Boolean shrinkAllColumns);

    /**
     * <p>Indicates whether all columns are stretchable or not.</p>
     *
     * @return true if all columns are stretchable, false otherwise
     */
    CARAPI IsStretchAllColumns(
        /* [out] */ Boolean* stretchAllColumns);

    /**
     * <p>Convenience method to mark all columns as stretchable.</p>
     *
     * @param stretchAllColumns true to mark all columns stretchable
     *
     * @attr ref android.R.styleable#TableLayout_stretchColumns
     */
    CARAPI SetStretchAllColumns(
        /* [in] */ Boolean stretchAllColumns);

    /**
     * <p>Collapses or restores a given column. When collapsed, a column
     * does not appear on screen and the extra space is reclaimed by the
     * other columns. A column is collapsed/restored only when it belongs to
     * a {@link android.widget.TableRow}.</p>
     *
     * <p>Calling this method requests a layout operation.</p>
     *
     * @param columnIndex the index of the column
     * @param isCollapsed true if the column must be collapsed, false otherwise
     *
     * @attr ref android.R.styleable#TableLayout_collapseColumns
     */
    CARAPI SetColumnCollapsed(
        /* [in] */ Int32 columnIndex,
        /* [in] */ Boolean isCollapsed);

    /**
     * <p>Returns the collapsed state of the specified column.</p>
     *
     * @param columnIndex the index of the column
     * @return true if the column is collapsed, false otherwise
     */
    IsColumnCollapsed(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Boolean* isCollapsed);

    /**
     * <p>Makes the given column stretchable or not. When stretchable, a column
     * takes up as much as available space as possible in its row.</p>
     *
     * <p>Calling this method requests a layout operation.</p>
     *
     * @param columnIndex the index of the column
     * @param isStretchable true if the column must be stretchable,
     *                      false otherwise. Default is false.
     *
     * @attr ref android.R.styleable#TableLayout_stretchColumns
     */
    CARAPI SetColumnStretchable(
        /* [in] */ Int32 columnIndex,
        /* [in] */ Boolean isStretchable);

    /**
     * <p>Returns whether the specified column is stretchable or not.</p>
     *
     * @param columnIndex the index of the column
     * @return true if the column is stretchable, false otherwise
     */
    CARAPI IsColumnStretchable(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Boolean* isStretchable);

    /**
     * <p>Makes the given column shrinkable or not. When a row is too wide, the
     * table can reclaim extra space from shrinkable columns.</p>
     *
     * <p>Calling this method requests a layout operation.</p>
     *
     * @param columnIndex the index of the column
     * @param isShrinkable true if the column must be shrinkable,
     *                     false otherwise. Default is false.
     *
     * @attr ref android.R.styleable#TableLayout_shrinkColumns
     */
    CARAPI SetColumnShrinkable(
        /* [in] */ Int32 columnIndex,
        /* [in] */ Boolean isShrinkable);

    /**
     * <p>Returns whether the specified column is shrinkable or not.</p>
     *
     * @param columnIndex the index of the column
     * @return true if the column is shrinkable, false otherwise. Default is false.
     */
    CARAPI IsColumnShrinkable(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Boolean* isShrinkable);
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__CTABLELAYOUT_H_
