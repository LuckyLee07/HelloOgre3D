#include "ai/perception/AgentPerceptionSystem.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "ai/perception/AgentSpatialIndexSystem.h"
#include "components/ai/AIController.h"
#include "objects/AgentObject.h"
#include "profiling/Profile.h"
#include "profiling/RuntimeProfileCounters.h"
#include "systems/manager/ObjectManager.h"

AgentPerceptionSystem::AgentPerceptionSystem()
	: m_enabled(true)
	, m_updateCount(0)
{
}

void AgentPerceptionSystem::SetEnabled(bool enabled)
{
	m_enabled = enabled;
}

void AgentPerceptionSystem::Clear()
{
	m_updateCount = 0;
	m_stats = Stats();
	m_stats.enabled = m_enabled ? 1 : 0;
	clearHearingDangerSense();
}

void AgentPerceptionSystem::Update(const std::vector<AgentObject*>& agents, int deltaMs, ObjectManager* objectManager)
{
	H3D_PROFILE_SCOPE("AgentPerceptionSystem::Update");
	Stats stats;
	stats.enabled = m_enabled ? 1 : 0;
	stats.updateCount = ++m_updateCount;
	stats.agentCount = static_cast<int>(agents.size());

	const AgentSpatialIndexSystem* spatialIndex = objectManager != nullptr ? objectManager->GetAgentSpatialIndexSystem() : nullptr;
	AgentSpatialIndexSystem::Stats spatialBefore;
	if (spatialIndex != nullptr)
		spatialBefore = spatialIndex->GetStats();

	if (!m_enabled)
	{
		for (AgentObject* agent : agents)
		{
			AIController* ai = agent != nullptr ? agent->GetAIComponent() : nullptr;
			if (ai != nullptr)
				ai->SetPerceptionTickInAiTickEnabled(true);
		}
		CopyHearingDangerStats(stats);
		m_stats = stats;
		return;
	}

	for (AgentObject* agent : agents)
	{
		AIController* ai = agent != nullptr ? agent->GetAIComponent() : nullptr;
		if (ai == nullptr)
			continue;

		ai->SetPerceptionTickInAiTickEnabled(false);
		AIPerceptionTickStats tickStats;
		ai->TickPerception(deltaMs, &tickStats);
		++stats.controllerCount;
		if (tickStats.scanned)
			++stats.scanCount;
		if (tickStats.hasVisibleTarget)
			++stats.visibleCount;
		stats.memoryMs += tickStats.memoryMs;
		stats.visionMs += tickStats.visionMs;
	}

	m_hearingDangerSense.Update(agents, deltaMs, objectManager);
	CopyHearingDangerStats(stats);

	if (spatialIndex != nullptr)
	{
		const AgentSpatialIndexSystem::Stats& spatialAfter = spatialIndex->GetStats();
		stats.spatialQueryCount = std::max(0, spatialAfter.queryCount - spatialBefore.queryCount);
		stats.spatialCandidateCount = std::max(0, spatialAfter.candidateCount - spatialBefore.candidateCount);
		stats.spatialFilteredCandidateCount = std::max(0, spatialAfter.filteredCandidateCount - spatialBefore.filteredCandidateCount);
		stats.spatialResultCount = std::max(0, spatialAfter.resultCount - spatialBefore.resultCount);
		stats.spatialRejectedSelfCount = std::max(0, spatialAfter.rejectedSelfCount - spatialBefore.rejectedSelfCount);
		stats.spatialRejectedTeamCount = std::max(0, spatialAfter.rejectedTeamCount - spatialBefore.rejectedTeamCount);
		stats.spatialRejectedDeadCount = std::max(0, spatialAfter.rejectedDeadCount - spatialBefore.rejectedDeadCount);
		stats.spatialRejectedTypeCount = std::max(0, spatialAfter.rejectedTypeCount - spatialBefore.rejectedTypeCount);
		stats.spatialQueryCostMs = std::max(0.0, spatialAfter.queryCostMs - spatialBefore.queryCostMs);
	}

	m_stats = stats;
}

std::string AgentPerceptionSystem::BuildDebugSummary() const
{
	const float avgCandidates = m_stats.spatialQueryCount > 0
		? static_cast<float>(m_stats.spatialCandidateCount) / static_cast<float>(m_stats.spatialQueryCount)
		: 0.0f;
	const float avgResults = m_stats.spatialQueryCount > 0
		? static_cast<float>(m_stats.spatialResultCount) / static_cast<float>(m_stats.spatialQueryCount)
		: 0.0f;
	const float avgFiltered = m_stats.spatialQueryCount > 0
		? static_cast<float>(m_stats.spatialFilteredCandidateCount) / static_cast<float>(m_stats.spatialQueryCount)
		: 0.0f;

	std::ostringstream stream;
	stream << "[AgentPerceptionSystem] enabled=" << (m_enabled ? "true" : "false")
		<< " updates=" << m_stats.updateCount
		<< " agents=" << m_stats.agentCount
		<< " controllers=" << m_stats.controllerCount
		<< " scans=" << m_stats.scanCount
		<< " visible=" << m_stats.visibleCount
		<< " spatialQueries=" << m_stats.spatialQueryCount
		<< " candidates=" << m_stats.spatialCandidateCount
		<< " filtered=" << m_stats.spatialFilteredCandidateCount
		<< " results=" << m_stats.spatialResultCount
		<< " avgCandidates=" << std::fixed << std::setprecision(1) << avgCandidates
		<< " avgFiltered=" << std::fixed << std::setprecision(1) << avgFiltered
		<< " avgResults=" << std::fixed << std::setprecision(1) << avgResults
		<< " rejectSelf=" << m_stats.spatialRejectedSelfCount
		<< " rejectTeam=" << m_stats.spatialRejectedTeamCount
		<< " rejectDead=" << m_stats.spatialRejectedDeadCount
		<< " rejectType=" << m_stats.spatialRejectedTypeCount
		<< " queryMs=" << std::fixed << std::setprecision(2) << m_stats.spatialQueryCostMs
		<< " memoryMs=" << std::fixed << std::setprecision(2) << m_stats.memoryMs
		<< " visionMs=" << std::fixed << std::setprecision(2) << m_stats.visionMs
		<< " hearingEnabled=" << (m_stats.hearingDangerEnabled != 0 ? "true" : "false")
		<< " hearingEvents=" << m_stats.hearingDangerEventCount
		<< " hearingPublished=" << m_stats.hearingDangerPublishedEventCount
		<< " hearingPruned=" << m_stats.hearingDangerPrunedEventCount
		<< " hearingRuns=" << m_stats.hearingDangerRunCount
		<< " hearingSkips=" << m_stats.hearingDangerSkipCount
		<< " hearingChecks=" << m_stats.hearingDangerAgentCheckCount
		<< " heard=" << m_stats.hearingDangerHeardResponseCount
		<< " danger=" << m_stats.hearingDangerDangerResponseCount
		<< " investigations=" << m_stats.hearingDangerInvestigationCount
		<< " retreatFacts=" << m_stats.hearingDangerRetreatFactApplyCount;
	return stream.str();
}

void AgentPerceptionSystem::PublishTracyCounters() const
{
	RuntimeProfileCounters::PlotAgentPerceptionStats(
		m_stats.enabled,
		m_stats.controllerCount,
		m_stats.scanCount,
		m_stats.visibleCount,
		m_stats.spatialQueryCount,
		m_stats.spatialCandidateCount,
		m_stats.spatialFilteredCandidateCount,
		m_stats.spatialResultCount);
	RuntimeProfileCounters::PlotHearingDangerSenseStats(
		m_stats.hearingDangerEnabled,
		m_stats.hearingDangerEventCount,
		m_stats.hearingDangerPublishedEventCount,
		m_stats.hearingDangerPrunedEventCount,
		m_stats.hearingDangerRunCount,
		m_stats.hearingDangerSkipCount,
		m_stats.hearingDangerAgentCheckCount,
		m_stats.hearingDangerHeardResponseCount,
		m_stats.hearingDangerDangerResponseCount,
		m_stats.hearingDangerInvestigationCount,
		m_stats.hearingDangerRetreatFactApplyCount);
}

void AgentPerceptionSystem::configureHearingDangerSense(bool enabled, int scanIntervalMs, int agentsPerTick, int responseCooldownMs, int dangerCooldownMs, float investigateStopDistance, float escapeDistance)
{
	m_hearingDangerSense.Configure(enabled, scanIntervalMs, agentsPerTick, responseCooldownMs, dangerCooldownMs, investigateStopDistance, escapeDistance);
	CopyHearingDangerStats(m_stats);
}

void AgentPerceptionSystem::clearHearingDangerSense()
{
	m_hearingDangerSense.Clear();
	CopyHearingDangerStats(m_stats);
}

bool AgentPerceptionSystem::publishHearingDangerEvent(int sourceId, int sourceTeamId, int targetId, const Ogre::Vector3& position, const Ogre::Vector3& impactPosition, int timeMs, int ttlMs, float hearingRadius, float dangerRadius)
{
	const bool published = m_hearingDangerSense.PublishEvent(sourceId, sourceTeamId, targetId, position, impactPosition, timeMs, ttlMs, hearingRadius, dangerRadius);
	CopyHearingDangerStats(m_stats);
	return published;
}

int AgentPerceptionSystem::getHearingDangerEventCount() const
{
	return m_hearingDangerSense.GetStats().eventCount;
}

int AgentPerceptionSystem::getHearingDangerPublishedEventCount() const
{
	return m_hearingDangerSense.GetStats().publishedEventCount;
}

int AgentPerceptionSystem::getHearingDangerPrunedEventCount() const
{
	return m_hearingDangerSense.GetStats().prunedEventCount;
}

int AgentPerceptionSystem::getHearingDangerRunCount() const
{
	return m_hearingDangerSense.GetStats().runCount;
}

int AgentPerceptionSystem::getHearingDangerSkipCount() const
{
	return m_hearingDangerSense.GetStats().skipCount;
}

int AgentPerceptionSystem::getHearingDangerAgentCheckCount() const
{
	return m_hearingDangerSense.GetStats().agentCheckCount;
}

int AgentPerceptionSystem::getHearingDangerHeardResponseCount() const
{
	return m_hearingDangerSense.GetStats().heardResponseCount;
}

int AgentPerceptionSystem::getHearingDangerDangerResponseCount() const
{
	return m_hearingDangerSense.GetStats().dangerResponseCount;
}

int AgentPerceptionSystem::getHearingDangerInvestigationCount() const
{
	return m_hearingDangerSense.GetStats().investigationCount;
}

int AgentPerceptionSystem::getHearingDangerRetreatFactApplyCount() const
{
	return m_hearingDangerSense.GetStats().retreatFactApplyCount;
}

std::string AgentPerceptionSystem::buildPerceptionDebugSummary() const
{
	return BuildDebugSummary();
}

void AgentPerceptionSystem::CopyHearingDangerStats(Stats& stats) const
{
	const HearingDangerSense::Stats& senseStats = m_hearingDangerSense.GetStats();
	stats.hearingDangerEnabled = senseStats.enabled;
	stats.hearingDangerEventCount = senseStats.eventCount;
	stats.hearingDangerPublishedEventCount = senseStats.publishedEventCount;
	stats.hearingDangerPrunedEventCount = senseStats.prunedEventCount;
	stats.hearingDangerRunCount = senseStats.runCount;
	stats.hearingDangerSkipCount = senseStats.skipCount;
	stats.hearingDangerAgentCheckCount = senseStats.agentCheckCount;
	stats.hearingDangerHeardResponseCount = senseStats.heardResponseCount;
	stats.hearingDangerDangerResponseCount = senseStats.dangerResponseCount;
	stats.hearingDangerInvestigationCount = senseStats.investigationCount;
	stats.hearingDangerRetreatFactApplyCount = senseStats.retreatFactApplyCount;
}
