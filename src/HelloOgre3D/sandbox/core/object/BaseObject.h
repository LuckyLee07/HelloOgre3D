#ifndef __BASE_OBJECT__
#define __BASE_OBJECT__

#include "SandboxMacros.h"
#include "object/SandboxObject.h"
#include "script/LuaClassNameTraits.h"

class IComponent;
class GameObject;
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
	unsigned int GetObjId();
	//tolua_end
	void SetObjId(unsigned int objId);
	
	ObjectType GetObjType();
	void SetObjType(ObjectType objType);

	float GetMass() const { return 0.0f; }
	
	int GetLiveTicks() { return m_liveTicks; }
	virtual bool CheckNeedClear();
	virtual void SetNeedClear(int delay_ticks = 0, bool force = false);

	virtual void CollideWithObject(BaseObject* pCollideObj, const Collision& collision);

	bool AddComponent(const std::string& key, IComponent* comp);
	bool RemoveComponent(IComponent* comp);

	IComponent* GetComponent(const std::string& key);

protected:
	ObjectType m_objType;
	GameObject* m_pGameObjet;

private:
	unsigned int m_objId;
	int m_liveTicks = -1;
	int m_needClearTicks = -1;
	
}; //tolua_exports


#endif  // __BASE_OBJECT__
