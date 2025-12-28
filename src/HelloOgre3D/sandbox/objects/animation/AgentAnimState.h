#ifndef __AGENT_ANIM_STATE_H__
#define __AGENT_ANIM_STATE_H__

#include <string>

class AgentAnim;
class AgentAnimState
{
public:
	AgentAnimState(const std::string& name, AgentAnim* animation, bool looping = false, float rate = 0.0f);
    ~AgentAnimState();
    
    static std::string GetNameByID(int stateId);
    static int GetIDByName(const std::string& stateName);

    void InitAnim(float startTime = 0.0f);
    void ClearAnim();

	float GetAnimWeight() const;
    void StepAnim(float deltaTimeInMillis, float rate = 0.0f);

	float GetRate() const;

    const std::string& GetName() const;
    int GetID() const { return m_stateId; }

public:
	AgentAnim* m_pAnim;
	AgentAnim* GetAnimation() { return m_pAnim; }

private:
    int m_stateId;

    bool m_looping;
    float m_rate;
    std::string m_stateName;
};

#endif  // __AGENT_ANIM_STATE_H__
