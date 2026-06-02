#ifndef __OBJECT_MANAGER_H__  
#define __OBJECT_MANAGER_H__

#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include "OISKeyboard.h"
#include "GameDefine.h"
#include "OgreString.h"
#include "OgreVector3.h"
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
class AIScheduler;
class AgentPerceptionSystem;
class AgentSpatialIndexSystem;
class TeamBlackboardService;

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

	static ObjectManager* GetInstance();

	void Update(int deltaMilliseconds);
	
	void HandleKeyEvent(OIS::KeyCode keycode, unsigned int key);
	void SetSandboxServices(const SandboxServices& services);
	const SandboxServices& GetSandboxServices() const { return m_services; }

public:
	//tolua_begin
	void clearAllObjects(int objType, bool forceAll = true);

	const std::vector<AgentObject*>& getAllAgents() { return m_agents; }
	const std::vector<BlockObject*>& getAllBlocks() { return m_blocks; }

	std::vector<AgentObject*> getSpecifyAgents(AGENT_OBJ_TYPE agentType);
	int getObjectCount() const;
	int getAiAgentCount() const;
	int getAiSoldierCount() const;
	std::string buildAiDebugSummary(int maxAgents);
	void configureAiScheduler(bool enabled, int tickIntervalMs, int maxTicksPerFrame);
	std::string buildAiSchedulerDebugSummary() const;
	void clearTeamBlackboardFacts();
	void configureTeamBlackboard(int ttlMs);
	bool rememberTeamEnemyFact(int teamId, int reporterId, int targetId, const Ogre::Vector3& targetPosition, int lastSeenMs, float confidence);
	bool writeBestTeamEnemyFactToBlackboard(AgentObject* agent, const std::string& keyPrefix, bool allowOwnReport = false);
	int getTeamBlackboardFactCount() const;
	int getTeamBlackboardReportCount() const;
	std::string buildTeamBlackboardDebugSummary() const;
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
	const AgentSpatialIndexSystem* GetAgentSpatialIndexSystem() const { return m_agentSpatialIndex; }
	AgentSpatialIndexSystem* GetAgentSpatialIndexSystem() { return m_agentSpatialIndex; }
	const AgentPerceptionSystem* GetAgentPerceptionSystem() const { return m_agentPerceptionSystem; }
	AgentPerceptionSystem* GetAgentPerceptionSystem() { return m_agentPerceptionSystem; }
	const TeamBlackboardService* GetTeamBlackboardService() const { return m_teamBlackboardService; }
	TeamBlackboardService* GetTeamBlackboardService() { return m_teamBlackboardService; }

private:
	void realAddObject(BaseObject* pObject);
	bool realRemoveObject(BaseObject* pObject);

private:
	unsigned int m_objIndex;
	unsigned int getNextObjId() { return ++m_objIndex; }

	std::vector<AgentObject*> m_agents;
	std::vector<BlockObject*> m_blocks;
	std::unordered_map<int, BaseObject*> m_objects;

	// 存储需要定时删除的RootScene下的Node
	std::unordered_map<Ogre::SceneNode*, int> m_remSceneNodes;

	std::unordered_map<Ogre::String, NavigationMesh*> m_navMeshes;

	ScriptLuaVM* m_pScriptVM;
	PhysicsWorld* m_pPhysicsWorld;
	AIScheduler* m_aiScheduler;
	AgentSpatialIndexSystem* m_agentSpatialIndex;
	AgentPerceptionSystem* m_agentPerceptionSystem;
	TeamBlackboardService* m_teamBlackboardService;
	SandboxServices m_services;
}; //tolua_exports

extern ObjectManager* g_ObjectManager;

#endif; // __OBJECT_MANAGER_H__
