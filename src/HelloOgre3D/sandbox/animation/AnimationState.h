#ifndef __ANIMATION_STATE_H__
#define __ANIMATION_STATE_H__

#include <string>

namespace Fancy
{
    class Animation;
    class AnimationState
    {
    public:
        AnimationState(const std::string& name, Animation* animation, bool looping = false, float rate = 0.0f);
        ~AnimationState();
        
        void InitAnimation(float startTime = 0.0f);
        void ClearAnimation();
        float GetAnimationWeight();

        void StepAnimation(float deltaTimeInMillis, float rate = 0.0f);

        const std::string& GetName() const;
        float AnimationState::GetRate() const;

    public:
        Animation* m_pAnimation;
        Animation* GetAnimation() { return m_pAnimation; }

    private:
        std::string m_stateName;
        bool m_looping;
        float m_rate;
    };
}

#endif  // __ANIMATION_STATE_H__
