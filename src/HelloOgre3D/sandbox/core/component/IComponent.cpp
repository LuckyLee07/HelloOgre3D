#include "IComponent.h"
#include "object/BaseObject.h"

void IComponent::onAttach(BaseObject* owner)
{
	m_owner = owner;
}

void IComponent::onDetach()
{
	m_owner = nullptr;
	m_componentKey.clear();
}

BaseObject* IComponent::getOwner() const
{
	return m_owner;
}

const SandboxServices* IComponent::GetSandboxServices() const
{
	return m_owner != nullptr ? m_owner->GetSandboxServices() : nullptr;
}
