#ifndef __RELOAD_STATE_H__
#define __RELOAD_STATE_H__

#include "AgentState.h"

class ReloadState : public AgentState
{
public:
	ReloadState(AgentObject* pAgent);
	virtual ~ReloadState();

	void OnEnter() override;
	void OnLeave() override;
	std::string OnUpdate(float deltaTime) override;

private:
	float m_elapsedMs;
};

#endif  // __RELOAD_STATE_H__
