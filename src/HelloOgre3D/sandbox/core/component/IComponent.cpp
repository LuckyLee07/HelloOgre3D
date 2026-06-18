#include "IComponent.h"
#include "object/BaseObject.h"

void IComponent::onAttach(BaseObject* owner)
{
	m_owner = owner;
	m_lifecycleState = LIFECYCLE_ATTACHED;
}

void IComponent::onDetach()
{
	m_owner = nullptr;
	m_componentKey.clear();
	m_lifecycleState = LIFECYCLE_DETACHED;
}

BaseObject* IComponent::getOwner() const
{
	return m_owner;
}

const SandboxServices* IComponent::GetSandboxServices() const
{
	return m_owner != nullptr ? m_owner->GetSandboxServices() : nullptr;
}

const char* IComponent::getLifecycleStateName() const
{
	switch (m_lifecycleState)
	{
	case LIFECYCLE_CONSTRUCTED: return "constructed";
	case LIFECYCLE_ATTACHING: return "attaching";
	case LIFECYCLE_ATTACHED: return "attached";
	case LIFECYCLE_DESTROYING: return "destroying";
	case LIFECYCLE_DETACHED: return "detached";
	case LIFECYCLE_DESTROYED: return "destroyed";
	default: return "unknown";
	}
}
