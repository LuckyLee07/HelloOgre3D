#ifndef __AGENT_SPATIAL_INDEX_SYSTEM_H__
#define __AGENT_SPATIAL_INDEX_SYSTEM_H__

#include <string>
#include <unordered_map>
#include <vector>

#include "OgreVector3.h"

class AgentObject;

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
			, resultCount(0)
			, maxCandidatesPerQuery(0)
			, maxResultsPerQuery(0)
		{
		}

		int agentCount;
		int occupiedCellCount;
		int rebuildCount;
		int queryCount;
		int candidateCount;
		int resultCount;
		int maxCandidatesPerQuery;
		int maxResultsPerQuery;
	};

	explicit AgentSpatialIndexSystem(float cellSize = 50.0f);

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
	void RecordFallbackQueryStats(int candidates, int results) const;

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
	void RecordQueryStats(int candidates, int results) const;

	float m_cellSize;
	bool m_enabled;
	bool m_built;
	std::unordered_map<CellKey, std::vector<AgentObject*>, CellKeyHasher> m_cells;
	std::unordered_map<AgentObject*, CellKey> m_agentCells;
	mutable Stats m_stats;
};

#endif // __AGENT_SPATIAL_INDEX_SYSTEM_H__
