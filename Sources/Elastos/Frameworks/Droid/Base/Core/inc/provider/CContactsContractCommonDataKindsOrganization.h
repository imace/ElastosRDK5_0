
#ifndef __CCONTACTSCONTRACTCOMMONDATAKINDSORGANIZATION_H__
#define __CCONTACTSCONTRACTCOMMONDATAKINDSORGANIZATION_H__

#include "_CContactsContractCommonDataKindsOrganization.h"

using Elastos::Core::ICharSequence;
using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractCommonDataKindsOrganization)
{
public:
    /**
     * This utility class cannot be instantiated
     */
    CARAPI constructor();

    /**
     * Return the string resource that best describes the given
     * {@link #TYPE}. Will always return a valid resource.
     */
    CARAPI GetTypeLabelResource(
        /* [in] */ Int32 type,
        /* [out] */ Int32* resource);

    /**
     * Return a {@link CharSequence} that best describes the given type,
     * possibly substituting the given {@link #LABEL} value
     * for {@link #TYPE_CUSTOM}.
     */
    CARAPI GetTypeLabel(
        /* [in] */ IResources* res,
        /* [in] */ Int32 type,
        /* [in] */ ICharSequence* label,
        /* [out] */ ICharSequence** lb);
};

} //Provider
} //Droid
} //Elastos

#endif //__CCONTACTSCONTRACTCOMMONDATAKINDSORGANIZATION_H__