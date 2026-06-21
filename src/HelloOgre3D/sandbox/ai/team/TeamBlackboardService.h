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
		int enemyFactCount;
		int typedFactCount;
		int reportCount;
		int typedReportCount;
		int updateCount;
		int expiredCount;
		int lastScanAgentCount;
		int lastWriterCount;
		int ttlMs;
		long long currentTimeMs;
	};

	struct TeamFact
	{
		TeamFact();

		std::string factType;
		std::string key;
		int teamId;
		int sourceAgentId;
		int targetAgentId;
		int reportCount;
		int priority;
		long long timeMs;
		int ttlMs;
		Ogre::Vector3 position;
		float confidence;
	};

	TeamBlackboardService();

	void Clear();
	void SetFactTtlMs(int ttlMs);
	int GetFactTtlMs() const { return m_factTtlMs; }

	bool RememberEnemySighting(int teamId, int reporterId, int targetId, const Ogre::Vector3& targetPosition, long long lastSeenMs, float confidence);
	bool RememberFact(const std::string& factType, int teamId, int sourceAgentId, int targetAgentId, const Ogre::Vector3& position, long long timeMs, float confidence, int priority, int ttlMs, const std::string& key);
	void SyncFromAgents(const std::vector<AgentObject*>& agents, int deltaMs);
	bool GetBestEnemyFact(int teamId, EnemySightingFact& outFact) const;
	bool GetBestEnemyFact(int teamId, int ignoredReporterId, EnemySightingFact& outFact) const;
	bool GetBestFact(int teamId, const std::string& factType, int ignoredSourceAgentId, TeamFact& outFact) const;
	const Stats& GetStats() const { return m_stats; }
	std::string BuildDebugSummary() const;
	void PublishTracyCounters() const;

	//tolua_begin
	void clearTeamBlackboardFacts();
	void configureTeamBlackboard(int ttlMs);
	bool rememberTeamEnemyFact(int teamId, int reporterId, int targetId, const Ogre::Vector3& targetPosition, int lastSeenMs, float confidence);
	bool rememberTeamFact(const std::string& factType, int teamId, int sourceAgentId, int targetAgentId, const Ogre::Vector3& position, int timeMs, float confidence, int priority, int ttlMs, const std::string& key);
	bool writeBestTeamEnemyFactToBlackboard(AgentObject* agent, const std::string& keyPrefix, bool allowOwnReport = false);
	bool writeBestTeamFactToBlackboard(AgentObject* agent, const std::string& factType, const std::string& keyPrefix, bool allowOwnReport = true);
	bool hasBestTeamFact(int teamId, const std::string& factType, int ignoredSourceAgentId = -1) const;
	int getBestTeamFactSourceAgentId(int teamId, const std::string& factType, int ignoredSourceAgentId = -1) const;
	int getBestTeamFactTargetId(int teamId, const std::string& factType, int ignoredSourceAgentId = -1) const;
	Ogre::Vector3 getBestTeamFactPosition(int teamId, const std::string& factType, int ignoredSourceAgentId = -1) const;
	std::string getBestTeamFactKey(int teamId, const std::string& factType, int ignoredSourceAgentId = -1) const;
	float getBestTeamFactConfidence(int teamId, const std::string& factType, int ignoredSourceAgentId = -1) const;
	int getBestTeamFactReportCount(int teamId, const std::string& factType, int ignoredSourceAgentId = -1) const;
	int getBestTeamFactPriority(int teamId, const std::string& factType, int ignoredSourceAgentId = -1) const;
	int getBestTeamFactTimeMs(int teamId, const std::string& factType, int ignoredSourceAgentId = -1) const;
	int getBestTeamFactAgeMs(int teamId, const std::string& factType, int ignoredSourceAgentId = -1) const;
	int getTeamBlackboardFactCount() const;
	int getTeamBlackboardReportCount() const;
	int getTeamBlackboardEnemyFactCount() const;
	int getTeamBlackboardTypedFactCount() const;
	int getTeamBlackboardTypedReportCount() const;
	std::string buildTeamBlackboardDebugSummary() const;
	//tolua_end

private:
	typedef std::unordered_map<int, EnemySightingFact> EnemyFactMap;
	typedef std::unordered_map<std::string, TeamFact> TeamFactMap;

	struct TeamState
	{
		EnemyFactMap enemyFacts;
		TeamFactMap facts;
	};

	void RememberEnemyFact(const EnemySightingFact& fact);
	void RememberTypedFact(const TeamFact& fact);
	int PruneExpiredFacts();
	void RefreshStats(int scannedAgents, int writers);
	static std::string BuildFactKey(const std::string& factType, int sourceAgentId, int targetAgentId, const std::string& explicitKey);
	static int BuildPriority(float confidence, int reportCount, int ageMs);

	std::unordered_map<int, TeamState> m_teams;
	Stats m_stats;
	int m_factTtlMs;
}; //tolua_exports

#endif // __TEAM_BLACKBOARD_SERVICE_H__
