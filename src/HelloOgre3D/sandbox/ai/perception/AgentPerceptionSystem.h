#ifndef __AGENT_PERCEPTION_SYSTEM_H__
#define __AGENT_PERCEPTION_SYSTEM_H__

#include <string>
#include <vector>

class AgentObject;
class ObjectManager;

class AgentPerceptionSystem
{
public:
	struct Stats
	{
		Stats()
			: enabled(1)
			, updateCount(0)
			, agentCount(0)
			, controllerCount(0)
			, scanCount(0)
			, visibleCount(0)
			, spatialQueryCount(0)
			, spatialCandidateCount(0)
			, spatialFilteredCandidateCount(0)
			, spatialResultCount(0)
			, spatialRejectedSelfCount(0)
			, spatialRejectedTeamCount(0)
			, spatialRejectedDeadCount(0)
			, spatialRejectedTypeCount(0)
			, spatialQueryCostMs(0.0)
			, memoryMs(0.0)
			, visionMs(0.0)
		{
		}

		int enabled;
		int updateCount;
		int agentCount;
		int controllerCount;
		int scanCount;
		int visibleCount;
		int spatialQueryCount;
		int spatialCandidateCount;
		int spatialFilteredCandidateCount;
		int spatialResultCount;
		int spatialRejectedSelfCount;
		int spatialRejectedTeamCount;
		int spatialRejectedDeadCount;
		int spatialRejectedTypeCount;
		double spatialQueryCostMs;
		double memoryMs;
		double visionMs;
	};

	AgentPerceptionSystem();

	void SetEnabled(bool enabled);
	bool IsEnabled() const { return m_enabled; }

	void Clear();
	void Update(const std::vector<AgentObject*>& agents, int deltaMs, ObjectManager* objectManager);
	const Stats& GetStats() const { return m_stats; }
	std::string BuildDebugSummary() const;
	void PublishTracyCounters() const;

private:
	bool m_enabled;
	int m_updateCount;
	Stats m_stats;
};

#endif // __AGENT_PERCEPTION_SYSTEM_H__
