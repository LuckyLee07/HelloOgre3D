#include "BaseObject.h"
#include "SandboxMacros.h"
#include <sstream>

namespace
{
	void DestroyComponent(IComponent* component)
	{
		if (component == nullptr)
			return;

		component->onDestroy();
		component->onDetach();
		SAFE_DELETE(component);
	}
}

BaseObject::BaseObject()
	: m_services(nullptr), m_objId(0), m_teamId(0), m_liveTicks(0), m_needClearTicks(-1)
{
	m_objType = OBJ_TYPE_NONE;
}

BaseObject::~BaseObject()
{
	for (std::map<std::string, IComponent*>::iterator iter = m_components.begin(); iter != m_components.end(); ++iter)
	{
		DestroyComponent(iter->second);
	}
	m_components.clear();
}

unsigned int BaseObject::GetObjId() const
{
	return m_objId;
}

unsigned int BaseObject::GetTeamId() const
{
	return m_teamId;
}

void BaseObject::SetTeamId(unsigned int teamId)
{
	m_teamId = teamId;
}

BaseObject::ObjectType BaseObject::GetObjType()
{
	return m_objType;
}

void BaseObject::SetObjId(unsigned int objId)
{
	m_objId = objId;
}

void BaseObject::SetSandboxServices(const SandboxServices* services)
{
	m_services = services;
}

void BaseObject::SetObjType(ObjectType objType)
{
	m_objType = objType;
}

void BaseObject::Update(int deltaMs)
{
	(void)deltaMs;
	//if (m_liveTick > 0) m_liveTick--;
}

void BaseObject::OnDestroy()
{

}

bool BaseObject::CheckNeedClear()
{
	if (m_needClearTicks > 0)
		m_needClearTicks--;

	if (m_needClearTicks == 0)
	{
		m_needClearTicks = -1;
		return true;
	}
	return false;
}

// 在特殊场景下强制重置销毁状态，确保对象能被立即回收，而无需等待之前设置的 liveTick
void BaseObject::SetNeedClear(int delay_ticks, bool force)
{
	if (!force && m_needClearTicks >= 0) return;
	
	m_needClearTicks = delay_ticks;
}

void BaseObject::CollideWithObject(BaseObject* pCollideObj, const Collision& collision)
{
	(void)pCollideObj;
	(void)collision;
	// CollideWithObject
}

bool BaseObject::AddComponent(const std::string& key, IComponent* comp)
{
	if (!comp) return false;

	if (m_components.find(key) != m_components.end())
		return false;

	if (comp->getOwner() != nullptr)
		return false;

	m_components[key] = comp;
	comp->setComponentKey(key);
	comp->onAttach(this);
	comp->start();
	return true;
}

bool BaseObject::HasComponent(const std::string& key) const
{
	return m_components.find(key) != m_components.end();
}

bool BaseObject::RemoveComponent(const std::string& key)
{
	std::map<std::string, IComponent*>::iterator iter = m_components.find(key);
	if (iter == m_components.end())
		return false;

	DestroyComponent(iter->second);
	m_components.erase(iter);
	return true;
}

bool BaseObject::RemoveComponent(IComponent* comp)
{
	if (!comp) return false;

	std::map<std::string, IComponent*>::iterator iter = m_components.begin();
	for (; iter != m_components.end(); ++iter)
	{
		if (iter->second == comp)
		{
			DestroyComponent(iter->second);
			m_components.erase(iter);
			return true;
		}
	}
	return false;
}

IComponent* BaseObject::GetComponent(const std::string& key)
{
	std::map<std::string, IComponent*>::iterator iter = m_components.find(key);
	if (iter != m_components.end())
	{
		return iter->second;
	}
	return nullptr;
}

const IComponent* BaseObject::GetComponent(const std::string& key) const
{
	std::map<std::string, IComponent*>::const_iterator iter = m_components.find(key);
	if (iter != m_components.end())
	{
		return iter->second;
	}
	return nullptr;
}

int BaseObject::GetComponentCount() const
{
	return static_cast<int>(m_components.size());
}

std::vector<std::string> BaseObject::GetComponentKeys() const
{
	std::vector<std::string> keys;
	keys.reserve(m_components.size());
	for (std::map<std::string, IComponent*>::const_iterator iter = m_components.begin(); iter != m_components.end(); ++iter)
	{
		keys.push_back(iter->first);
	}
	return keys;
}

std::string BaseObject::BuildComponentDebugString() const
{
	if (m_components.empty())
		return "-";

	std::ostringstream stream;
	bool first = true;
	for (std::map<std::string, IComponent*>::const_iterator iter = m_components.begin(); iter != m_components.end(); ++iter)
	{
		if (!first)
			stream << ",";
		stream << iter->first << (iter->second != nullptr ? "" : ":null");
		first = false;
	}
	return stream.str();
}
