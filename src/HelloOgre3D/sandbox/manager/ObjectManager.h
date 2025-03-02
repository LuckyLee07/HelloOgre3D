#ifndef __OBJECT_MANAGER_H__  
#define __OBJECT_MANAGER_H__

#include <map>
#include <vector>
#include "OISKeyboard.h"
#include "SandboxDef.h"

class BaseObject;
class AgentObject;
class UIComponent;
class BlockObject;
class EntityObject;
class VehicleObject;
class ScriptLuaVM;
class PhysicsWorld;

//tolua_begin
enum MGR_OBJ_TYPE
{
	MGR_OBJ_NONE = 0,
	MGR_OBJ_UIOBJ = 1 << 0,
	MGR_OBJ_ENTITY = 1 << 1,
	MGR_OBJ_BLOCK = 1 << 2,
	MGR_OBJ_AGENT = 1 << 3,
	MGR_OBJ_ALLS = 15, // 0x1111
};
//tolua_end

class ObjectManager //tolua_exports
{ //tolua_exports
public:
	ObjectManager(PhysicsWorld* pPhysicsWorld);
	~ObjectManager();

	static ObjectManager* GetInstance();

	void Update(int deltaMilliseconds);
	
	void HandleKeyEvent(OIS::KeyCode keycode, unsigned int key);

public:
	//tolua_begin
	void clearAllObjects(int objType, bool forceAll = true);

	const std::vector<AgentObject*>& getAllAgents() { return m_agents; }
	const std::vector<BlockObject*>& getAllBlocks() { return m_blocks; }
	const std::vector<EntityObject*>& getAllEntitys() { return m_entitys; }

	std::vector<AgentObject*> getSpecifyAgents(AGENT_OBJ_TYPE agentType);
	//tolua_end

	std::vector<VehicleObject*> getAllVehicles();

	void addNewObject(BaseObject* pObject);

	bool removeObjectById(int objid);
	BaseObject* getObjectById(int objid);

	void markNodeRemInSeconds(Ogre::SceneNode* pSceneNode, float seconds);

private:
	void realAddObject(BaseObject* pObject);
	bool realRemoveObject(BaseObject* pObject);

private:
	unsigned int m_objIndex;
	unsigned int getNextObjId() { return ++m_objIndex; }

	std::vector<AgentObject*> m_agents;
	std::vector<EntityObject*> m_entitys;
	std::vector<BlockObject*> m_blocks;
	std::vector<UIComponent*> m_uicomps;

	std::map<int, BaseObject*> m_objects;

	// 存储需要定时删除的RootScene下的Node
	std::unordered_map<Ogre::SceneNode*, int> m_remSceneNodes;

	ScriptLuaVM* m_pScriptVM;
	PhysicsWorld* m_pPhysicsWorld;
}; //tolua_exports

extern ObjectManager* g_ObjectManager;

#endif; // __OBJECT_MANAGER_H__