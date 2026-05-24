#ifndef __BASE_OBJECT__
#define __BASE_OBJECT__

#include <string>
#include "SandboxMacros.h"
#include "object/SandboxObject.h"
#include "object/GameObject.h"
#include "script/LuaClassNameTraits.h"

class IComponent;
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
		return m_pGameObjet != nullptr ? m_pGameObjet->getComponentAs<T>(key) : nullptr;
	}

	template<typename T>
	const T* GetComponentAs(const std::string& key) const
	{
		return m_pGameObjet != nullptr ? m_pGameObjet->getComponentAs<T>(key) : nullptr;
	}

	template<typename T>
	T* FindComponent()
	{
		return m_pGameObjet != nullptr ? m_pGameObjet->findComponent<T>() : nullptr;
	}

	template<typename T>
	const T* FindComponent() const
	{
		return m_pGameObjet != nullptr ? m_pGameObjet->findComponent<T>() : nullptr;
	}

	int GetComponentCount() const;
	std::string BuildComponentDebugString() const;

protected:
	ObjectType m_objType;
	// Historical name kept to avoid a broad rename. Owns the component container.
	GameObject* m_pGameObjet;

private:
	unsigned int m_objId;
	unsigned int m_teamId;
	int m_liveTicks = -1;
	int m_needClearTicks = -1;
	
}; //tolua_exports


#endif  // __BASE_OBJECT__
