#ifndef __AGENT_STATE_H__
#define __AGENT_STATE_H__

#include <string>

class AgentObject;

class AgentState
{
public:
	AgentState(AgentObject* pAgent);
	virtual ~AgentState();

	virtual void OnEnter() = 0;
	virtual void OnLeave() = 0;
	virtual std::string OnUpdate(float dt) = 0;

	std::string GetName() const { return m_stateId; }

protected:
	std::string m_stateId;

	AgentObject* m_pAgent;
};


#endif  // __ANIMATION_STATE_H__
