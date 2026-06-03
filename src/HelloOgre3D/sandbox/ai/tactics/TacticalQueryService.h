#ifndef __TACTICAL_QUERY_SERVICE_H__
#define __TACTICAL_QUERY_SERVICE_H__

#include <string>
#include <vector>

#include "OgreVector3.h"
#include "ai/tactics/InfluenceMapSystem.h"

class AgentObject;
class SandboxContext;

class TacticalQueryService
{
public:
	struct Stats
	{
		Stats();

		int capturedEventCount;
		int currentEventCount;
		int expiredEventCount;
		int dangerBuildCount;
		int teamBuildCount;
		int objectiveBuildCount;
		int lastDangerWrites;
		int lastTeamWrites;
		int lastObjectiveWrites;
		int lastSpreadWrites;
	};

	TacticalQueryService();
	~TacticalQueryService();

	void Initialize();
	void Shutdown();
	void Clear();
	void ClearEvents();
	void Update(int deltaMs);
	void SetEventTtlMs(int ttlMs);

	InfluenceMapSystem* GetInfluenceMapSystem() { return &m_influenceMap; }
	const InfluenceMapSystem* GetInfluenceMapSystem() const { return &m_influenceMap; }
	const Stats& GetStats() const { return m_stats; }

	void PublishEvent(const std::string& eventType, int senderId, int targetId, int teamId, int targetTeamId, const Ogre::Vector3& position, int timeMs, const std::string& scopeName, bool queueEvent);
	int GetEventCount() const;
	int GetEventCountByType(const std::string& eventType) const;
	Ogre::Vector3 GetLastEventPosition(const std::string& eventType, const Ogre::Vector3& fallback) const;
	int GetEventDebugRecordCount() const;
	std::string GetEventDebugType(int luaIndex) const;
	int GetEventDebugSenderId(int luaIndex) const;
	int GetEventDebugTargetId(int luaIndex) const;
	int GetEventDebugTeamId(int luaIndex) const;
	int GetEventDebugTargetTeamId(int luaIndex) const;
	Ogre::Vector3 GetEventDebugPosition(int luaIndex) const;
	int GetEventDebugTimeMs(int luaIndex) const;
	int GetEventDebugRemainingTtlMs(int luaIndex) const;

	int RebuildDangerLayer(int perspectiveTeamId, float dangerStrength, float bulletShotRadius, float bulletImpactRadius, float deadFriendlyRadius, float enemySightingRadius, int spreadPasses);
	int RebuildTeamLayer(const std::vector<AgentObject*>& agents, int positiveTeamId, float teamStrength, float radius, int spreadPasses);
	int RebuildObjectiveLayer(const Ogre::Vector3& center, float strength, float radius, int spreadPasses);

	float ScoreQueryPosition(const std::string& queryType, const Ogre::Vector3& position) const;
	Ogre::Vector3 FindBestQueryPosition(const std::string& queryType, const Ogre::Vector3& center, float radius, float step);
	Ogre::Vector3 FindBestSupportPosition(const Ogre::Vector3& center, float radius, float step);
	Ogre::Vector3 FindLowThreatPosition(const Ogre::Vector3& center, float radius, float step);
	std::string BuildDebugSummary() const;

private:
	struct EventRecord
	{
		EventRecord();

		std::string eventType;
		int senderId;
		int targetId;
		int teamId;
		int targetTeamId;
		Ogre::Vector3 position;
		int timeMs;
		int remainingTtlMs;
	};

	struct Subscription
	{
		Subscription();

		std::string eventType;
		int scope;
		int token;
	};

	void SubscribeEvents();
	void HandleEvent(const SandboxContext& context);
	void AddOrUpdateEvent(const EventRecord& event);
	const EventRecord* GetEventRecordByLuaIndex(int luaIndex) const;
	void ResolveQueryWeights(const std::string& queryType, float& dangerWeight, float& teamWeight, float& objectiveWeight) const;

	InfluenceMapSystem m_influenceMap;
	std::vector<EventRecord> m_events;
	std::vector<Subscription> m_subscriptions;
	Stats m_stats;
	int m_eventTtlMs;
	bool m_subscribed;
};

#endif // __TACTICAL_QUERY_SERVICE_H__
