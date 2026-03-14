#ifndef __DEATH_STATE_H__
#define __DEATH_STATE_H__

#include "AgentState.h"

class DeathState : public AgentState
{
public:
	DeathState(AgentObject* pAgent);
	virtual ~DeathState();

	void OnEnter() override;
	void OnLeave() override;
	std::string OnUpdate(float deltaTime) override;

private:
	float m_elapsedMs;
	bool m_cleanupQueued;
};

#endif  // __DEATH_STATE_H__