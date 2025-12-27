#ifndef __AGENT_CONTEXT_H__
#define __AGENT_CONTEXT_H__

#include "OgreVector3.h"

class AgentObject;

struct AgentContext
{
	AgentObject* agent;
	AgentObject* enemy;

	int ammo = 0;
	float health = 0.0f;
	float maxHealth = 0.0f;

	bool hasMovePos = false;
	Ogre::Vector3 movePos;
};


#endif  // __ANIMATION_STATE_H__
