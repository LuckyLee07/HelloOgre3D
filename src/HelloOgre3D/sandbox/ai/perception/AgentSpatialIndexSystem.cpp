#include "ai/perception/AgentSpatialIndexSystem.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

#include "objects/AgentObject.h"
#include "profiling/Profile.h"

namespace
{
	const float kMinCellSize = 1.0f;
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
	H3D_PROFILE_SCOPE("AgentSpatialIndexSystem::QueryAgentsInRange");
	outAgents.clear();
	if (!m_enabled || !m_built)
		return;

	const bool limitResults = maxResults > 0;
	int candidates = 0;

	if (radius <= 0.0f)
	{
		for (std::unordered_map<CellKey, std::vector<AgentObject*>, CellKeyHasher>::const_iterator cell = m_cells.begin(); cell != m_cells.end(); ++cell)
		{
			for (AgentObject* agent : cell->second)
			{
				++candidates;
				if (agent == nullptr)
					continue;
				outAgents.push_back(agent);
				if (limitResults && static_cast<int>(outAgents.size()) >= maxResults)
				{
					RecordQueryStats(candidates, static_cast<int>(outAgents.size()));
					return;
				}
			}
		}
		RecordQueryStats(candidates, static_cast<int>(outAgents.size()));
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
				++candidates;
				if (agent == nullptr)
					continue;

				if (center.squaredDistance(agent->GetPosition()) > radiusSquared)
					continue;

				outAgents.push_back(agent);
				if (limitResults && static_cast<int>(outAgents.size()) >= maxResults)
				{
					RecordQueryStats(candidates, static_cast<int>(outAgents.size()));
					return;
				}
			}
		}
	}

	RecordQueryStats(candidates, static_cast<int>(outAgents.size()));
}

void AgentSpatialIndexSystem::RecordFallbackQueryStats(int candidates, int results) const
{
	RecordQueryStats(candidates, results);
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

	stream << "[AgentSpatialIndex] enabled=" << (m_enabled ? "true" : "false")
		<< " mode=" << (m_enabled ? "grid" : "linearFallback")
		<< " built=" << (m_built ? "true" : "false")
		<< " cellSize=" << static_cast<int>(m_cellSize)
		<< " agents=" << m_stats.agentCount
		<< " cells=" << m_stats.occupiedCellCount
		<< " rebuilds=" << m_stats.rebuildCount
		<< " queries=" << m_stats.queryCount
		<< " candidates=" << m_stats.candidateCount
		<< " results=" << m_stats.resultCount
		<< " avgCandidates=" << std::fixed << std::setprecision(1) << avgCandidates
		<< " avgResults=" << std::fixed << std::setprecision(1) << avgResults
		<< " maxCandidates=" << m_stats.maxCandidatesPerQuery
		<< " maxResults=" << m_stats.maxResultsPerQuery;
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

void AgentSpatialIndexSystem::RecordQueryStats(int candidates, int results) const
{
	++m_stats.queryCount;
	m_stats.candidateCount += candidates;
	m_stats.resultCount += results;
	m_stats.maxCandidatesPerQuery = std::max(m_stats.maxCandidatesPerQuery, candidates);
	m_stats.maxResultsPerQuery = std::max(m_stats.maxResultsPerQuery, results);
}
