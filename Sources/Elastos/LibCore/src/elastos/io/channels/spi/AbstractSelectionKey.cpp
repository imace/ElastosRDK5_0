
#include "AbstractSelectionKey.h"

namespace Elastos {
namespace IO {
namespace Channels {
namespace Spi {

CAR_INTERFACE_IMPL(AbstractSelectionKey, SelectionKey, IAbstractSelectionKey)

AbstractSelectionKey::AbstractSelectionKey()
    : mIsValid(FALSE)
{}

AbstractSelectionKey::~AbstractSelectionKey()
{}

ECode AbstractSelectionKey::IsValid(
    /* out*/ Boolean* isValid)
{
    *isValid = mIsValid;
    return NOERROR;
}

ECode AbstractSelectionKey::Cancel()
{
    if (mIsValid) {
        mIsValid = FALSE;
        AutoPtr<ISelector> selector;
        ECode ecRet = Selector((ISelector**)&selector);
        if (NOERROR == ecRet) {
            IAbstractSelector* absSel = IAbstractSelector::Probe(selector);
            assert(0 && "TODO");
            // ISelectionKey* selKey = (ISelectionKey*)(IInterface*)this;
            // absSel->Cancel(selKey);
        }
    }
    return NOERROR;
}

} // namespace Spi
} // namespace Channels
} // namespace IO
} // namespace Elastos