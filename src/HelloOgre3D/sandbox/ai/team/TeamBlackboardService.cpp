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

	std::string MakeBlackboardKey(const std::string& prefix, const char* suffix)
	{
		const std::string safePrefix = prefix.empty() ? "team.cpp" : prefix;
		return safePrefix + "." + suffix;
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
	, enemyFactCount(0)
	, typedFactCount(0)
	, reportCount(0)
	, typedReportCount(0)
	, updateCount(0)
	, expiredCount(0)
	, lastScanAgentCount(0)
	, lastWriterCount(0)
	, ttlMs(kDefaultFactTtlMs)
	, currentTimeMs(0)
{
}

TeamBlackboardService::TeamFact::TeamFact()
	: factType()
	, key()
	, teamId(0)
	, sourceAgentId(-1)
	, targetAgentId(-1)
	, reportCount(0)
	, priority(0)
	, timeMs(0)
	, ttlMs(0)
	, position(Ogre::Vector3::ZERO)
	, confidence(0.0f)
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

bool TeamBlackboardService::RememberFact(const std::string& factType, int teamId, int sourceAgentId, int targetAgentId, const Ogre::Vector3& position, long long timeMs, float confidence, int priority, int ttlMs, const std::string& key)
{
	if (factType.empty())
		return false;

	TeamFact fact;
	fact.factType = factType;
	fact.key = BuildFactKey(factType, sourceAgentId, targetAgentId, key);
	fact.teamId = teamId;
	fact.sourceAgentId = sourceAgentId;
	fact.targetAgentId = targetAgentId;
	fact.reportCount = 1;
	fact.timeMs = timeMs;
	fact.ttlMs = ttlMs > 0 ? ttlMs : m_factTtlMs;
	fact.position = position;
	fact.confidence = std::max(0.0f, std::min(1.0f, confidence));
	const int ageMs = static_cast<int>(std::max<long long>(0, m_stats.currentTimeMs - fact.timeMs));
	fact.priority = priority > 0 ? priority : BuildPriority(fact.confidence, fact.reportCount, ageMs);
	RememberTypedFact(fact);
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

bool TeamBlackboardService::GetBestFact(int teamId, const std::string& factType, int ignoredSourceAgentId, TeamFact& outFact) const
{
	outFact = TeamFact();
	if (factType.empty())
		return false;

	std::unordered_map<int, TeamState>::const_iterator foundTeam = m_teams.find(teamId);
	if (foundTeam == m_teams.end())
		return false;

	bool found = false;
	for (TeamFactMap::const_iterator iter = foundTeam->second.facts.begin(); iter != foundTeam->second.facts.end(); ++iter)
	{
		const TeamFact& fact = iter->second;
		if (fact.factType != factType)
			continue;
		if (ignoredSourceAgentId >= 0 && fact.sourceAgentId == ignoredSourceAgentId)
			continue;
		if (!found || fact.priority > outFact.priority || (fact.priority == outFact.priority && fact.timeMs > outFact.timeMs))
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
		<< " enemyFacts=" << m_stats.enemyFactCount
		<< " typedFacts=" << m_stats.typedFactCount
		<< " reports=" << m_stats.reportCount
		<< " typedReports=" << m_stats.typedReportCount
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

	TeamFact bestTypedFact;
	bool hasTypedFact = false;
	for (std::unordered_map<int, TeamState>::const_iterator teamIter = m_teams.begin(); teamIter != m_teams.end(); ++teamIter)
	{
		const TeamFactMap& facts = teamIter->second.facts;
		for (TeamFactMap::const_iterator factIter = facts.begin(); factIter != facts.end(); ++factIter)
		{
			const TeamFact& fact = factIter->second;
			if (!hasTypedFact || fact.priority > bestTypedFact.priority || (fact.priority == bestTypedFact.priority && fact.timeMs > bestTypedFact.timeMs))
			{
				bestTypedFact = fact;
				hasTypedFact = true;
			}
		}
	}
	if (hasTypedFact)
	{
		const int ageMs = static_cast<int>(std::max<long long>(0, m_stats.currentTimeMs - bestTypedFact.timeMs));
		stream << " typedFocus=" << bestTypedFact.factType
			<< "/team:" << bestTypedFact.teamId
			<< "/source:" << bestTypedFact.sourceAgentId
			<< "/target:" << bestTypedFact.targetAgentId
			<< "@" << FormatVec3(bestTypedFact.position)
			<< " age=" << ageMs
			<< " conf=" << std::fixed << std::setprecision(2) << bestTypedFact.confidence
			<< " reports=" << bestTypedFact.reportCount
			<< " priority=" << bestTypedFact.priority;
	}
	else
	{
		stream << " typedFocus=none";
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

void TeamBlackboardService::clearTeamBlackboardFacts()
{
	Clear();
}

void TeamBlackboardService::configureTeamBlackboard(int ttlMs)
{
	SetFactTtlMs(ttlMs);
}

bool TeamBlackboardService::rememberTeamEnemyFact(int teamId, int reporterId, int targetId, const Ogre::Vector3& targetPosition, int lastSeenMs, float confidence)
{
	return RememberEnemySighting(teamId, reporterId, targetId, targetPosition, lastSeenMs, confidence);
}

bool TeamBlackboardService::rememberTeamFact(const std::string& factType, int teamId, int sourceAgentId, int targetAgentId, const Ogre::Vector3& position, int timeMs, float confidence, int priority, int ttlMs, const std::string& key)
{
	return RememberFact(factType, teamId, sourceAgentId, targetAgentId, position, timeMs, confidence, priority, ttlMs, key);
}

bool TeamBlackboardService::writeBestTeamEnemyFactToBlackboard(AgentObject* agent, const std::string& keyPrefix, bool allowOwnReport)
{
	if (agent == nullptr)
		return false;

	AIController* ai = agent->FindComponent<AIController>();
	Blackboard* blackboard = ai != nullptr ? ai->GetBlackboard() : nullptr;
	if (blackboard == nullptr)
		return false;

	EnemySightingFact fact;
	const int ignoredReporterId = allowOwnReport ? -1 : static_cast<int>(agent->GetObjId());
	if (!GetBestEnemyFact(agent->GetTeamId(), ignoredReporterId, fact))
	{
		blackboard->SetBool(MakeBlackboardKey(keyPrefix, "hasEnemy"), false);
		return false;
	}

	const long long currentTimeMs = GetStats().currentTimeMs;
	const int ageMs = static_cast<int>(std::max<long long>(0, currentTimeMs - fact.lastSeenMs));
	blackboard->SetBool(MakeBlackboardKey(keyPrefix, "hasEnemy"), true);
	blackboard->SetObjectId(MakeBlackboardKey(keyPrefix, "targetId"), fact.targetId);
	blackboard->SetVec3(MakeBlackboardKey(keyPrefix, "targetPos"), fact.targetPosition);
	blackboard->SetInt(MakeBlackboardKey(keyPrefix, "reporterId"), fact.reporterId);
	blackboard->SetInt(MakeBlackboardKey(keyPrefix, "lastSeenMs"), static_cast<int>(fact.lastSeenMs));
	blackboard->SetInt(MakeBlackboardKey(keyPrefix, "ageMs"), ageMs);
	blackboard->SetFloat(MakeBlackboardKey(keyPrefix, "confidence"), fact.confidence);
	blackboard->SetInt(MakeBlackboardKey(keyPrefix, "reportCount"), fact.reportCount);
	blackboard->SetInt(MakeBlackboardKey(keyPrefix, "priority"), fact.priority);
	return true;
}

bool TeamBlackboardService::writeBestTeamFactToBlackboard(AgentObject* agent, const std::string& factType, const std::string& keyPrefix, bool allowOwnReport)
{
	if (agent == nullptr)
		return false;

	AIController* ai = agent->FindComponent<AIController>();
	Blackboard* blackboard = ai != nullptr ? ai->GetBlackboard() : nullptr;
	if (blackboard == nullptr)
		return false;

	TeamFact fact;
	const int ignoredSourceAgentId = allowOwnReport ? -1 : static_cast<int>(agent->GetObjId());
	if (!GetBestFact(agent->GetTeamId(), factType, ignoredSourceAgentId, fact))
	{
		blackboard->SetBool(MakeBlackboardKey(keyPrefix, "hasFact"), false);
		return false;
	}

	const long long currentTimeMs = GetStats().currentTimeMs;
	const int ageMs = static_cast<int>(std::max<long long>(0, currentTimeMs - fact.timeMs));
	blackboard->SetBool(MakeBlackboardKey(keyPrefix, "hasFact"), true);
	blackboard->SetString(MakeBlackboardKey(keyPrefix, "factType"), fact.factType);
	blackboard->SetString(MakeBlackboardKey(keyPrefix, "key"), fact.key);
	blackboard->SetObjectId(MakeBlackboardKey(keyPrefix, "sourceAgentId"), fact.sourceAgentId);
	blackboard->SetObjectId(MakeBlackboardKey(keyPrefix, "targetId"), fact.targetAgentId);
	blackboard->SetVec3(MakeBlackboardKey(keyPrefix, "position"), fact.position);
	blackboard->SetInt(MakeBlackboardKey(keyPrefix, "timeMs"), static_cast<int>(fact.timeMs));
	blackboard->SetInt(MakeBlackboardKey(keyPrefix, "ageMs"), ageMs);
	blackboard->SetFloat(MakeBlackboardKey(keyPrefix, "confidence"), fact.confidence);
	blackboard->SetInt(MakeBlackboardKey(keyPrefix, "reportCount"), fact.reportCount);
	blackboard->SetInt(MakeBlackboardKey(keyPrefix, "priority"), fact.priority);
	return true;
}

bool TeamBlackboardService::hasBestTeamFact(int teamId, const std::string& factType, int ignoredSourceAgentId) const
{
	TeamFact fact;
	return GetBestFact(teamId, factType, ignoredSourceAgentId, fact);
}

int TeamBlackboardService::getBestTeamFactSourceAgentId(int teamId, const std::string& factType, int ignoredSourceAgentId) const
{
	TeamFact fact;
	return GetBestFact(teamId, factType, ignoredSourceAgentId, fact) ? fact.sourceAgentId : -1;
}

int TeamBlackboardService::getBestTeamFactTargetId(int teamId, const std::string& factType, int ignoredSourceAgentId) const
{
	TeamFact fact;
	return GetBestFact(teamId, factType, ignoredSourceAgentId, fact) ? fact.targetAgentId : -1;
}

Ogre::Vector3 TeamBlackboardService::getBestTeamFactPosition(int teamId, const std::string& factType, int ignoredSourceAgentId) const
{
	TeamFact fact;
	return GetBestFact(teamId, factType, ignoredSourceAgentId, fact) ? fact.position : Ogre::Vector3::ZERO;
}

std::string TeamBlackboardService::getBestTeamFactKey(int teamId, const std::string& factType, int ignoredSourceAgentId) const
{
	TeamFact fact;
	return GetBestFact(teamId, factType, ignoredSourceAgentId, fact) ? fact.key : std::string();
}

float TeamBlackboardService::getBestTeamFactConfidence(int teamId, const std::string& factType, int ignoredSourceAgentId) const
{
	TeamFact fact;
	return GetBestFact(teamId, factType, ignoredSourceAgentId, fact) ? fact.confidence : 0.0f;
}

int TeamBlackboardService::getBestTeamFactReportCount(int teamId, const std::string& factType, int ignoredSourceAgentId) const
{
	TeamFact fact;
	return GetBestFact(teamId, factType, ignoredSourceAgentId, fact) ? fact.reportCount : 0;
}

int TeamBlackboardService::getBestTeamFactPriority(int teamId, const std::string& factType, int ignoredSourceAgentId) const
{
	TeamFact fact;
	return GetBestFact(teamId, factType, ignoredSourceAgentId, fact) ? fact.priority : 0;
}

int TeamBlackboardService::getBestTeamFactTimeMs(int teamId, const std::string& factType, int ignoredSourceAgentId) const
{
	TeamFact fact;
	return GetBestFact(teamId, factType, ignoredSourceAgentId, fact) ? static_cast<int>(fact.timeMs) : 0;
}

int TeamBlackboardService::getBestTeamFactAgeMs(int teamId, const std::string& factType, int ignoredSourceAgentId) const
{
	TeamFact fact;
	if (!GetBestFact(teamId, factType, ignoredSourceAgentId, fact))
		return 0;
	return static_cast<int>(std::max<long long>(0, m_stats.currentTimeMs - fact.timeMs));
}

int TeamBlackboardService::getTeamBlackboardFactCount() const
{
	return GetStats().factCount;
}

int TeamBlackboardService::getTeamBlackboardReportCount() const
{
	return GetStats().reportCount;
}

int TeamBlackboardService::getTeamBlackboardEnemyFactCount() const
{
	return GetStats().enemyFactCount;
}

int TeamBlackboardService::getTeamBlackboardTypedFactCount() const
{
	return GetStats().typedFactCount;
}

int TeamBlackboardService::getTeamBlackboardTypedReportCount() const
{
	return GetStats().typedReportCount;
}

std::string TeamBlackboardService::buildTeamBlackboardDebugSummary() const
{
	return BuildDebugSummary();
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

void TeamBlackboardService::RememberTypedFact(const TeamFact& fact)
{
	if (fact.key.empty())
		return;

	TeamState& team = m_teams[fact.teamId];
	TeamFact& stored = team.facts[fact.key];
	if (stored.key.empty())
	{
		stored = fact;
		return;
	}

	stored.factType = fact.factType;
	stored.key = fact.key;
	stored.teamId = fact.teamId;
	stored.targetAgentId = fact.targetAgentId;
	stored.ttlMs = fact.ttlMs;
	stored.timeMs = std::max(stored.timeMs, fact.timeMs);
	if (fact.confidence >= stored.confidence || fact.timeMs >= stored.timeMs)
	{
		stored.sourceAgentId = fact.sourceAgentId;
		stored.position = fact.position;
		stored.confidence = std::max(stored.confidence, fact.confidence);
	}
	stored.reportCount += 1;
	const int ageMs = static_cast<int>(std::max<long long>(0, m_stats.currentTimeMs - stored.timeMs));
	stored.priority = fact.priority > 0 ? std::max(stored.priority, fact.priority) : BuildPriority(stored.confidence, stored.reportCount, ageMs);
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

		TeamFactMap& typedFacts = teamIter->second.facts;
		for (TeamFactMap::iterator factIter = typedFacts.begin(); factIter != typedFacts.end();)
		{
			const int ttlMs = factIter->second.ttlMs > 0 ? factIter->second.ttlMs : m_factTtlMs;
			if (ttlMs > 0 && m_stats.currentTimeMs - factIter->second.timeMs > ttlMs)
			{
				factIter = typedFacts.erase(factIter);
				++expired;
			}
			else
			{
				++factIter;
			}
		}

		if (facts.empty() && typedFacts.empty())
			teamIter = m_teams.erase(teamIter);
		else
			++teamIter;
	}
	return expired;
}

void TeamBlackboardService::RefreshStats(int scannedAgents, int writers)
{
	int enemyFactCount = 0;
	int typedFactCount = 0;
	int reportCount = 0;
	int typedReportCount = 0;
	for (std::unordered_map<int, TeamState>::const_iterator teamIter = m_teams.begin(); teamIter != m_teams.end(); ++teamIter)
	{
		enemyFactCount += static_cast<int>(teamIter->second.enemyFacts.size());
		for (EnemyFactMap::const_iterator factIter = teamIter->second.enemyFacts.begin(); factIter != teamIter->second.enemyFacts.end(); ++factIter)
			reportCount += factIter->second.reportCount;
		typedFactCount += static_cast<int>(teamIter->second.facts.size());
		for (TeamFactMap::const_iterator factIter = teamIter->second.facts.begin(); factIter != teamIter->second.facts.end(); ++factIter)
			typedReportCount += factIter->second.reportCount;
	}

	m_stats.teamCount = static_cast<int>(m_teams.size());
	m_stats.enemyFactCount = enemyFactCount;
	m_stats.typedFactCount = typedFactCount;
	m_stats.factCount = enemyFactCount + typedFactCount;
	m_stats.reportCount = reportCount + typedReportCount;
	m_stats.typedReportCount = typedReportCount;
	m_stats.lastScanAgentCount = scannedAgents;
	m_stats.lastWriterCount = writers;
	m_stats.ttlMs = m_factTtlMs;
}

std::string TeamBlackboardService::BuildFactKey(const std::string& factType, int sourceAgentId, int targetAgentId, const std::string& explicitKey)
{
	if (!explicitKey.empty())
		return factType + ":" + explicitKey;

	std::ostringstream stream;
	stream << factType << ":source:" << sourceAgentId << ":target:" << targetAgentId;
	return stream.str();
}

int TeamBlackboardService::BuildPriority(float confidence, int reportCount, int ageMs)
{
	const int confidenceScore = static_cast<int>(std::max(0.0f, std::min(1.0f, confidence)) * 1000.0f);
	const int reportScore = std::min(reportCount, 16) * 25;
	const int agePenalty = std::max(0, ageMs / 100);
	return std::max(0, confidenceScore + reportScore - agePenalty);
}
