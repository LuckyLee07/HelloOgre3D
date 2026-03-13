#ifndef __PURSUE_STATE_H__
#define __PURSUE_STATE_H__

#include "AgentState.h"
#include "OgreVector3.h"

class PursueState : public AgentState
{
public:
	PursueState(AgentObject* pAgent);
	virtual ~PursueState();

	void OnEnter() override;
	void OnLeave() override;
	std::string OnUpdate(float deltaTime) override;

private:
	float m_repathAccumMs;
	float m_stuckMs;
	float m_stuckRepathMs;
	Ogre::Vector3 m_lastPosition;
};

#endif  // __PURSUE_STATE_H__