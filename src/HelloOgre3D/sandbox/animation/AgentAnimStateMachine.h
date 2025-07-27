#ifndef __AGENT_ANIM_STATE_MACHINE_H__
#define __AGENT_ANIM_STATE_MACHINE_H__

#include <string>
#include <unordered_map>

class BaseObject;
class AgentAnim;
class AgentAnimState;
class AgentAnimTransition;

class AgentAnimStateMachine //tolua_exports
{ //tolua_exports
public:
	AgentAnimStateMachine(BaseObject* owner);
    ~AgentAnimStateMachine();

    void Update(float deltaTimeInMillis, long long currentTimeInMillis);

    //tolua_begin
    bool RequestState(int stateId);
    bool RequestState(const std::string& stateName);
    
    int GetCurrStateID();
    std::string GetCurrStateName();

    void AddState(const std::string& name, AgentAnim* animation, bool looping = false, float rate = 1.0f);
    void AddTransition(const std::string& fromState, const std::string& toState, float blendOutWindow = 0.0f, float duration = 0.2f, float blendInWindow = 0.0f);
    //tolua_end

    void SetCanFireEvent(bool canFireEvent);
    void FireStateChageEvent(AgentAnimState* pNextState);

private:
    void AddState(AgentAnimState* animState);
    void AddTransition(const std::string& fromState, const std::string& toState, AgentAnimTransition* transition);

    void SetCurrentState(const std::string& stateName);

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
    BaseObject* m_owner; //������
    bool m_canFireEvent; //�ɷ����¼�

    std::unordered_map<std::string, AgentAnimState*> m_animStates;

    typedef std::unordered_map<std::string, AgentAnimTransition*> TransitionMap;
    std::unordered_map<std::string, TransitionMap> m_animTransitions;

	AgentAnimState* m_pCurrState;
	AgentAnimState* m_pNextState;
	AgentAnimTransition* m_pCurrTransition;
    float m_TransitionStartTime;
}; //tolua_exports

#endif  // __AGENT_ANIM_STATE_MACHINE_H__
