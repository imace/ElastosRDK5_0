#ifndef __BaseMovementMethod_H__
#define __BaseMovementMethod_H__

#include "Elastos.Droid.Core_server.h"

using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

#ifndef Integer_MAX_VALUE
    #define Integer_MAX_VALUE 0x7fffffff
#endif
#ifndef Integer_MIN_VALUE
    #define Integer_MIN_VALUE 0x80000000
#endif

/**
 * Base classes for movement methods.
 */
class BaseMovementMethod{
public:
    //@Override
    CARAPI_(Boolean) CanSelectArbitrarily();

    //@Override
    CARAPI_(void) Initialize(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* text);

    //@Override
    CARAPI_(Boolean) OnKeyDown(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* text,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event);

    //@Override
    CARAPI_(Boolean) OnKeyOther(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* text,
        /* [in] */ IKeyEvent* event);

    //@Override
    CARAPI_(Boolean) OnKeyUp(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* text,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event);

    //@Override
    CARAPI_(void) OnTakeFocus(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* text,
        /* [in] */ Int32 direction);

    //@Override
    CARAPI_(Boolean) OnTouchEvent(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* text,
        /* [in] */ IMotionEvent* event);

    //@Override
    CARAPI_(Boolean) OnTrackballEvent(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* text,
        /* [in] */ IMotionEvent* event);

    //@Override
    CARAPI_(Boolean) OnGenericMotionEvent(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* text,
        /* [in] */ IMotionEvent* event);

protected:
    /**
     * Gets the meta state used for movement using the modifiers tracked by the text
     * buffer as well as those present in the key event.
     *
     * The movement meta state excludes the state of locked modifiers or the SHIFT key
     * since they are not used by movement actions (but they may be used for selection).
     *
     * @param buffer The text buffer.
     * @param event The key event.
     * @return The keyboard meta states used for movement.
     */
    CARAPI_(Int32) GetMovementMetaState(
        /* [in] */ ISpannable* buffer,
        /* [in] */ IKeyEvent* event);

    /**
     * Performs a movement key action.
     * The default implementation decodes the key down and invokes movement actions
     * such as {@link #down} and {@link #up}.
     * {@link #onKeyDown(TextView, Spannable, int, KeyEvent)} calls this method once
     * to handle an {@link KeyEvent#ACTION_DOWN}.
     * {@link #onKeyOther(TextView, Spannable, KeyEvent)} calls this method repeatedly
     * to handle each repetition of an {@link KeyEvent#ACTION_MULTIPLE}.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @param event The key event.
     * @param keyCode The key code.
     * @param movementMetaState The keyboard meta states used for movement.
     * @param event The key event.
     * @return True if the event was handled.
     */
    CARAPI_(Boolean) HandleMovementKey(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer,
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 movementMetaState,
        /* [in] */ IKeyEvent* event);

    /**
     * Performs a left movement action.
     * Moves the cursor or scrolls left by one character.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @return True if the event was handled.
     */
    CARAPI_(Boolean) Left(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /**
     * Performs a right movement action.
     * Moves the cursor or scrolls right by one character.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @return True if the event was handled.
     */
    CARAPI_(Boolean) Right(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /**
     * Performs an up movement action.
     * Moves the cursor or scrolls up by one line.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @return True if the event was handled.
     */
    CARAPI_(Boolean) Up(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /**
     * Performs a down movement action.
     * Moves the cursor or scrolls down by one line.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @return True if the event was handled.
     */
    CARAPI_(Boolean) Down(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /**
     * Performs a page-up movement action.
     * Moves the cursor or scrolls up by one page.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @return True if the event was handled.
     */
    CARAPI_(Boolean) PageUp(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /**
     * Performs a page-down movement action.
     * Moves the cursor or scrolls down by one page.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @return True if the event was handled.
     */
    CARAPI_(Boolean) PageDown(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /**
     * Performs a top movement action.
     * Moves the cursor or scrolls to the top of the buffer.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @return True if the event was handled.
     */
    CARAPI_(Boolean) Top(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /**
     * Performs a bottom movement action.
     * Moves the cursor or scrolls to the bottom of the buffer.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @return True if the event was handled.
     */
    CARAPI_(Boolean) Bottom(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /**
     * Performs a line-start movement action.
     * Moves the cursor or scrolls to the start of the line.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @return True if the event was handled.
     */
    CARAPI_(Boolean) LineStart(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /**
     * Performs a line-end movement action.
     * Moves the cursor or scrolls to the end of the line.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @return True if the event was handled.
     */
    CARAPI_(Boolean) LineEnd(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /** {@hide} */
    CARAPI_(Boolean) LeftWord(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /** {@hide} */
    CARAPI_(Boolean) RightWord(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /**
     * Performs a home movement action.
     * Moves the cursor or scrolls to the start of the line or to the top of the
     * document depending on whether the insertion point is being moved or
     * the document is being scrolled.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @return True if the event was handled.
     */
    CARAPI_(Boolean) Home(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /**
     * Performs an end movement action.
     * Moves the cursor or scrolls to the start of the line or to the top of the
     * document depending on whether the insertion point is being moved or
     * the document is being scrolled.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @return True if the event was handled.
     */
    CARAPI_(Boolean) End(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /**
     * Performs a scroll left action.
     * Scrolls left by the specified number of characters.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @param amount The number of characters to scroll by.  Must be at least 1.
     * @return True if the event was handled.
     * @hide
     */
    CARAPI_(Boolean) ScrollLeft(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer,
        /* [in] */ Int32 amount);

    /**
     * Performs a scroll right action.
     * Scrolls right by the specified number of characters.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @param amount The number of characters to scroll by.  Must be at least 1.
     * @return True if the event was handled.
     * @hide
     */
    CARAPI_(Boolean) ScrollRight(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer,
        /* [in] */ Int32 amount);

    /**
     * Performs a scroll up action.
     * Scrolls up by the specified number of lines.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @param amount The number of lines to scroll by.  Must be at least 1.
     * @return True if the event was handled.
     * @hide
     */
    CARAPI_(Boolean) ScrollUp(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer,
        /* [in] */ Int32 amount);

    /**
     * Performs a scroll down action.
     * Scrolls down by the specified number of lines.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @param amount The number of lines to scroll by.  Must be at least 1.
     * @return True if the event was handled.
     * @hide
     */
    CARAPI_(Boolean) ScrollDown(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer,
        /* [in] */ Int32 amount);

    /**
     * Performs a scroll page up action.
     * Scrolls up by one page.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @return True if the event was handled.
     * @hide
     */
    CARAPI_(Boolean) ScrollPageUp(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /**
     * Performs a scroll page up action.
     * Scrolls down by one page.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @return True if the event was handled.
     * @hide
     */
    CARAPI_(Boolean) ScrollPageDown(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /**
     * Performs a scroll to top action.
     * Scrolls to the top of the document.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @return True if the event was handled.
     * @hide
     */
    CARAPI_(Boolean) ScrollTop(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /**
     * Performs a scroll to bottom action.
     * Scrolls to the bottom of the document.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @return True if the event was handled.
     * @hide
     */
    CARAPI_(Boolean) ScrollBottom(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /**
     * Performs a scroll to line start action.
     * Scrolls to the start of the line.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @return True if the event was handled.
     * @hide
     */
    CARAPI_(Boolean) ScrollLineStart(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /**
     * Performs a scroll to line end action.
     * Scrolls to the end of the line.
     *
     * @param widget The text view.
     * @param buffer The text buffer.
     * @return True if the event was handled.
     * @hide
     */
    CARAPI_(Boolean) ScrollLineEnd(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

private:
    CARAPI_(Int32) GetTopLine(
        /* [in] */ ITextView* widget);

    CARAPI_(Int32) GetBottomLine(
        /* [in] */ ITextView* widget);

    CARAPI_(Int32) GetInnerWidth(
        /* [in] */ ITextView* widget);

    CARAPI_(Int32) GetInnerHeight(
        /* [in] */ ITextView* widget);

    CARAPI_(Int32) GetCharacterWidth(
        /* [in] */ ITextView* widget);

    CARAPI_(Int32) GetScrollBoundsLeft(
        /* [in] */ ITextView* widget);

    CARAPI_(Int32) GetScrollBoundsRight(
        /* [in] */ ITextView* widget);
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __BaseMovementMethod_H__
