#ifndef __IDLE_STATE_H__
#define __IDLE_STATE_H__

#include "AgentState.h"

class IdleState : public AgentState
{
public:
	IdleState(AgentObject* pAgent);
	virtual ~IdleState();

	void OnEnter();
	void OnLeave();
	std::string OnUpdate(float deltaTime);
};


#endif  // __IDLE_STATE_H__
