#ifndef __CEMPTYABSSAVEDSTATE_H__
#define __CEMPTYABSSAVEDSTATE_H__

#include "_CEmptyAbsSavedState.h"
#include "view/AbsSavedState.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CEmptyAbsSavedState) , public AbsSavedState
{
public:
    CEmptyAbsSavedState();

    constructor();

    CARAPI GetSuperState(
        /* [out] */ IParcelable** superState);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif