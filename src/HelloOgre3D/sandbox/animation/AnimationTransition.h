#ifndef __ANIMATION_TRANSITION_H__
#define __ANIMATION_TRANSITION_H__

#include <string>

namespace Fancy
{
    class AnimationTransition
    {
    public:
        AnimationTransition(float blendOutWindow = 0.0f, float duration = 0.2f, float blendInWindow = 0.0f);
        ~AnimationTransition();

    private:
        float m_duration;

        std::string m_blendCurve;
        float m_blendInWindow;
        float m_blendOutWindow;
    };
}

#endif  // __ANIMATION_TRANSITION_H__
