#include "AIController.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <string>

#include "GameFunction.h"
#include "SandboxMacros.h"
#include "ai/behavior/BehaviorTreeDriver.h"
#include "ai/common/AICommand.h"
#include "ai/common/Blackboard.h"
#include "ai/common/IDecisionDriver.h"
#include "ai/decision/DecisionTreeDriver.h"
#include "ai/fsm/AgentStateController.h"
#include "ai/perception/AgentPerceptionQuery.h"
#include "core/SandboxServices.h"
#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"
#include "profiling/Profile.h"
#include "systems/manager/ObjectManager.h"
#include "systems/manager/SandboxMgr.h"

namespace
{
	const char* const kPerceptionVisionRange = "perception.visionRange";
	const char* const kPerceptionRequirePath = "perception.requirePath";
	const char* const kPerceptionHasTarget = "perception.hasTarget";
	const char* const kPerceptionTargetId = "perception.targetId";
	const char* const kPerceptionTargetPos = "perception.targetPos";
	const char* const kPerceptionTargetDistance = "perception.targetDistance";
	const char* const kPerceptionTargetDistanceSq = "perception.targetDistanceSq";
	const char* const kPerceptionLastKnownTargetId = "perception.lastKnownTargetId";
	const char* const kPerceptionLastKnownTargetPos = "perception.lastKnownTargetPos";
	const char* const kSenseTargetId = "sense.targetId";
	const char* const kSenseTargetPos = "sense.targetPos";
	const char* const kSenseTargetDistance = "sense.targetDistance";
	const char* const kMemoryLastKnownEnemyId = "memory.lastKnownEnemyId";
	const char* const kMemoryLastKnownEnemyPos = "memory.lastKnownEnemyPos";
	const char* const kPerceptionSource = "PerceptionQuery";
	const int kSenseEntryTtlMs = 750;
	const int kMemoryEntryTtlMs = 8000;
	const float kMemoryConfidenceDecayPerMs = 1.0f / static_cast<float>(kMemoryEntryTtlMs);

	ObjectManager* ResolveObjectManager(const AIController* controller)
	{
		const SandboxServices* services = controller != nullptr ? controller->GetSandboxServices() : nullptr;
		if (services != nullptr && services->objects != nullptr)
			return services->objects;
		return g_ObjectManager;
	}

	SandboxMgr* ResolveSandboxMgr(const AIController* controller)
	{
		const SandboxServices* services = controller != nullptr ? controller->GetSandboxServices() : nullptr;
		if (services != nullptr && services->sandbox != nullptr)
			return services->sandbox;
		return g_SandboxMgr;
	}
}

AIController::AIController(BaseObject* owner)
	: m_blackboard(dynamic_cast<SoldierObject*>(owner))
	, m_driver(nullptr)
	, m_enemy(nullptr)
	, m_enemyId(-1)
	, m_hasMovePos(false)
	, m_movePos(Ogre::Vector3::ZERO)
	, m_tickInOwnerUpdateEnabled(true)
	, m_localTimeMs(0)
{
}

AIController::~AIController()
{
	SAFE_DELETE(m_driver);
}

void AIController::onAttach(BaseObject* owner)
{
	IComponent::onAttach(owner);
	m_blackboard.SetOwner(GetSoldierOwner());
	m_localTimeMs = 0;
	InitDefaultDriver();
}

void AIController::onDetach()
{
	m_blackboard.SetOwner(nullptr);
	IComponent::onDetach();
}

void AIController::InitDefaultDriver()
{
	AgentObject* owner = GetAgentOwner();
	if (owner == nullptr || m_driver != nullptr)
	{
		return;
	}

	if (owner->GetUseCppFSM())
	{
		AgentStateController* fsm = new AgentStateController(owner);
		fsm->Init();
		m_driver = fsm;
	}
}

unsigned int AIController::GetAgentId() const
{
	AgentObject* owner = GetAgentOwner();
	return owner != nullptr ? owner->GetObjId() : 0;
}

void AIController::TickAI(int deltaMs)
{
	AgentObject* owner = GetAgentOwner();
	if (owner == nullptr)
	{
		return;
	}

	const int elapsedMs = std::max(0, deltaMs);
	m_localTimeMs += elapsedMs;
	m_blackboard.UpdateEntries(m_localTimeMs, elapsedMs);

	H3D_PROFILE_SCOPE("AIController::TickAI");
	static int totalMilisec = 0;
	totalMilisec += deltaMs;

	bool forceUpdate = true;
	if (forceUpdate || totalMilisec > 1000)
	{
		totalMilisec = 0;
		H3D_PROFILE_SCOPE("Lua::Agent_Update");
		SoldierObject* soldier = GetSoldierOwner();
		if (soldier != nullptr)
		{
			owner->callFunction("Agent_Update", "u[SoldierObject]i", soldier, deltaMs);
		}
		else
		{
			owner->callFunction("Agent_Update", "u[AgentObject]i", owner, deltaMs);
		}
	}

	if (m_driver)
	{
		H3D_PROFILE_SCOPE("IDecisionDriver::Tick");
		m_driver->Tick((float)deltaMs);
	}
}

void AIController::SetTickInOwnerUpdateEnabled(bool enabled)
{
	m_tickInOwnerUpdateEnabled = enabled;
}

Blackboard* AIController::GetBlackboard() const
{
	return const_cast<Blackboard*>(&m_blackboard);
}

SoldierObject* AIController::GetOwner() const
{
	return GetSoldierOwner();
}

AgentObject* AIController::GetAgentOwner() const
{
	return dynamic_cast<AgentObject*>(getOwner());
}

SoldierObject* AIController::GetSoldierOwner() const
{
	return dynamic_cast<SoldierObject*>(GetAgentOwner());
}

bool AIController::IsEnemyValid(AgentObject* enemy, const Ogre::String& navMeshName, bool requirePath) const
{
	AgentPerceptionQuery query(ResolveObjectManager(this), ResolveSandboxMgr(this));
	AgentPerceptionResult result;
	return query.TryGetEnemy(GetAgentOwner(), enemy, BuildPerceptionOptions(navMeshName, requirePath), result);
}

AgentObject* AIController::FindNearestEnemy(const Ogre::String& navMeshName) const
{
	AgentPerceptionResult result;
	return FindNearestEnemy(navMeshName, result) ? result.target : nullptr;
}

bool AIController::FindNearestEnemy(const Ogre::String& navMeshName, AgentPerceptionResult& result) const
{
	AgentPerceptionQuery query(ResolveObjectManager(this), ResolveSandboxMgr(this));
	return query.FindNearestEnemy(GetAgentOwner(), BuildPerceptionOptions(navMeshName, true), result);
}

AgentPerceptionOptions AIController::BuildPerceptionOptions(const Ogre::String& navMeshName, bool requirePath) const
{
	AgentPerceptionOptions options;
	options.navMeshName = navMeshName;
	options.maxDistance = std::max(0.0f, m_blackboard.GetFloat(kPerceptionVisionRange, 0.0f));
	options.requirePath = requirePath && m_blackboard.GetBool(kPerceptionRequirePath, true);
	return options;
}

void AIController::WritePerceptionResult(const AgentPerceptionResult& result)
{
	if (result.target == nullptr)
	{
		ClearPerceptionResult();
		return;
	}

	m_blackboard.SetAgent("enemy", result.target);
	m_blackboard.SetBool(kPerceptionHasTarget, true);
	m_blackboard.SetObjectId(kPerceptionTargetId, result.targetId);
	m_blackboard.SetVec3(kPerceptionTargetPos, result.targetPosition);
	m_blackboard.SetFloat(kPerceptionTargetDistanceSq, result.distanceSquared);
	const float distance = std::sqrt(std::max(0.0f, result.distanceSquared));
	m_blackboard.SetFloat(kPerceptionTargetDistance, distance);
	m_blackboard.SetObjectId(kPerceptionLastKnownTargetId, result.targetId);
	m_blackboard.SetVec3(kPerceptionLastKnownTargetPos, result.targetPosition);

	m_blackboard.SetObjectIdEntry(kSenseTargetId, result.targetId, 1.0f, m_localTimeMs, kSenseEntryTtlMs, kPerceptionSource);
	m_blackboard.SetVec3Entry(kSenseTargetPos, result.targetPosition, 1.0f, m_localTimeMs, kSenseEntryTtlMs, kPerceptionSource);
	m_blackboard.SetFloatEntry(kSenseTargetDistance, distance, 1.0f, m_localTimeMs, kSenseEntryTtlMs, kPerceptionSource);
	m_blackboard.SetObjectIdEntry(kMemoryLastKnownEnemyId, result.targetId, 1.0f, m_localTimeMs, kMemoryEntryTtlMs, kPerceptionSource);
	m_blackboard.SetVec3Entry(kMemoryLastKnownEnemyPos, result.targetPosition, 1.0f, m_localTimeMs, kMemoryEntryTtlMs, kPerceptionSource);
	m_blackboard.SetEntryDecayPolicy(kMemoryLastKnownEnemyId, Blackboard::ENTRY_DECAY_LINEAR, kMemoryConfidenceDecayPerMs);
	m_blackboard.SetEntryDecayPolicy(kMemoryLastKnownEnemyPos, Blackboard::ENTRY_DECAY_LINEAR, kMemoryConfidenceDecayPerMs);
}

void AIController::ClearPerceptionResult()
{
	m_blackboard.Remove("enemy");
	m_blackboard.SetBool(kPerceptionHasTarget, false);
	m_blackboard.Remove(kPerceptionTargetId);
	m_blackboard.Remove(kPerceptionTargetPos);
	m_blackboard.Remove(kPerceptionTargetDistance);
	m_blackboard.Remove(kPerceptionTargetDistanceSq);
	m_blackboard.Remove(kSenseTargetId);
	m_blackboard.Remove(kSenseTargetPos);
	m_blackboard.Remove(kSenseTargetDistance);
}

void AIController::SetEnemy(AgentObject* enemy)
{
	m_enemy = enemy;
	m_enemyId = enemy ? static_cast<int>(enemy->GetObjId()) : -1;
}

AgentObject* AIController::GetEnemy() const
{
	ObjectManager* objectManager = ResolveObjectManager(this);
	if (m_enemy == nullptr || m_enemyId < 0 || objectManager == nullptr)
	{
		return m_enemy;
	}

	BaseObject* object = objectManager->getObjectById(m_enemyId);
	return object == m_enemy ? dynamic_cast<AgentObject*>(object) : nullptr;
}

bool AIController::HasEnemy(const Ogre::String& navMeshName)
{
	AgentObject* enemy = GetEnemy();
	AgentPerceptionQuery query(ResolveObjectManager(this), ResolveSandboxMgr(this));
	AgentPerceptionResult result;
	if (query.TryGetEnemy(GetAgentOwner(), enemy, BuildPerceptionOptions(navMeshName, true), result))
	{
		WritePerceptionResult(result);
		return true;
	}

	if (FindNearestEnemy(navMeshName, result))
	{
		SetEnemy(result.target);
		WritePerceptionResult(result);
		return true;
	}

	SetEnemy(nullptr);
	ClearPerceptionResult();
	return false;
}

bool AIController::CanShootEnemy(const Ogre::String& navMeshName, float shootDistance)
{
	AgentObject* owner = GetAgentOwner();
	if (owner == nullptr || shootDistance <= 0.0f)
	{
		return false;
	}

	AgentObject* enemy = GetEnemy();
	AgentPerceptionOptions options = BuildPerceptionOptions(navMeshName, false);
	options.maxDistance = std::max(0.0f, shootDistance);
	AgentPerceptionQuery query(ResolveObjectManager(this), ResolveSandboxMgr(this));
	AgentPerceptionResult result;
	if (!query.TryGetEnemy(owner, enemy, options, result))
	{
		return false;
	}

	WritePerceptionResult(result);
	return true;
}

bool AIController::HasMovePosition(float reachDistance) const
{
	AgentObject* owner = GetAgentOwner();
	if (owner == nullptr)
	{
		return false;
	}

	const float distance = std::max(0.0f, reachDistance);
	const float reachSquared = distance * distance;

	if (m_hasMovePos)
	{
		return owner->GetPosition().squaredDistance(m_movePos) > reachSquared;
	}

	if (!owner->HasPath())
	{
		return false;
	}

	const Ogre::Vector3 target = owner->GetTarget();
	return owner->GetPosition().squaredDistance(target) > reachSquared;
}

void AIController::SetMovePosition(const Ogre::Vector3& movePos)
{
	m_movePos = movePos;
	m_hasMovePos = true;
}

void AIController::ClearMovePosition()
{
	m_hasMovePos = false;
	m_movePos = Ogre::Vector3::ZERO;
}

bool AIController::IsTargetReached(float threshold) const
{
	AgentObject* owner = GetAgentOwner();
	if (owner == nullptr)
	{
		return false;
	}

	const float distance = std::max(0.0f, threshold);
	const float thresholdSquared = distance * distance;

	if (m_hasMovePos)
	{
		return owner->GetPosition().squaredDistance(m_movePos) < thresholdSquared;
	}

	if (!owner->HasPath())
	{
		return false;
	}

	const Ogre::Vector3 target = owner->GetTarget();
	return owner->GetPosition().squaredDistance(target) < thresholdSquared;
}

DecisionTreeDriver* AIController::GetDecisionTreeDriver() const
{
	return dynamic_cast<DecisionTreeDriver*>(m_driver);
}

BehaviorTreeDriver* AIController::GetBehaviorTreeDriver() const
{
	return dynamic_cast<BehaviorTreeDriver*>(m_driver);
}

AgentStateController* AIController::GetFsmController() const
{
	return dynamic_cast<AgentStateController*>(m_driver);
}

void AIController::IssueCommand(const AICommand& command)
{
	AgentObject* owner = GetAgentOwner();
	if (owner != nullptr)
	{
		if (command.kind == AICommand::COMMAND_MOVE_TO)
		{
			SetMovePosition(command.targetPosition);
			m_blackboard.SetVec3("movePos", command.targetPosition);
		}
		else if (command.kind == AICommand::COMMAND_STOP)
		{
			ClearMovePosition();
			m_blackboard.Remove("movePos");
		}
		owner->ApplyCommand(command);
	}
}

void AIController::SetDriverByType(const char* type)
{
	if (GetAgentOwner() == nullptr || type == nullptr)
	{
		return;
	}

	std::string key(type);
	std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) {
		return static_cast<char>(std::tolower(c));
	});

	if (key == "fsm")
	{
		SetFsmDriver();
	}
	else if (key == "dt")
	{
		SetDecisionTreeDriver();
	}
	else if (key == "bt")
	{
		SetBehaviorTreeDriver();
	}
}

void AIController::SetFsmDriver()
{
	AgentObject* owner = GetAgentOwner();
	if (owner == nullptr)
	{
		return;
	}

	if (dynamic_cast<AgentStateController*>(m_driver) != nullptr)
	{
		return;
	}
	SAFE_DELETE(m_driver);

	AgentStateController* fsm = new AgentStateController(owner);
	fsm->Init();
	m_driver = fsm;
}

void AIController::SetDecisionTreeDriver()
{
	SoldierObject* soldier = GetSoldierOwner();
	if (soldier == nullptr)
	{
		return;
	}

	if (dynamic_cast<DecisionTreeDriver*>(m_driver) != nullptr)
	{
		return;
	}
	SAFE_DELETE(m_driver);

	DecisionTreeDriver* dt = new DecisionTreeDriver(soldier, &m_blackboard);
	dt->Init();
	m_driver = dt;
}

void AIController::SetBehaviorTreeDriver()
{
	SoldierObject* soldier = GetSoldierOwner();
	if (soldier == nullptr)
	{
		return;
	}

	if (dynamic_cast<BehaviorTreeDriver*>(m_driver) != nullptr)
	{
		return;
	}
	SAFE_DELETE(m_driver);

	BehaviorTreeDriver* bt = new BehaviorTreeDriver(soldier, &m_blackboard);
	bt->Init();
	m_driver = bt;
}
