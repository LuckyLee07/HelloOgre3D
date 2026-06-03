#include "ai/tactics/TacticalQueryService.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

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
}

void TacticalQueryService::ClearEvents()
{
	m_events.clear();
	m_stats.currentEventCount = 0;
}

void TacticalQueryService::Update(int deltaMs)
{
	const int elapsedMs = std::max(0, deltaMs);
	if (elapsedMs <= 0 || m_events.empty())
	{
		m_stats.currentEventCount = static_cast<int>(m_events.size());
		return;
	}

	std::vector<EventRecord> validEvents;
	validEvents.reserve(m_events.size());
	for (std::vector<EventRecord>::iterator iter = m_events.begin(); iter != m_events.end(); ++iter)
	{
		EventRecord event = *iter;
		if (event.remainingTtlMs > 0)
		{
			event.remainingTtlMs -= elapsedMs;
			if (event.remainingTtlMs <= 0)
			{
				++m_stats.expiredEventCount;
				continue;
			}
		}
		validEvents.push_back(event);
	}
	m_events.swap(validEvents);
	m_stats.currentEventCount = static_cast<int>(m_events.size());
}

void TacticalQueryService::SetEventTtlMs(int ttlMs)
{
	m_eventTtlMs = std::max(1, ttlMs);
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
	return writes + spreadWrites;
}

int TacticalQueryService::RebuildTeamLayer(const std::vector<AgentObject*>& agents, int positiveTeamId, float teamStrength, float radius, int spreadPasses)
{
	m_influenceMap.ClearLayer("team");
	int writes = 0;
	for (std::vector<AgentObject*>::const_iterator iter = agents.begin(); iter != agents.end(); ++iter)
	{
		AgentObject* agent = *iter;
		if (agent == nullptr || agent->GetHealth() <= 0.0f)
			continue;
		const float value = static_cast<int>(agent->GetTeamId()) == positiveTeamId ? teamStrength : -teamStrength;
		writes += m_influenceMap.AddRadialSource("team", agent->GetPosition(), value, radius);
	}

	const int spreadWrites = m_influenceMap.SpreadLayer("team", spreadPasses);
	m_stats.lastTeamWrites = writes;
	m_stats.lastTeamSpreadWrites = spreadWrites;
	++m_stats.teamBuildCount;
	return writes + spreadWrites;
}

int TacticalQueryService::RebuildObjectiveLayer(const Ogre::Vector3& center, float strength, float radius, int spreadPasses)
{
	m_influenceMap.ClearLayer("objective");
	const int writes = m_influenceMap.AddRadialSource("objective", center, strength, radius);
	const int spreadWrites = m_influenceMap.SpreadLayer("objective", spreadPasses);
	m_stats.lastObjectiveWrites = writes;
	m_stats.lastObjectiveSpreadWrites = spreadWrites;
	++m_stats.objectiveBuildCount;
	return writes + spreadWrites;
}

float TacticalQueryService::ScoreQueryPosition(const std::string& queryType, const Ogre::Vector3& position) const
{
	float dangerWeight = 1.0f;
	float teamWeight = 1.0f;
	float objectiveWeight = 1.0f;
	ResolveQueryWeights(queryType, dangerWeight, teamWeight, objectiveWeight);
	return m_influenceMap.ScorePosition(position, dangerWeight, teamWeight, objectiveWeight);
}

Ogre::Vector3 TacticalQueryService::FindBestQueryPosition(const std::string& queryType, const Ogre::Vector3& center, float radius, float step)
{
	float dangerWeight = 1.0f;
	float teamWeight = 1.0f;
	float objectiveWeight = 1.0f;
	ResolveQueryWeights(queryType, dangerWeight, teamWeight, objectiveWeight);
	return m_influenceMap.FindBestPosition(center, radius, step, dangerWeight, teamWeight, objectiveWeight);
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
		<< "\n" << m_influenceMap.BuildDebugSummary();
	return stream.str();
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
				return;
			}
		}
	}
	m_events.push_back(event);
	m_stats.currentEventCount = static_cast<int>(m_events.size());
}

const TacticalQueryService::EventRecord* TacticalQueryService::GetEventRecordByLuaIndex(int luaIndex) const
{
	if (luaIndex <= 0)
		return nullptr;
	const size_t index = static_cast<size_t>(luaIndex - 1);
	return index < m_events.size() ? &m_events[index] : nullptr;
}

void TacticalQueryService::ResolveQueryWeights(const std::string& queryType, float& dangerWeight, float& teamWeight, float& objectiveWeight) const
{
	if (queryType == "lowThreat" || queryType == "LowThreat")
	{
		dangerWeight = 1.0f;
		teamWeight = 0.2f;
		objectiveWeight = 0.0f;
		return;
	}
	if (queryType == "advance" || queryType == "Advance")
	{
		dangerWeight = 0.8f;
		teamWeight = 0.35f;
		objectiveWeight = 1.3f;
		return;
	}

	dangerWeight = 1.0f;
	teamWeight = 1.0f;
	objectiveWeight = 1.0f;
}
