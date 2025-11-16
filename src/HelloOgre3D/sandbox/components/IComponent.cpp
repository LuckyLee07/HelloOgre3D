#include "IComponent.h"
#include "object/GameObject.h"

void IComponent::onAttach(GameObject* owner)
{
	m_owner = owner;
}

void IComponent::onDetach()
{
	m_owner = nullptr;
}

void IComponent::update(int deltaMs)
{

}

GameObject* IComponent::getOwner() const
{
	return m_owner;
}