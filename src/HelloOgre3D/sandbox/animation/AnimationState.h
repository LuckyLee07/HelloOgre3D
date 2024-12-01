#ifndef __ANIMATION_STATE_H__
#define __ANIMATION_STATE_H__

#include <string>

namespace Fancy
{
    class Animation;
    class AnimationState
    {
    public:
        AnimationState(const std::string& name, Animation* animation, bool looping = false, float rate = 1.0f);
        ~AnimationState();
        
        const std::string& GetName() const;
        float AnimationState::getRate() const;

    public:
        Animation* m_pAnimation;

    private:
        std::string m_stateName;
        bool m_looping;
        float m_rate;
    };
}

#endif  // __ANIMATION_STATE_H__
