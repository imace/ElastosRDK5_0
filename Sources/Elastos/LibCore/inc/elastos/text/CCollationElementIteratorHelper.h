
#ifndef __ELASTOS_TEXT_CCOLLATIONELEMENTITERATORHELPER_H__
#define __ELASTOS_TEXT_CCOLLATIONELEMENTITERATORHELPER_H__

#include "_Elastos_Text_CCollationElementIteratorHelper.h"

namespace Elastos {
namespace Text {

CarClass(CCollationElementIteratorHelper)
{
public:

    CARAPI PrimaryOrder(
        /* [in] */ Int32 order,
        /* [out] */ Int32* primaryOrder);

    CARAPI SecondaryOrder(
        /* [in] */ Int32 order,
        /* [out] */ Int16* secondaryOrder);

    CARAPI TertiaryOrder(
        /* [in] */ Int32 order,
        /* [out] */ Int16* tertiaryOrder);
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CCOLLATIONELEMENTITERATORHELPER_H__
