#ifndef __OBJECT_MANAGER_H__  
#define __OBJECT_MANAGER_H__

#include <vector>
#include "OISKeyboard.h"
#include "SandboxDef.h"

class AgentObject;
class UIComponent;
class BlockObject;
class EntityObject;
class ScriptLuaVM;
class PhysicsWorld;

class ObjectManager //tolua_exports
{ //tolua_exports
public:
	ObjectManager(PhysicsWorld* pPhysicsWorld);
	~ObjectManager();

	static ObjectManager* GetInstance();

	void Update(int deltaMilliseconds);
	
	void HandleKeyEvent(OIS::KeyCode keycode, unsigned int key);

public:
	void clearAllUIObjects();
	//tolua_begin
	void clearAllAgents();
	void clearAllEntitys();
	void clearAllBlocks(bool forceAll = true);

	const std::vector<AgentObject*>& getAllAgents() { return m_agents; }
	const std::vector<BlockObject*>& getAllBlocks() { return m_blocks; }
	const std::vector<EntityObject*>& getAllEntitys() { return m_entitys; }

	std::vector<AgentObject*> getSpecifyAgents(AGENT_OBJ_TYPE agentType);
	//tolua_end

	void addUIObject(UIComponent* pUIObject);
	void addAgentObject(AgentObject* pAgentObject);
	void addBlockObject(BlockObject* pBlockObject);
	void addEntityObject(EntityObject* pEntityObject);

private:
	unsigned int m_objectIndex;
	unsigned int getNextObjectId() { return ++m_objectIndex; }

	std::vector<AgentObject*> m_agents;
	std::vector<EntityObject*> m_entitys;
	std::vector<BlockObject*> m_blocks;
	std::vector<UIComponent*> m_uicomps;

	ScriptLuaVM* m_pScriptVM;
	PhysicsWorld* m_pPhysicsWorld;
}; //tolua_exports

extern ObjectManager* g_ObjectManager;

#endif; // __OBJECT_MANAGER_H__