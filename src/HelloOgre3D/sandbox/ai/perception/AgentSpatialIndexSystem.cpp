#include "ai/perception/AgentSpatialIndexSystem.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

#include "objects/AgentObject.h"
#include "profiling/Profile.h"
#include "profiling/RuntimeProfileCounters.h"

namespace
{
	const float kMinCellSize = 1.0f;

	struct QueryFilterStats
	{
		QueryFilterStats()
			: rawCandidates(0)
			, filteredCandidates(0)
			, rejectedSelf(0)
			, rejectedTeam(0)
			, rejectedDead(0)
			, rejectedType(0)
		{
		}

		int rawCandidates;
		int filteredCandidates;
		int rejectedSelf;
		int rejectedTeam;
		int rejectedDead;
		int rejectedType;
	};

	bool PassesQueryOptions(AgentObject* agent, const AgentSpatialQueryOptions& options, QueryFilterStats& stats)
	{
		if (agent == nullptr)
			return false;

		if (!options.includeSelf && options.owner != nullptr && agent == options.owner)
		{
			++stats.rejectedSelf;
			return false;
		}

		if (options.requireAlive && agent->GetHealth() <= 0.0f)
		{
			++stats.rejectedDead;
			return false;
		}

		if (options.requiredTeamId >= 0 && static_cast<int>(agent->GetTeamId()) != options.requiredTeamId)
		{
			++stats.rejectedTeam;
			return false;
		}

		if (options.excludedTeamId >= 0 && static_cast<int>(agent->GetTeamId()) == options.excludedTeamId)
		{
			++stats.rejectedTeam;
			return false;
		}

		if (options.requiredObjectType > 0 && static_cast<int>(agent->GetObjType()) != options.requiredObjectType)
		{
			++stats.rejectedType;
			return false;
		}

		++stats.filteredCandidates;
		return true;
	}

	void AddResultCandidate(
		AgentObject* agent,
		float distanceSquared,
		const AgentSpatialQueryOptions& options,
		std::vector<AgentObject*>& outAgents,
		std::vector<float>& outDistances)
	{
		if (options.maxResults <= 0)
		{
			outAgents.push_back(agent);
			return;
		}

		if (static_cast<int>(outAgents.size()) < options.maxResults)
		{
			outAgents.push_back(agent);
			outDistances.push_back(distanceSquared);
			return;
		}

		int farthestIndex = -1;
		float farthestDistance = -1.0f;
		for (int i = 0; i < static_cast<int>(outDistances.size()); ++i)
		{
			if (outDistances[i] > farthestDistance)
			{
				farthestDistance = outDistances[i];
				farthestIndex = i;
			}
		}

		if (farthestIndex >= 0 && distanceSquared < farthestDistance)
		{
			outAgents[farthestIndex] = agent;
			outDistances[farthestIndex] = distanceSquared;
		}
	}
}

AgentSpatialIndexSystem::AgentSpatialIndexSystem(float cellSize)
	: m_cellSize(cellSize > kMinCellSize ? cellSize : kMinCellSize)
	, m_enabled(true)
	, m_built(false)
{
}

void AgentSpatialIndexSystem::SetEnabled(bool enabled)
{
	if (m_enabled == enabled)
		return;

	m_enabled = enabled;
	Clear();
}

void AgentSpatialIndexSystem::SetCellSize(float cellSize)
{
	m_cellSize = cellSize > kMinCellSize ? cellSize : kMinCellSize;
	m_built = false;
}

void AgentSpatialIndexSystem::Clear()
{
	m_cells.clear();
	m_agentCells.clear();
	m_stats = Stats();
	m_built = false;
}

void AgentSpatialIndexSystem::BeginFrameLinearFallback(int agentCount)
{
	m_cells.clear();
	m_agentCells.clear();
	m_stats = Stats();
	m_stats.agentCount = agentCount;
	m_built = false;
}

void AgentSpatialIndexSystem::Rebuild(const std::vector<AgentObject*>& agents)
{
	H3D_PROFILE_SCOPE("AgentSpatialIndexSystem::Rebuild");
	if (!m_enabled)
	{
		BeginFrameLinearFallback(static_cast<int>(agents.size()));
		return;
	}

	const int rebuildCount = m_stats.rebuildCount + 1;
	m_cells.clear();
	m_agentCells.clear();
	m_stats = Stats();
	m_stats.rebuildCount = rebuildCount;

	for (AgentObject* agent : agents)
	{
		if (agent == nullptr)
			continue;

		InsertAgent(agent, BuildCellKey(agent->GetPosition()));
	}

	m_stats.agentCount = static_cast<int>(m_agentCells.size());
	m_stats.occupiedCellCount = static_cast<int>(m_cells.size());
	m_built = true;
}

void AgentSpatialIndexSystem::AddOrUpdateAgent(AgentObject* agent)
{
	if (agent == nullptr)
		return;
	if (!m_enabled)
		return;

	RemoveAgent(agent);
	InsertAgent(agent, BuildCellKey(agent->GetPosition()));
	m_stats.agentCount = static_cast<int>(m_agentCells.size());
	m_stats.occupiedCellCount = static_cast<int>(m_cells.size());
	m_built = true;
}

void AgentSpatialIndexSystem::RemoveAgent(AgentObject* agent)
{
	if (agent == nullptr)
		return;

	std::unordered_map<AgentObject*, CellKey>::iterator foundAgent = m_agentCells.find(agent);
	if (foundAgent == m_agentCells.end())
		return;

	std::unordered_map<CellKey, std::vector<AgentObject*>, CellKeyHasher>::iterator foundCell = m_cells.find(foundAgent->second);
	if (foundCell != m_cells.end())
	{
		std::vector<AgentObject*>& agents = foundCell->second;
		agents.erase(std::remove(agents.begin(), agents.end(), agent), agents.end());
		if (agents.empty())
			m_cells.erase(foundCell);
	}

	m_agentCells.erase(foundAgent);
	m_stats.agentCount = static_cast<int>(m_agentCells.size());
	m_stats.occupiedCellCount = static_cast<int>(m_cells.size());
}

void AgentSpatialIndexSystem::QueryAgentsInRange(const Ogre::Vector3& center, float radius, std::vector<AgentObject*>& outAgents, int maxResults) const
{
	AgentSpatialQueryOptions options;
	options.maxResults = maxResults;
	QueryAgentsInRange(center, radius, outAgents, options);
}

void AgentSpatialIndexSystem::QueryAgentsInRange(const Ogre::Vector3& center, float radius, std::vector<AgentObject*>& outAgents, const AgentSpatialQueryOptions& options) const
{
	H3D_PROFILE_SCOPE("AgentSpatialIndexSystem::QueryAgentsInRange");
	const long long queryStartMicros = RuntimeStallProfiler::NowMicroseconds();
	outAgents.clear();
	if (!m_enabled || !m_built)
		return;

	QueryFilterStats filterStats;
	std::vector<float> resultDistances;
	if (options.maxResults > 0)
		resultDistances.reserve(options.maxResults);

	if (radius <= 0.0f)
	{
		for (std::unordered_map<CellKey, std::vector<AgentObject*>, CellKeyHasher>::const_iterator cell = m_cells.begin(); cell != m_cells.end(); ++cell)
		{
			for (AgentObject* agent : cell->second)
			{
				++filterStats.rawCandidates;
				if (!PassesQueryOptions(agent, options, filterStats))
					continue;

				AddResultCandidate(agent, center.squaredDistance(agent->GetPosition()), options, outAgents, resultDistances);
			}
		}
		RecordQueryStats(filterStats.rawCandidates, filterStats.filteredCandidates, static_cast<int>(outAgents.size()), filterStats.rejectedSelf, filterStats.rejectedTeam, filterStats.rejectedDead, filterStats.rejectedType, RuntimeStallProfiler::ElapsedMsSince(queryStartMicros));
		return;
	}

	const float radiusSquared = radius * radius;
	const CellKey minKey = BuildCellKey(center - Ogre::Vector3(radius, 0.0f, radius));
	const CellKey maxKey = BuildCellKey(center + Ogre::Vector3(radius, 0.0f, radius));

	for (int cellX = minKey.x; cellX <= maxKey.x; ++cellX)
	{
		for (int cellZ = minKey.z; cellZ <= maxKey.z; ++cellZ)
		{
			const CellKey key(cellX, cellZ);
			std::unordered_map<CellKey, std::vector<AgentObject*>, CellKeyHasher>::const_iterator foundCell = m_cells.find(key);
			if (foundCell == m_cells.end())
				continue;

			for (AgentObject* agent : foundCell->second)
			{
				++filterStats.rawCandidates;
				if (agent == nullptr)
					continue;

				const float distanceSquared = center.squaredDistance(agent->GetPosition());
				if (distanceSquared > radiusSquared)
					continue;

				if (!PassesQueryOptions(agent, options, filterStats))
					continue;

				AddResultCandidate(agent, distanceSquared, options, outAgents, resultDistances);
			}
		}
	}

	RecordQueryStats(filterStats.rawCandidates, filterStats.filteredCandidates, static_cast<int>(outAgents.size()), filterStats.rejectedSelf, filterStats.rejectedTeam, filterStats.rejectedDead, filterStats.rejectedType, RuntimeStallProfiler::ElapsedMsSince(queryStartMicros));
}

void AgentSpatialIndexSystem::RecordFallbackQueryStats(int candidates, int filteredCandidates, int results, int rejectedSelf, int rejectedTeam, int rejectedDead, int rejectedType, double queryCostMs) const
{
	RecordQueryStats(candidates, filteredCandidates, results, rejectedSelf, rejectedTeam, rejectedDead, rejectedType, queryCostMs);
}

std::string AgentSpatialIndexSystem::BuildDebugSummary() const
{
	std::ostringstream stream;
	const float avgCandidates = m_stats.queryCount > 0
		? static_cast<float>(m_stats.candidateCount) / static_cast<float>(m_stats.queryCount)
		: 0.0f;
	const float avgResults = m_stats.queryCount > 0
		? static_cast<float>(m_stats.resultCount) / static_cast<float>(m_stats.queryCount)
		: 0.0f;
	const float avgFiltered = m_stats.queryCount > 0
		? static_cast<float>(m_stats.filteredCandidateCount) / static_cast<float>(m_stats.queryCount)
		: 0.0f;

	stream << "[AgentSpatialIndex] enabled=" << (m_enabled ? "true" : "false")
		<< " mode=" << (m_enabled ? "grid" : "linearFallback")
		<< " built=" << (m_built ? "true" : "false")
		<< " cellSize=" << static_cast<int>(m_cellSize)
		<< " agents=" << m_stats.agentCount
		<< " cells=" << m_stats.occupiedCellCount
		<< " rebuilds=" << m_stats.rebuildCount
		<< " queries=" << m_stats.queryCount
		<< " candidates=" << m_stats.candidateCount
		<< " filtered=" << m_stats.filteredCandidateCount
		<< " results=" << m_stats.resultCount
		<< " avgCandidates=" << std::fixed << std::setprecision(1) << avgCandidates
		<< " avgFiltered=" << std::fixed << std::setprecision(1) << avgFiltered
		<< " avgResults=" << std::fixed << std::setprecision(1) << avgResults
		<< " maxCandidates=" << m_stats.maxCandidatesPerQuery
		<< " maxFiltered=" << m_stats.maxFilteredCandidatesPerQuery
		<< " maxResults=" << m_stats.maxResultsPerQuery
		<< " rejectSelf=" << m_stats.rejectedSelfCount
		<< " rejectTeam=" << m_stats.rejectedTeamCount
		<< " rejectDead=" << m_stats.rejectedDeadCount
		<< " rejectType=" << m_stats.rejectedTypeCount
		<< " queryMs=" << std::fixed << std::setprecision(2) << m_stats.queryCostMs;
	return stream.str();
}

AgentSpatialIndexSystem::CellKey AgentSpatialIndexSystem::BuildCellKey(const Ogre::Vector3& position) const
{
	return CellKey(
		static_cast<int>(std::floor(position.x / m_cellSize)),
		static_cast<int>(std::floor(position.z / m_cellSize)));
}

void AgentSpatialIndexSystem::InsertAgent(AgentObject* agent, const CellKey& key)
{
	m_cells[key].push_back(agent);
	m_agentCells[agent] = key;
}

void AgentSpatialIndexSystem::RecordQueryStats(int candidates, int filteredCandidates, int results, int rejectedSelf, int rejectedTeam, int rejectedDead, int rejectedType, double queryCostMs) const
{
	++m_stats.queryCount;
	m_stats.candidateCount += candidates;
	m_stats.filteredCandidateCount += filteredCandidates;
	m_stats.resultCount += results;
	m_stats.maxCandidatesPerQuery = std::max(m_stats.maxCandidatesPerQuery, candidates);
	m_stats.maxFilteredCandidatesPerQuery = std::max(m_stats.maxFilteredCandidatesPerQuery, filteredCandidates);
	m_stats.maxResultsPerQuery = std::max(m_stats.maxResultsPerQuery, results);
	m_stats.rejectedSelfCount += rejectedSelf;
	m_stats.rejectedTeamCount += rejectedTeam;
	m_stats.rejectedDeadCount += rejectedDead;
	m_stats.rejectedTypeCount += rejectedType;
	m_stats.queryCostMs += queryCostMs;
}
