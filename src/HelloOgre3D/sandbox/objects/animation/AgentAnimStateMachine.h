#ifndef __AGENT_ANIM_STATE_MACHINE_H__
#define __AGENT_ANIM_STATE_MACHINE_H__

#include <string>
#include <unordered_map>
#include <vector>

class BaseObject;
class AgentAnim;
class AgentAnimState;
class AgentAnimTransition;

class AgentAnimStateMachine //tolua_exports
{ //tolua_exports
public:
	AgentAnimStateMachine(BaseObject* owner);
	AgentAnimStateMachine(BaseObject* owner, bool canFireEvent);
    ~AgentAnimStateMachine();

    void Update(float deltaTimeInMillis, long long currentTimeInMillis);

    //tolua_begin
    bool RequestState(int stateId);
    bool RequestState(const std::string& stateName);

    int GetCurrStateID();
    std::string GetCurrStateName();
    std::string GetNextStateName();
    std::string GetDesiredStateName();
    bool HasNextState() { return m_pNextState != nullptr; }
    bool IsTransitioning() { return m_pCurrTransition != nullptr || m_pNextState != nullptr; }
    bool IsCurrentState(const std::string& stateName);
    bool IsNextState(const std::string& stateName);
    float GetCurrStateTime();
    float GetCurrStateLength();
    float GetCurrStateProgress();

    void AddState(const std::string& name, AgentAnim* animation, bool looping = false, float rate = 1.0f);
    void AddTransition(const std::string& fromState, const std::string& toState, float blendOutWindow = 0.0f, float duration = 0.2f, float blendInWindow = 0.0f);
    //tolua_end

    void AddNotify(const std::string& stateName, const std::string& eventName, float normalizedTime, bool fireOnce = true);
    void SetCanFireEvent(bool canFireEvent);
    void FireStateChageEvent(AgentAnimState* pNextState);

private:
	struct AnimNotify
	{
		std::string eventName;
		float normalizedTime = 0.0f;
		bool fireOnce = true;
		bool fired = false;
	};

    void AddState(AgentAnimState* animState);
    void AddTransition(const std::string& fromState, const std::string& toState, AgentAnimTransition* transition);

    void SetCurrentState(const std::string& stateName);
    void ResetNotifies(const std::string& stateName);
    void FireNotifyEvent(AgentAnimState* state, const AnimNotify& notify);
    void EvaluateNotifies(AgentAnimState* state, float previousTime, float currentTime);
    void StepStateAnimation(AgentAnimState* state, float deltaTimeInMillis);

    bool ContainsState(const std::string& stateName);
    bool ContainsTransition(const std::string& fromState, const std::string& toState);

    void StartTransition(AgentAnimTransition* transition, float currTimeInSeconds);
    void UpdateTransition(float deltaTimeInMillis, float currTimeInSeconds);
    void CompleteTransition();
    void StepCurrentAnimation(float deltaTimeInMillis);

public:
    static void Animation_LinearBlendIn(AgentAnim* startAnim, float blendTime, float startTime, float currTime);
    static void Animation_LinearBlendOut(AgentAnim* startAnim, float blendTime, float startTime, float currTime);
    static void Animation_LinearBlendTo(AgentAnim* startAnim, AgentAnim* endAnim, float blendTime, float startTime, float currTime);

private:
    BaseObject* m_owner = nullptr;	//持有者
    bool m_canFireEvent = false;	//可发送事件

    std::unordered_map<std::string, AgentAnimState*> m_animStates;
    typedef std::unordered_map<std::string, AgentAnimTransition*> TransitionMap;
    std::unordered_map<std::string, TransitionMap> m_animTransitions;
    std::unordered_map<std::string, std::vector<AnimNotify>> m_animNotifies;

	AgentAnimState* m_pCurrState = nullptr;
	AgentAnimState* m_pNextState = nullptr;
	AgentAnimTransition* m_pCurrTransition = nullptr;
    float m_TransitionStartTime = 0.0f;
    std::string m_desiredStateName;
}; //tolua_exports

#endif  // __AGENT_ANIM_STATE_MACHINE_H__