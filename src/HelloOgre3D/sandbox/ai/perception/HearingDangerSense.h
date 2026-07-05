#ifndef __HEARING_DANGER_SENSE_H__
#define __HEARING_DANGER_SENSE_H__

#include <string>
#include <unordered_map>
#include <vector>

#include "OgreVector3.h"

class AgentObject;
class ObjectManager;

class HearingDangerSense
{
public:
	struct Config
	{
		Config();

		bool enabled;
		int scanIntervalMs;
		int agentsPerTick;
		int responseCooldownMs;
		int dangerCooldownMs;
		float investigateStopDistance;
		float escapeDistance;
	};

	struct Event
	{
		Event();

		int sourceId;
		int sourceTeamId;
		int targetId;
		Ogre::Vector3 position;
		Ogre::Vector3 impactPosition;
		long long timeMs;
		int ttlMs;
		float hearingRadius;
		float dangerRadius;
	};

	struct Stats
	{
		Stats();

		int enabled;
		int eventCount;
		int publishedEventCount;
		int prunedEventCount;
		int runCount;
		int skipCount;
		int agentCheckCount;
		int heardResponseCount;
		int dangerResponseCount;
		int investigationCount;
		int retreatFactApplyCount;
	};

	HearingDangerSense();

	void Configure(bool enabled, int scanIntervalMs, int agentsPerTick, int responseCooldownMs, int dangerCooldownMs, float investigateStopDistance, float escapeDistance);
	void Clear();
	bool PublishEvent(int sourceId, int sourceTeamId, int targetId, const Ogre::Vector3& position, const Ogre::Vector3& impactPosition, int timeMs, int ttlMs, float hearingRadius, float dangerRadius);
	void Update(const std::vector<AgentObject*>& agents, int deltaMs, ObjectManager* objectManager);

	const Stats& GetStats() const { return m_stats; }

private:
	void ApplyToAgent(AgentObject* agent, ObjectManager* objectManager);
	int PruneEvents();
	int PruneCooldowns();
	void RefreshStats();
	Ogre::Vector3 ProjectToNavigation(ObjectManager* objectManager, const Ogre::Vector3& position) const;

	Config m_config;
	std::vector<Event> m_events;
	std::unordered_map<std::string, long long> m_lastHeardAt;
	std::unordered_map<std::string, long long> m_lastDangerAt;
	long long m_currentTimeMs;
	int m_elapsedMs;
	int m_cursor;
	int m_publishedEventCount;
	int m_prunedEventCount;
	int m_runCount;
	int m_skipCount;
	int m_agentCheckCount;
	int m_heardResponseCount;
	int m_dangerResponseCount;
	int m_investigationCount;
	int m_retreatFactApplyCount;
	Stats m_stats;
};

#endif // __HEARING_DANGER_SENSE_H__
