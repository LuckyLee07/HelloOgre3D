#include "BaseObject.h"
#include "SandboxMacros.h"
#include "components/agent/AgentAttrib.h"
#include "components/agent/AgentLocomotion.h"
#include "components/ai/AIController.h"
#include "components/anim/AnimComponent.h"
#include "components/combat/WeaponComponent.h"
#include "components/physics/PhysicsComponent.h"
#include <algorithm>
#include <cassert>
#include <sstream>
#include <memory>
#include <vector>

BaseObject::BaseObject()
	: m_services(nullptr), m_objId(0), m_teamId(0), m_liveTicks(0), m_needClearTicks(-1)
{
	m_objType = OBJ_TYPE_NONE;
}

BaseObject::~BaseObject()
{
	for (ComponentMap::iterator iter = m_components.begin(); iter != m_components.end(); ++iter)
	{
		DestroyComponent(iter->second);
	}
	m_components.clear();
}

void BaseObject::DestroyComponent(ComponentPtr& component)
{
	if (!component)
		return;

	assert(component->getLifecycleState() == IComponent::LIFECYCLE_ATTACHED);
	component->setLifecycleState(IComponent::LIFECYCLE_DESTROYING);
	component->onDestroy();
	component->onDetach();
	assert(component->getOwner() == nullptr);
	assert(component->getComponentKey().empty());
	assert(component->getLifecycleState() == IComponent::LIFECYCLE_DETACHED);
	component->setLifecycleState(IComponent::LIFECYCLE_DESTROYED);
	component.reset();
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
	for (ComponentMap::iterator iter = m_components.begin(); iter != m_components.end(); ++iter)
	{
		if (iter->second)
			iter->second->onSandboxServicesChanged(m_services);
	}
}

void BaseObject::SetObjType(ObjectType objType)
{
	m_objType = objType;
}

void BaseObject::Update(int deltaMs)
{
	//if (m_liveTick > 0) m_liveTick--;
	std::vector<IComponent*> components;
	components.reserve(m_components.size());
	for (ComponentMap::iterator iter = m_components.begin(); iter != m_components.end(); ++iter)
	{
		if (iter->second)
			components.push_back(iter->second.get());
	}
	std::stable_sort(components.begin(), components.end(), [](const IComponent* lhs, const IComponent* rhs) {
		return lhs->getUpdateOrder() < rhs->getUpdateOrder();
	});
	for (std::vector<IComponent*>::iterator iter = components.begin(); iter != components.end(); ++iter)
	{
		IComponent* component = *iter;
		bool stillOwned = false;
		for (ComponentMap::iterator ownedIter = m_components.begin(); ownedIter != m_components.end(); ++ownedIter)
		{
			if (ownedIter->second.get() == component)
			{
				stillOwned = true;
				break;
			}
		}
		assert(stillOwned);
		if (!stillOwned)
			continue;

		assert(component->getLifecycleState() == IComponent::LIFECYCLE_ATTACHED);
		if (component->getLifecycleState() != IComponent::LIFECYCLE_ATTACHED)
			continue;

		component->update(deltaMs);
	}
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

	ComponentMap::iterator inserted = m_components.insert(std::make_pair(key, ComponentPtr(comp))).first;
	IComponent* component = inserted->second.get();
	component->setComponentKey(key);
	// Refresh before onAttach so the component is already visible via cached
	// getters during its own attach/start, matching FindComponent<T>() (which
	// sees a component as soon as it is in the map).
	RefreshComponentCache();
	component->setLifecycleState(IComponent::LIFECYCLE_ATTACHING);
	component->onAttach(this);
	assert(component->getOwner() == this);
	assert(component->getLifecycleState() == IComponent::LIFECYCLE_ATTACHED);
	if (component->getOwner() != this || component->getLifecycleState() != IComponent::LIFECYCLE_ATTACHED)
	{
		DestroyComponent(inserted->second);
		m_components.erase(inserted);
		RefreshComponentCache();
		return false;
	}
	if (m_services != nullptr)
		component->onSandboxServicesChanged(m_services);
	component->start();
	return true;
}

bool BaseObject::HasComponent(const std::string& key) const
{
	return m_components.find(key) != m_components.end();
}

bool BaseObject::RemoveComponent(const std::string& key)
{
	ComponentMap::iterator iter = m_components.find(key);
	if (iter == m_components.end())
		return false;

	DestroyComponent(iter->second);
	m_components.erase(iter);
	RefreshComponentCache();
	return true;
}

bool BaseObject::RemoveComponent(IComponent* comp)
{
	if (!comp) return false;

	ComponentMap::iterator iter = m_components.begin();
	for (; iter != m_components.end(); ++iter)
	{
		if (iter->second.get() == comp)
		{
			DestroyComponent(iter->second);
			m_components.erase(iter);
			RefreshComponentCache();
			return true;
		}
	}
	return false;
}

IComponent* BaseObject::GetComponent(const std::string& key)
{
	ComponentMap::iterator iter = m_components.find(key);
	if (iter != m_components.end())
	{
		return iter->second.get();
	}
	return nullptr;
}

const IComponent* BaseObject::GetComponent(const std::string& key) const
{
	ComponentMap::const_iterator iter = m_components.find(key);
	if (iter != m_components.end())
	{
		return iter->second.get();
	}
	return nullptr;
}

void BaseObject::RefreshComponentCache()
{
	m_cachedAI = nullptr;
	m_cachedWeapon = nullptr;
	m_cachedAnim = nullptr;
	m_cachedAttrib = nullptr;
	m_cachedLocomotion = nullptr;
	m_cachedPhysics = nullptr;

	for (ComponentMap::iterator iter = m_components.begin(); iter != m_components.end(); ++iter)
	{
		IComponent* comp = iter->second.get();
		if (comp == nullptr)
			continue;

		if (m_cachedAI == nullptr) { if (AIController* c = dynamic_cast<AIController*>(comp)) { m_cachedAI = c; continue; } }
		if (m_cachedWeapon == nullptr) { if (WeaponComponent* c = dynamic_cast<WeaponComponent*>(comp)) { m_cachedWeapon = c; continue; } }
		if (m_cachedAnim == nullptr) { if (AnimComponent* c = dynamic_cast<AnimComponent*>(comp)) { m_cachedAnim = c; continue; } }
		if (m_cachedAttrib == nullptr) { if (AgentAttrib* c = dynamic_cast<AgentAttrib*>(comp)) { m_cachedAttrib = c; continue; } }
		if (m_cachedLocomotion == nullptr) { if (AgentLocomotion* c = dynamic_cast<AgentLocomotion*>(comp)) { m_cachedLocomotion = c; continue; } }
		if (m_cachedPhysics == nullptr) { if (PhysicsComponent* c = dynamic_cast<PhysicsComponent*>(comp)) { m_cachedPhysics = c; continue; } }
	}
}

AIController* BaseObject::GetAIComponent()
{
	return m_cachedAI;
}

WeaponComponent* BaseObject::GetWeaponComponent()
{
	return m_cachedWeapon;
}

AnimComponent* BaseObject::GetAnimComponent()
{
	return m_cachedAnim;
}

AgentAttrib* BaseObject::GetAttribComponent()
{
	return m_cachedAttrib;
}

AgentLocomotion* BaseObject::GetLocomotionComponent()
{
	return m_cachedLocomotion;
}

PhysicsComponent* BaseObject::GetPhysicsComponent()
{
	return m_cachedPhysics;
}

const AgentLocomotion* BaseObject::GetLocomotionComponent() const
{
	return m_cachedLocomotion;
}

int BaseObject::GetComponentCount() const
{
	return static_cast<int>(m_components.size());
}

std::vector<std::string> BaseObject::GetComponentKeys() const
{
	std::vector<std::string> keys;
	keys.reserve(m_components.size());
	for (ComponentMap::const_iterator iter = m_components.begin(); iter != m_components.end(); ++iter)
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
	for (ComponentMap::const_iterator iter = m_components.begin(); iter != m_components.end(); ++iter)
	{
		if (!first)
			stream << ",";
		stream << iter->first << ":";
		if (iter->second)
			stream << iter->second->getLifecycleStateName();
		else
			stream << "null";
		first = false;
	}
	return stream.str();
}
