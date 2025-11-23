#include "IComponent.h"
#include "object/GameObject.h"

void IComponent::onAttach(GameObject* gameobj)
{
	m_gameobj = gameobj;
}

void IComponent::onDetach()
{
	m_gameobj = nullptr;
}

BaseObject* IComponent::getOwner() const
{
	return static_cast<BaseObject*>(m_gameobj->getUserData());
}