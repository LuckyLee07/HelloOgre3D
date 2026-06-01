#ifndef __TEAM_BLACKBOARD_SERVICE_H__
#define __TEAM_BLACKBOARD_SERVICE_H__

#include <string>
#include <unordered_map>
#include <vector>

#include "OgreVector3.h"

class AgentObject;

class TeamBlackboardService
{
public:
	struct EnemySightingFact
	{
		EnemySightingFact();

		int teamId;
		int targetId;
		int reporterId;
		int reportCount;
		int priority;
		long long lastSeenMs;
		Ogre::Vector3 targetPosition;
		float confidence;
	};

	struct Stats
	{
		Stats();

		int teamCount;
		int factCount;
		int reportCount;
		int updateCount;
		int expiredCount;
		int lastScanAgentCount;
		int lastWriterCount;
		int ttlMs;
		long long currentTimeMs;
	};

	TeamBlackboardService();

	void Clear();
	void SetFactTtlMs(int ttlMs);
	int GetFactTtlMs() const { return m_factTtlMs; }

	bool RememberEnemySighting(int teamId, int reporterId, int targetId, const Ogre::Vector3& targetPosition, long long lastSeenMs, float confidence);
	void SyncFromAgents(const std::vector<AgentObject*>& agents, int deltaMs);
	bool GetBestEnemyFact(int teamId, EnemySightingFact& outFact) const;
	bool GetBestEnemyFact(int teamId, int ignoredReporterId, EnemySightingFact& outFact) const;
	const Stats& GetStats() const { return m_stats; }
	std::string BuildDebugSummary() const;
	void PublishTracyCounters() const;

private:
	typedef std::unordered_map<int, EnemySightingFact> EnemyFactMap;

	struct TeamState
	{
		EnemyFactMap enemyFacts;
	};

	void RememberEnemyFact(const EnemySightingFact& fact);
	int PruneExpiredFacts();
	void RefreshStats(int scannedAgents, int writers);
	static int BuildPriority(float confidence, int reportCount, int ageMs);

	std::unordered_map<int, TeamState> m_teams;
	Stats m_stats;
	int m_factTtlMs;
};

#endif // __TEAM_BLACKBOARD_SERVICE_H__
