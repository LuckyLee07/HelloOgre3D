#ifndef __AGENT_SPATIAL_INDEX_SYSTEM_H__
#define __AGENT_SPATIAL_INDEX_SYSTEM_H__

#include <string>
#include <unordered_map>
#include <vector>

#include "OgreVector3.h"

class AgentObject;

struct AgentSpatialQueryOptions
{
	AgentSpatialQueryOptions()
		: owner(nullptr)
		, includeSelf(true)
		, requireAlive(false)
		, requiredTeamId(-1)
		, excludedTeamId(-1)
		, requiredObjectType(0)
		, maxResults(0)
	{
	}

	const AgentObject* owner;
	bool includeSelf;
	bool requireAlive;
	int requiredTeamId;
	int excludedTeamId;
	int requiredObjectType;
	int maxResults;
};

class AgentSpatialIndexSystem
{
public:
	struct Stats
	{
		Stats()
			: agentCount(0)
			, occupiedCellCount(0)
			, rebuildCount(0)
			, queryCount(0)
			, candidateCount(0)
			, filteredCandidateCount(0)
			, resultCount(0)
			, maxCandidatesPerQuery(0)
			, maxFilteredCandidatesPerQuery(0)
			, maxResultsPerQuery(0)
			, rejectedSelfCount(0)
			, rejectedTeamCount(0)
			, rejectedDeadCount(0)
			, rejectedTypeCount(0)
			, queryCostMs(0.0)
		{
		}

		int agentCount;
		int occupiedCellCount;
		int rebuildCount;
		int queryCount;
		int candidateCount;
		int filteredCandidateCount;
		int resultCount;
		int maxCandidatesPerQuery;
		int maxFilteredCandidatesPerQuery;
		int maxResultsPerQuery;
		int rejectedSelfCount;
		int rejectedTeamCount;
		int rejectedDeadCount;
		int rejectedTypeCount;
		double queryCostMs;
	};

	explicit AgentSpatialIndexSystem(float cellSize = 20.0f);

	void SetEnabled(bool enabled);
	bool IsEnabled() const { return m_enabled; }
	void SetCellSize(float cellSize);
	float GetCellSize() const { return m_cellSize; }

	void Clear();
	void BeginFrameLinearFallback(int agentCount);
	void Rebuild(const std::vector<AgentObject*>& agents);
	void AddOrUpdateAgent(AgentObject* agent);
	void RemoveAgent(AgentObject* agent);
	void QueryAgentsInRange(const Ogre::Vector3& center, float radius, std::vector<AgentObject*>& outAgents, int maxResults = 0) const;
	void QueryAgentsInRange(const Ogre::Vector3& center, float radius, std::vector<AgentObject*>& outAgents, const AgentSpatialQueryOptions& options) const;
	void RecordFallbackQueryStats(int candidates, int filteredCandidates, int results, int rejectedSelf = 0, int rejectedTeam = 0, int rejectedDead = 0, int rejectedType = 0, double queryCostMs = 0.0) const;

	bool IsBuilt() const { return m_built; }
	const Stats& GetStats() const { return m_stats; }
	std::string BuildDebugSummary() const;

private:
	struct CellKey
	{
		CellKey()
			: x(0)
			, z(0)
		{
		}

		CellKey(int cellX, int cellZ)
			: x(cellX)
			, z(cellZ)
		{
		}

		bool operator==(const CellKey& other) const
		{
			return x == other.x && z == other.z;
		}

		int x;
		int z;
	};

	struct CellKeyHasher
	{
		std::size_t operator()(const CellKey& key) const
		{
			const std::size_t xHash = std::hash<int>()(key.x);
			const std::size_t zHash = std::hash<int>()(key.z);
			return xHash ^ (zHash + 0x9e3779b9 + (xHash << 6) + (xHash >> 2));
		}
	};

	CellKey BuildCellKey(const Ogre::Vector3& position) const;
	void InsertAgent(AgentObject* agent, const CellKey& key);
	void RecordQueryStats(int candidates, int filteredCandidates, int results, int rejectedSelf, int rejectedTeam, int rejectedDead, int rejectedType, double queryCostMs) const;

	float m_cellSize;
	bool m_enabled;
	bool m_built;
	std::unordered_map<CellKey, std::vector<AgentObject*>, CellKeyHasher> m_cells;
	std::unordered_map<AgentObject*, CellKey> m_agentCells;
	mutable Stats m_stats;
};

#endif // __AGENT_SPATIAL_INDEX_SYSTEM_H__
