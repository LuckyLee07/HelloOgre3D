#ifndef __SHOOT_STATE_H__
#define __SHOOT_STATE_H__

#include "AgentState.h"

class ShootState : public AgentState
{
public:
	ShootState(AgentObject* pAgent);
	virtual ~ShootState();

	void OnEnter();
	void OnLeave();
	std::string OnUpdate(float deltaTime);
};


#endif  // __SHOOT_STATE_H__
