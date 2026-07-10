#ifndef __AI_CONTROLLER_H__
#define __AI_CONTROLLER_H__

#include <string>

#include "OgreString.h"
#include "OgreVector3.h"
#include "ai/common/Blackboard.h"
#include "ai/perception/MemoryStore.h"
#include "ai/perception/PerceptionResultCache.h"
#include "ai/perception/VisionSensor.h"
#include "component/IComponent.h"
#include "components/control/IAgentController.h"
#include "script/LuaClassNameTraits.h"

class AgentObject;
class AgentStateController;
class BehaviorTreeDriver;
class DecisionTreeDriver;
class IDecisionDriver;
class SoldierObject;
struct AgentPerceptionOptions;
struct AgentPerceptionResult;
struct AICommand;

struct AIPerceptionTickStats
{
	AIPerceptionTickStats()
		: scanned(false)
		, hasVisibleTarget(false)
		, memoryMs(0.0)
		, visionMs(0.0)
	{
	}

	bool scanned;
	bool hasVisibleTarget;
	double memoryMs;
	double visionMs;
};

class AIController : public IComponent, public IAgentController //tolua_exports
{ //tolua_exports
public:
	explicit AIController(BaseObject* owner = nullptr);
	virtual ~AIController();

	virtual void onAttach(BaseObject* owner) override;
	virtual void onDetach() override;
	virtual void onSandboxServicesChanged(const SandboxServices* services) override;
	virtual int getUpdateOrder() const override;
	virtual void update(int deltaMs) override;
	virtual AgentControllerKind GetControllerKind() const override { return AGENT_CONTROLLER_AI; }
	virtual const char* GetControllerName() const override { return "AIController"; }

	//tolua_begin
	AgentObject* GetAgentOwner() const;
	Blackboard* GetBlackboard() const;

	AgentObject* GetEnemy() const;
	bool HasEnemy(const Ogre::String& navMeshName = "default");
	bool CanShootEnemy(const Ogre::String& navMeshName = "default", float shootDistance = 3.0f);

	bool HasMovePosition(float reachDistance = 1.5f) const;
	void SetMovePosition(const Ogre::Vector3& movePos);
	void ClearMovePosition();
	bool IsTargetReached(float threshold) const;

	void SetDriverByType(const char* type);
	DecisionTreeDriver* GetDecisionTreeDriver() const;

	BehaviorTreeDriver* GetBehaviorTreeDriver() const;
	//tolua_end

	SoldierObject* GetOwner() const;
	AgentStateController* GetFsmController() const;
	std::string BuildSensorDebugString() const;
	std::string BuildMemoryDebugString() const;

	// 感知结果缓存（单一事实来源）。第一切片：增量观测，由 TickPerception 在扫描后填充。
	const PerceptionResultCache& GetPerceptionCache() const { return m_perceptionCache; }
	void SetPerceptionResultCacheEnabled(bool enabled);
	bool IsPerceptionResultCacheEnabled() const { return m_perceptionCacheEnabled; }

	void IssueCommand(const AICommand& command);
	unsigned int GetAgentId() const;
	void TickPerception(int deltaMs, AIPerceptionTickStats* outStats = nullptr);
	void TickAI(int deltaMs);
	void SetTickInOwnerUpdateEnabled(bool enabled);
	bool IsTickInOwnerUpdateEnabled() const { return m_tickInOwnerUpdateEnabled; }
	void SetPerceptionTickInAiTickEnabled(bool enabled);
	bool IsPerceptionTickInAiTickEnabled() const { return m_perceptionTickInAiTickEnabled; }

private:
	SoldierObject* GetSoldierOwner() const;
	void InitDefaultDriver();
	void SetDriverByNormalizedType(const std::string& key);
	void SetFsmDriver();
	void SetDecisionTreeDriver();
	void SetBehaviorTreeDriver();
	void SetEnemy(AgentObject* enemy);
	AgentObject* FindNearestEnemy(const Ogre::String& navMeshName) const;
	bool FindNearestEnemy(const Ogre::String& navMeshName, AgentPerceptionResult& result) const;
	bool IsEnemyValid(AgentObject* enemy, const Ogre::String& navMeshName, bool requirePath) const;
	AgentPerceptionOptions BuildPerceptionOptions(const Ogre::String& navMeshName, bool requirePath) const;
	VisionSensorConfig BuildVisionSensorConfig(const Ogre::String& navMeshName, bool requirePath) const;
	MemoryStoreConfig BuildMemoryStoreConfig() const;
	bool UpdateVisionSensor(int deltaMs, const Ogre::String& navMeshName, bool requirePath, bool forceScan, bool* outScanned = nullptr);
	void WritePerceptionResult(const AgentPerceptionResult& result);
	void ClearPerceptionResult();
	// 从当帧已写入的 blackboard / memory snapshot 收口感知结果到 m_perceptionCache（增量观测，不改其它路径）。
	void UpdatePerceptionCache(bool scannedThisTick, double scanCostMs);

private:
	Blackboard m_blackboard;
	IDecisionDriver* m_driver; // owned decision driver
	AgentObject* m_enemy; // non-owning cached target; validated by m_enemyId/ObjectManager on read
	int m_enemyId;
	bool m_hasMovePos;
	Ogre::Vector3 m_movePos;
	bool m_tickInOwnerUpdateEnabled;
	bool m_perceptionTickInAiTickEnabled;
	bool m_perceptionCacheEnabled;
	long long m_localTimeMs;
	VisionSensor m_visionSensor;
	MemoryStore m_memoryStore;
	PerceptionResultCache m_perceptionCache;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(AIController);

#endif // __AI_CONTROLLER_H__
