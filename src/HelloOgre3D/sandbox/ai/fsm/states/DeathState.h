#ifndef __DEATH_STATE_H__
#define __DEATH_STATE_H__

#include "AgentState.h"

class DeathState : public AgentState
{
public:
	DeathState(AgentObject* pAgent);
	virtual ~DeathState();

	void OnEnter();
	void OnLeave();
	std::string OnUpdate(float deltaTime);
};


#endif  // __DEATH_STATE_H__
