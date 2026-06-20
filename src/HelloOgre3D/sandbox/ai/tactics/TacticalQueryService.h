#ifndef __TACTICAL_QUERY_SERVICE_H__
#define __TACTICAL_QUERY_SERVICE_H__

#include <string>
#include <unordered_map>
#include <vector>

#include "OgreVector3.h"
#include "ai/tactics/InfluenceMapSystem.h"

class AgentObject;
class NavigationMesh;
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
		int lastDangerSpreadWrites;
		int lastTeamSpreadWrites;
		int lastObjectiveSpreadWrites;
		int layerPolicyCount;
		int dirtyLayerCount;
		int skippedBuildCount;
		int intervalSkippedBuildCount;
		int dirtySkippedBuildCount;
	};

	TacticalQueryService();
	~TacticalQueryService();

	void Initialize();
	void Shutdown();
	void Clear();
	void ClearEvents();
	void Update(int deltaMs);
	void SetEventTtlMs(int ttlMs);
	void ConfigureLayerUpdatePolicy(const std::string& layerName, int intervalMs, bool dirtyOnly);
	void MarkLayerDirty(const std::string& layerName);
	void SetQueryCandidateLimit(int maxCandidates);
	int GetQueryCandidateLimit() const;

	InfluenceMapSystem* GetInfluenceMapSystem() { return &m_influenceMap; }
	const InfluenceMapSystem* GetInfluenceMapSystem() const { return &m_influenceMap; }
	const Stats& GetStats() const { return m_stats; }

	void ClearInfluence();
	void ConfigureInfluence(float minX, float maxX, float minZ, float maxZ, float cellSize);
	bool ConfigureInfluenceFromNavMesh(const NavigationMesh* navMesh, float cellWidth, float cellHeight, const Ogre::Vector3& boundaryMinOffset, const Ogre::Vector3& boundaryMaxOffset);
	void ClearInfluenceLayer(const std::string& layerName);
	void SetInfluenceLayerOptions(const std::string& layerName, float falloff, float inertia);
	int AddInfluenceSource(const std::string& layerName, const Ogre::Vector3& center, float strength, float radius);
	int AddInfluencePoint(const std::string& layerName, const Ogre::Vector3& center, float strength);
	int SpreadInfluenceLayer(const std::string& layerName, int passCount);
	float SampleInfluenceLayer(const std::string& layerName, const Ogre::Vector3& position) const;
	float ScoreInfluencePosition(const Ogre::Vector3& position, float dangerWeight, float teamWeight, float objectiveWeight) const;
	Ogre::Vector3 FindBestInfluencePosition(const Ogre::Vector3& center, float radius, float step, float dangerWeight, float teamWeight, float objectiveWeight);
	int GetInfluenceLayerActiveCellCount(const std::string& layerName) const;
	int GetInfluenceLayerCellWriteCount(const std::string& layerName) const;
	int GetInfluenceLayerDebugCellCount(const std::string& layerName, float threshold, int maxCells) const;
	Ogre::Vector3 GetInfluenceLayerDebugCellPosition(const std::string& layerName, int luaIndex, float threshold) const;
	float GetInfluenceLayerDebugCellValue(const std::string& layerName, int luaIndex, float threshold) const;
	int GetInfluenceActiveCellCount() const;
	int GetInfluenceCellWriteCount() const;
	int GetInfluenceQueryCount() const;

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

	struct LayerUpdatePolicy
	{
		LayerUpdatePolicy();

		int intervalMs;
		int elapsedMs;
		bool dirtyOnly;
		bool dirty;
		int skippedBuildCount;
		int intervalSkippedBuildCount;
		int dirtySkippedBuildCount;
	};

	struct QueryWeights
	{
		QueryWeights();

		float danger;
		float team;
		float objective;
		float support;
		float cover;
		float crowd;
	};

	void SubscribeEvents();
	void HandleEvent(const SandboxContext& context);
	void AddOrUpdateEvent(const EventRecord& event);
	const EventRecord* GetEventRecordByLuaIndex(int luaIndex) const;
	void ResolveQueryWeights(const std::string& queryType, QueryWeights& weights) const;
	std::string NormalizeLayerName(const std::string& layerName) const;
	LayerUpdatePolicy* FindLayerPolicy(const std::string& layerName);
	const LayerUpdatePolicy* FindLayerPolicy(const std::string& layerName) const;
	LayerUpdatePolicy& GetLayerPolicy(const std::string& layerName);
	void AdvanceLayerPolicies(int elapsedMs);
	bool ShouldRebuildLayer(const std::string& layerName);
	void CompleteLayerRebuild(const std::string& layerName);
	void MarkTrackedLayerDirty(const std::string& layerName);
	void MarkAllLayerPoliciesDirty();
	void MarkDefaultTacticalLayerPoliciesDirty();
	void MarkLayerPoliciesDirtyForEvent(const std::string& eventType);
	int CountDirtyLayerPolicies() const;
	void RefreshPolicyStats();

	InfluenceMapSystem m_influenceMap;
	std::vector<EventRecord> m_events;
	std::vector<Subscription> m_subscriptions;
	std::unordered_map<std::string, LayerUpdatePolicy> m_layerPolicies;
	Stats m_stats;
	int m_eventTtlMs;
	bool m_subscribed;
};

#endif // __TACTICAL_QUERY_SERVICE_H__
