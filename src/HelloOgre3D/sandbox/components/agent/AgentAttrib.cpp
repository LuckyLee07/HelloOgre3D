#include "AgentAttrib.h"

#include <algorithm>

AgentAttrib::AgentAttrib()
	: AgentAttrib(100.0f, 100.0f, SOLDIER_STAND, -1)
{
}

AgentAttrib::AgentAttrib(Ogre::Real health, Ogre::Real maxHealth, int stanceType, int pendingStanceType)
	: m_health(health)
	, m_maxHealth(std::max<Ogre::Real>(maxHealth, 1.0f))
	, m_stanceType(stanceType)
	, m_pendingStanceType(pendingStanceType)
{
}

void AgentAttrib::SetHealth(Ogre::Real health)
{
	m_health = health;
}

void AgentAttrib::SetMaxHealth(Ogre::Real maxHealth)
{
	m_maxHealth = std::max<Ogre::Real>(maxHealth, 1.0f);
}

void AgentAttrib::SetStanceType(int stanceType)
{
	if (stanceType == SOLDIER_STAND || stanceType == SOLDIER_CROUCH)
	{
		m_stanceType = stanceType;
	}
}

void AgentAttrib::SyncFromBackup(Ogre::Real health, Ogre::Real maxHealth, int stanceType, int pendingStanceType)
{
	SetHealth(health);
	SetMaxHealth(maxHealth);
	SetStanceType(stanceType);
	SetPendingStanceType(pendingStanceType);
}
