#include "ai/tactics/TacticalQueryService.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

#include "ai/navigation/NavigationMesh.h"
#include "event/SandboxContext.h"
#include "event/SandboxEventDispatcherManager.h"
#include "objects/AgentObject.h"

namespace
{
	const int kDefaultEventTtlMs = 1800;
	const int kScopeLocal = 0;
	const int kScopeTeam = 1;
	const int kScopeGlobal = 2;

	bool IsBulletEvent(const std::string& eventType)
	{
		return eventType == SandboxEventTypes::BulletShot() || eventType == SandboxEventTypes::BulletImpact();
	}

	bool IsPersistentTacticalEvent(const std::string& eventType)
	{
		return eventType == SandboxEventTypes::EnemySighted() || eventType == SandboxEventTypes::DeadFriendlySighted();
	}

	SandboxEventScope ToScope(int scope)
	{
		switch (scope)
		{
		case kScopeTeam:
			return SandboxEventScope::Team;
		case kScopeGlobal:
			return SandboxEventScope::Global;
		case kScopeLocal:
		default:
			return SandboxEventScope::Local;
		}
	}

	SandboxEventScope ParseScope(const std::string& scopeName)
	{
		if (scopeName == "team" || scopeName == "Team" || scopeName == "TEAM")
			return SandboxEventScope::Team;
		if (scopeName == "global" || scopeName == "Global" || scopeName == "GLOBAL")
			return SandboxEventScope::Global;
		return SandboxEventScope::Local;
	}

	bool SamePersistentEvent(const std::string& leftType, int leftSenderId, int leftTargetId, const std::string& rightType, int rightSenderId, int rightTargetId)
	{
		if (leftType != rightType)
			return false;
		if (leftTargetId >= 0 || rightTargetId >= 0)
			return leftTargetId == rightTargetId;
		return leftSenderId == rightSenderId;
	}
}

TacticalQueryService::Stats::Stats()
	: capturedEventCount(0)
	, currentEventCount(0)
	, expiredEventCount(0)
	, dangerBuildCount(0)
	, teamBuildCount(0)
	, objectiveBuildCount(0)
	, lastDangerWrites(0)
	, lastTeamWrites(0)
	, lastObjectiveWrites(0)
	, lastDangerSpreadWrites(0)
	, lastTeamSpreadWrites(0)
	, lastObjectiveSpreadWrites(0)
	, layerPolicyCount(0)
	, dirtyLayerCount(0)
	, skippedBuildCount(0)
	, intervalSkippedBuildCount(0)
	, dirtySkippedBuildCount(0)
{
}

TacticalQueryService::EventRecord::EventRecord()
	: senderId(-1)
	, targetId(-1)
	, teamId(-1)
	, targetTeamId(-1)
	, position(Ogre::Vector3::ZERO)
	, timeMs(0)
	, remainingTtlMs(-1)
{
}

TacticalQueryService::Subscription::Subscription()
	: scope(kScopeLocal)
	, token(0)
{
}

TacticalQueryService::LayerUpdatePolicy::LayerUpdatePolicy()
	: intervalMs(0)
	, elapsedMs(0)
	, dirtyOnly(false)
	, dirty(true)
	, skippedBuildCount(0)
	, intervalSkippedBuildCount(0)
	, dirtySkippedBuildCount(0)
{
}

TacticalQueryService::QueryWeights::QueryWeights()
	: danger(1.0f)
	, team(1.0f)
	, objective(1.0f)
	, support(0.0f)
	, cover(0.0f)
	, crowd(0.0f)
{
}

TacticalQueryService::TacticalQueryService()
	: m_eventTtlMs(kDefaultEventTtlMs)
	, m_subscribed(false)
{
}

TacticalQueryService::~TacticalQueryService()
{
	Shutdown();
}

void TacticalQueryService::Initialize()
{
	SubscribeEvents();
}

void TacticalQueryService::Shutdown()
{
	if (!m_subscribed)
		return;

	SandboxEventDispatcherManager& manager = SandboxEventDispatcherManager::GetGlobalInst();
	for (std::vector<Subscription>::iterator iter = m_subscriptions.begin(); iter != m_subscriptions.end(); ++iter)
	{
		if (iter->token > 0)
			manager.UnsubscribeScoped(iter->eventType, ToScope(iter->scope), iter->token);
	}
	m_subscriptions.clear();
	m_subscribed = false;
}

void TacticalQueryService::Clear()
{
	m_influenceMap.Clear();
	ClearEvents();
	MarkAllLayerPoliciesDirty();
}

void TacticalQueryService::ClearEvents()
{
	m_events.clear();
	m_stats.currentEventCount = 0;
	MarkDefaultTacticalLayerPoliciesDirty();
}

void TacticalQueryService::Update(int deltaMs)
{
	const int elapsedMs = std::max(0, deltaMs);
	AdvanceLayerPolicies(elapsedMs);
	if (elapsedMs <= 0 || m_events.empty())
	{
		m_stats.currentEventCount = static_cast<int>(m_events.size());
		return;
	}

	std::vector<EventRecord> validEvents;
	validEvents.reserve(m_events.size());
	bool expiredDangerEvent = false;
	for (std::vector<EventRecord>::iterator iter = m_events.begin(); iter != m_events.end(); ++iter)
	{
		EventRecord event = *iter;
		if (event.remainingTtlMs > 0)
		{
			event.remainingTtlMs -= elapsedMs;
			if (event.remainingTtlMs <= 0)
			{
				++m_stats.expiredEventCount;
				if (IsBulletEvent(event.eventType))
					expiredDangerEvent = true;
				continue;
			}
		}
		validEvents.push_back(event);
	}
	m_events.swap(validEvents);
	m_stats.currentEventCount = static_cast<int>(m_events.size());
	if (expiredDangerEvent)
		MarkTrackedLayerDirty("danger");
}

void TacticalQueryService::SetEventTtlMs(int ttlMs)
{
	m_eventTtlMs = std::max(1, ttlMs);
}

void TacticalQueryService::ConfigureLayerUpdatePolicy(const std::string& layerName, int intervalMs, bool dirtyOnly)
{
	LayerUpdatePolicy& policy = GetLayerPolicy(layerName);
	policy.intervalMs = std::max(0, intervalMs);
	policy.elapsedMs = policy.intervalMs;
	policy.dirtyOnly = dirtyOnly;
	policy.dirty = true;
	RefreshPolicyStats();
}

void TacticalQueryService::MarkLayerDirty(const std::string& layerName)
{
	LayerUpdatePolicy& policy = GetLayerPolicy(layerName);
	policy.dirty = true;
	RefreshPolicyStats();
}

void TacticalQueryService::SetQueryCandidateLimit(int maxCandidates)
{
	m_influenceMap.SetMaxQueryCandidates(maxCandidates);
}

int TacticalQueryService::GetQueryCandidateLimit() const
{
	return m_influenceMap.GetMaxQueryCandidates();
}

void TacticalQueryService::ClearInfluence()
{
	m_influenceMap.Clear();
	MarkAllLayerPoliciesDirty();
}

void TacticalQueryService::ConfigureInfluence(float minX, float maxX, float minZ, float maxZ, float cellSize)
{
	m_influenceMap.Configure(minX, maxX, minZ, maxZ, cellSize);
	MarkDefaultTacticalLayerPoliciesDirty();
}

bool TacticalQueryService::ConfigureInfluenceFromNavMesh(const NavigationMesh* navMesh, float cellWidth, float cellHeight, const Ogre::Vector3& boundaryMinOffset, const Ogre::Vector3& boundaryMaxOffset)
{
	if (navMesh == nullptr)
		return false;

	std::vector<float> verts;
	std::vector<int> indices;
	if (!navMesh->GetWalkableTriangles(verts, indices))
		return false;

	m_influenceMap.BuildFromNavMesh(verts, indices, cellWidth, cellHeight, boundaryMinOffset, boundaryMaxOffset);
	MarkDefaultTacticalLayerPoliciesDirty();
	return true;
}

void TacticalQueryService::ClearInfluenceLayer(const std::string& layerName)
{
	m_influenceMap.ClearLayer(layerName);
	MarkTrackedLayerDirty(layerName);
}

void TacticalQueryService::SetInfluenceLayerOptions(const std::string& layerName, float falloff, float inertia)
{
	m_influenceMap.SetLayerOptions(layerName, falloff, inertia);
	MarkTrackedLayerDirty(layerName);
}

int TacticalQueryService::AddInfluenceSource(const std::string& layerName, const Ogre::Vector3& center, float strength, float radius)
{
	const int writes = m_influenceMap.AddRadialSource(layerName, center, strength, radius);
	if (writes > 0)
		MarkTrackedLayerDirty(layerName);
	return writes;
}

int TacticalQueryService::AddInfluencePoint(const std::string& layerName, const Ogre::Vector3& center, float strength)
{
	const int writes = m_influenceMap.AddPointSource(layerName, center, strength);
	if (writes > 0)
		MarkTrackedLayerDirty(layerName);
	return writes;
}

int TacticalQueryService::SpreadInfluenceLayer(const std::string& layerName, int passCount)
{
	const int writes = m_influenceMap.SpreadLayer(layerName, passCount);
	if (writes > 0)
		MarkTrackedLayerDirty(layerName);
	return writes;
}

float TacticalQueryService::SampleInfluenceLayer(const std::string& layerName, const Ogre::Vector3& position) const
{
	return m_influenceMap.SampleLayer(layerName, position);
}

float TacticalQueryService::ScoreInfluencePosition(const Ogre::Vector3& position, float dangerWeight, float teamWeight, float objectiveWeight) const
{
	return m_influenceMap.ScorePosition(position, dangerWeight, teamWeight, objectiveWeight);
}

Ogre::Vector3 TacticalQueryService::FindBestInfluencePosition(const Ogre::Vector3& center, float radius, float step, float dangerWeight, float teamWeight, float objectiveWeight)
{
	return m_influenceMap.FindBestPosition(center, radius, step, dangerWeight, teamWeight, objectiveWeight);
}

int TacticalQueryService::GetInfluenceLayerActiveCellCount(const std::string& layerName) const
{
	return m_influenceMap.GetLayerActiveCellCount(layerName);
}

int TacticalQueryService::GetInfluenceLayerCellWriteCount(const std::string& layerName) const
{
	return m_influenceMap.GetLayerCellWriteCount(layerName);
}

int TacticalQueryService::GetInfluenceLayerDebugCellCount(const std::string& layerName, float threshold, int maxCells) const
{
	return m_influenceMap.GetLayerDebugCellCount(layerName, threshold, maxCells);
}

Ogre::Vector3 TacticalQueryService::GetInfluenceLayerDebugCellPosition(const std::string& layerName, int luaIndex, float threshold) const
{
	return m_influenceMap.GetLayerDebugCellPosition(layerName, luaIndex, threshold);
}

float TacticalQueryService::GetInfluenceLayerDebugCellValue(const std::string& layerName, int luaIndex, float threshold) const
{
	return m_influenceMap.GetLayerDebugCellValue(layerName, luaIndex, threshold);
}

int TacticalQueryService::GetInfluenceActiveCellCount() const
{
	return m_influenceMap.GetStats().activeCellCount;
}

int TacticalQueryService::GetInfluenceCellWriteCount() const
{
	return m_influenceMap.GetStats().cellWriteCount;
}

int TacticalQueryService::GetInfluenceQueryCount() const
{
	return m_influenceMap.GetStats().queryCount;
}

void TacticalQueryService::PublishEvent(const std::string& eventType, int senderId, int targetId, int teamId, int targetTeamId, const Ogre::Vector3& position, int timeMs, const std::string& scopeName, bool queueEvent)
{
	if (eventType.empty())
		return;

	SandboxContext context;
	context.Set_String(SandboxEventFields::EventType(), eventType);
	context.Set_String(SandboxEventFields::Scope(), SandboxEventPayload::ToString(ParseScope(scopeName)));
	context.Set_Number(SandboxEventFields::SenderId(), static_cast<double>(senderId));
	context.Set_Number(SandboxEventFields::TargetId(), static_cast<double>(targetId));
	context.Set_Number(SandboxEventFields::TeamId(), static_cast<double>(teamId));
	context.Set_Number("targetTeamId", static_cast<double>(targetTeamId));
	context.Set_Number(SandboxEventFields::TimeMs(), static_cast<double>(timeMs));
	SandboxEventPayload::SetPosition(context, position);

	SandboxEventDispatcherManager& manager = SandboxEventDispatcherManager::GetGlobalInst();
	if (queueEvent)
		manager.QueueEmit(eventType, context);
	else
		manager.Emit(eventType, context);
}

int TacticalQueryService::GetEventCount() const
{
	return static_cast<int>(m_events.size());
}

int TacticalQueryService::GetEventCountByType(const std::string& eventType) const
{
	int count = 0;
	for (std::vector<EventRecord>::const_iterator iter = m_events.begin(); iter != m_events.end(); ++iter)
	{
		if (iter->eventType == eventType)
			++count;
	}
	return count;
}

Ogre::Vector3 TacticalQueryService::GetLastEventPosition(const std::string& eventType, const Ogre::Vector3& fallback) const
{
	for (std::vector<EventRecord>::const_reverse_iterator iter = m_events.rbegin(); iter != m_events.rend(); ++iter)
	{
		if (eventType.empty() || iter->eventType == eventType)
			return iter->position;
	}
	return fallback;
}

int TacticalQueryService::GetEventDebugRecordCount() const
{
	return static_cast<int>(m_events.size());
}

std::string TacticalQueryService::GetEventDebugType(int luaIndex) const
{
	const EventRecord* event = GetEventRecordByLuaIndex(luaIndex);
	return event != nullptr ? event->eventType : "";
}

int TacticalQueryService::GetEventDebugSenderId(int luaIndex) const
{
	const EventRecord* event = GetEventRecordByLuaIndex(luaIndex);
	return event != nullptr ? event->senderId : -1;
}

int TacticalQueryService::GetEventDebugTargetId(int luaIndex) const
{
	const EventRecord* event = GetEventRecordByLuaIndex(luaIndex);
	return event != nullptr ? event->targetId : -1;
}

int TacticalQueryService::GetEventDebugTeamId(int luaIndex) const
{
	const EventRecord* event = GetEventRecordByLuaIndex(luaIndex);
	return event != nullptr ? event->teamId : -1;
}

int TacticalQueryService::GetEventDebugTargetTeamId(int luaIndex) const
{
	const EventRecord* event = GetEventRecordByLuaIndex(luaIndex);
	return event != nullptr ? event->targetTeamId : -1;
}

Ogre::Vector3 TacticalQueryService::GetEventDebugPosition(int luaIndex) const
{
	const EventRecord* event = GetEventRecordByLuaIndex(luaIndex);
	return event != nullptr ? event->position : Ogre::Vector3::ZERO;
}

int TacticalQueryService::GetEventDebugTimeMs(int luaIndex) const
{
	const EventRecord* event = GetEventRecordByLuaIndex(luaIndex);
	return event != nullptr ? event->timeMs : 0;
}

int TacticalQueryService::GetEventDebugRemainingTtlMs(int luaIndex) const
{
	const EventRecord* event = GetEventRecordByLuaIndex(luaIndex);
	return event != nullptr ? event->remainingTtlMs : 0;
}

int TacticalQueryService::RebuildDangerLayer(int perspectiveTeamId, float dangerStrength, float bulletShotRadius, float bulletImpactRadius, float deadFriendlyRadius, float enemySightingRadius, int spreadPasses)
{
	if (!ShouldRebuildLayer("danger"))
	{
		m_stats.lastDangerWrites = 0;
		m_stats.lastDangerSpreadWrites = 0;
		return 0;
	}

	m_influenceMap.ClearLayer("danger");
	int writes = 0;
	for (std::vector<EventRecord>::const_iterator iter = m_events.begin(); iter != m_events.end(); ++iter)
	{
		if (iter->eventType == SandboxEventTypes::BulletShot())
		{
			writes += m_influenceMap.AddRadialSource("danger", iter->position, dangerStrength, bulletShotRadius);
		}
		else if (iter->eventType == SandboxEventTypes::BulletImpact())
		{
			writes += m_influenceMap.AddRadialSource("danger", iter->position, dangerStrength, bulletImpactRadius);
		}
		else if (iter->eventType == SandboxEventTypes::DeadFriendlySighted())
		{
			if (iter->teamId == perspectiveTeamId)
				writes += m_influenceMap.AddRadialSource("danger", iter->position, dangerStrength, deadFriendlyRadius);
		}
		else if (iter->eventType == SandboxEventTypes::EnemySighted())
		{
			const int targetTeamId = iter->targetTeamId >= 0 ? iter->targetTeamId : iter->teamId;
			if (targetTeamId != perspectiveTeamId)
				writes += m_influenceMap.AddRadialSource("danger", iter->position, dangerStrength, enemySightingRadius);
		}
	}

	const int spreadWrites = m_influenceMap.SpreadLayer("danger", spreadPasses);
	m_stats.lastDangerWrites = writes;
	m_stats.lastDangerSpreadWrites = spreadWrites;
	++m_stats.dangerBuildCount;
	CompleteLayerRebuild("danger");
	return writes + spreadWrites;
}

int TacticalQueryService::RebuildTeamLayer(const std::vector<AgentObject*>& agents, int positiveTeamId, float teamStrength, float radius, int spreadPasses)
{
	if (!ShouldRebuildLayer("team"))
	{
		m_stats.lastTeamWrites = 0;
		m_stats.lastTeamSpreadWrites = 0;
		return 0;
	}

	m_influenceMap.ClearLayer("team");
	m_influenceMap.ClearLayer("support");
	m_influenceMap.ClearLayer("cover");
	m_influenceMap.ClearLayer("crowd");
	int writes = 0;
	for (std::vector<AgentObject*>::const_iterator iter = agents.begin(); iter != agents.end(); ++iter)
	{
		AgentObject* agent = *iter;
		if (agent == nullptr || agent->GetHealth() <= 0.0f)
			continue;
		const bool friendly = static_cast<int>(agent->GetTeamId()) == positiveTeamId;
		const float absStrength = std::fabs(teamStrength);
		const float value = static_cast<int>(agent->GetTeamId()) == positiveTeamId ? teamStrength : -teamStrength;
		writes += m_influenceMap.AddRadialSource("team", agent->GetPosition(), value, radius);
		if (friendly)
		{
			writes += m_influenceMap.AddRadialSource("support", agent->GetPosition(), absStrength, radius);
			writes += m_influenceMap.AddRadialSource("cover", agent->GetPosition(), absStrength * 0.35f, radius * 0.75f);
		}
		writes += m_influenceMap.AddRadialSource("crowd", agent->GetPosition(), -absStrength * 0.35f, radius * 0.75f);
	}

	const int spreadWrites = m_influenceMap.SpreadLayer("team", spreadPasses);
	const int supportSpreadWrites = m_influenceMap.SpreadLayer("support", spreadPasses);
	const int coverSpreadWrites = m_influenceMap.SpreadLayer("cover", spreadPasses);
	const int crowdSpreadWrites = m_influenceMap.SpreadLayer("crowd", spreadPasses);
	m_stats.lastTeamWrites = writes;
	m_stats.lastTeamSpreadWrites = spreadWrites + supportSpreadWrites + coverSpreadWrites + crowdSpreadWrites;
	++m_stats.teamBuildCount;
	CompleteLayerRebuild("team");
	return writes + m_stats.lastTeamSpreadWrites;
}

int TacticalQueryService::RebuildObjectiveLayer(const Ogre::Vector3& center, float strength, float radius, int spreadPasses)
{
	if (!ShouldRebuildLayer("objective"))
	{
		m_stats.lastObjectiveWrites = 0;
		m_stats.lastObjectiveSpreadWrites = 0;
		return 0;
	}

	m_influenceMap.ClearLayer("objective");
	const int writes = m_influenceMap.AddRadialSource("objective", center, strength, radius);
	const int spreadWrites = m_influenceMap.SpreadLayer("objective", spreadPasses);
	m_stats.lastObjectiveWrites = writes;
	m_stats.lastObjectiveSpreadWrites = spreadWrites;
	++m_stats.objectiveBuildCount;
	CompleteLayerRebuild("objective");
	return writes + spreadWrites;
}

float TacticalQueryService::ScoreQueryPosition(const std::string& queryType, const Ogre::Vector3& position) const
{
	QueryWeights weights;
	ResolveQueryWeights(queryType, weights);
	return m_influenceMap.ScorePositionLayers(position, weights.danger, weights.team, weights.objective, weights.support, weights.cover, weights.crowd);
}

Ogre::Vector3 TacticalQueryService::FindBestQueryPosition(const std::string& queryType, const Ogre::Vector3& center, float radius, float step)
{
	QueryWeights weights;
	ResolveQueryWeights(queryType, weights);
	return m_influenceMap.FindBestPositionLayers(center, radius, step, weights.danger, weights.team, weights.objective, weights.support, weights.cover, weights.crowd);
}

Ogre::Vector3 TacticalQueryService::FindBestSupportPosition(const Ogre::Vector3& center, float radius, float step)
{
	return FindBestQueryPosition("support", center, radius, step);
}

Ogre::Vector3 TacticalQueryService::FindLowThreatPosition(const Ogre::Vector3& center, float radius, float step)
{
	return FindBestQueryPosition("lowThreat", center, radius, step);
}

std::string TacticalQueryService::BuildDebugSummary() const
{
	std::ostringstream stream;
	stream << "[TacticalQueryService] events=" << m_stats.currentEventCount
		<< " captured=" << m_stats.capturedEventCount
		<< " expired=" << m_stats.expiredEventCount
		<< " dangerBuilds=" << m_stats.dangerBuildCount
		<< " teamBuilds=" << m_stats.teamBuildCount
		<< " objectiveBuilds=" << m_stats.objectiveBuildCount
		<< " writes=" << (m_stats.lastDangerWrites + m_stats.lastTeamWrites + m_stats.lastObjectiveWrites)
		<< " spreadWrites=" << (m_stats.lastDangerSpreadWrites + m_stats.lastTeamSpreadWrites + m_stats.lastObjectiveSpreadWrites)
		<< " policies=" << static_cast<int>(m_layerPolicies.size())
		<< " dirtyLayers=" << CountDirtyLayerPolicies()
		<< " skippedBuilds=" << m_stats.skippedBuildCount
		<< " intervalSkips=" << m_stats.intervalSkippedBuildCount
		<< " dirtySkips=" << m_stats.dirtySkippedBuildCount
		<< " schema=support/cover/crowd"
		<< " supportCells=" << m_influenceMap.GetLayerActiveCellCount("support")
		<< " coverCells=" << m_influenceMap.GetLayerActiveCellCount("cover")
		<< " crowdCells=" << m_influenceMap.GetLayerActiveCellCount("crowd")
		<< "\n" << m_influenceMap.BuildDebugSummary();
	if (!m_layerPolicies.empty())
	{
		stream << "\n[TacticalLayerPolicies]";
		for (std::unordered_map<std::string, LayerUpdatePolicy>::const_iterator iter = m_layerPolicies.begin(); iter != m_layerPolicies.end(); ++iter)
		{
			const LayerUpdatePolicy& policy = iter->second;
			stream << " " << iter->first
				<< "(intervalMs=" << policy.intervalMs
				<< ",elapsedMs=" << policy.elapsedMs
				<< ",dirtyOnly=" << (policy.dirtyOnly ? "true" : "false")
				<< ",dirty=" << (policy.dirty ? "true" : "false")
				<< ",skips=" << policy.skippedBuildCount
				<< ")";
		}
	}
	return stream.str();
}

std::string TacticalQueryService::NormalizeLayerName(const std::string& layerName) const
{
	return layerName.empty() ? "default" : layerName;
}

TacticalQueryService::LayerUpdatePolicy* TacticalQueryService::FindLayerPolicy(const std::string& layerName)
{
	std::unordered_map<std::string, LayerUpdatePolicy>::iterator found = m_layerPolicies.find(NormalizeLayerName(layerName));
	return found != m_layerPolicies.end() ? &found->second : nullptr;
}

const TacticalQueryService::LayerUpdatePolicy* TacticalQueryService::FindLayerPolicy(const std::string& layerName) const
{
	std::unordered_map<std::string, LayerUpdatePolicy>::const_iterator found = m_layerPolicies.find(NormalizeLayerName(layerName));
	return found != m_layerPolicies.end() ? &found->second : nullptr;
}

TacticalQueryService::LayerUpdatePolicy& TacticalQueryService::GetLayerPolicy(const std::string& layerName)
{
	return m_layerPolicies[NormalizeLayerName(layerName)];
}

void TacticalQueryService::AdvanceLayerPolicies(int elapsedMs)
{
	if (elapsedMs <= 0)
		return;

	for (std::unordered_map<std::string, LayerUpdatePolicy>::iterator iter = m_layerPolicies.begin(); iter != m_layerPolicies.end(); ++iter)
	{
		LayerUpdatePolicy& policy = iter->second;
		if (policy.intervalMs > 0)
			policy.elapsedMs = std::min(policy.intervalMs, policy.elapsedMs + elapsedMs);
		else
			policy.elapsedMs += elapsedMs;
	}
}

bool TacticalQueryService::ShouldRebuildLayer(const std::string& layerName)
{
	LayerUpdatePolicy* policy = FindLayerPolicy(layerName);
	if (policy == nullptr)
		return true;

	const bool intervalBlocked = policy->intervalMs > 0 && policy->elapsedMs < policy->intervalMs;
	const bool dirtyBlocked = policy->dirtyOnly && !policy->dirty;
	if (!intervalBlocked && !dirtyBlocked)
		return true;

	++policy->skippedBuildCount;
	++m_stats.skippedBuildCount;
	if (intervalBlocked)
	{
		++policy->intervalSkippedBuildCount;
		++m_stats.intervalSkippedBuildCount;
	}
	if (dirtyBlocked)
	{
		++policy->dirtySkippedBuildCount;
		++m_stats.dirtySkippedBuildCount;
	}
	RefreshPolicyStats();
	return false;
}

void TacticalQueryService::CompleteLayerRebuild(const std::string& layerName)
{
	LayerUpdatePolicy* policy = FindLayerPolicy(layerName);
	if (policy == nullptr)
		return;

	policy->elapsedMs = 0;
	policy->dirty = false;
	RefreshPolicyStats();
}

void TacticalQueryService::MarkTrackedLayerDirty(const std::string& layerName)
{
	LayerUpdatePolicy* policy = FindLayerPolicy(layerName);
	if (policy == nullptr)
		return;

	policy->dirty = true;
	RefreshPolicyStats();
}

void TacticalQueryService::MarkAllLayerPoliciesDirty()
{
	for (std::unordered_map<std::string, LayerUpdatePolicy>::iterator iter = m_layerPolicies.begin(); iter != m_layerPolicies.end(); ++iter)
		iter->second.dirty = true;
	RefreshPolicyStats();
}

void TacticalQueryService::MarkDefaultTacticalLayerPoliciesDirty()
{
	MarkTrackedLayerDirty("danger");
	MarkTrackedLayerDirty("team");
	MarkTrackedLayerDirty("objective");
}

void TacticalQueryService::MarkLayerPoliciesDirtyForEvent(const std::string& eventType)
{
	if (eventType == SandboxEventTypes::EnemySighted())
	{
		MarkTrackedLayerDirty("danger");
		MarkTrackedLayerDirty("objective");
		return;
	}
	if (eventType == SandboxEventTypes::DeadFriendlySighted() || IsBulletEvent(eventType))
		MarkTrackedLayerDirty("danger");
}

int TacticalQueryService::CountDirtyLayerPolicies() const
{
	int count = 0;
	for (std::unordered_map<std::string, LayerUpdatePolicy>::const_iterator iter = m_layerPolicies.begin(); iter != m_layerPolicies.end(); ++iter)
	{
		if (iter->second.dirty)
			++count;
	}
	return count;
}

void TacticalQueryService::RefreshPolicyStats()
{
	m_stats.layerPolicyCount = static_cast<int>(m_layerPolicies.size());
	m_stats.dirtyLayerCount = CountDirtyLayerPolicies();
}

void TacticalQueryService::SubscribeEvents()
{
	if (m_subscribed)
		return;

	const char* eventTypes[] = {
		SandboxEventTypes::BulletShot(),
		SandboxEventTypes::BulletImpact(),
		SandboxEventTypes::DeadFriendlySighted(),
		SandboxEventTypes::EnemySighted(),
	};
	const int scopes[] = { kScopeLocal, kScopeTeam, kScopeGlobal };
	SandboxEventDispatcherManager& manager = SandboxEventDispatcherManager::GetGlobalInst();
	for (int eventIndex = 0; eventIndex < 4; ++eventIndex)
	{
		for (int scopeIndex = 0; scopeIndex < 3; ++scopeIndex)
		{
			Subscription subscription;
			subscription.eventType = eventTypes[eventIndex];
			subscription.scope = scopes[scopeIndex];
			subscription.token = manager.SubscribeScoped(subscription.eventType, ToScope(subscription.scope), [this](const SandboxContext& context) {
				HandleEvent(context);
			});
			m_subscriptions.push_back(subscription);
		}
	}
	m_subscribed = true;
}

void TacticalQueryService::HandleEvent(const SandboxContext& context)
{
	EventRecord event;
	event.eventType = context.Get_String(SandboxEventFields::EventType(), "");
	if (event.eventType.empty())
		return;
	event.senderId = static_cast<int>(context.Get_Number(SandboxEventFields::SenderId(), -1.0));
	event.targetId = static_cast<int>(context.Get_Number(SandboxEventFields::TargetId(), -1.0));
	event.teamId = static_cast<int>(context.Get_Number(SandboxEventFields::TeamId(), -1.0));
	event.targetTeamId = static_cast<int>(context.Get_Number("targetTeamId", -1.0));
	event.timeMs = static_cast<int>(context.Get_Number(SandboxEventFields::TimeMs(), 0.0));
	event.position = Ogre::Vector3(
		static_cast<float>(context.Get_Number(SandboxEventFields::PositionX(), 0.0)),
		static_cast<float>(context.Get_Number(SandboxEventFields::PositionY(), 0.0)),
		static_cast<float>(context.Get_Number(SandboxEventFields::PositionZ(), 0.0)));
	event.remainingTtlMs = IsBulletEvent(event.eventType) ? m_eventTtlMs : -1;
	AddOrUpdateEvent(event);
}

void TacticalQueryService::AddOrUpdateEvent(const EventRecord& event)
{
	++m_stats.capturedEventCount;
	if (IsPersistentTacticalEvent(event.eventType))
	{
		for (std::vector<EventRecord>::iterator iter = m_events.begin(); iter != m_events.end(); ++iter)
		{
			if (SamePersistentEvent(iter->eventType, iter->senderId, iter->targetId, event.eventType, event.senderId, event.targetId))
			{
				m_events.erase(iter);
				m_events.push_back(event);
				m_stats.currentEventCount = static_cast<int>(m_events.size());
				MarkLayerPoliciesDirtyForEvent(event.eventType);
				return;
			}
		}
	}
	m_events.push_back(event);
	m_stats.currentEventCount = static_cast<int>(m_events.size());
	MarkLayerPoliciesDirtyForEvent(event.eventType);
}

const TacticalQueryService::EventRecord* TacticalQueryService::GetEventRecordByLuaIndex(int luaIndex) const
{
	if (luaIndex <= 0)
		return nullptr;
	const size_t index = static_cast<size_t>(luaIndex - 1);
	return index < m_events.size() ? &m_events[index] : nullptr;
}

void TacticalQueryService::ResolveQueryWeights(const std::string& queryType, QueryWeights& weights) const
{
	if (queryType == "lowThreat" || queryType == "LowThreat")
	{
		weights.danger = 1.0f;
		weights.team = 0.2f;
		weights.objective = 0.0f;
		weights.support = 0.2f;
		weights.cover = 0.45f;
		weights.crowd = 0.8f;
		return;
	}
	if (queryType == "advance" || queryType == "Advance")
	{
		weights.danger = 0.8f;
		weights.team = 0.35f;
		weights.objective = 1.3f;
		weights.support = 0.45f;
		weights.cover = 0.3f;
		weights.crowd = 0.7f;
		return;
	}

	weights.danger = 1.0f;
	weights.team = 1.0f;
	weights.objective = 1.0f;
	weights.support = 0.6f;
	weights.cover = 0.35f;
	weights.crowd = 0.7f;
}
