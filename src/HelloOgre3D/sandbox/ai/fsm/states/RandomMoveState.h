#ifndef __RANDOM_MOVE_STATE_H__
#define __RANDOM_MOVE_STATE_H__

#include "AgentState.h"

class RandomMoveState : public AgentState
{
public:
	RandomMoveState(AgentObject* pAgent);
	virtual ~RandomMoveState();

	void OnEnter() override;
	void OnLeave() override;
	std::string OnUpdate(float deltaTime) override;
};

#endif  // __RANDOM_MOVE_STATE_H__
