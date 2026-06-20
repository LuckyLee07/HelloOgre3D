#ifndef __TACTICAL_SERVICE_H__
#define __TACTICAL_SERVICE_H__

#include <string>

#include "OgreColourValue.h"
#include "OgreString.h"
#include "OgreVector3.h"

class ObjectManager;
class NavigationMesh;
class TacticalDebugDrawService;
class TacticalQueryService;

class TacticalService //tolua_exports
{ //tolua_exports
public:
	TacticalService(ObjectManager* objectManager, TacticalQueryService* queryService, TacticalDebugDrawService* debugDrawService);
	~TacticalService();

	void SetDependencies(ObjectManager* objectManager, TacticalQueryService* queryService, TacticalDebugDrawService* debugDrawService);
	void SetCurrentTimeMs(long long currentTimeMs) { m_currentTimeMs = currentTimeMs; }

	//tolua_begin
	void clearTacticalInfluence();
	void configureTacticalInfluence(float minX, float maxX, float minZ, float maxZ, float cellSize);
	void configureTacticalInfluenceFromNavMesh(const std::string& navMeshName, float cellWidth, float cellHeight, const Ogre::Vector3& boundaryMinOffset, const Ogre::Vector3& boundaryMaxOffset);
	void configureTacticalInfluenceLayerUpdate(const std::string& layerName, int intervalMs, bool dirtyOnly);
	void markTacticalInfluenceLayerDirty(const std::string& layerName);
	void configureTacticalQueryCandidateLimit(int maxCandidates);
	int getTacticalQueryCandidateLimit() const;
	void clearTacticalInfluenceLayer(const std::string& layerName);
	void setTacticalInfluenceLayerOptions(const std::string& layerName, float falloff, float inertia);
	int addTacticalInfluenceSource(const std::string& layerName, const Ogre::Vector3& center, float strength, float radius);
	int addTacticalInfluencePoint(const std::string& layerName, const Ogre::Vector3& center, float strength);
	int spreadTacticalInfluenceLayer(const std::string& layerName, int passCount);
	float sampleTacticalInfluence(const std::string& layerName, const Ogre::Vector3& position) const;
	float scoreTacticalPosition(const Ogre::Vector3& position, float dangerWeight, float teamWeight, float objectiveWeight) const;
	Ogre::Vector3 findBestTacticalPosition(const Ogre::Vector3& center, float radius, float step, float dangerWeight, float teamWeight, float objectiveWeight);
	float scoreTacticalQueryPosition(const std::string& queryType, const Ogre::Vector3& position) const;
	Ogre::Vector3 findBestTacticalQueryPosition(const std::string& queryType, const Ogre::Vector3& center, float radius, float step);
	Ogre::Vector3 findBestSupportPosition(const Ogre::Vector3& center, float radius, float step);
	Ogre::Vector3 findLowThreatPosition(const Ogre::Vector3& center, float radius, float step);
	void configureTacticalEvents(int eventTtlMs);
	void clearTacticalEvents();
	void publishTacticalEvent(const std::string& eventType, int senderId, int targetId, int teamId, int targetTeamId, const Ogre::Vector3& position, int timeMs, const std::string& scopeName, bool queueEvent);
	int getTacticalEventCount() const;
	int getTacticalEventTypeCount(const std::string& eventType) const;
	Ogre::Vector3 getLastTacticalEventPosition(const std::string& eventType, const Ogre::Vector3& fallback) const;
	int getTacticalEventDebugRecordCount() const;
	std::string getTacticalEventDebugType(int luaIndex) const;
	int getTacticalEventDebugSenderId(int luaIndex) const;
	int getTacticalEventDebugTargetId(int luaIndex) const;
	int getTacticalEventDebugTeamId(int luaIndex) const;
	int getTacticalEventDebugTargetTeamId(int luaIndex) const;
	Ogre::Vector3 getTacticalEventDebugPosition(int luaIndex) const;
	int getTacticalEventDebugTimeMs(int luaIndex) const;
	int getTacticalEventDebugRemainingTtlMs(int luaIndex) const;
	int rebuildTacticalDangerLayer(int perspectiveTeamId, float dangerStrength, float bulletShotRadius, float bulletImpactRadius, float deadFriendlyRadius, float enemySightingRadius, int spreadPasses);
	int rebuildTacticalTeamLayer(int positiveTeamId, float teamStrength, float radius, int spreadPasses);
	int rebuildTacticalObjectiveLayer(const Ogre::Vector3& center, float strength, float radius, int spreadPasses);
	int getTacticalInfluenceLayerActiveCellCount(const std::string& layerName) const;
	int getTacticalInfluenceLayerCellWriteCount(const std::string& layerName) const;
	int getTacticalInfluenceLayerDebugCellCount(const std::string& layerName, float threshold, int maxCells) const;
	Ogre::Vector3 getTacticalInfluenceLayerDebugCellPosition(const std::string& layerName, int luaIndex, float threshold) const;
	float getTacticalInfluenceLayerDebugCellValue(const std::string& layerName, int luaIndex, float threshold) const;
	int drawTacticalInfluenceLayer(const std::string& layerName, float yOffset, const Ogre::ColourValue& positiveValue, const Ogre::ColourValue& zeroValue, const Ogre::ColourValue& negativeValue, const Ogre::ColourValue& gridColor, float threshold, int maxCells, bool drawNeutralCells, bool projectToNav, float maxProjectionDistance, const Ogre::String& navMeshName);
	int rebuildTacticalInfluenceLayerDebugVisual(const std::string& layerName, float yOffset, const Ogre::ColourValue& positiveValue, const Ogre::ColourValue& zeroValue, const Ogre::ColourValue& negativeValue, const Ogre::ColourValue& gridColor, float threshold, int maxCells, bool drawNeutralCells, bool projectToNav, float maxProjectionDistance, const Ogre::String& navMeshName);
	void setTacticalInfluenceLayerDebugOrder(const std::string& layerName, int drawOrder);
	void setTacticalInfluenceDebugVisible(bool visible);
	void clearTacticalInfluenceDebugVisuals();
	int getTacticalInfluenceActiveCellCount() const;
	int getTacticalInfluenceCellWriteCount() const;
	int getTacticalInfluenceQueryCount() const;
	std::string buildTacticalInfluenceDebugSummary() const;
	//tolua_end

private:
	NavigationMesh* ResolveNavigationMesh(const std::string& navMeshName) const;

	ObjectManager* m_objectManager;
	TacticalQueryService* m_queryService;
	TacticalDebugDrawService* m_debugDrawService;
	long long m_currentTimeMs;
}; //tolua_exports

#endif // __TACTICAL_SERVICE_H__
