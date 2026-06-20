#ifndef __OBJECT_MANAGER_H__  
#define __OBJECT_MANAGER_H__

#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include "OISKeyboard.h"
#include "GameDefine.h"
#include "OgreColourValue.h"
#include "OgreString.h"
#include "OgreVector3.h"
#include "ai/common/AIUpdateSystem.h"
#include "systems/manager/ObjectLifecycleSystem.h"
#include "SandboxServices.h"

namespace Ogre
{
	class SceneNode;
}
class BaseObject;
class AgentObject;
class BlockObject;
class ScriptLuaVM;
class PhysicsWorld;
class NavigationMesh;
class NavigationService;
class AIScheduler;
class AgentPerceptionSystem;
class AgentSpatialIndexSystem;
class TacticalDebugDrawService;
class TacticalQueryService;
class TacticalService;
class TeamBlackboardService;
class ObjectRegistry;
struct RuntimeObjectUpdateTiming;

//tolua_begin
enum MGR_OBJ_TYPE
{
	MGR_OBJ_NONE = 0,
	MGR_OBJ_BLOCK = 1 << 0,
	MGR_OBJ_AGENT = 1 << 1,
	MGR_OBJ_ALLS = 11, // 0x11
};
//tolua_end

class ObjectManager //tolua_exports
{ //tolua_exports
public:
	ObjectManager(PhysicsWorld* pPhysicsWorld);
	~ObjectManager();

	void Update(int deltaMilliseconds);
	void SetCurrentTimeMs(long long currentTimeMs);
	
	void HandleKeyEvent(OIS::KeyCode keycode, unsigned int key);
	void SetSandboxServices(const SandboxServices& services);
	const SandboxServices& GetSandboxServices() const { return m_services; }

public:
	//tolua_begin
	void clearAllObjects(int objType, bool forceAll = true);

	const std::vector<AgentObject*>& getAllAgents();
	const std::vector<BlockObject*>& getAllBlocks();

	std::vector<AgentObject*> getSpecifyAgents(AGENT_OBJ_TYPE agentType);
	int getObjectCount() const;
	int getAiAgentCount() const;
	int getAiSoldierCount() const;
	std::string buildAiDebugSummary(int maxAgents);
	std::string buildAiEventDebugSummary(int maxAgents, int maxEvents);
	std::string runAiEventScopeSelfTest();
	std::string buildObjectDebugSummary(int maxObjects);
	//tolua_end

	std::vector<BlockObject*> getFixedObjects();

	void addNewObject(BaseObject* pObject);

	bool removeObjectById(int objid);
	BaseObject* getObjectById(int objid);

	void markNodeRemInSeconds(Ogre::SceneNode* pSceneNode, float seconds);

	NavigationMesh* getNavigationMesh(const Ogre::String& navName);
	bool addNavigationMesh(const Ogre::String& navName, NavigationMesh* pNavMesh);
	const AIScheduler* GetAIScheduler() const { return m_aiScheduler; }
	AIScheduler* GetAIScheduler() { return m_aiScheduler; }
	const AgentSpatialIndexSystem* GetAgentSpatialIndexSystem() const { return m_agentSpatialIndex; }
	AgentSpatialIndexSystem* GetAgentSpatialIndexSystem() { return m_agentSpatialIndex; }
	const AgentPerceptionSystem* GetAgentPerceptionSystem() const { return m_agentPerceptionSystem; }
	AgentPerceptionSystem* GetAgentPerceptionSystem() { return m_agentPerceptionSystem; }
	const TeamBlackboardService* GetTeamBlackboardService() const { return m_teamBlackboardService; }
	TeamBlackboardService* GetTeamBlackboardService() { return m_teamBlackboardService; }
	const TacticalQueryService* GetTacticalQueryService() const { return m_tacticalQueryService; }
	TacticalQueryService* GetTacticalQueryService() { return m_tacticalQueryService; }
	const TacticalService* GetTacticalService() const { return m_tacticalService; }
	TacticalService* GetTacticalService() { return m_tacticalService; }

private:
	friend class ObjectLifecycleSystem;

	void realAddObject(BaseObject* pObject);
	bool realRemoveObject(BaseObject* pObject);

	ObjectRegistry* m_registry;
	AIUpdateSystem m_aiUpdateSystem;
	ObjectLifecycleSystem m_objectLifecycleSystem;

	// 存储需要定时删除的RootScene下的Node
	std::unordered_map<Ogre::SceneNode*, int> m_remSceneNodes;

	ScriptLuaVM* m_pScriptVM;
	PhysicsWorld* m_pPhysicsWorld;
	AIScheduler* m_aiScheduler;
	AgentSpatialIndexSystem* m_agentSpatialIndex;
	AgentPerceptionSystem* m_agentPerceptionSystem;
	TeamBlackboardService* m_teamBlackboardService;
	TacticalQueryService* m_tacticalQueryService;
	TacticalDebugDrawService* m_tacticalDebugDrawService;
	TacticalService* m_tacticalService;
	long long m_currentTimeMs;
	SandboxServices m_services;
}; //tolua_exports

#endif; // __OBJECT_MANAGER_H__
