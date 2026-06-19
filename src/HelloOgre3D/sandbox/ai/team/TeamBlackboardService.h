#ifndef __TEAM_BLACKBOARD_SERVICE_H__
#define __TEAM_BLACKBOARD_SERVICE_H__

#include <string>
#include <unordered_map>
#include <vector>

#include "OgreVector3.h"

class AgentObject;

class TeamBlackboardService //tolua_exports
{ //tolua_exports
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

	//tolua_begin
	void clearTeamBlackboardFacts();
	void configureTeamBlackboard(int ttlMs);
	bool rememberTeamEnemyFact(int teamId, int reporterId, int targetId, const Ogre::Vector3& targetPosition, int lastSeenMs, float confidence);
	bool writeBestTeamEnemyFactToBlackboard(AgentObject* agent, const std::string& keyPrefix, bool allowOwnReport = false);
	int getTeamBlackboardFactCount() const;
	int getTeamBlackboardReportCount() const;
	std::string buildTeamBlackboardDebugSummary() const;
	//tolua_end

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
}; //tolua_exports

#endif // __TEAM_BLACKBOARD_SERVICE_H__
