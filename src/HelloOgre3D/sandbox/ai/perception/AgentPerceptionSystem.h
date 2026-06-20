#ifndef __AGENT_PERCEPTION_SYSTEM_H__
#define __AGENT_PERCEPTION_SYSTEM_H__

#include <string>
#include <vector>

#include "OgreVector3.h"
#include "ai/perception/HearingDangerSense.h"

class AgentObject;
class ObjectManager;

class AgentPerceptionSystem //tolua_exports
{ //tolua_exports
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
			, hearingDangerEnabled(0)
			, hearingDangerEventCount(0)
			, hearingDangerPublishedEventCount(0)
			, hearingDangerPrunedEventCount(0)
			, hearingDangerRunCount(0)
			, hearingDangerSkipCount(0)
			, hearingDangerAgentCheckCount(0)
			, hearingDangerHeardResponseCount(0)
			, hearingDangerDangerResponseCount(0)
			, hearingDangerInvestigationCount(0)
			, hearingDangerRetreatFactApplyCount(0)
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
		int hearingDangerEnabled;
		int hearingDangerEventCount;
		int hearingDangerPublishedEventCount;
		int hearingDangerPrunedEventCount;
		int hearingDangerRunCount;
		int hearingDangerSkipCount;
		int hearingDangerAgentCheckCount;
		int hearingDangerHeardResponseCount;
		int hearingDangerDangerResponseCount;
		int hearingDangerInvestigationCount;
		int hearingDangerRetreatFactApplyCount;
	};

	AgentPerceptionSystem();

	void SetEnabled(bool enabled);
	bool IsEnabled() const { return m_enabled; }

	void Clear();
	void Update(const std::vector<AgentObject*>& agents, int deltaMs, ObjectManager* objectManager);
	const Stats& GetStats() const { return m_stats; }
	std::string BuildDebugSummary() const;
	void PublishTracyCounters() const;

	//tolua_begin
	void configureHearingDangerSense(bool enabled, int scanIntervalMs, int agentsPerTick, int responseCooldownMs, int dangerCooldownMs, float investigateStopDistance, float escapeDistance);
	void clearHearingDangerSense();
	bool publishHearingDangerEvent(int sourceId, int sourceTeamId, int targetId, const Ogre::Vector3& position, const Ogre::Vector3& impactPosition, int timeMs, int ttlMs, float hearingRadius, float dangerRadius);
	int getHearingDangerEventCount() const;
	int getHearingDangerPublishedEventCount() const;
	int getHearingDangerPrunedEventCount() const;
	int getHearingDangerRunCount() const;
	int getHearingDangerSkipCount() const;
	int getHearingDangerAgentCheckCount() const;
	int getHearingDangerHeardResponseCount() const;
	int getHearingDangerDangerResponseCount() const;
	int getHearingDangerInvestigationCount() const;
	int getHearingDangerRetreatFactApplyCount() const;
	std::string buildPerceptionDebugSummary() const;
	//tolua_end

private:
	void CopyHearingDangerStats(Stats& stats) const;

	bool m_enabled;
	int m_updateCount;
	Stats m_stats;

	HearingDangerSense m_hearingDangerSense;
}; //tolua_exports

#endif // __AGENT_PERCEPTION_SYSTEM_H__
