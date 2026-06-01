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
#include "profiling/RuntimeProfileCounters.h"
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
	const char* const kPerceptionVisionIntervalMs = "perception.visionIntervalMs";
	const char* const kPerceptionVisionInitialDelayMs = "perception.visionInitialDelayMs";
	const char* const kPerceptionStaggerScans = "perception.staggerScans";
	const char* const kPerceptionFieldOfViewDegrees = "perception.fieldOfViewDegrees";
	const char* const kSenseTargetId = "sense.targetId";
	const char* const kSenseTargetPos = "sense.targetPos";
	const char* const kSenseTargetDistance = "sense.targetDistance";
	const char* const kPerceptionSource = "VisionSensor";
	const int kVisionScanIntervalMs = 200;
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
	: m_blackboard(dynamic_cast<AgentObject*>(owner))
	, m_driver(nullptr)
	, m_enemy(nullptr)
	, m_enemyId(-1)
	, m_hasMovePos(false)
	, m_movePos(Ogre::Vector3::ZERO)
	, m_tickInOwnerUpdateEnabled(true)
	, m_localTimeMs(0)
	, m_memoryStore(&m_blackboard)
{
}

AIController::~AIController()
{
	SAFE_DELETE(m_driver);
}

void AIController::onAttach(BaseObject* owner)
{
	IComponent::onAttach(owner);
	m_blackboard.SetOwner(GetAgentOwner());
	m_memoryStore.SetBlackboard(&m_blackboard);
	m_localTimeMs = 0;
	m_visionSensor.Clear();
	InitDefaultDriver();
}

void AIController::onDetach()
{
	m_visionSensor.Clear();
	m_memoryStore.SetBlackboard(nullptr);
	m_blackboard.SetOwner(static_cast<AgentObject*>(nullptr));
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

	H3D_PROFILE_SCOPE("AIController::TickAI");
	const bool perfEnabled = RuntimeStallProfiler::IsEnabled();
	RuntimeAiTickTiming perfTiming;
	const long long tickStartMicros = perfEnabled ? RuntimeStallProfiler::NowMicroseconds() : 0;
	long long stageStartMicros = 0;

	const int elapsedMs = std::max(0, deltaMs);
	m_localTimeMs += elapsedMs;
	{
		H3D_PROFILE_SCOPE("AIController::MemoryUpdate");
		if (perfEnabled)
			stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
		m_blackboard.UpdateEntries(m_localTimeMs, elapsedMs);
		m_memoryStore.SyncSnapshot(m_localTimeMs);
		if (perfEnabled)
			perfTiming.memoryMs += RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
	}
	{
		H3D_PROFILE_SCOPE("AIController::VisionSensor");
		if (perfEnabled)
			stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
		UpdateVisionSensor(elapsedMs, "default", true, false);
		if (perfEnabled)
			perfTiming.visionMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
	}
	{
		H3D_PROFILE_SCOPE("AIController::MemorySnapshot");
		if (perfEnabled)
			stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
		m_memoryStore.SyncSnapshot(m_localTimeMs);
		if (perfEnabled)
			perfTiming.memoryMs += RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
	}
	static int totalMilisec = 0;
	totalMilisec += deltaMs;

	bool forceUpdate = true;
	if (forceUpdate || totalMilisec > 1000)
	{
		totalMilisec = 0;
		H3D_PROFILE_SCOPE("Lua::Agent_Update");
		if (perfEnabled)
			stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
		SoldierObject* soldier = GetSoldierOwner();
		if (soldier != nullptr)
		{
			owner->callFunction("Agent_Update", "u[SoldierObject]i", soldier, deltaMs);
		}
		else
		{
			owner->callFunction("Agent_Update", "u[AgentObject]i", owner, deltaMs);
		}
		if (perfEnabled)
			perfTiming.luaMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
	}

	if (m_driver)
	{
		H3D_PROFILE_SCOPE("IDecisionDriver::Tick");
		if (perfEnabled)
			stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
		m_driver->Tick((float)deltaMs);
		if (perfEnabled)
			perfTiming.driverMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
	}
	if (perfEnabled)
	{
		perfTiming.totalMs = RuntimeStallProfiler::ElapsedMsSince(tickStartMicros);
		RuntimeStallProfiler::AddAiTickTiming(perfTiming);
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
	options.fieldOfViewDegrees = std::max(0.0f, m_blackboard.GetFloat(kPerceptionFieldOfViewDegrees, 0.0f));
	return options;
}

VisionSensorConfig AIController::BuildVisionSensorConfig(const Ogre::String& navMeshName, bool requirePath) const
{
	VisionSensorConfig config;
	config.perception = BuildPerceptionOptions(navMeshName, requirePath);
	config.scanIntervalMs = std::max(0, m_blackboard.GetInt(kPerceptionVisionIntervalMs, kVisionScanIntervalMs));
	const int configuredDelayMs = m_blackboard.GetInt(kPerceptionVisionInitialDelayMs, -1);
	if (configuredDelayMs >= 0)
	{
		config.initialDelayMs = configuredDelayMs;
	}
	else if (config.scanIntervalMs > 0 && m_blackboard.GetBool(kPerceptionStaggerScans, false))
	{
		config.initialDelayMs = static_cast<int>((GetAgentId() * 37u) % static_cast<unsigned int>(config.scanIntervalMs));
	}
	return config;
}

MemoryStoreConfig AIController::BuildMemoryStoreConfig() const
{
	MemoryStoreConfig config;
	config.ttlMs = kMemoryEntryTtlMs;
	config.decayPolicy = Blackboard::ENTRY_DECAY_LINEAR;
	config.decayRate = kMemoryConfidenceDecayPerMs;
	config.source = kPerceptionSource;
	return config;
}

bool AIController::UpdateVisionSensor(int deltaMs, const Ogre::String& navMeshName, bool requirePath, bool forceScan)
{
	AgentObject* owner = GetAgentOwner();
	if (owner == nullptr)
	{
		return false;
	}

	AgentPerceptionQuery query(ResolveObjectManager(this), ResolveSandboxMgr(this));
	const bool scanned = m_visionSensor.Tick(owner, GetEnemy(), &query, BuildVisionSensorConfig(navMeshName, requirePath), deltaMs, forceScan);
	if (scanned)
	{
		if (m_visionSensor.HasVisibleTarget())
		{
			const AgentPerceptionResult& result = m_visionSensor.GetLastResult();
			SetEnemy(result.target);
			WritePerceptionResult(result);
		}
		else
		{
			SetEnemy(nullptr);
			ClearPerceptionResult();
		}
	}

	return m_visionSensor.HasVisibleTarget();
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

	const float confidence = std::max(0.0f, std::min(1.0f, result.confidence));
	m_blackboard.SetObjectIdEntry(kSenseTargetId, result.targetId, confidence, m_localTimeMs, kSenseEntryTtlMs, kPerceptionSource);
	m_blackboard.SetVec3Entry(kSenseTargetPos, result.targetPosition, confidence, m_localTimeMs, kSenseEntryTtlMs, kPerceptionSource);
	m_blackboard.SetFloatEntry(kSenseTargetDistance, distance, confidence, m_localTimeMs, kSenseEntryTtlMs, kPerceptionSource);
	m_memoryStore.RememberVisibleEnemy(result, m_localTimeMs, BuildMemoryStoreConfig());
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
	const bool forceInitialScan = !m_blackboard.GetBool(kPerceptionStaggerScans, false) && !m_visionSensor.HasScanned();
	return UpdateVisionSensor(0, navMeshName, true, forceInitialScan);
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

std::string AIController::BuildSensorDebugString() const
{
	return m_visionSensor.BuildDebugString();
}

std::string AIController::BuildMemoryDebugString() const
{
	return m_memoryStore.BuildDebugString(m_localTimeMs);
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
