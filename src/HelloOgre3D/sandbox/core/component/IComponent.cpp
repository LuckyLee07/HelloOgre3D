#include "IComponent.h"
#include "object/GameObject.h"

void IComponent::onAttach(GameObject* owner)
{
	m_gameobj = owner;
}

void IComponent::onDetach()
{
	m_gameobj = nullptr;
}

BaseObject* IComponent::getOwner() const
{
	return static_cast<BaseObject*>(m_gameobj->getUserData());
}