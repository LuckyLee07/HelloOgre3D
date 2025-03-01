#ifndef __BASE_OBJECT__
#define __BASE_OBJECT__

#include "driver/SandboxEventDispatcherManager.h"

class btRigidBody;
class Collision;
class BaseObject //tolua_exports
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
	virtual btRigidBody* getRigidBody();

	SandboxEventDispatcherManager* Event();

	//tolua_begin
	void setObjId(unsigned int objId);
	unsigned int getObjId();
	
	void setObjType(OBJTYPE objType);
	OBJTYPE getObjType();
	//tolua_end

	virtual bool checkNeedClear();
	virtual void setNeedClear(int liveTick = 0);

	virtual void onCollideWith(BaseObject*, const Collision&);

protected:
	OBJTYPE m_objType;
private:
	unsigned int m_objId;
	int m_liveTick; //Ĭ��-1

	SandboxEventDispatcherManager* m_eventManager;
}; //tolua_exports


#endif  // __BASE_OBJECT__
