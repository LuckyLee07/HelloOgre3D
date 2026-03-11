#ifndef __SHOOT_STATE_H__
#define __SHOOT_STATE_H__

#include "AgentState.h"

class ShootState : public AgentState
{
public:
	ShootState(AgentObject* pAgent);
	virtual ~ShootState();

	void OnEnter() override;
	void OnLeave() override;
	std::string OnUpdate(float deltaTime) override;

private:
	float m_elapsedMs;
	bool m_shotConsumed;
};

#endif  // __SHOOT_STATE_H__
