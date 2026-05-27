#ifndef __BASE_OBJECT__
#define __BASE_OBJECT__

#include <string>
#include <map>
#include <vector>
#include "SandboxMacros.h"
#include "component/IComponent.h"
#include "object/SandboxObject.h"
#include "script/LuaClassNameTraits.h"

struct Collision;

class BaseObject : public SandboxObject //tolua_exports
{ //tolua_exports
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
	//tolua_end

	void SetObjId(unsigned int objId);
	
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
	bool HasComponent(const std::string& key) const;
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
		for (std::map<std::string, IComponent*>::iterator iter = m_components.begin(); iter != m_components.end(); ++iter)
		{
			T* component = dynamic_cast<T*>(iter->second);
			if (component != nullptr)
				return component;
		}
		return nullptr;
	}

	template<typename T>
	const T* FindComponent() const
	{
		for (std::map<std::string, IComponent*>::const_iterator iter = m_components.begin(); iter != m_components.end(); ++iter)
		{
			const T* component = dynamic_cast<const T*>(iter->second);
			if (component != nullptr)
				return component;
		}
		return nullptr;
	}

	int GetComponentCount() const;
	std::vector<std::string> GetComponentKeys() const;
	std::string BuildComponentDebugString() const;

protected:
	ObjectType m_objType;
	// BaseObject owns every component inserted through AddComponent.
	std::map<std::string, IComponent*> m_components;

private:
	unsigned int m_objId;
	unsigned int m_teamId;
	int m_liveTicks = -1;
	int m_needClearTicks = -1;
	
}; //tolua_exports


#endif  // __BASE_OBJECT__
