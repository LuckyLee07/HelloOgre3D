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
			AIController* ai = agent != nullptr ? agent->FindComponent<AIController>() : nullptr;
			if (ai != nullptr)
				ai->SetPerceptionTickInAiTickEnabled(true);
		}
		m_stats = stats;
		return;
	}

	for (AgentObject* agent : agents)
	{
		AIController* ai = agent != nullptr ? agent->FindComponent<AIController>() : nullptr;
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

	if (spatialIndex != nullptr)
	{
		const AgentSpatialIndexSystem::Stats& spatialAfter = spatialIndex->GetStats();
		stats.spatialQueryCount = std::max(0, spatialAfter.queryCount - spatialBefore.queryCount);
		stats.spatialCandidateCount = std::max(0, spatialAfter.candidateCount - spatialBefore.candidateCount);
		stats.spatialResultCount = std::max(0, spatialAfter.resultCount - spatialBefore.resultCount);
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

	std::ostringstream stream;
	stream << "[AgentPerceptionSystem] enabled=" << (m_enabled ? "true" : "false")
		<< " updates=" << m_stats.updateCount
		<< " agents=" << m_stats.agentCount
		<< " controllers=" << m_stats.controllerCount
		<< " scans=" << m_stats.scanCount
		<< " visible=" << m_stats.visibleCount
		<< " spatialQueries=" << m_stats.spatialQueryCount
		<< " candidates=" << m_stats.spatialCandidateCount
		<< " results=" << m_stats.spatialResultCount
		<< " avgCandidates=" << std::fixed << std::setprecision(1) << avgCandidates
		<< " avgResults=" << std::fixed << std::setprecision(1) << avgResults
		<< " memoryMs=" << std::fixed << std::setprecision(2) << m_stats.memoryMs
		<< " visionMs=" << std::fixed << std::setprecision(2) << m_stats.visionMs;
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
		m_stats.spatialResultCount);
}
