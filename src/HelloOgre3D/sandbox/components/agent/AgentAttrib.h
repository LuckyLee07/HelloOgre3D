#ifndef __AGENT_ATTRIB_H__
#define __AGENT_ATTRIB_H__

#include "component/IComponent.h"
#include "GameDefine.h"
#include "OgrePrerequisites.h"

class AgentAttrib : public IComponent
{
public:
	AgentAttrib();
	AgentAttrib(Ogre::Real health, Ogre::Real maxHealth, int stanceType, int pendingStanceType = -1);
	virtual ~AgentAttrib() {}

	void SetHealth(Ogre::Real health);
	Ogre::Real GetHealth() const { return m_health; }

	void SetMaxHealth(Ogre::Real maxHealth);
	Ogre::Real GetMaxHealth() const { return m_maxHealth; }

	void SetStanceType(int stanceType);
	int GetStanceType() const { return m_stanceType; }

	void SetPendingStanceType(int stanceType) { m_pendingStanceType = stanceType; }
	int GetPendingStanceType() const { return m_pendingStanceType; }
	void ClearPendingStanceType() { m_pendingStanceType = -1; }

	void SyncFromBackup(Ogre::Real health, Ogre::Real maxHealth, int stanceType, int pendingStanceType);

private:
	Ogre::Real m_health;
	Ogre::Real m_maxHealth;
	int m_stanceType;
	int m_pendingStanceType;
};

#endif // __AGENT_ATTRIB_H__
