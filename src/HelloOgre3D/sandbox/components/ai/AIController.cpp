#include "AIController.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <sstream>
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
#include "ai/tactics/TacticalService.h"
#include "core/SandboxServices.h"
#include "event/SandboxEventPayload.h"
#include "components/agent/AgentLocomotion.h"
#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"
#include "profiling/Profile.h"
#include "profiling/RuntimeProfileCounters.h"
#include "systems/manager/ObjectManager.h"
#include "systems/service/NavigationService.h"

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
	const char* const kPerceptionMaxSpatialResults = "perception.maxSpatialResults";
	const char* const kSenseTargetId = "sense.targetId";
	const char* const kSenseTargetPos = "sense.targetPos";
	const char* const kSenseTargetDistance = "sense.targetDistance";
	const char* const kPerceptionSource = "VisionSensor";
	const int kVisionScanIntervalMs = 200;
	const int kSenseEntryTtlMs = 750;
	const int kMemoryEntryTtlMs = 8000;
	const float kMemoryConfidenceDecayPerMs = 1.0f / static_cast<float>(kMemoryEntryTtlMs);

	typedef bool (*DriverMatchFn)(IDecisionDriver*);
	typedef IDecisionDriver* (*DriverCreateFn)(AgentObject*, Blackboard*);

	struct DriverFactoryEntry
	{
		const char* key;
		DriverMatchFn matches;
		DriverCreateFn create;
	};

	ObjectManager* ResolveObjectManager(const AIController* controller)
	{
		const SandboxServices* services = controller != nullptr ? controller->GetSandboxServices() : nullptr;
		if (services != nullptr && services->objects != nullptr)
			return services->objects;
		return nullptr;
	}

	NavigationService* ResolveNavigationService(const AIController* controller)
	{
		const SandboxServices* services = controller != nullptr ? controller->GetSandboxServices() : nullptr;
		if (services != nullptr && services->navigation != nullptr)
			return services->navigation;
		return nullptr;
	}

	bool IsFsmDriver(IDecisionDriver* driver)
	{
		return dynamic_cast<AgentStateController*>(driver) != nullptr;
	}

	bool IsDecisionTreeDriver(IDecisionDriver* driver)
	{
		return dynamic_cast<DecisionTreeDriver*>(driver) != nullptr;
	}

	bool IsBehaviorTreeDriver(IDecisionDriver* driver)
	{
		return dynamic_cast<BehaviorTreeDriver*>(driver) != nullptr;
	}

	IDecisionDriver* CreateFsmDriver(AgentObject* owner, Blackboard*)
	{
		if (owner == nullptr)
			return nullptr;

		AgentStateController* fsm = new AgentStateController(owner);
		fsm->Init();
		return fsm;
	}

	IDecisionDriver* CreateDecisionTreeDriver(AgentObject* owner, Blackboard* blackboard)
	{
		if (owner == nullptr || blackboard == nullptr)
			return nullptr;

		DecisionTreeDriver* dt = new DecisionTreeDriver(owner, blackboard);
		dt->Init();
		return dt;
	}

	IDecisionDriver* CreateBehaviorTreeDriver(AgentObject* owner, Blackboard* blackboard)
	{
		if (owner == nullptr || blackboard == nullptr)
			return nullptr;

		BehaviorTreeDriver* bt = new BehaviorTreeDriver(owner, blackboard);
		bt->Init();
		return bt;
	}

	const DriverFactoryEntry* FindDriverFactory(const std::string& key)
	{
		static const DriverFactoryEntry kEntries[] =
		{
			{ "fsm", IsFsmDriver, CreateFsmDriver },
			{ "dt", IsDecisionTreeDriver, CreateDecisionTreeDriver },
			{ "bt", IsBehaviorTreeDriver, CreateBehaviorTreeDriver },
		};

		for (size_t i = 0; i < sizeof(kEntries) / sizeof(kEntries[0]); ++i)
		{
			if (key == kEntries[i].key)
				return &kEntries[i];
		}
		return nullptr;
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
	, m_perceptionTickInAiTickEnabled(true)
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
	m_perceptionTickInAiTickEnabled = true;
	m_visionSensor.Clear();
	m_perceptionCache.Reset();
	InitDefaultDriver();
}

void AIController::onDetach()
{
	m_visionSensor.Clear();
	m_memoryStore.SetBlackboard(nullptr);
	m_blackboard.SetOwner(static_cast<AgentObject*>(nullptr));
	m_perceptionCache.Reset();
	SetEnemy(nullptr);
	IComponent::onDetach();
}

int AIController::getUpdateOrder() const
{
	return ComponentUpdateOrder::AI;
}

void AIController::update(int deltaMs)
{
	if (IsTickInOwnerUpdateEnabled())
		TickAI(deltaMs);
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
		SetDriverByNormalizedType("fsm");
	}
}

unsigned int AIController::GetAgentId() const
{
	AgentObject* owner = GetAgentOwner();
	return owner != nullptr ? owner->GetObjId() : 0;
}

void AIController::TickPerception(int deltaMs, AIPerceptionTickStats* outStats)
{
	if (outStats != nullptr)
		*outStats = AIPerceptionTickStats();

	AgentObject* owner = GetAgentOwner();
	if (owner == nullptr)
	{
		return;
	}

	H3D_PROFILE_SCOPE("AIController::TickPerception");
	const bool perfEnabled = RuntimeStallProfiler::IsEnabled();
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
		{
			const double memoryMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
			if (outStats != nullptr)
				outStats->memoryMs += memoryMs;
		}
	}
	bool perceptionScanned = false;
	double perceptionScanMs = 0.0;
	{
		H3D_PROFILE_SCOPE("AIController::VisionSensor");
		if (perfEnabled)
			stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
		bool scanned = false;
		const bool hasVisibleTarget = UpdateVisionSensor(elapsedMs, "default", true, false, &scanned);
		perceptionScanned = scanned;
		if (perfEnabled)
		{
			const double visionMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
			perceptionScanMs = visionMs;
			if (outStats != nullptr)
				outStats->visionMs = visionMs;
		}
		if (outStats != nullptr)
		{
			outStats->scanned = scanned;
			outStats->hasVisibleTarget = hasVisibleTarget;
		}
	}
	{
		H3D_PROFILE_SCOPE("AIController::MemorySnapshot");
		if (perfEnabled)
			stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
		m_memoryStore.SyncSnapshot(m_localTimeMs);
		if (perfEnabled)
		{
			const double memoryMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
			if (outStats != nullptr)
				outStats->memoryMs += memoryMs;
		}
	}

	// 收口本帧感知结果到显式缓存（增量观测；不改 blackboard / HasEnemy / 行为路径）。
	UpdatePerceptionCache(perceptionScanned, perceptionScanMs);
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

	if (m_perceptionTickInAiTickEnabled)
	{
		AIPerceptionTickStats perceptionStats;
		TickPerception(deltaMs, &perceptionStats);
		perfTiming.memoryMs += perceptionStats.memoryMs;
		perfTiming.visionMs += perceptionStats.visionMs;
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

void AIController::SetPerceptionTickInAiTickEnabled(bool enabled)
{
	m_perceptionTickInAiTickEnabled = enabled;
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
	AgentPerceptionQuery query(ResolveObjectManager(this), ResolveNavigationService(this));
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
	AgentPerceptionQuery query(ResolveObjectManager(this), ResolveNavigationService(this));
	return query.FindNearestEnemy(GetAgentOwner(), BuildPerceptionOptions(navMeshName, true), result);
}

AgentPerceptionOptions AIController::BuildPerceptionOptions(const Ogre::String& navMeshName, bool requirePath) const
{
	AgentPerceptionOptions options;
	options.navMeshName = navMeshName;
	options.maxDistance = std::max(0.0f, m_blackboard.GetFloat(kPerceptionVisionRange, 0.0f));
	options.requirePath = requirePath && m_blackboard.GetBool(kPerceptionRequirePath, true);
	options.fieldOfViewDegrees = std::max(0.0f, m_blackboard.GetFloat(kPerceptionFieldOfViewDegrees, 0.0f));
	options.maxSpatialResults = std::max(0, m_blackboard.GetInt(kPerceptionMaxSpatialResults, 0));
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

bool AIController::UpdateVisionSensor(int deltaMs, const Ogre::String& navMeshName, bool requirePath, bool forceScan, bool* outScanned)
{
	AgentObject* owner = GetAgentOwner();
	if (owner == nullptr)
	{
		if (outScanned != nullptr)
			*outScanned = false;
		return false;
	}

	AgentPerceptionQuery query(ResolveObjectManager(this), ResolveNavigationService(this));
	const bool scanned = m_visionSensor.Tick(owner, GetEnemy(), &query, BuildVisionSensorConfig(navMeshName, requirePath), deltaMs, forceScan);
	if (outScanned != nullptr)
		*outScanned = scanned;
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

	ObjectManager* objectManager = ResolveObjectManager(this);
	AgentObject* owner = GetAgentOwner();
	TacticalService* tactics = objectManager != nullptr ? objectManager->GetTacticalService() : nullptr;
	if (tactics != nullptr && owner != nullptr)
	{
		tactics->publishTacticalEvent(
			SandboxEventTypes::EnemySighted(),
			static_cast<int>(owner->GetObjId()),
			result.targetId,
			owner->GetTeamId(),
			result.target != nullptr ? result.target->GetTeamId() : -1,
			result.targetPosition,
			static_cast<int>(m_localTimeMs),
			"global",
			false);
	}
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

void AIController::UpdatePerceptionCache(bool scannedThisTick, double scanCostMs)
{
	PerceptionResultCache& cache = m_perceptionCache;

	// 当前可见目标：读当帧 WritePerceptionResult 写入的 perception.* 键。
	cache.hasCurrentTarget = m_blackboard.GetBool(kPerceptionHasTarget, false);
	if (cache.hasCurrentTarget)
	{
		cache.currentTargetId = m_blackboard.GetObjectId(kPerceptionTargetId, -1);
		cache.currentTargetPos = m_blackboard.GetVec3(kPerceptionTargetPos);
		cache.currentDistance = m_blackboard.GetFloat(kPerceptionTargetDistance, 0.0f);
		cache.currentConfidence = 1.0f; // 当前可见 = 满置信；细分置信留待与 sense entry 合并的后续切片
	}
	else
	{
		cache.currentTargetId = -1;
		cache.currentTargetPos = Ogre::Vector3::ZERO;
		cache.currentDistance = 0.0f;
		cache.currentConfidence = 0.0f;
	}

	// 最后已知位置：读 memory snapshot 键（目标丢失后仍保留并衰减）。
	cache.hasLastKnown = m_blackboard.GetBool(AIMemoryKeys::kMemorySnapshotHasLastKnownEnemy, false);
	if (cache.hasLastKnown)
	{
		cache.lastKnownTargetId = m_blackboard.GetObjectId(AIMemoryKeys::kMemorySnapshotLastKnownEnemyId, -1);
		cache.lastKnownPos = m_blackboard.GetVec3(AIMemoryKeys::kMemorySnapshotLastKnownEnemyPos);
		cache.lastKnownConfidence = m_blackboard.GetFloat(AIMemoryKeys::kMemorySnapshotLastKnownEnemyConfidence, 0.0f);
		cache.lastKnownAgeMs = m_blackboard.GetInt(AIMemoryKeys::kMemorySnapshotLastKnownEnemyAgeMs, 0);
	}
	else
	{
		cache.lastKnownTargetId = -1;
		cache.lastKnownPos = Ogre::Vector3::ZERO;
		cache.lastKnownConfidence = 0.0f;
		cache.lastKnownAgeMs = 0;
	}

	cache.scannedThisTick = scannedThisTick;
	if (scannedThisTick)
		cache.lastScanTimeMs = m_localTimeMs;
	cache.scanCostMs = scanCostMs;
	cache.source = kPerceptionSource;
	++cache.updateCount;
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
	// 改读 PerceptionResultCache（4b）：感知系统 / TickPerception 每帧已在 driver/Agent_Update 之前
	// 填充缓存，cache.hasCurrentTarget 与 m_visionSensor.HasVisibleTarget() 来自同一条扫描路径、恒等。
	// 这里直接读缓存，避免每个 BT/DT 条件 tick 重复构造 AgentPerceptionQuery + 走一遍 Tick(0)。
	// 仅在尚未发生过任何扫描的边界情况回退到原首扫逻辑，保持原语义。
	if (m_visionSensor.HasScanned())
	{
		return m_perceptionCache.hasCurrentTarget;
	}
	const bool forceInitialScan = !m_blackboard.GetBool(kPerceptionStaggerScans, false);
	bool scanned = false;
	const bool hasEnemy = UpdateVisionSensor(0, navMeshName, true, forceInitialScan, &scanned);
	if (scanned)
	{
		m_memoryStore.SyncSnapshot(m_localTimeMs);
		UpdatePerceptionCache(true, 0.0);
	}
	return hasEnemy;
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
	AgentPerceptionQuery query(ResolveObjectManager(this), ResolveNavigationService(this));
	AgentPerceptionResult result;
	if (!query.TryGetEnemy(owner, enemy, options, result))
	{
		return false;
	}

	WritePerceptionResult(result);
	m_memoryStore.SyncSnapshot(m_localTimeMs);
	UpdatePerceptionCache(true, 0.0);
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

	const AgentLocomotion* locomotion = owner->FindComponent<AgentLocomotion>();
	if (locomotion == nullptr || !locomotion->HasPath())
	{
		return false;
	}

	const Ogre::Vector3 target = locomotion->GetTarget();
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

	const AgentLocomotion* locomotion = owner->FindComponent<AgentLocomotion>();
	if (locomotion == nullptr || !locomotion->HasPath())
	{
		return false;
	}

	const Ogre::Vector3 target = locomotion->GetTarget();
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
	std::ostringstream stream;
	stream << m_visionSensor.BuildDebugString();
	// 感知结果缓存快照（便于核实缓存填充与一致性）。
	const PerceptionResultCache& c = m_perceptionCache;
	stream << " | cache cur=" << (c.hasCurrentTarget ? 1 : 0)
		<< " curId=" << c.currentTargetId
		<< " curDist=" << std::fixed << std::setprecision(1) << c.currentDistance
		<< " lastKnown=" << (c.hasLastKnown ? 1 : 0)
		<< " lkId=" << c.lastKnownTargetId
		<< " lkConf=" << std::fixed << std::setprecision(2) << c.lastKnownConfidence
		<< " lkAgeMs=" << c.lastKnownAgeMs
		<< " scanned=" << (c.scannedThisTick ? 1 : 0)
		<< " updates=" << c.updateCount;
	return stream.str();
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
		SetFsmDriver();
	else if (key == "dt")
		SetDecisionTreeDriver();
	else if (key == "bt")
		SetBehaviorTreeDriver();
	else
		SetDriverByNormalizedType(key);
}

void AIController::SetDriverByNormalizedType(const std::string& key)
{
	AgentObject* owner = GetAgentOwner();
	if (owner == nullptr)
	{
		return;
	}

	const DriverFactoryEntry* factory = FindDriverFactory(key);
	if (factory == nullptr)
	{
		return;
	}

	if (factory->matches != nullptr && factory->matches(m_driver))
	{
		return;
	}

	IDecisionDriver* nextDriver = factory->create(owner, &m_blackboard);
	if (nextDriver == nullptr)
	{
		return;
	}

	SAFE_DELETE(m_driver);
	m_driver = nextDriver;
}

void AIController::SetFsmDriver()
{
	SetDriverByNormalizedType("fsm");
}

void AIController::SetDecisionTreeDriver()
{
	SetDriverByNormalizedType("dt");
}

void AIController::SetBehaviorTreeDriver()
{
	SetDriverByNormalizedType("bt");
}
