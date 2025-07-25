#ifndef __MOVE_STATE_H__
#define __MOVE_STATE_H__

#include "AgentState.h"

class MoveState : public AgentState
{
public:
	MoveState(AgentObject* pAgent);
	virtual ~MoveState();

	void OnEnter();
	void OnLeave();
	std::string OnUpdate(float deltaTime);
};


#endif  // __MOVE_STATE_H__
