#ifndef __FLEE_STATE_H__
#define __FLEE_STATE_H__

#include "AgentState.h"
#include "OgreVector3.h"

class FleeState : public AgentState
{
public:
	FleeState(AgentObject* pAgent);
	virtual ~FleeState();

	void OnEnter() override;
	void OnLeave() override;
	std::string OnUpdate(float deltaTime) override;

private:
	float m_stuckMs;
	Ogre::Vector3 m_lastPosition;
};

#endif  // __FLEE_STATE_H__