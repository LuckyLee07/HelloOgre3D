#include "AgentAttrib.h"

#include <algorithm>

#include "event/SandboxContext.h"
#include "objects/AgentObject.h"

AgentAttrib::AgentAttrib()
	: AgentAttrib(100.0f, 100.0f, SOLDIER_STAND, -1)
{
}

AgentAttrib::AgentAttrib(Ogre::Real health, Ogre::Real maxHealth, int stanceType, int pendingStanceType)
	: m_health(health)
	, m_maxHealth(std::max<Ogre::Real>(maxHealth, 1.0f))
	, m_stanceType(stanceType)
	, m_pendingStanceType(pendingStanceType)
	, m_healthChangeEventToken(0)
{
}

void AgentAttrib::onAttach(BaseObject* owner)
{
	IComponent::onAttach(owner);

	AgentObject* agent = dynamic_cast<AgentObject*>(getOwner());
	if (agent == nullptr)
	{
		return;
	}

	agent->Event()->CreateDispatcher("HEALTH_CHANGE");
	m_healthChangeEventToken = agent->Event()->Subscribe("HEALTH_CHANGE", [agent](const SandboxContext& context) -> void {
		double health = context.Get_Number("health");
		if (health <= 0.0 && !agent->GetUseCppFSM()) agent->OnDeath(3.5f);
	});
}

void AgentAttrib::onDetach()
{
	AgentObject* agent = dynamic_cast<AgentObject*>(getOwner());
	if (agent != nullptr)
	{
		agent->Event()->Unsubscribe("HEALTH_CHANGE", m_healthChangeEventToken);
		agent->Event()->RemoveDispatcher("HEALTH_CHANGE");
	}
	m_healthChangeEventToken = 0;
	IComponent::onDetach();
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
