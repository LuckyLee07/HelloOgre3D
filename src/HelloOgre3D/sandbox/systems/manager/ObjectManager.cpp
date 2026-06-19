#include "ObjectManager.h"
#include "objects/AgentObject.h"
#include "objects/BlockObject.h"
#include "objects/SoldierObject.h"
#include "ai/common/AIScheduler.h"
#include "ai/behavior/BehaviorTreeDriver.h"
#include "ai/decision/DecisionTreeDriver.h"
#include "ai/common/Blackboard.h"
#include "ai/perception/AgentPerceptionSystem.h"
#include "ai/perception/AgentSpatialIndexSystem.h"
#include "ai/perception/MemoryStore.h"
#include "ai/tactics/TacticalDebugDrawService.h"
#include "ai/tactics/TacticalQueryService.h"
#include "ai/team/TeamBlackboardService.h"
#include "common/ScriptLuaVM.h"
#include "systems/manager/ObjectRegistry.h"
#include "systems/physics/PhysicsWorld.h"
#include "systems/service/NavigationService.h"
#include "systems/service/SceneFactory.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"
#include "components/ai/AIController.h"
#include "components/physics/PhysicsComponent.h"
#include "ai/navigation/NavigationMesh.h"
#include "event/SandboxEventDispatcherManager.h"
#include "profiling/Profile.h"
#include "profiling/RuntimeProfileCounters.h"

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <vector>

namespace
{
	const char* ObjectTypeToString(BaseObject::ObjectType type)
	{
		switch (type)
		{
		case BaseObject::OBJ_TYPE_BLOCK:
			return "Block";
		case BaseObject::OBJ_TYPE_PLANE:
			return "Plane";
		case BaseObject::OBJ_TYPE_BULLET:
			return "Bullet";
		case BaseObject::OBJ_TYPE_AGENT:
			return "Agent";
		case BaseObject::OBJ_TYPE_SOLDIER:
			return "Soldier";
		case BaseObject::OBJ_TYPE_NONE:
		default:
			return "None";
		}
	}

	std::string FormatVec3(const Ogre::Vector3& value)
	{
		std::ostringstream stream;
		stream << static_cast<int>(value.x) << "," << static_cast<int>(value.y) << "," << static_cast<int>(value.z);
		return stream.str();
	}

	std::string FormatReal(Ogre::Real value)
	{
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(2) << value;
		return stream.str();
	}

	std::string FormatVec3Precise(const Ogre::Vector3& value)
	{
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(2)
			<< value.x << "," << value.y << "," << value.z;
		return stream.str();
	}

	std::string TrimDebugText(const std::string& text, size_t maxLength)
	{
		if (text.size() <= maxLength)
			return text;
		if (maxLength <= 3)
			return text.substr(0, maxLength);
		return text.substr(0, maxLength - 3) + "...";
	}

	bool NearlyEqual(float left, float right, float epsilon)
	{
		return std::fabs(left - right) <= epsilon;
	}

	bool IsFalseEnvValue(const char* value)
	{
		if (value == nullptr || value[0] == '\0')
			return false;

		const std::string text(value);
		return text == "0" || text == "false" || text == "FALSE" || text == "False" || text == "off" || text == "OFF";
	}

	void AppendSelfTestFailure(std::ostringstream& stream, const std::string& text, int& failures)
	{
		stream << "\n[BlackboardSelfTest] failure: " << text;
		++failures;
	}

	void AppendAiEventSelfTestFailure(std::ostringstream& stream, const std::string& text, int& failures)
	{
		stream << "\n[AIEventSelfTest] failure: " << text;
		++failures;
	}

	SandboxContext BuildAiEventSelfTestContext(const char* eventType, SandboxEventScope scope, int senderId, int targetId, int teamId, double timeMs)
	{
		SandboxContext context;
		context.Set_String(SandboxEventFields::EventType(), eventType != nullptr ? eventType : "");
		context.Set_String(SandboxEventFields::Scope(), SandboxEventPayload::ToString(scope));
		context.Set_Number(SandboxEventFields::SenderId(), static_cast<double>(senderId));
		context.Set_Number(SandboxEventFields::TargetId(), static_cast<double>(targetId));
		context.Set_Number(SandboxEventFields::TeamId(), static_cast<double>(teamId));
		context.Set_Number(SandboxEventFields::TimeMs(), timeMs);
		return context;
	}

	void AppendBlackboardBrief(std::ostringstream& stream, Blackboard* blackboard)
	{
		if (blackboard == nullptr)
			return;

		bool wrote = false;
		stream << " bb={";
		if (blackboard->Has("maxHealth"))
		{
			stream << "maxHealth:" << blackboard->GetFloat("maxHealth", 0.0f);
			wrote = true;
		}
		if (blackboard->Has("movePos"))
		{
			const Ogre::Vector3 movePos = blackboard->GetVec3("movePos");
			stream << (wrote ? "," : "") << "movePos:" << static_cast<int>(movePos.x) << "," << static_cast<int>(movePos.y) << "," << static_cast<int>(movePos.z);
			wrote = true;
		}
		if (blackboard->Has("perception.targetId"))
		{
			stream << (wrote ? "," : "") << "senseTarget:" << blackboard->GetObjectId("perception.targetId", -1);
			if (blackboard->Has("perception.targetDistance"))
			{
				stream << "@" << static_cast<int>(blackboard->GetFloat("perception.targetDistance", 0.0f));
			}
			wrote = true;
		}
		else if (blackboard->Has("perception.hasTarget"))
		{
			stream << (wrote ? "," : "") << "senseTarget:none";
			wrote = true;
		}
		int memoryTargetId = -1;
		if (blackboard->GetObjectIdEntry("memory.lastKnownEnemyId", memoryTargetId))
		{
			stream << (wrote ? "," : "") << "memoryTarget:" << memoryTargetId;
			Blackboard::Entry memoryPosEntry;
			Ogre::Vector3 memoryPos = Ogre::Vector3::ZERO;
			if (blackboard->GetVec3Entry("memory.lastKnownEnemyPos", memoryPos, &memoryPosEntry))
			{
				stream << "@" << static_cast<int>(memoryPos.x) << "," << static_cast<int>(memoryPos.y) << "," << static_cast<int>(memoryPos.z);
				stream << " t=" << memoryPosEntry.timestampMs;
				stream << " c=" << FormatReal(memoryPosEntry.confidence);
				if (memoryPosEntry.ttlMs >= 0)
					stream << " ttl=" << memoryPosEntry.ttlMs;
			}
			wrote = true;
		}
		if (blackboard->GetBool("memory.snapshot.hasLastKnownEnemy", false))
		{
			const Ogre::Vector3 lastKnownPos = blackboard->GetVec3("memory.snapshot.lastKnownEnemyPos");
			stream << (wrote ? "," : "") << "lastKnown:" << blackboard->GetObjectId("memory.snapshot.lastKnownEnemyId", -1)
				<< "@" << FormatVec3(lastKnownPos)
				<< " age=" << blackboard->GetInt("memory.snapshot.lastKnownEnemyAgeMs", -1)
				<< " c=" << FormatReal(blackboard->GetFloat("memory.snapshot.lastKnownEnemyConfidence", 0.0f));
			wrote = true;
		}
		else if (blackboard->Has("debug.lastKnownSearchEnemyPos"))
		{
			const Ogre::Vector3 lastKnownPos = blackboard->GetVec3("debug.lastKnownSearchEnemyPos");
			stream << (wrote ? "," : "") << "lastKnown:" << blackboard->GetObjectId("debug.lastKnownSearchEnemyId", -1)
				<< "@" << FormatVec3(lastKnownPos)
				<< " t=" << blackboard->GetInt("debug.lastKnownSearchObservedAtMs", -1);
			wrote = true;
		}
		if (blackboard->Has("memory.search.completedEnemyId"))
		{
			stream << (wrote ? "," : "") << "searchDone:" << blackboard->GetObjectId("memory.search.completedEnemyId", -1)
				<< "@" << blackboard->GetInt("memory.search.completedObservedAtMs", -1);
			wrote = true;
		}
		if (blackboard->Has("knowledge.enemyId"))
		{
			stream << (wrote ? "," : "") << "knowledgeEnemy:" << blackboard->GetObjectId("knowledge.enemyId", -1);
			if (blackboard->Has("knowledge.enemyDistance"))
			{
				stream << "@" << static_cast<int>(blackboard->GetFloat("knowledge.enemyDistance", 0.0f));
			}
			if (blackboard->Has("knowledge.enemyEvalCount"))
			{
				stream << " eval=" << blackboard->GetInt("knowledge.enemyEvalCount", 0);
			}
			wrote = true;
		}
		if (blackboard->Has("knowledge.bestFleePosition"))
		{
			const Ogre::Vector3 fleePos = blackboard->GetVec3("knowledge.bestFleePosition");
			stream << (wrote ? "," : "") << "knowledgeFlee:" << FormatVec3(fleePos);
			if (blackboard->Has("knowledge.fleeEvalCount"))
			{
				stream << " eval=" << blackboard->GetInt("knowledge.fleeEvalCount", 0);
			}
			wrote = true;
		}
		if (blackboard->GetEntryCount() > 0)
		{
			stream << (wrote ? "," : "") << "meta:" << blackboard->GetEntryCount();
			wrote = true;
		}
		if (blackboard->Has("__bt.traceFrame"))
		{
			stream << (wrote ? "," : "") << "btFrame:" << blackboard->GetInt("__bt.traceFrame", 0);
			wrote = true;
		}
		if (blackboard->Has("__bt.traceEventCount"))
		{
			stream << (wrote ? "," : "") << "btEvents:" << blackboard->GetInt("__bt.traceEventCount", 0);
			wrote = true;
		}
		if (blackboard->Has("__bt.currentAction"))
		{
			stream << (wrote ? "," : "") << "btAction:" << blackboard->GetString("__bt.currentAction");
			if (blackboard->Has("__bt.currentActionStatus"))
			{
				stream << "/" << blackboard->GetString("__bt.currentActionStatus");
			}
			wrote = true;
		}
		if (blackboard->Has("debug.demoPhase"))
		{
			stream << (wrote ? "," : "") << "demo:" << blackboard->GetString("debug.demoPhase");
			wrote = true;
		}
		if (!wrote)
			stream << "-";
		stream << "}";
	}
}


ObjectManager::ObjectManager(PhysicsWorld* pPhysicsWorld)
	: m_registry(new ObjectRegistry()), m_pPhysicsWorld(pPhysicsWorld), m_currentTimeMs(0)
{
	m_pScriptVM = GetScriptLuaVM();
	m_aiScheduler = new AIScheduler();
	m_agentSpatialIndex = new AgentSpatialIndexSystem();
	m_agentPerceptionSystem = new AgentPerceptionSystem();
	m_teamBlackboardService = new TeamBlackboardService();
	m_tacticalQueryService = new TacticalQueryService();
	m_tacticalDebugDrawService = new TacticalDebugDrawService();
	m_tacticalQueryService->Initialize();
	if (IsFalseEnvValue(std::getenv("HELLO_AI_SPATIAL_INDEX_ENABLE")))
		m_agentSpatialIndex->SetEnabled(false);
	if (IsFalseEnvValue(std::getenv("HELLO_AI_PERCEPTION_SYSTEM_ENABLE")))
		m_agentPerceptionSystem->SetEnabled(false);
	const char* spatialCellSize = std::getenv("HELLO_AI_SPATIAL_INDEX_CELL_SIZE");
	if (spatialCellSize != nullptr)
	{
		const float cellSize = static_cast<float>(std::atof(spatialCellSize));
		if (cellSize > 0.0f)
			m_agentSpatialIndex->SetCellSize(cellSize);
	}
	m_services.objects = this;
	m_services.physics = m_pPhysicsWorld;
	m_services.script = m_pScriptVM;
}

ObjectManager::~ObjectManager()
{
	m_pScriptVM = nullptr;
	m_pPhysicsWorld = nullptr;

	clearTacticalInfluenceDebugVisuals();
	this->clearAllObjects(MGR_OBJ_ALLS);
	SAFE_DELETE(m_registry);
	SAFE_DELETE(m_aiScheduler);
	SAFE_DELETE(m_agentSpatialIndex);
	SAFE_DELETE(m_agentPerceptionSystem);
	SAFE_DELETE(m_teamBlackboardService);
	SAFE_DELETE(m_tacticalQueryService);
	SAFE_DELETE(m_tacticalDebugDrawService);
}

void ObjectManager::Update(int deltaMilliseconds)
{
	H3D_PROFILE_SCOPE("ObjectManager::Update");
	const bool perfEnabled = RuntimeStallProfiler::IsEnabled();
	RuntimeObjectUpdateTiming perfTiming;
	perfTiming.objectCount = static_cast<int>(m_registry->Objects().size());
	perfTiming.agentCount = static_cast<int>(m_registry->Agents().size());
	long long stageStartMicros = 0;
	const int aiControllerCount = getAiSoldierCount();
	perfTiming.aiControllerCount = aiControllerCount;
	AIUpdateSystem::FrameContext aiFrameContext;
	aiFrameContext.agents = &m_registry->Agents();
	aiFrameContext.objectManager = this;
	aiFrameContext.scheduler = m_aiScheduler;
	aiFrameContext.spatialIndex = m_agentSpatialIndex;
	aiFrameContext.perceptionSystem = m_agentPerceptionSystem;
	aiFrameContext.teamBlackboard = m_teamBlackboardService;
	aiFrameContext.aiControllerCount = aiControllerCount;
	aiFrameContext.perfEnabled = perfEnabled;
	aiFrameContext.timing = &perfTiming;
	const bool useAiScheduler = m_aiUpdateSystem.BeginFrame(deltaMilliseconds, aiFrameContext);

	if (perfEnabled)
		stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
	SandboxEventDispatcherManager::GetGlobalInst().FlushQueuedEvents();
	if (perfEnabled)
		perfTiming.eventFlushMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
	if (m_tacticalQueryService != nullptr)
		m_tacticalQueryService->Update(deltaMilliseconds);

	ObjectLifecycleSystem::UpdateContext lifecycleContext;
	lifecycleContext.objects = &m_registry->Objects();
	lifecycleContext.removedSceneNodes = &m_remSceneNodes;
	lifecycleContext.objectManager = this;
	lifecycleContext.aiUpdateSystem = &m_aiUpdateSystem;
	lifecycleContext.scheduler = m_aiScheduler;
	lifecycleContext.useAiScheduler = useAiScheduler;
	lifecycleContext.perfEnabled = perfEnabled;
	lifecycleContext.timing = &perfTiming;

	m_aiUpdateSystem.UpdatePerception(deltaMilliseconds, aiFrameContext);
	m_objectLifecycleSystem.UpdateObjects(deltaMilliseconds, lifecycleContext);
	m_aiUpdateSystem.EndFrame(deltaMilliseconds, aiFrameContext);
	m_objectLifecycleSystem.CleanupRemovedSceneNodes(deltaMilliseconds, lifecycleContext);
	if (perfEnabled)
	{
		m_aiUpdateSystem.FillPerfStats(aiFrameContext);
		RuntimeStallProfiler::SetObjectUpdateTiming(perfTiming);
	}
}

void ObjectManager::HandleKeyEvent(OIS::KeyCode keycode, unsigned int key)
{
	for (auto iter = m_registry->Agents().begin(); iter != m_registry->Agents().end(); iter++)
	{
		if (AgentObject* pAgent = *iter)
		{
			pAgent->HandleKeyEvent(keycode, key);
		}
	}
}

void ObjectManager::SetSandboxServices(const SandboxServices& services)
{
	m_services = services;
	if (m_services.objects == nullptr)
		m_services.objects = this;
	if (m_services.physics == nullptr)
		m_services.physics = m_pPhysicsWorld;
	if (m_services.script == nullptr)
		m_services.script = m_pScriptVM;

	for (std::unordered_map<int, BaseObject*>::iterator iter = m_registry->Objects().begin(); iter != m_registry->Objects().end(); ++iter)
	{
		if (iter->second != nullptr)
			iter->second->SetSandboxServices(&m_services);
	}
}

const std::vector<AgentObject*>& ObjectManager::getAllAgents()
{
	return m_registry->Agents();
}

const std::vector<BlockObject*>& ObjectManager::getAllBlocks()
{
	return m_registry->Blocks();
}

std::vector<AgentObject*> ObjectManager::getSpecifyAgents(AGENT_OBJ_TYPE agentType)
{
	if (agentType == AGENT_OBJ_NONE)
	{
		return m_registry->Agents();
	}

	std::vector<AgentObject*> specifyAgents;

	std::vector<AgentObject*>::iterator iter;
	for (iter = m_registry->Agents().begin(); iter != m_registry->Agents().end(); iter++)
	{
		AgentObject* pAgentObject = *iter;
		if (pAgentObject->getAgentType() == agentType)
		{
			specifyAgents.push_back(pAgentObject);
		}
	}

	return specifyAgents;
}

int ObjectManager::getObjectCount() const
{
	return static_cast<int>(m_registry->Objects().size());
}

int ObjectManager::getAiAgentCount() const
{
	return static_cast<int>(m_registry->Agents().size());
}

int ObjectManager::getAiSoldierCount() const
{
	int count = 0;
	for (std::unordered_map<int, BaseObject*>::const_iterator it = m_registry->Objects().begin(); it != m_registry->Objects().end(); ++it)
	{
		if (it->second != nullptr && it->second->FindComponent<AIController>() != nullptr)
			++count;
	}
	return count;
}

void ObjectManager::configureAiScheduler(bool enabled, int tickIntervalMs, int maxTicksPerFrame)
{
	if (m_aiScheduler == nullptr)
		return;
	m_aiScheduler->configureAiScheduler(enabled, tickIntervalMs, maxTicksPerFrame);
}

void ObjectManager::clearTeamBlackboardFacts()
{
	if (m_teamBlackboardService != nullptr)
		m_teamBlackboardService->clearTeamBlackboardFacts();
}

void ObjectManager::configureTeamBlackboard(int ttlMs)
{
	if (m_teamBlackboardService != nullptr)
		m_teamBlackboardService->configureTeamBlackboard(ttlMs);
}

bool ObjectManager::rememberTeamEnemyFact(int teamId, int reporterId, int targetId, const Ogre::Vector3& targetPosition, int lastSeenMs, float confidence)
{
	if (m_teamBlackboardService == nullptr)
		return false;
	return m_teamBlackboardService->rememberTeamEnemyFact(teamId, reporterId, targetId, targetPosition, lastSeenMs, confidence);
}

bool ObjectManager::writeBestTeamEnemyFactToBlackboard(AgentObject* agent, const std::string& keyPrefix, bool allowOwnReport)
{
	if (m_teamBlackboardService == nullptr)
		return false;
	return m_teamBlackboardService->writeBestTeamEnemyFactToBlackboard(agent, keyPrefix, allowOwnReport);
}

int ObjectManager::getTeamBlackboardFactCount() const
{
	return m_teamBlackboardService != nullptr ? m_teamBlackboardService->getTeamBlackboardFactCount() : 0;
}

int ObjectManager::getTeamBlackboardReportCount() const
{
	return m_teamBlackboardService != nullptr ? m_teamBlackboardService->getTeamBlackboardReportCount() : 0;
}

std::string ObjectManager::buildTeamBlackboardDebugSummary() const
{
	if (m_teamBlackboardService == nullptr)
		return "[TeamBlackboardService] unavailable";
	return m_teamBlackboardService->buildTeamBlackboardDebugSummary();
}

void ObjectManager::clearTacticalInfluence()
{
	if (m_tacticalQueryService != nullptr)
		m_tacticalQueryService->ClearInfluence();
}

void ObjectManager::configureTacticalInfluence(float minX, float maxX, float minZ, float maxZ, float cellSize)
{
	if (m_tacticalQueryService != nullptr)
		m_tacticalQueryService->ConfigureInfluence(minX, maxX, minZ, maxZ, cellSize);
	clearTacticalInfluenceDebugVisuals();
}

void ObjectManager::configureTacticalInfluenceFromNavMesh(const std::string& navMeshName, float cellWidth, float cellHeight, const Ogre::Vector3& boundaryMinOffset, const Ogre::Vector3& boundaryMaxOffset)
{
	if (m_tacticalQueryService == nullptr)
		return;

	NavigationMesh* navMesh = getNavigationMesh(navMeshName);
	m_tacticalQueryService->ConfigureInfluenceFromNavMesh(navMesh, cellWidth, cellHeight, boundaryMinOffset, boundaryMaxOffset);

	clearTacticalInfluenceDebugVisuals();
}

void ObjectManager::clearTacticalInfluenceLayer(const std::string& layerName)
{
	if (m_tacticalQueryService != nullptr)
		m_tacticalQueryService->ClearInfluenceLayer(layerName);
}

void ObjectManager::setTacticalInfluenceLayerOptions(const std::string& layerName, float falloff, float inertia)
{
	if (m_tacticalQueryService != nullptr)
		m_tacticalQueryService->SetInfluenceLayerOptions(layerName, falloff, inertia);
}

int ObjectManager::addTacticalInfluenceSource(const std::string& layerName, const Ogre::Vector3& center, float strength, float radius)
{
	if (m_tacticalQueryService == nullptr)
		return 0;
	return m_tacticalQueryService->AddInfluenceSource(layerName, center, strength, radius);
}

int ObjectManager::addTacticalInfluencePoint(const std::string& layerName, const Ogre::Vector3& center, float strength)
{
	if (m_tacticalQueryService == nullptr)
		return 0;
	return m_tacticalQueryService->AddInfluencePoint(layerName, center, strength);
}

int ObjectManager::spreadTacticalInfluenceLayer(const std::string& layerName, int passCount)
{
	if (m_tacticalQueryService == nullptr)
		return 0;
	return m_tacticalQueryService->SpreadInfluenceLayer(layerName, passCount);
}

float ObjectManager::sampleTacticalInfluence(const std::string& layerName, const Ogre::Vector3& position) const
{
	if (m_tacticalQueryService == nullptr)
		return 0.0f;
	return m_tacticalQueryService->SampleInfluenceLayer(layerName, position);
}

float ObjectManager::scoreTacticalPosition(const Ogre::Vector3& position, float dangerWeight, float teamWeight, float objectiveWeight) const
{
	if (m_tacticalQueryService == nullptr)
		return 0.0f;
	return m_tacticalQueryService->ScoreInfluencePosition(position, dangerWeight, teamWeight, objectiveWeight);
}

Ogre::Vector3 ObjectManager::findBestTacticalPosition(const Ogre::Vector3& center, float radius, float step, float dangerWeight, float teamWeight, float objectiveWeight)
{
	if (m_tacticalQueryService == nullptr)
		return center;
	return m_tacticalQueryService->FindBestInfluencePosition(center, radius, step, dangerWeight, teamWeight, objectiveWeight);
}

float ObjectManager::scoreTacticalQueryPosition(const std::string& queryType, const Ogre::Vector3& position) const
{
	if (m_tacticalQueryService == nullptr)
		return 0.0f;
	return m_tacticalQueryService->ScoreQueryPosition(queryType, position);
}

Ogre::Vector3 ObjectManager::findBestTacticalQueryPosition(const std::string& queryType, const Ogre::Vector3& center, float radius, float step)
{
	if (m_tacticalQueryService == nullptr)
		return center;
	return m_tacticalQueryService->FindBestQueryPosition(queryType, center, radius, step);
}

Ogre::Vector3 ObjectManager::findBestSupportPosition(const Ogre::Vector3& center, float radius, float step)
{
	if (m_tacticalQueryService == nullptr)
		return center;
	return m_tacticalQueryService->FindBestSupportPosition(center, radius, step);
}

Ogre::Vector3 ObjectManager::findLowThreatPosition(const Ogre::Vector3& center, float radius, float step)
{
	if (m_tacticalQueryService == nullptr)
		return center;
	return m_tacticalQueryService->FindLowThreatPosition(center, radius, step);
}

void ObjectManager::configureTacticalEvents(int eventTtlMs)
{
	if (m_tacticalQueryService != nullptr)
		m_tacticalQueryService->SetEventTtlMs(eventTtlMs);
}

void ObjectManager::clearTacticalEvents()
{
	if (m_tacticalQueryService != nullptr)
		m_tacticalQueryService->ClearEvents();
}

void ObjectManager::publishTacticalEvent(const std::string& eventType, int senderId, int targetId, int teamId, int targetTeamId, const Ogre::Vector3& position, int timeMs, const std::string& scopeName, bool queueEvent)
{
	if (m_tacticalQueryService != nullptr)
	{
		int resolvedTimeMs = timeMs;
		if (resolvedTimeMs <= 0)
		{
			resolvedTimeMs = static_cast<int>(std::max<long long>(0, std::min<long long>(m_currentTimeMs, std::numeric_limits<int>::max())));
		}
		m_tacticalQueryService->PublishEvent(eventType, senderId, targetId, teamId, targetTeamId, position, resolvedTimeMs, scopeName, queueEvent);
	}
}

int ObjectManager::getTacticalEventCount() const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetEventCount() : 0;
}

int ObjectManager::getTacticalEventTypeCount(const std::string& eventType) const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetEventCountByType(eventType) : 0;
}

Ogre::Vector3 ObjectManager::getLastTacticalEventPosition(const std::string& eventType, const Ogre::Vector3& fallback) const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetLastEventPosition(eventType, fallback) : fallback;
}

int ObjectManager::getTacticalEventDebugRecordCount() const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetEventDebugRecordCount() : 0;
}

std::string ObjectManager::getTacticalEventDebugType(int luaIndex) const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetEventDebugType(luaIndex) : "";
}

int ObjectManager::getTacticalEventDebugSenderId(int luaIndex) const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetEventDebugSenderId(luaIndex) : -1;
}

int ObjectManager::getTacticalEventDebugTargetId(int luaIndex) const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetEventDebugTargetId(luaIndex) : -1;
}

int ObjectManager::getTacticalEventDebugTeamId(int luaIndex) const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetEventDebugTeamId(luaIndex) : -1;
}

int ObjectManager::getTacticalEventDebugTargetTeamId(int luaIndex) const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetEventDebugTargetTeamId(luaIndex) : -1;
}

Ogre::Vector3 ObjectManager::getTacticalEventDebugPosition(int luaIndex) const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetEventDebugPosition(luaIndex) : Ogre::Vector3::ZERO;
}

int ObjectManager::getTacticalEventDebugTimeMs(int luaIndex) const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetEventDebugTimeMs(luaIndex) : 0;
}

int ObjectManager::getTacticalEventDebugRemainingTtlMs(int luaIndex) const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetEventDebugRemainingTtlMs(luaIndex) : 0;
}

int ObjectManager::rebuildTacticalDangerLayer(int perspectiveTeamId, float dangerStrength, float bulletShotRadius, float bulletImpactRadius, float deadFriendlyRadius, float enemySightingRadius, int spreadPasses)
{
	if (m_tacticalQueryService == nullptr)
		return 0;
	return m_tacticalQueryService->RebuildDangerLayer(perspectiveTeamId, dangerStrength, bulletShotRadius, bulletImpactRadius, deadFriendlyRadius, enemySightingRadius, spreadPasses);
}

int ObjectManager::rebuildTacticalTeamLayer(int positiveTeamId, float teamStrength, float radius, int spreadPasses)
{
	if (m_tacticalQueryService == nullptr)
		return 0;
	return m_tacticalQueryService->RebuildTeamLayer(m_registry->Agents(), positiveTeamId, teamStrength, radius, spreadPasses);
}

int ObjectManager::rebuildTacticalObjectiveLayer(const Ogre::Vector3& center, float strength, float radius, int spreadPasses)
{
	if (m_tacticalQueryService == nullptr)
		return 0;
	return m_tacticalQueryService->RebuildObjectiveLayer(center, strength, radius, spreadPasses);
}

int ObjectManager::getTacticalInfluenceLayerActiveCellCount(const std::string& layerName) const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetInfluenceLayerActiveCellCount(layerName) : 0;
}

int ObjectManager::getTacticalInfluenceLayerCellWriteCount(const std::string& layerName) const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetInfluenceLayerCellWriteCount(layerName) : 0;
}

int ObjectManager::getTacticalInfluenceLayerDebugCellCount(const std::string& layerName, float threshold, int maxCells) const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetInfluenceLayerDebugCellCount(layerName, threshold, maxCells) : 0;
}

Ogre::Vector3 ObjectManager::getTacticalInfluenceLayerDebugCellPosition(const std::string& layerName, int luaIndex, float threshold) const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetInfluenceLayerDebugCellPosition(layerName, luaIndex, threshold) : Ogre::Vector3::ZERO;
}

float ObjectManager::getTacticalInfluenceLayerDebugCellValue(const std::string& layerName, int luaIndex, float threshold) const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetInfluenceLayerDebugCellValue(layerName, luaIndex, threshold) : 0.0f;
}

int ObjectManager::drawTacticalInfluenceLayer(const std::string& layerName, float yOffset, const Ogre::ColourValue& positiveValue, const Ogre::ColourValue& zeroValue, const Ogre::ColourValue& negativeValue, const Ogre::ColourValue& gridColor, float threshold, int maxCells, bool drawNeutralCells, bool projectToNav, float maxProjectionDistance, const Ogre::String& navMeshName)
{
	if (m_tacticalQueryService == nullptr || m_tacticalDebugDrawService == nullptr)
		return 0;
	return m_tacticalDebugDrawService->DrawLayer(m_tacticalQueryService->GetInfluenceMapSystem(), layerName, yOffset, positiveValue, zeroValue, negativeValue, gridColor, threshold, maxCells, drawNeutralCells, projectToNav, maxProjectionDistance, navMeshName);
}

int ObjectManager::rebuildTacticalInfluenceLayerDebugVisual(const std::string& layerName, float yOffset, const Ogre::ColourValue& positiveValue, const Ogre::ColourValue& zeroValue, const Ogre::ColourValue& negativeValue, const Ogre::ColourValue& gridColor, float threshold, int maxCells, bool drawNeutralCells, bool projectToNav, float maxProjectionDistance, const Ogre::String& navMeshName)
{
	if (m_tacticalQueryService == nullptr || m_tacticalDebugDrawService == nullptr)
		return 0;
	return m_tacticalDebugDrawService->RebuildLayerDebugVisual(m_tacticalQueryService->GetInfluenceMapSystem(), layerName, yOffset, positiveValue, zeroValue, negativeValue, gridColor, threshold, maxCells, drawNeutralCells, projectToNav, maxProjectionDistance, navMeshName);
}

void ObjectManager::setTacticalInfluenceDebugVisible(bool visible)
{
	if (m_tacticalDebugDrawService != nullptr)
		m_tacticalDebugDrawService->SetVisible(visible);
}

void ObjectManager::clearTacticalInfluenceDebugVisuals()
{
	if (m_tacticalDebugDrawService != nullptr)
		m_tacticalDebugDrawService->ClearVisuals();
}

int ObjectManager::getTacticalInfluenceActiveCellCount() const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetInfluenceActiveCellCount() : 0;
}

int ObjectManager::getTacticalInfluenceCellWriteCount() const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetInfluenceCellWriteCount() : 0;
}

int ObjectManager::getTacticalInfluenceQueryCount() const
{
	return m_tacticalQueryService != nullptr ? m_tacticalQueryService->GetInfluenceQueryCount() : 0;
}

std::string ObjectManager::buildTacticalInfluenceDebugSummary() const
{
	if (m_tacticalQueryService == nullptr)
		return "[TacticalQueryService] unavailable";
	return m_tacticalQueryService->BuildDebugSummary();
}

std::string ObjectManager::buildAiSchedulerDebugSummary() const
{
	if (m_aiScheduler == nullptr)
		return "[AIScheduler] unavailable";
	return m_aiScheduler->buildAiSchedulerDebugSummary();
}

std::string ObjectManager::buildAiEventDebugSummary(int maxAgents, int maxEvents)
{
	if (maxAgents < 0)
		maxAgents = 0;
	if (maxAgents > 32)
		maxAgents = 32;
	if (maxEvents < 0)
		maxEvents = 0;
	if (maxEvents > 32)
		maxEvents = 32;

	const int showingCount = maxAgents < static_cast<int>(m_registry->Agents().size()) ? maxAgents : static_cast<int>(m_registry->Agents().size());
	std::ostringstream stream;
	stream << "[AIEvent] agents=" << m_registry->Agents().size()
		<< " showing=" << showingCount
		<< " maxEvents=" << maxEvents;
	for (int index = 0; index < showingCount; ++index)
	{
		AgentObject* agent = m_registry->Agents()[index];
		if (agent == nullptr)
			continue;

		stream << "\n" << agent->Event()->BuildDebugSummary("agent#" + std::to_string(agent->GetObjId()), maxEvents);
	}
	return stream.str();
}

std::string ObjectManager::runAiEventScopeSelfTest()
{
	static int selfTestRunIndex = 0;
	const int runIndex = ++selfTestRunIndex;
	const std::string eventName = std::string(SandboxEventTypes::EnemySighted()) + "_ScopeSelfTest_" + std::to_string(runIndex);
	SandboxEventDispatcherManager localManager;
	SandboxEventDispatcherManager& globalManager = SandboxEventDispatcherManager::GetGlobalInst();

	int failures = 0;
	int localCount = 0;
	int teamOneCount = 0;
	int teamTwoCount = 0;
	int globalCount = 0;
	int localSenderId = -1;
	int teamTwoTargetId = -1;
	int globalTeamId = -1;
	std::ostringstream details;

	const SandboxEventDispatcherManager::Token localToken = localManager.SubscribeScoped(eventName, SandboxEventScope::Local, [&localCount, &localSenderId](const SandboxContext& context) {
		++localCount;
		localSenderId = static_cast<int>(context.Get_Number(SandboxEventFields::SenderId(), -1.0));
	});
	const SandboxEventDispatcherManager::Token teamOneToken = localManager.SubscribeScoped(eventName + "?teamId=1", SandboxEventScope::Team, [&teamOneCount](const SandboxContext&) {
		++teamOneCount;
	});
	const SandboxEventDispatcherManager::Token teamTwoToken = localManager.SubscribeScoped(eventName + "?teamId=2", SandboxEventScope::Team, [&teamTwoCount, &teamTwoTargetId](const SandboxContext& context) {
		++teamTwoCount;
		teamTwoTargetId = static_cast<int>(context.Get_Number(SandboxEventFields::TargetId(), -1.0));
	});
	const SandboxEventDispatcherManager::Token globalToken = globalManager.SubscribeScoped(eventName, SandboxEventScope::Global, [&globalCount, &globalTeamId](const SandboxContext& context) {
		++globalCount;
		globalTeamId = static_cast<int>(context.Get_Number(SandboxEventFields::TeamId(), -1.0));
	});

	if (localToken <= 0 || teamOneToken <= 0 || teamTwoToken <= 0 || globalToken <= 0)
		AppendAiEventSelfTestFailure(details, "subscribe token was not created", failures);

	localManager.Emit(eventName, BuildAiEventSelfTestContext(SandboxEventTypes::EnemySighted(), SandboxEventScope::Local, 101, 201, 1, 10.0));
	if (localCount != 1 || localSenderId != 101)
		AppendAiEventSelfTestFailure(details, "local scope did not dispatch exactly once", failures);
	if (teamOneCount != 0 || teamTwoCount != 0 || globalCount != 0)
		AppendAiEventSelfTestFailure(details, "local scope leaked into team/global subscribers", failures);

	localManager.Emit(eventName, BuildAiEventSelfTestContext(SandboxEventTypes::EnemySighted(), SandboxEventScope::Team, 102, 202, 1, 20.0));
	if (teamOneCount != 1 || teamTwoCount != 0)
		AppendAiEventSelfTestFailure(details, "team scope did not respect teamId=1 filter", failures);

	localManager.Emit(eventName + "?teamId=2", BuildAiEventSelfTestContext(SandboxEventTypes::EnemySighted(), SandboxEventScope::Team, 103, 203, -1, 30.0));
	if (teamOneCount != 1 || teamTwoCount != 1 || teamTwoTargetId != 203)
		AppendAiEventSelfTestFailure(details, "team scope did not apply event-name filter params", failures);

	const bool queued = localManager.QueueEmit(eventName, BuildAiEventSelfTestContext(SandboxEventTypes::SupportRequested(), SandboxEventScope::Team, 104, 204, 1, 40.0));
	const int queuedBeforeFlush = localManager.GetQueuedEventCount();
	const int flushedCount = localManager.FlushQueuedEvents(1);
	const int queuedAfterFlush = localManager.GetQueuedEventCount();
	if (!queued || queuedBeforeFlush != 1 || flushedCount != 1 || queuedAfterFlush != 0 || teamOneCount != 2)
		AppendAiEventSelfTestFailure(details, "queued team event did not flush through scoped dispatcher", failures);

	const bool unsubscribed = localManager.UnsubscribeScoped(eventName, SandboxEventScope::Team, teamOneToken);
	localManager.Emit(eventName, BuildAiEventSelfTestContext(SandboxEventTypes::SupportResponded(), SandboxEventScope::Team, 105, 205, 1, 50.0));
	if (!unsubscribed || teamOneCount != 2)
		AppendAiEventSelfTestFailure(details, "team unsubscribe did not remove callback", failures);

	localManager.Emit(eventName, BuildAiEventSelfTestContext(SandboxEventTypes::BulletShot(), SandboxEventScope::Global, 106, 206, 9, 60.0));
	if (globalCount != 1 || globalTeamId != 9)
		AppendAiEventSelfTestFailure(details, "global scope did not route through global dispatcher", failures);
	if (localCount != 1 || teamOneCount != 2 || teamTwoCount != 1)
		AppendAiEventSelfTestFailure(details, "global scope leaked back into local/team subscribers", failures);

	globalManager.UnsubscribeScoped(eventName, SandboxEventScope::Global, globalToken);
	globalManager.RemoveDispatcher(eventName, SandboxEventScope::Global);
	localManager.UnsubscribeScoped(eventName, SandboxEventScope::Local, localToken);
	localManager.UnsubscribeScoped(eventName, SandboxEventScope::Team, teamTwoToken);

	std::ostringstream result;
	result << "[AIEventSelfTest] result=" << (failures == 0 ? "true" : "false")
		<< " failures=" << failures
		<< " event=" << eventName
		<< " local=" << localCount
		<< " team1=" << teamOneCount
		<< " team2=" << teamTwoCount
		<< " global=" << globalCount
		<< " queuedBefore=" << queuedBeforeFlush
		<< " flushed=" << flushedCount
		<< " queuedAfter=" << queuedAfterFlush
		<< " unsubscribed=" << (unsubscribed ? "true" : "false")
		<< "\n" << localManager.BuildDebugSummary("selftest.local", 8);
	const std::string detailText = details.str();
	if (!detailText.empty())
		result << detailText;
	return result.str();
}

static std::string BuildBlackboardSelfTestSummary()
{
	std::ostringstream stream;
	int failures = 0;

	Blackboard blackboard;
	blackboard.SetObjectIdEntry("self.target", 42, 1.0f, 1000, 500, "selftest");
	blackboard.SetEntryDecayPolicy("self.target", Blackboard::ENTRY_DECAY_LINEAR, 0.001f);
	blackboard.UpdateEntries(1250, 250);

	int targetId = -1;
	Blackboard::Entry targetEntry;
	if (!blackboard.GetObjectIdEntry("self.target", targetId, &targetEntry) || targetId != 42)
	{
		AppendSelfTestFailure(stream, "typed object id entry was not preserved after decay", failures);
	}
	else if (!NearlyEqual(targetEntry.confidence, 0.75f, 0.01f))
	{
		std::ostringstream failure;
		failure << "linear decay confidence expected 0.75 got " << FormatReal(targetEntry.confidence);
		AppendSelfTestFailure(stream, failure.str(), failures);
	}

	const std::string debugLine = blackboard.BuildEntryDebugString("self.target", 1250);
	if (debugLine.find("conf=0.75") == std::string::npos || debugLine.find("ttl=500") == std::string::npos || debugLine.find("decay=linear") == std::string::npos)
	{
		AppendSelfTestFailure(stream, "debug string did not include confidence, ttl and decay policy", failures);
	}

	const int ttlRemoved = blackboard.UpdateEntries(1601, 351);
	if (ttlRemoved != 1 || blackboard.HasEntry("self.target"))
	{
		AppendSelfTestFailure(stream, "ttl expiration did not remove stale entry", failures);
	}

	blackboard.SetFloatEntry("self.confidence", 3.0f, 1.0f, 2000, -1, "selftest");
	blackboard.SetEntryDecayPolicy("self.confidence", Blackboard::ENTRY_DECAY_LINEAR, 0.5f);
	const int confidenceRemoved = blackboard.UpdateEntries(2003, 3);
	if (confidenceRemoved != 1 || blackboard.HasEntry("self.confidence"))
	{
		AppendSelfTestFailure(stream, "confidence reaching zero did not remove entry", failures);
	}

	Blackboard memoryBlackboard;
	MemoryStore memoryStore(&memoryBlackboard);
	MemoryStoreConfig memoryConfig;
	memoryConfig.ttlMs = 500;
	memoryConfig.decayRate = 0.001f;
	memoryConfig.source = "selftest";
	AgentPerceptionResult perceptionResult;
	perceptionResult.targetId = 7;
	perceptionResult.targetPosition = Ogre::Vector3(1.0f, 2.0f, 3.0f);
	perceptionResult.distanceSquared = 25.0f;
	perceptionResult.confidence = 1.0f;
	memoryStore.RememberVisibleEnemy(perceptionResult, 1000, memoryConfig);
	memoryBlackboard.UpdateEntries(1250, 250);

	EnemyMemory enemyMemory;
	if (!memoryStore.GetLastKnownEnemy(1250, enemyMemory) || enemyMemory.enemyId != 7 || !NearlyEqual(enemyMemory.confidence, 0.75f, 0.01f))
	{
		AppendSelfTestFailure(stream, "memory store did not preserve last known enemy with decayed confidence", failures);
	}

	const int memoryExpired = memoryBlackboard.UpdateEntries(1601, 351);
	memoryStore.SyncSnapshot(1601);
	if (memoryExpired != 3 || memoryStore.HasLastKnownEnemy(1601))
	{
		AppendSelfTestFailure(stream, "memory store ttl expiration did not clear last known enemy entries", failures);
	}
	if (memoryBlackboard.GetBool(AIMemoryKeys::kMemorySnapshotHasLastKnownEnemy, true))
	{
		AppendSelfTestFailure(stream, "memory store did not clear typed Lua snapshot after expiration", failures);
	}

	std::string details = stream.str();
	std::ostringstream result;
	result << "[BlackboardSelfTest] result=" << (failures == 0 ? "true" : "false")
		<< " failures=" << failures
		<< " ttlRemoved=" << ttlRemoved
		<< " confidenceRemoved=" << confidenceRemoved
		<< " memoryExpired=" << memoryExpired
		<< " debug=\"" << debugLine << "\"";
	if (!details.empty())
		result << details;
	return result.str();
}

std::string ObjectManager::buildAiDebugSummary(int maxAgents)
{
	if (maxAgents < 0)
		maxAgents = 0;
	if (maxAgents > 32)
		maxAgents = 32;

	const int aiControllerCount = getAiSoldierCount();
	const int showingCount = maxAgents < static_cast<int>(m_registry->Agents().size()) ? maxAgents : static_cast<int>(m_registry->Agents().size());
	int visibleTargetCount = 0;
	int memoryTargetCount = 0;
	int blackboardEntryCount = 0;
	for (AgentObject* agent : m_registry->Agents())
	{
		if (agent == nullptr)
			continue;

		AIController* ai = agent->FindComponent<AIController>();
		Blackboard* blackboard = ai != nullptr ? ai->GetBlackboard() : nullptr;
		if (blackboard == nullptr)
			continue;

		if (blackboard->GetBool("perception.hasTarget", false))
			++visibleTargetCount;
		if (blackboard->GetBool("memory.snapshot.hasLastKnownEnemy", false))
			++memoryTargetCount;
		blackboardEntryCount += blackboard->GetEntryCount();
	}
	std::ostringstream stream;
	stream << "AI agents=" << m_registry->Agents().size()
		<< " controllers=" << aiControllerCount
		<< " showing=" << showingCount
		<< " visibleTargets=" << visibleTargetCount
		<< " memoryTargets=" << memoryTargetCount
		<< " bbEntries=" << blackboardEntryCount;
	if (m_agentSpatialIndex != nullptr)
		stream << "\n" << m_agentSpatialIndex->BuildDebugSummary();
	if (m_agentPerceptionSystem != nullptr)
		stream << "\n" << m_agentPerceptionSystem->BuildDebugSummary();
	if (m_teamBlackboardService != nullptr)
		stream << "\n" << m_teamBlackboardService->BuildDebugSummary();
	if (m_tacticalQueryService != nullptr)
		stream << "\n" << m_tacticalQueryService->BuildDebugSummary();

	for (int index = 0; index < showingCount; ++index)
	{
		AgentObject* agent = m_registry->Agents()[index];
		if (agent == nullptr)
			continue;

		SoldierObject* soldier = dynamic_cast<SoldierObject*>(agent);
		AIController* ai = agent->FindComponent<AIController>();
		stream << "\n#" << index
			<< " id=" << agent->GetObjId()
			<< " team=" << agent->GetTeamId()
			<< " type=" << agent->getAgentType()
			<< " state=" << agent->GetCurStateName() << "(" << agent->GetCurStateId() << ")"
			<< " hp=" << static_cast<int>(agent->GetHealth())
			<< " pos=" << FormatVec3Precise(agent->GetPosition())
			<< " vel=" << FormatVec3Precise(agent->GetVelocity())
			<< " speed=" << FormatReal(agent->GetSpeed())
			<< " target=" << FormatVec3Precise(agent->GetTarget());

		if (ai == nullptr)
		{
			stream << " driver=Agent";
			continue;
		}

		if (soldier != nullptr)
		{
			stream << "/" << static_cast<int>(soldier->GetMaxHealth())
				<< " ammo=" << soldier->GetAmmo() << "/" << soldier->GetMaxAmmo();
		}

		BehaviorTreeDriver* behaviorDriver = ai->GetBehaviorTreeDriver();
		DecisionTreeDriver* decisionDriver = ai->GetDecisionTreeDriver();
		if (behaviorDriver != nullptr)
		{
			if (!behaviorDriver->IsDebugTraceEnabled())
				behaviorDriver->SetDebugTraceEnabled(true);
			stream << " driver=BT traceFrame=" << behaviorDriver->GetDebugTraceFrame();
			AppendBlackboardBrief(stream, behaviorDriver->GetBlackboard());
			const std::string trace = behaviorDriver->GetLastDebugTrace();
			if (!trace.empty())
				stream << " trace=" << TrimDebugText(trace, 96);
			else
				stream << " trace=pending";
		}
		else if (decisionDriver != nullptr)
		{
			stream << " driver=DT";
			AppendBlackboardBrief(stream, decisionDriver->GetBlackboard());
		}
		else
		{
			stream << " driver=" << (ai->GetFsmController() != nullptr ? "FSM" : "None");
		}
		const std::string sensorSummary = ai->BuildSensorDebugString();
		if (!sensorSummary.empty())
			stream << " " << sensorSummary;
		const std::string memorySummary = ai->BuildMemoryDebugString();
		if (!memorySummary.empty())
			stream << " " << memorySummary;
	}
	const char* blackboardSelfTest = std::getenv("HELLO_AI_BLACKBOARD_SELF_TEST");
	if (blackboardSelfTest != nullptr && (std::string(blackboardSelfTest) == "1" || std::string(blackboardSelfTest) == "true"))
	{
		stream << "\n" << BuildBlackboardSelfTestSummary();
	}
	return stream.str();
}

std::string ObjectManager::buildObjectDebugSummary(int maxObjects)
{
	if (maxObjects < 0)
		maxObjects = 0;
	if (maxObjects > 64)
		maxObjects = 64;

	std::vector<int> objectIds;
	objectIds.reserve(m_registry->Objects().size());
	for (std::unordered_map<int, BaseObject*>::const_iterator iter = m_registry->Objects().begin(); iter != m_registry->Objects().end(); ++iter)
	{
		objectIds.push_back(iter->first);
	}
	std::sort(objectIds.begin(), objectIds.end());

	const int showingCount = maxObjects < static_cast<int>(objectIds.size()) ? maxObjects : static_cast<int>(objectIds.size());
	const int navMeshCount = m_services.navigation != nullptr ? m_services.navigation->GetNavigationMeshCount() : 0;
	std::ostringstream stream;
	stream << "[ObjectInspector] objects=" << m_registry->Objects().size()
		<< " agents=" << m_registry->Agents().size()
		<< " blocks=" << m_registry->Blocks().size()
		<< " navMeshes=" << navMeshCount
		<< " pendingSceneNodes=" << m_remSceneNodes.size()
		<< " showing=" << showingCount;

	for (int index = 0; index < showingCount; ++index)
	{
		std::unordered_map<int, BaseObject*>::const_iterator found = m_registry->Objects().find(objectIds[index]);
		if (found == m_registry->Objects().end() || found->second == nullptr)
			continue;

		BaseObject* object = found->second;
		stream << "\n[ObjectInspector] id=" << object->GetObjId()
			<< " type=" << ObjectTypeToString(object->GetObjType())
			<< " team=" << object->GetTeamId()
			<< " liveTicks=" << object->GetLiveTicks()
			<< " components=" << object->BuildComponentDebugString();

		if (AgentObject* agent = dynamic_cast<AgentObject*>(object))
		{
			stream << " pos=" << FormatVec3(agent->GetPosition())
				<< " vel=" << FormatVec3(agent->GetVelocity())
				<< " hp=" << static_cast<int>(agent->GetHealth())
				<< " speed=" << static_cast<int>(agent->GetSpeed())
				<< " target=" << FormatVec3(agent->GetTarget())
				<< " hasPath=" << (agent->HasPath() ? "true" : "false")
				<< " agentType=" << agent->getAgentType()
				<< " state=" << agent->GetCurStateName() << "(" << agent->GetCurStateId() << ")";
		}
		else if (BlockObject* block = dynamic_cast<BlockObject*>(object))
		{
			stream << " pos=" << FormatVec3(block->GetPosition())
				<< " mass=" << static_cast<int>(block->GetMass());
			if (block->GetObjType() != BaseObject::OBJ_TYPE_PLANE)
			{
				stream << " radius=" << static_cast<int>(block->GetRadius());
			}
		}

		if (SoldierObject* soldier = dynamic_cast<SoldierObject*>(object))
		{
			AIController* ai = soldier->GetAIController();
			const char* driverName = "None";
			if (ai != nullptr && ai->GetBehaviorTreeDriver() != nullptr)
				driverName = "BT";
			else if (ai != nullptr && ai->GetDecisionTreeDriver() != nullptr)
				driverName = "DT";
			else if (ai != nullptr && ai->GetFsmController() != nullptr)
				driverName = "FSM";

			stream << " soldierHpMax=" << static_cast<int>(soldier->GetMaxHealth())
				<< " ammo=" << soldier->GetAmmo() << "/" << soldier->GetMaxAmmo()
				<< " driver=" << driverName;
		}
	}
	return stream.str();
}

void ObjectManager::clearAllObjects(int objType, bool forceAll)
{
	if ((objType & MGR_OBJ_BLOCK) != 0)
	{
		auto iter = m_registry->Blocks().begin();
		for (; iter != m_registry->Blocks().end(); iter++)
		{
			auto pBlock = *iter;
			if (!forceAll && pBlock->GetObjType() != BaseObject::OBJ_TYPE_BLOCK)
				continue; //防止删除Plane

			m_registry->Objects().erase(pBlock->GetObjId());
			SAFE_DELETE(pBlock);
		}
		m_registry->Blocks().clear();
	}

	if ((objType & MGR_OBJ_AGENT) != 0)
	{
		auto iter = m_registry->Agents().begin();
		for (; iter != m_registry->Agents().end(); iter++)
		{
			auto pAgent = *iter;
			m_registry->Objects().erase(pAgent->GetObjId());
			if (m_aiScheduler != nullptr)
				m_aiScheduler->RemoveAgent(pAgent->GetObjId());

			SAFE_DELETE(pAgent);
		}
		m_registry->Agents().clear();
		if (m_aiScheduler != nullptr)
			m_aiScheduler->Clear();
		if (m_agentSpatialIndex != nullptr)
			m_agentSpatialIndex->Clear();
		if (m_agentPerceptionSystem != nullptr)
			m_agentPerceptionSystem->Clear();
		if (m_teamBlackboardService != nullptr)
			m_teamBlackboardService->Clear();
		if (m_tacticalQueryService != nullptr)
			m_tacticalQueryService->Clear();
	}
}

std::vector<BlockObject*> ObjectManager::getFixedObjects()
{
	std::size_t fixedObjects = 0;

	auto iter = m_registry->Objects().begin();
	for (; iter != m_registry->Objects().end(); iter++)
	{
		BaseObject* pObject = iter->second;
		if (pObject && pObject->GetMass() <= 0.0f)
		{
			if (pObject->GetObjType() != BaseObject::OBJ_TYPE_PLANE)
			{
				fixedObjects++;
			}
		}
	}

	std::vector<BlockObject*> objects;
	objects.reserve(fixedObjects);

	auto iter1 = m_registry->Objects().begin();
	for (; iter1 != m_registry->Objects().end(); iter1++)
	{
		BlockObject* pObject = dynamic_cast<BlockObject*>(iter1->second);
		if (pObject && pObject->GetMass() <= 0.0f)
		{
			if (pObject->GetObjType() != BaseObject::OBJ_TYPE_PLANE)
			{
				objects.push_back(pObject);
			}
		}
	}

	return objects;
}

void ObjectManager::addNewObject(BaseObject* pObject)
{
	if (pObject == nullptr)
		return;

	pObject->SetSandboxServices(&m_services);

	unsigned int objectId = m_registry->AllocateObjectId();
	pObject->SetObjId(objectId);
	pObject->Init();
	m_registry->RegisterObject(objectId, pObject);

	this->realAddObject(pObject);
}

void ObjectManager::realAddObject(BaseObject* pObject)
{
	int objtype = pObject->GetObjType();
	if (objtype >= BaseObject::OBJ_TYPE_AGENT)
	{
		auto newObject = dynamic_cast<AgentObject*>(pObject);
		assert(newObject != nullptr);
		if (m_agentSpatialIndex != nullptr)
			m_agentSpatialIndex->AddOrUpdateAgent(newObject);
	}
	else if (objtype >= BaseObject::OBJ_TYPE_BLOCK)
	{
		assert(dynamic_cast<BlockObject*>(pObject) != nullptr);
	}
}

bool ObjectManager::realRemoveObject(BaseObject* pObject)
{
	if (pObject == nullptr) 
		return false;
	
	int objtype = pObject->GetObjType();
	unsigned int objid = pObject->GetObjId();
	if (objtype >= BaseObject::OBJ_TYPE_AGENT)
	{
		AgentObject* agent = dynamic_cast<AgentObject*>(pObject);
		assert(agent != nullptr);
		if (!m_registry->UnregisterTypeIndex(pObject))
			return false;
		if (m_agentSpatialIndex != nullptr)
			m_agentSpatialIndex->RemoveAgent(agent);
		if (m_aiScheduler != nullptr)
			m_aiScheduler->RemoveAgent(objid);
		SAFE_DELETE(pObject);
		return true;
	}
	
	if (objtype >= BaseObject::OBJ_TYPE_BLOCK)
	{
		assert(dynamic_cast<BlockObject*>(pObject) != nullptr);
		if (!m_registry->UnregisterTypeIndex(pObject))
			return false;
		SAFE_DELETE(pObject);
		return true;
	}
	
	return false;
}

bool ObjectManager::removeObjectById(int objid)
{
	BaseObject* pObject = nullptr;
	if (!m_registry->EraseObjectById(objid, &pObject))
		return false;
	return realRemoveObject(pObject);
}

BaseObject* ObjectManager::getObjectById(int objid)
{
	return m_registry->FindObject(objid);
}

void ObjectManager::markNodeRemInSeconds(Ogre::SceneNode* pSceneNode, float seconds)
{
	if (pSceneNode == nullptr)
		return;
	m_remSceneNodes[pSceneNode] = int(seconds * 1000);
}

NavigationMesh* ObjectManager::getNavigationMesh(const Ogre::String& navName)
{
	return m_services.navigation != nullptr ? m_services.navigation->GetNavigationMesh(navName) : nullptr;
}

bool ObjectManager::addNavigationMesh(const Ogre::String& navName, NavigationMesh* pNavMesh)
{
	return m_services.navigation != nullptr ? m_services.navigation->AddNavigationMesh(navName, pNavMesh) : false;
}

