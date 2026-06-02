#ifndef __AI_CONTROLLER_H__
#define __AI_CONTROLLER_H__

#include <string>

#include "OgreString.h"
#include "OgreVector3.h"
#include "ai/common/Blackboard.h"
#include "ai/perception/MemoryStore.h"
#include "ai/perception/VisionSensor.h"
#include "component/IComponent.h"
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

class AIController : public IComponent //tolua_exports
{ //tolua_exports
public:
	explicit AIController(BaseObject* owner = nullptr);
	virtual ~AIController();

	virtual void onAttach(BaseObject* owner) override;
	virtual void onDetach() override;

	//tolua_begin
	SoldierObject* GetOwner() const;
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

	AgentStateController* GetFsmController() const;
	std::string BuildSensorDebugString() const;
	std::string BuildMemoryDebugString() const;

	void IssueCommand(const AICommand& command);
	unsigned int GetAgentId() const;
	void TickPerception(int deltaMs, AIPerceptionTickStats* outStats = nullptr);
	void TickAI(int deltaMs);
	void SetTickInOwnerUpdateEnabled(bool enabled);
	bool IsTickInOwnerUpdateEnabled() const { return m_tickInOwnerUpdateEnabled; }
	void SetPerceptionTickInAiTickEnabled(bool enabled);
	bool IsPerceptionTickInAiTickEnabled() const { return m_perceptionTickInAiTickEnabled; }

private:
	AgentObject* GetAgentOwner() const;
	SoldierObject* GetSoldierOwner() const;
	void InitDefaultDriver();
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

private:
	Blackboard m_blackboard;
	IDecisionDriver* m_driver;
	AgentObject* m_enemy;
	int m_enemyId;
	bool m_hasMovePos;
	Ogre::Vector3 m_movePos;
	bool m_tickInOwnerUpdateEnabled;
	bool m_perceptionTickInAiTickEnabled;
	long long m_localTimeMs;
	VisionSensor m_visionSensor;
	MemoryStore m_memoryStore;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(AIController);

#endif // __AI_CONTROLLER_H__
