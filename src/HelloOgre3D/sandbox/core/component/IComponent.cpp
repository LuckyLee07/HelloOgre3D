#include "IComponent.h"
#include "object/GameObject.h"

void IComponent::onAttach(GameObject* gameobj)
{
	m_gameobj = gameobj;
}

void IComponent::onDetach()
{
	m_gameobj = nullptr;
	m_componentKey.clear();
}

BaseObject* IComponent::getOwner() const
{
	if (m_gameobj == nullptr)
		return nullptr;
	return static_cast<BaseObject*>(m_gameobj->getUserData());
}
