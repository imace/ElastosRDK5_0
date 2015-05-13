
#ifndef __CTEXTKEYLISTENER_H__
#define __CTEXTKEYLISTENER_H__

#include "_CTextKeyListener.h"
#include "text/method/TextKeyListener.h"
#include "text/method/BaseKeyListenerMacro.h"
#include "text/method/KeyListenerMacro.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CTextKeyListener)
    , public TextKeyListener
{
public:
    IBaseKeyListener_METHODS_DECL()
    IKeyListener_METHODS_DECL()

    CARAPI OnSpanAdded(
        /* [in] */ ISpannable* text,
        /* [in] */ IInterface* what,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI OnSpanRemoved(
        /* [in] */ ISpannable* text,
        /* [in] */ IInterface* what,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI OnSpanChanged(
        /* [in] */ ISpannable* text,
        /* [in] */ IInterface* what,
        /* [in] */ Int32 ostart,
        /* [in] */ Int32 oend,
        /* [in] */ Int32 nstart,
        /* [in] */ Int32 nend);

    CARAPI ReleaseListener();

    CARAPI constructor(
        /* [in] */ Capitalize cap,
        /* [in] */ Boolean autotext);

    static CARAPI_(AutoPtr<ITextKeyListener>) GetInstance(
        /* [in] */ Boolean autotext,
        /* [in] */ Capitalize cap);

    static CARAPI_(AutoPtr<ITextKeyListener>) GetInstance();

private:
    static AutoPtr<ArrayOf<ITextKeyListener*> > sInstance;
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __CTEXTKEYLISTENER_H__
