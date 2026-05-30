#ifndef __AI_CONTROLLER_H__
#define __AI_CONTROLLER_H__

#include <string>

#include "OgreString.h"
#include "OgreVector3.h"
#include "ai/common/Blackboard.h"
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

	void IssueCommand(const AICommand& command);
	unsigned int GetAgentId() const;
	void TickAI(int deltaMs);
	void SetTickInOwnerUpdateEnabled(bool enabled);
	bool IsTickInOwnerUpdateEnabled() const { return m_tickInOwnerUpdateEnabled; }

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
	bool UpdateVisionSensor(int deltaMs, const Ogre::String& navMeshName, bool requirePath, bool forceScan);
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
	long long m_localTimeMs;
	VisionSensor m_visionSensor;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(AIController);

#endif // __AI_CONTROLLER_H__
