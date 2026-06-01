#include "ai/team/TeamBlackboardService.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "ai/common/Blackboard.h"
#include "ai/perception/MemoryStore.h"
#include "components/ai/AIController.h"
#include "objects/AgentObject.h"
#include "profiling/Profile.h"
#include "profiling/RuntimeProfileCounters.h"

namespace
{
	const int kDefaultFactTtlMs = 8000;

	std::string FormatVec3(const Ogre::Vector3& value)
	{
		std::ostringstream stream;
		stream << static_cast<int>(value.x) << "," << static_cast<int>(value.y) << "," << static_cast<int>(value.z);
		return stream.str();
	}
}

TeamBlackboardService::EnemySightingFact::EnemySightingFact()
	: teamId(0)
	, targetId(-1)
	, reporterId(-1)
	, reportCount(0)
	, priority(0)
	, lastSeenMs(0)
	, targetPosition(Ogre::Vector3::ZERO)
	, confidence(0.0f)
{
}

TeamBlackboardService::Stats::Stats()
	: teamCount(0)
	, factCount(0)
	, reportCount(0)
	, updateCount(0)
	, expiredCount(0)
	, lastScanAgentCount(0)
	, lastWriterCount(0)
	, ttlMs(kDefaultFactTtlMs)
	, currentTimeMs(0)
{
}

TeamBlackboardService::TeamBlackboardService()
	: m_factTtlMs(kDefaultFactTtlMs)
{
	m_stats.ttlMs = m_factTtlMs;
}

void TeamBlackboardService::Clear()
{
	m_teams.clear();
	m_stats = Stats();
	m_stats.ttlMs = m_factTtlMs;
}

void TeamBlackboardService::SetFactTtlMs(int ttlMs)
{
	m_factTtlMs = std::max(0, ttlMs);
	m_stats.ttlMs = m_factTtlMs;
}

bool TeamBlackboardService::RememberEnemySighting(int teamId, int reporterId, int targetId, const Ogre::Vector3& targetPosition, long long lastSeenMs, float confidence)
{
	if (targetId < 0)
		return false;

	EnemySightingFact fact;
	fact.teamId = teamId;
	fact.targetId = targetId;
	fact.reporterId = reporterId;
	fact.reportCount = 1;
	fact.lastSeenMs = lastSeenMs;
	fact.targetPosition = targetPosition;
	fact.confidence = std::max(0.0f, std::min(1.0f, confidence));
	const int ageMs = static_cast<int>(std::max<long long>(0, m_stats.currentTimeMs - fact.lastSeenMs));
	fact.priority = BuildPriority(fact.confidence, fact.reportCount, ageMs);
	RememberEnemyFact(fact);
	RefreshStats(m_stats.lastScanAgentCount, m_stats.lastWriterCount);
	return true;
}

void TeamBlackboardService::SyncFromAgents(const std::vector<AgentObject*>& agents, int deltaMs)
{
	H3D_PROFILE_SCOPE("TeamBlackboardService::SyncFromAgents");
	m_stats.currentTimeMs += std::max(0, deltaMs);
	++m_stats.updateCount;

	int scannedAgents = 0;
	int writers = 0;
	for (AgentObject* agent : agents)
	{
		if (agent == nullptr)
			continue;
		++scannedAgents;

		AIController* ai = agent->FindComponent<AIController>();
		Blackboard* blackboard = ai != nullptr ? ai->GetBlackboard() : nullptr;
		if (blackboard == nullptr || !blackboard->GetBool(AIMemoryKeys::kMemorySnapshotHasLastKnownEnemy, false))
			continue;

		EnemySightingFact fact;
		fact.teamId = agent->GetTeamId();
		fact.targetId = blackboard->GetObjectId(AIMemoryKeys::kMemorySnapshotLastKnownEnemyId, -1);
		if (fact.targetId < 0)
			continue;

		const int ageMs = std::max(0, blackboard->GetInt(AIMemoryKeys::kMemorySnapshotLastKnownEnemyAgeMs, 0));
		fact.reporterId = static_cast<int>(agent->GetObjId());
		fact.reportCount = 1;
		fact.lastSeenMs = m_stats.currentTimeMs - ageMs;
		fact.targetPosition = blackboard->GetVec3(AIMemoryKeys::kMemorySnapshotLastKnownEnemyPos);
		fact.confidence = std::max(0.0f, std::min(1.0f, blackboard->GetFloat(AIMemoryKeys::kMemorySnapshotLastKnownEnemyConfidence, 0.0f)));
		fact.priority = BuildPriority(fact.confidence, fact.reportCount, ageMs);
		RememberEnemyFact(fact);
		++writers;
	}

	const int expired = PruneExpiredFacts();
	m_stats.expiredCount += expired;
	RefreshStats(scannedAgents, writers);
}

bool TeamBlackboardService::GetBestEnemyFact(int teamId, EnemySightingFact& outFact) const
{
	return GetBestEnemyFact(teamId, -1, outFact);
}

bool TeamBlackboardService::GetBestEnemyFact(int teamId, int ignoredReporterId, EnemySightingFact& outFact) const
{
	outFact = EnemySightingFact();
	std::unordered_map<int, TeamState>::const_iterator foundTeam = m_teams.find(teamId);
	if (foundTeam == m_teams.end())
		return false;

	bool found = false;
	for (EnemyFactMap::const_iterator iter = foundTeam->second.enemyFacts.begin(); iter != foundTeam->second.enemyFacts.end(); ++iter)
	{
		const EnemySightingFact& fact = iter->second;
		if (ignoredReporterId >= 0 && fact.reporterId == ignoredReporterId)
			continue;
		if (!found || fact.priority > outFact.priority || (fact.priority == outFact.priority && fact.lastSeenMs > outFact.lastSeenMs))
		{
			outFact = fact;
			found = true;
		}
	}
	return found;
}

std::string TeamBlackboardService::BuildDebugSummary() const
{
	std::ostringstream stream;
	stream << "[TeamBlackboardService] teams=" << m_stats.teamCount
		<< " facts=" << m_stats.factCount
		<< " reports=" << m_stats.reportCount
		<< " writers=" << m_stats.lastWriterCount
		<< " scanned=" << m_stats.lastScanAgentCount
		<< " updates=" << m_stats.updateCount
		<< " expired=" << m_stats.expiredCount
		<< " ttlMs=" << m_stats.ttlMs;

	EnemySightingFact bestFact;
	bool hasBestFact = false;
	for (std::unordered_map<int, TeamState>::const_iterator iter = m_teams.begin(); iter != m_teams.end(); ++iter)
	{
		EnemySightingFact candidate;
		if (!GetBestEnemyFact(iter->first, candidate))
			continue;
		if (!hasBestFact || candidate.priority > bestFact.priority || (candidate.priority == bestFact.priority && candidate.lastSeenMs > bestFact.lastSeenMs))
		{
			bestFact = candidate;
			hasBestFact = true;
		}
	}

	if (hasBestFact)
	{
		const int ageMs = static_cast<int>(std::max<long long>(0, m_stats.currentTimeMs - bestFact.lastSeenMs));
		stream << " focus=team:" << bestFact.teamId
			<< "/target:" << bestFact.targetId
			<< "@" << FormatVec3(bestFact.targetPosition)
			<< " age=" << ageMs
			<< " conf=" << std::fixed << std::setprecision(2) << bestFact.confidence
			<< " reports=" << bestFact.reportCount
			<< " priority=" << bestFact.priority;
	}
	else
	{
		stream << " focus=none";
	}
	return stream.str();
}

void TeamBlackboardService::PublishTracyCounters() const
{
	RuntimeProfileCounters::PlotTeamBlackboardStats(
		m_stats.teamCount,
		m_stats.factCount,
		m_stats.reportCount,
		m_stats.lastWriterCount,
		m_stats.expiredCount);
}

void TeamBlackboardService::RememberEnemyFact(const EnemySightingFact& fact)
{
	TeamState& team = m_teams[fact.teamId];
	EnemySightingFact& stored = team.enemyFacts[fact.targetId];
	if (stored.targetId < 0)
	{
		stored = fact;
		return;
	}

	stored.teamId = fact.teamId;
	stored.targetId = fact.targetId;
	stored.lastSeenMs = std::max(stored.lastSeenMs, fact.lastSeenMs);
	if (fact.confidence >= stored.confidence || fact.lastSeenMs >= stored.lastSeenMs)
	{
		stored.reporterId = fact.reporterId;
		stored.targetPosition = fact.targetPosition;
		stored.confidence = std::max(stored.confidence, fact.confidence);
	}
	stored.reportCount += 1;
	const int ageMs = static_cast<int>(std::max<long long>(0, m_stats.currentTimeMs - stored.lastSeenMs));
	stored.priority = BuildPriority(stored.confidence, stored.reportCount, ageMs);
}

int TeamBlackboardService::PruneExpiredFacts()
{
	if (m_factTtlMs <= 0)
		return 0;

	int expired = 0;
	for (std::unordered_map<int, TeamState>::iterator teamIter = m_teams.begin(); teamIter != m_teams.end();)
	{
		EnemyFactMap& facts = teamIter->second.enemyFacts;
		for (EnemyFactMap::iterator factIter = facts.begin(); factIter != facts.end();)
		{
			if (m_stats.currentTimeMs - factIter->second.lastSeenMs > m_factTtlMs)
			{
				factIter = facts.erase(factIter);
				++expired;
			}
			else
			{
				++factIter;
			}
		}

		if (facts.empty())
			teamIter = m_teams.erase(teamIter);
		else
			++teamIter;
	}
	return expired;
}

void TeamBlackboardService::RefreshStats(int scannedAgents, int writers)
{
	int factCount = 0;
	int reportCount = 0;
	for (std::unordered_map<int, TeamState>::const_iterator teamIter = m_teams.begin(); teamIter != m_teams.end(); ++teamIter)
	{
		factCount += static_cast<int>(teamIter->second.enemyFacts.size());
		for (EnemyFactMap::const_iterator factIter = teamIter->second.enemyFacts.begin(); factIter != teamIter->second.enemyFacts.end(); ++factIter)
			reportCount += factIter->second.reportCount;
	}

	m_stats.teamCount = static_cast<int>(m_teams.size());
	m_stats.factCount = factCount;
	m_stats.reportCount = reportCount;
	m_stats.lastScanAgentCount = scannedAgents;
	m_stats.lastWriterCount = writers;
	m_stats.ttlMs = m_factTtlMs;
}

int TeamBlackboardService::BuildPriority(float confidence, int reportCount, int ageMs)
{
	const int confidenceScore = static_cast<int>(std::max(0.0f, std::min(1.0f, confidence)) * 1000.0f);
	const int reportScore = std::min(reportCount, 16) * 25;
	const int agePenalty = std::max(0, ageMs / 100);
	return std::max(0, confidenceScore + reportScore - agePenalty);
}
