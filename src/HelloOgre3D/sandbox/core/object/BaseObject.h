#ifndef __BASE_OBJECT__
#define __BASE_OBJECT__

#include <string>
#include <map>
#include <memory>
#include <vector>
#include "SandboxMacros.h"
#include "component/IComponent.h"
#include "object/SandboxObject.h"
#include "script/LuaClassNameTraits.h"

struct Collision;
struct SandboxServices;
class AgentAttrib;
class AgentLocomotion;
class AIController;
class AnimComponent;
class PhysicsComponent;
class WeaponComponent;

class BaseObject : public SandboxObject //tolua_exports
{ //tolua_exports
protected:
	typedef std::unique_ptr<IComponent> ComponentPtr;
	typedef std::map<std::string, ComponentPtr> ComponentMap;

public:
	enum ObjectType
	{
		OBJ_TYPE_NONE = 0,
		// OBJ_BLOCK
		OBJ_TYPE_BLOCK = 10,
		OBJ_TYPE_PLANE,
		OBJ_TYPE_BULLET,
		// OBJ_AGENT
		OBJ_TYPE_AGENT = 20,
		OBJ_TYPE_SOLDIER,
	};
public:
	BaseObject();
	virtual ~BaseObject();

	virtual void Init() {}
	virtual void Update(int deltaMs);
	virtual void OnDestroy();

	//tolua_begin
	unsigned int GetObjId() const;
	unsigned int GetTeamId() const;
	void SetTeamId(unsigned int teamId);
	bool HasComponent(const std::string& key) const;
	int GetComponentCount() const;
	std::string BuildComponentDebugString() const;
	AIController* GetAIComponent();
	WeaponComponent* GetWeaponComponent();
	AnimComponent* GetAnimComponent();
	AgentAttrib* GetAttribComponent();
	AgentLocomotion* GetLocomotionComponent();
	//tolua_end

	// Non-tolua C++ accessors for cached hot components (kept off the Lua
	// surface; Lua reads transforms through the object getters). The const
	// GetLocomotionComponent lets const-context hot paths (e.g. perception
	// sight origin) read the cache instead of scanning + dynamic_cast.
	PhysicsComponent* GetPhysicsComponent();
	const AgentLocomotion* GetLocomotionComponent() const;

	void SetObjId(unsigned int objId);
	void SetSandboxServices(const SandboxServices* services);
	const SandboxServices* GetSandboxServices() const { return m_services; }
	
	ObjectType GetObjType();
	void SetObjType(ObjectType objType);

	float GetMass() const { return 0.0f; }
	
	int GetLiveTicks() { return m_liveTicks; }
	virtual bool CheckNeedClear();
	virtual void SetNeedClear(int delay_ticks = 0, bool force = false);

	virtual void CollideWithObject(BaseObject* pCollideObj, const Collision& collision);

	// BaseObject owns its component container. Components added here are owned by
	// the container and deleted when removed or when the object is destroyed.
	bool AddComponent(const std::string& key, IComponent* comp);
	bool RemoveComponent(const std::string& key);
	bool RemoveComponent(IComponent* comp);

	IComponent* GetComponent(const std::string& key);
	const IComponent* GetComponent(const std::string& key) const;

	template<typename T>
	T* GetComponentAs(const std::string& key)
	{
		return dynamic_cast<T*>(GetComponent(key));
	}

	template<typename T>
	const T* GetComponentAs(const std::string& key) const
	{
		return dynamic_cast<const T*>(GetComponent(key));
	}

	template<typename T>
	T* FindComponent()
	{
		for (ComponentMap::iterator iter = m_components.begin(); iter != m_components.end(); ++iter)
		{
			T* component = dynamic_cast<T*>(iter->second.get());
			if (component != nullptr)
				return component;
		}
		return nullptr;
	}

	template<typename T>
	const T* FindComponent() const
	{
		for (ComponentMap::const_iterator iter = m_components.begin(); iter != m_components.end(); ++iter)
		{
			const T* component = dynamic_cast<const T*>(iter->second.get());
			if (component != nullptr)
				return component;
		}
		return nullptr;
	}

	std::vector<std::string> GetComponentKeys() const;

protected:
	ObjectType m_objType;
	// BaseObject owns every component inserted through AddComponent.
	ComponentMap m_components;

	// Non-owning cached pointers to hot components, refreshed on every
	// AddComponent/RemoveComponent. Per-frame getters read these directly
	// instead of scanning the whole component map + dynamic_cast each call.
	// Semantics match FindComponent<T>(): first component of that type in map
	// iteration (key sort) order. Owned by m_components; never freed here.
	AIController* m_cachedAI = nullptr;
	WeaponComponent* m_cachedWeapon = nullptr;
	AnimComponent* m_cachedAnim = nullptr;
	AgentAttrib* m_cachedAttrib = nullptr;
	AgentLocomotion* m_cachedLocomotion = nullptr;
	PhysicsComponent* m_cachedPhysics = nullptr;

private:
	void DestroyComponent(ComponentPtr& component);
	// Rebuild m_cached* from the current component map. Called whenever the
	// component set changes so cached pointers never dangle or go stale.
	void RefreshComponentCache();

	const SandboxServices* m_services;
	unsigned int m_objId;
	unsigned int m_teamId;
	int m_liveTicks = -1;
	int m_needClearTicks = -1;
	
}; //tolua_exports


#endif  // __BASE_OBJECT__
