#ifndef __KEYFRAMESETBASE_H__
#define __KEYFRAMESETBASE_H__

#include "ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Animation {

class KeyframeSetBase
{
public:

    KeyframeSetBase(
        /* [in] */ ArrayOf<IKeyframe*>* keyframes);

    /**
     * Sets the TypeEvaluator to be used when calculating animated values. This object
     * is required only for KeyframeSets that are not either IntKeyframeSet or FloatKeyframeSet,
     * both of which assume their own evaluator to speed up calculations with those primitive
     * types.
     *
     * @param evaluator The TypeEvaluator to be used to calculate animated values.
     */
    virtual CARAPI SetEvaluator(
        /* [in] */ ITypeEvaluator* evaluator);

    /**
     * Gets the animated value, given the elapsed fraction of the animation (interpolated by the
     * animation's interpolator) and the evaluator used to calculate in-between values. This
     * function maps the input fraction to the appropriate keyframe interval and a fraction
     * between them and returns the interpolated value. Note that the input fraction may fall
     * outside the [0-1] bounds, if the animation's interpolator made that happen (e.g., a
     * spring interpolation that might send the fraction past 1.0). We handle this situation by
     * just using the two keyframes at the appropriate end when the value is outside those bounds.
     *
     * @param fraction The elapsed fraction of the animation
     * @return The animated value.
     */
    virtual CARAPI_(AutoPtr<IInterface>) GetValue(
        /* [in] */ Float fraction);

protected:
    Int32 mNumKeyframes;

    AutoPtr<IKeyframe> mFirstKeyframe;
    AutoPtr<IKeyframe> mLastKeyframe;
    AutoPtr<ITimeInterpolator> mInterpolator; // only used in the 2-keyframe case
    AutoPtr<ArrayOf<IKeyframe*> > mKeyframes; // only used when there are not 2 keyframes
    AutoPtr<ITypeEvaluator> mEvaluator;
};

}// namespace Animation
}// namespace Droid
}// namespace Elastos
#endif
