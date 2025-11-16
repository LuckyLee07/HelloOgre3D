#ifndef __AGENT_STATE_H__
#define __AGENT_STATE_H__

#include "object/SandboxObject.h"
#include <string>

class AgentObject;

class AgentState : public SandboxObject
{
public:
	AgentState(AgentObject* pAgent);
	virtual ~AgentState();

	virtual void OnEnter();
	virtual void OnLeave();
	
	virtual std::string OnUpdate(float dt) = 0;

	virtual void doBeforeEntering() {}
	virtual void doBeforeLeaving() {}

	void SetStateId(const std::string& stateId);
	std::string GetStateId() const { return m_stateId; }

protected:
	std::string m_stateId;

	AgentObject* m_pAgent;
};


#endif  // __ANIMATION_STATE_H__
