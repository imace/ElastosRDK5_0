
#ifndef __CEMBOSSMASKFILTER_H__
#define __CEMBOSSMASKFILTER_H__

#include "_CEmbossMaskFilter.h"
#include "graphics/MaskFilter.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CEmbossMaskFilter), public MaskFilter
{
public:
    /**
     * Create an emboss maskfilter
     *
     * @param direction  array of 3 scalars [x, y, z] specifying the direction of the light source
     * @param ambient    0...1 amount of ambient light
     * @param specular   coefficient for specular highlights (e.g. 8)
     * @param blurRadius amount to blur before applying lighting (e.g. 3)
     * @return           the emboss maskfilter
     */
    CARAPI constructor(
        /* [in] */ const ArrayOf<Float>& direction,
        /* [in] */ Float ambient,
        /* [in] */ Float specular,
        /* [in] */ Float blurRadius);

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

private:
    static CARAPI_(Int32) NativeConstructor(
        /* [in] */ const ArrayOf<Float>& dirArray,
        /* [in] */ Float ambient,
        /* [in] */ Float specular,
        /* [in] */ Float blurRadius);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __CEMBOSSMASKFILTER_H__
