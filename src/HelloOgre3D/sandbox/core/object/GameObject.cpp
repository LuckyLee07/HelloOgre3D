#include "GameObject.h"
#include "component/IComponent.h"
#include "SandboxMacros.h"

GameObject::GameObject() : m_userdata(nullptr)
{
}

GameObject::~GameObject()
{
	auto iter = m_components.begin();
	for (; iter != m_components.end(); iter++)
	{
		if (iter->second)
		{
			iter->second->onDetach();
			SAFE_DELETE(iter->second);
		}
	}
	m_components.clear();
}

void GameObject::setUserData(void* userdata)
{
	m_userdata = userdata;
}

void* GameObject::getUserData() const
{
	return m_userdata;
}

bool GameObject::addComponent(const std::string& key, IComponent* comp)
{
	if (!comp) return false;
	
	if (m_components.find(key) != m_components.end())
		return false;
	
	m_components[key] = comp;
	comp->onAttach(this);
	comp->start();
	return true;
}

IComponent* GameObject::getComponent(const std::string& key)
{
	auto iter = m_components.find(key);
	if (iter != m_components.end())
	{
		return iter->second;
	}
	return nullptr;
}

bool GameObject::removeComponent(const std::string& key)
{
	auto iter = m_components.find(key);
	if (iter == m_components.end()) 
		return false;
	
	if (iter->second)
	{
		iter->second->onDetach();
		SAFE_DELETE(iter->second);
	}
	m_components.erase(iter);

	return true;
}

bool GameObject::removeComponent(IComponent* comp)
{
	if (!comp) return false;

	auto iter = m_components.begin();
	for (; iter != m_components.end(); iter++)
	{
		if (iter->second == comp)
		{
			iter->second->onDetach();
			SAFE_DELETE(iter->second);

			m_components.erase(iter);
			return true;
		}
	}
	return false;
}

void GameObject::update(int deltaMs)
{
	auto iter = m_components.begin();
	for (; iter != m_components.end(); iter++)
	{
		if (iter->second) 
			iter->second->update(deltaMs);
	}
}