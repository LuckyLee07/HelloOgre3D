#ifndef __ANIMATION_STATE_MACHINE_H__
#define __ANIMATION_STATE_MACHINE_H__

#include <string>
#include <unordered_map>

namespace Fancy //tolua_exports
{ //tolua_exports
    class Animation;
    class AnimationState;
    class AnimationTransition;
    class AnimationStateMachine //tolua_exports
    { //tolua_exports
    public:
        AnimationStateMachine();
        ~AnimationStateMachine();

        void Update(float deltaTimeInMillis, long long currentTimeInMillis);

        //tolua_begin
        void RequestState(const std::string& stateName);

        void AddState(const std::string& name, Animation* animation, bool looping = false, float rate = 1.0f);
        void AddTransition(const std::string& fromState, const std::string& toState, float blendOutWindow = 0.0f, float duration = 0.2f, float blendInWindow = 0.0f);
        //tolua_end

    private:
        void AddState(AnimationState* animState);
        void AddTransition(const std::string& fromState, const std::string& toState, AnimationTransition* transition);

        void SetCurrentState(const std::string& stateName);

        bool ContainsState(const std::string& stateName);
        bool ContainsTransition(const std::string& fromState, const std::string& toState);

        void StartTransition(AnimationTransition* transition, float currTimeInSeconds);
        void UpdateTransition(float deltaTimeInMillis, float currTimeInSeconds);
        void CompleteTransition();
        void StepCurrentAnimation(float deltaTimeInMillis);

    private:
        std::unordered_map<std::string, AnimationState*> m_animStates;
        typedef std::unordered_map<std::string, AnimationTransition*> TransitionMap;
        std::unordered_map<std::string, TransitionMap> m_animTransitions;

        AnimationState* m_pCurrentState;
        AnimationState* m_pNextState;
        AnimationTransition* m_pCurrentTransition;
        float m_TransitionStartTime;
    }; //tolua_exports
} //tolua_exports

#endif  // __ANIMATION_STATE_MACHINE_H__
