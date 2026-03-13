#ifndef __MOVE_STATE_H__
#define __MOVE_STATE_H__

#include "AgentState.h"
#include "OgreVector3.h"

class MoveState : public AgentState
{
public:
	MoveState(AgentObject* pAgent);
	virtual ~MoveState();

	void OnEnter() override;
	void OnLeave() override;
	std::string OnUpdate(float deltaTime) override;

private:
	float m_stuckMs;
	Ogre::Vector3 m_lastPosition;
};

#endif  // __MOVE_STATE_H__