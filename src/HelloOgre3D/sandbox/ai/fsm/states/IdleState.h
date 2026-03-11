#ifndef __IDLE_STATE_H__
#define __IDLE_STATE_H__

#include "AgentState.h"

class IdleState : public AgentState
{
public:
	IdleState(AgentObject* pAgent);
	virtual ~IdleState();

	void OnEnter() override;
	void OnLeave() override;
	std::string OnUpdate(float deltaTime) override;

private:
	float m_elapsedMs;
};

#endif  // __IDLE_STATE_H__
