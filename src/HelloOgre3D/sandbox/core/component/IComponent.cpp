#include "IComponent.h"

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
