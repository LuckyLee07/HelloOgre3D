#ifndef __BASE_OBJECT__
#define __BASE_OBJECT__

#include "object/SandboxObject.h"
#include "script/LuaClassNameTraits.h"

class GameObject;
struct Collision;
class btRigidBody;

class BaseObject : public SandboxObject //tolua_exports
{ //tolua_exports
public:
	//tolua_begin
	enum OBJTYPE
	{
		OBJ_TYPE_NONE = 0,
		OBJ_TYPE_UIOBJ,
		OBJ_TYPE_ENTITY,
		// OBJ_BLOCK
		OBJ_TYPE_BLOCK = 10,
		OBJ_TYPE_PLANE,
		OBJ_TYPE_BULLET,
		// OBJ_AGENT
		OBJ_TYPE_AGENT = 20,
		OBJ_TYPE_SOLDIER,
	};
	//tolua_end
public:
	BaseObject();
	virtual ~BaseObject();

	virtual void Initialize() {}

	virtual void update(int deltaMsec);
	virtual btRigidBody* getRigidBody() const;

	//tolua_begin
	void setObjId(unsigned int objId);
	unsigned int getObjId();
	
	void setObjType(OBJTYPE objType);
	OBJTYPE getObjType();
	//tolua_end
	
	int getClearTick() { return m_liveTick; }
	virtual bool checkNeedClear();
	virtual void setNeedClear(int liveTick = 0, bool force = false);

	virtual bool canCollide() { return false; }
	virtual void onCollideWith(BaseObject*, const Collision&);

protected:
	OBJTYPE m_objType;
	GameObject* m_pGameObjet;

private:
	unsigned int m_objId;
	int m_liveTick; //д╛хо-1
}; //tolua_exports


#endif  // __BASE_OBJECT__
