#ifndef __AGENT_STATE_H__
#define __AGENT_STATE_H__

#include "GameObject.h"
#include <string>

class AgentObject;

class AgentState : public GameObject
{
public:
	AgentState(AgentObject* pAgent);
	virtual ~AgentState();

	virtual void OnEnter();
	virtual void OnLeave();
	
	virtual std::string OnUpdate(float dt) = 0;

	virtual void doBeforeEntering() {}
	virtual void doBeforeLeaving() {}

	std::string GetName() const { return m_stateId; }

protected:
	std::string m_stateId;

	AgentObject* m_pAgent;
};


#endif  // __ANIMATION_STATE_H__
