#ifndef __AGENT_STATE_H__
#define __AGENT_STATE_H__

#include <string>

class AgentObject;

class AgentState
{
public:
	AgentState(AgentObject* pAgent);
	virtual ~AgentState();

	void OnEnter();
	void OnLeave();
	virtual std::string OnUpdate(float deltaTime);

protected:
	AgentObject* m_pAgent;
};


#endif  // __ANIMATION_STATE_H__
