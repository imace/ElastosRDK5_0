
#ifndef __CCOLORSTATELIST_H__
#define __CCOLORSTATELIST_H__

#include "_CColorStateList.h"
#include "ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

/**
 *
 * Lets you map {@link android.view.View} state sets to colors.
 *
 * {@link android.content.res.ColorStateList}s are created from XML resource files defined in the
 * "color" subdirectory directory of an application's resource directory.  The XML file contains
 * a single "selector" element with a number of "item" elements inside.  For example:
 *
 * <pre>
 * &lt;selector xmlns:android="http://schemas.android.com/apk/res/android"&gt;
 *   &lt;item android:state_focused="true" android:color="@color/testcolor1"/&gt;
 *   &lt;item android:state_pressed="true" android:state_enabled="false" android:color="@color/testcolor2" /&gt;
 *   &lt;item android:state_enabled="false" android:color="@color/testcolor3" /&gt;
 *   &lt;item android:color="@color/testcolor5"/&gt;
 * &lt;/selector&gt;
 * </pre>
 *
 * This defines a set of state spec / color pairs where each state spec specifies a set of
 * states that a view must either be in or not be in and the color specifies the color associated
 * with that spec.  The list of state specs will be processed in order of the items in the XML file.
 * An item with no state spec is considered to match any set of states and is generally useful as
 * a final item to be used as a default.  Note that if you have such an item before any other items
 * in the list then any subsequent items will end up being ignored.
 * <p>For more information, see the guide to <a
 * href="{@docRoot}guide/topics/resources/color-list-resource.html">Color State
 * List Resource</a>.</p>
 */
CarClass(CColorStateList)
{
public:
    CColorStateList();

    ~CColorStateList();

    /**
     * Creates or retrieves a ColorStateList that always returns a single color.
     */
    static CARAPI ValueOf(
        /* [in] */ Int32 color,
        /* [out] */ IColorStateList** csl);

    /**
     * Create a ColorStateList from an XML document, given a set of {@link Resources}.
     */
    static CARAPI CreateFromXml(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ IColorStateList** csl);

    /**
     * Creates a new ColorStateList that has the same states and
     * colors as this one but where each color has the specified alpha value
     * (0-255).
     */
    CARAPI WithAlpha(
        /* [in] */ Int32 alpha,
        /* [out] */ IColorStateList** colorState);

    CARAPI IsStateful(
        /* [out] */ Boolean* isStateful);

    /**
     * Return the color associated with the given set of {@link android.view.View} states.
     *
     * @param stateSet an array of {@link android.view.View} states
     * @param defaultColor the color to return if there's not state spec in this
     * {@link ColorStateList} that matches the stateSet.
     *
     * @return the color associated with that set of states in this {@link ColorStateList}.
     */
    CARAPI GetColorForState(
        /* [in] */ ArrayOf<Int32>* stateSet,
        /* [in] */ Int32 defaultColor,
        /* [out] */ Int32* color);

    CARAPI GetDefaultColor(
        /* [out] */ Int32* color);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ArrayOf<Handle32>* states,
        /* [in] */ ArrayOf<Int32>* colors);

private:
    /* Create from inside an XML document.  Called on a parser positioned at
     * a tag in an XML document, tries to create a ColorStateList from that tag.
     * Returns null if the tag is not a valid ColorStateList.
     */
    static CARAPI CreateFromXmlInner(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IColorStateList** csl);

    /**
     * Fill in this object based on the contents of an XML "selector" element.
     */
    CARAPI Inflate(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs);

private:
    static const String TAG;
    static AutoPtr< ArrayOf<Int32Array > > EMPTY;
    //static HashMap<Int32, AutoPtr<IColorStateList> > sCache;
    static HashMap<Int32, AutoPtr<IWeakReference> > sCache;
    static Object sCacheLock;

    AutoPtr< ArrayOf<Int32Array > > mStateSpecs; // must be parallel to mColors
    AutoPtr< ArrayOf<Int32> > mColors;      // must be parallel to mStateSpecs
    Int32 mDefaultColor;
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __CCOLORSTATELIST_H__
