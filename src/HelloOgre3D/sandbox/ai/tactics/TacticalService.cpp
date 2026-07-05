#include "ai/tactics/TacticalService.h"

#include "ai/navigation/NavigationMesh.h"
#include "ai/tactics/TacticalDebugDrawService.h"
#include "ai/tactics/TacticalQueryService.h"
#include "core/SandboxServices.h"
#include "systems/manager/ObjectManager.h"
#include "systems/service/NavigationService.h"

#include <algorithm>
#include <limits>
#include <sstream>

TacticalService::TacticalService(ObjectManager* objectManager, TacticalQueryService* queryService, TacticalDebugDrawService* debugDrawService)
	: m_objectManager(objectManager)
	, m_queryService(queryService)
	, m_debugDrawService(debugDrawService)
	, m_currentTimeMs(0)
{
}

TacticalService::~TacticalService()
{
	m_objectManager = nullptr;
	m_queryService = nullptr;
	m_debugDrawService = nullptr;
}

void TacticalService::SetDependencies(ObjectManager* objectManager, TacticalQueryService* queryService, TacticalDebugDrawService* debugDrawService)
{
	m_objectManager = objectManager;
	m_queryService = queryService;
	m_debugDrawService = debugDrawService;
}

NavigationMesh* TacticalService::ResolveNavigationMesh(const std::string& navMeshName) const
{
	if (m_objectManager == nullptr)
		return nullptr;

	const SandboxServices& services = m_objectManager->GetSandboxServices();
	if (services.navigation != nullptr)
		return services.navigation->GetNavigationMesh(navMeshName);
	return m_objectManager->getNavigationMesh(navMeshName);
}

void TacticalService::clearTacticalInfluence()
{
	if (m_queryService != nullptr)
		m_queryService->ClearInfluence();
}

void TacticalService::configureTacticalInfluence(float minX, float maxX, float minZ, float maxZ, float cellSize)
{
	if (m_queryService != nullptr)
		m_queryService->ConfigureInfluence(minX, maxX, minZ, maxZ, cellSize);
	clearTacticalInfluenceDebugVisuals();
}

void TacticalService::configureTacticalInfluenceFromNavMesh(const std::string& navMeshName, float cellWidth, float cellHeight, const Ogre::Vector3& boundaryMinOffset, const Ogre::Vector3& boundaryMaxOffset)
{
	if (m_queryService == nullptr)
		return;

	NavigationMesh* navMesh = ResolveNavigationMesh(navMeshName);
	m_queryService->ConfigureInfluenceFromNavMesh(navMesh, cellWidth, cellHeight, boundaryMinOffset, boundaryMaxOffset);
	clearTacticalInfluenceDebugVisuals();
}

void TacticalService::configureTacticalInfluenceLayerUpdate(const std::string& layerName, int intervalMs, bool dirtyOnly)
{
	if (m_queryService != nullptr)
		m_queryService->ConfigureLayerUpdatePolicy(layerName, intervalMs, dirtyOnly);
}

void TacticalService::markTacticalInfluenceLayerDirty(const std::string& layerName)
{
	if (m_queryService != nullptr)
		m_queryService->MarkLayerDirty(layerName);
}

void TacticalService::configureTacticalQueryCandidateLimit(int maxCandidates)
{
	if (m_queryService != nullptr)
		m_queryService->SetQueryCandidateLimit(maxCandidates);
}

int TacticalService::getTacticalQueryCandidateLimit() const
{
	return m_queryService != nullptr ? m_queryService->GetQueryCandidateLimit() : 0;
}

void TacticalService::clearTacticalInfluenceLayer(const std::string& layerName)
{
	if (m_queryService != nullptr)
		m_queryService->ClearInfluenceLayer(layerName);
}

void TacticalService::setTacticalInfluenceLayerOptions(const std::string& layerName, float falloff, float inertia)
{
	if (m_queryService != nullptr)
		m_queryService->SetInfluenceLayerOptions(layerName, falloff, inertia);
}

int TacticalService::addTacticalInfluenceSource(const std::string& layerName, const Ogre::Vector3& center, float strength, float radius)
{
	if (m_queryService == nullptr)
		return 0;
	return m_queryService->AddInfluenceSource(layerName, center, strength, radius);
}

int TacticalService::addTacticalInfluencePoint(const std::string& layerName, const Ogre::Vector3& center, float strength)
{
	if (m_queryService == nullptr)
		return 0;
	return m_queryService->AddInfluencePoint(layerName, center, strength);
}

int TacticalService::spreadTacticalInfluenceLayer(const std::string& layerName, int passCount)
{
	if (m_queryService == nullptr)
		return 0;
	return m_queryService->SpreadInfluenceLayer(layerName, passCount);
}

float TacticalService::sampleTacticalInfluence(const std::string& layerName, const Ogre::Vector3& position) const
{
	if (m_queryService == nullptr)
		return 0.0f;
	return m_queryService->SampleInfluenceLayer(layerName, position);
}

float TacticalService::scoreTacticalPosition(const Ogre::Vector3& position, float dangerWeight, float teamWeight, float objectiveWeight) const
{
	if (m_queryService == nullptr)
		return 0.0f;
	return m_queryService->ScoreInfluencePosition(position, dangerWeight, teamWeight, objectiveWeight);
}

Ogre::Vector3 TacticalService::findBestTacticalPosition(const Ogre::Vector3& center, float radius, float step, float dangerWeight, float teamWeight, float objectiveWeight)
{
	if (m_queryService == nullptr)
		return center;
	return m_queryService->FindBestInfluencePosition(center, radius, step, dangerWeight, teamWeight, objectiveWeight);
}

float TacticalService::scoreTacticalQueryPosition(const std::string& queryType, const Ogre::Vector3& position) const
{
	if (m_queryService == nullptr)
		return 0.0f;
	return m_queryService->ScoreQueryPosition(queryType, position);
}

Ogre::Vector3 TacticalService::findBestTacticalQueryPosition(const std::string& queryType, const Ogre::Vector3& center, float radius, float step)
{
	if (m_queryService == nullptr)
		return center;
	return m_queryService->FindBestQueryPosition(queryType, center, radius, step);
}

Ogre::Vector3 TacticalService::findBestSupportPosition(const Ogre::Vector3& center, float radius, float step)
{
	if (m_queryService == nullptr)
		return center;
	return m_queryService->FindBestSupportPosition(center, radius, step);
}

Ogre::Vector3 TacticalService::findLowThreatPosition(const Ogre::Vector3& center, float radius, float step)
{
	if (m_queryService == nullptr)
		return center;
	return m_queryService->FindLowThreatPosition(center, radius, step);
}

void TacticalService::configureTacticalEvents(int eventTtlMs)
{
	if (m_queryService != nullptr)
		m_queryService->SetEventTtlMs(eventTtlMs);
}

void TacticalService::clearTacticalEvents()
{
	if (m_queryService != nullptr)
		m_queryService->ClearEvents();
}

void TacticalService::publishTacticalEvent(const std::string& eventType, int senderId, int targetId, int teamId, int targetTeamId, const Ogre::Vector3& position, int timeMs, const std::string& scopeName, bool queueEvent)
{
	if (m_queryService == nullptr)
		return;

	int resolvedTimeMs = timeMs;
	if (resolvedTimeMs <= 0)
		resolvedTimeMs = static_cast<int>(std::max<long long>(0, std::min<long long>(m_currentTimeMs, std::numeric_limits<int>::max())));
	m_queryService->PublishEvent(eventType, senderId, targetId, teamId, targetTeamId, position, resolvedTimeMs, scopeName, queueEvent);
}

int TacticalService::getTacticalEventCount() const
{
	return m_queryService != nullptr ? m_queryService->GetEventCount() : 0;
}

int TacticalService::getTacticalEventTypeCount(const std::string& eventType) const
{
	return m_queryService != nullptr ? m_queryService->GetEventCountByType(eventType) : 0;
}

Ogre::Vector3 TacticalService::getLastTacticalEventPosition(const std::string& eventType, const Ogre::Vector3& fallback) const
{
	return m_queryService != nullptr ? m_queryService->GetLastEventPosition(eventType, fallback) : fallback;
}

int TacticalService::getTacticalEventDebugRecordCount() const
{
	return m_queryService != nullptr ? m_queryService->GetEventDebugRecordCount() : 0;
}

std::string TacticalService::getTacticalEventDebugType(int luaIndex) const
{
	return m_queryService != nullptr ? m_queryService->GetEventDebugType(luaIndex) : "";
}

int TacticalService::getTacticalEventDebugSenderId(int luaIndex) const
{
	return m_queryService != nullptr ? m_queryService->GetEventDebugSenderId(luaIndex) : -1;
}

int TacticalService::getTacticalEventDebugTargetId(int luaIndex) const
{
	return m_queryService != nullptr ? m_queryService->GetEventDebugTargetId(luaIndex) : -1;
}

int TacticalService::getTacticalEventDebugTeamId(int luaIndex) const
{
	return m_queryService != nullptr ? m_queryService->GetEventDebugTeamId(luaIndex) : -1;
}

int TacticalService::getTacticalEventDebugTargetTeamId(int luaIndex) const
{
	return m_queryService != nullptr ? m_queryService->GetEventDebugTargetTeamId(luaIndex) : -1;
}

Ogre::Vector3 TacticalService::getTacticalEventDebugPosition(int luaIndex) const
{
	return m_queryService != nullptr ? m_queryService->GetEventDebugPosition(luaIndex) : Ogre::Vector3::ZERO;
}

int TacticalService::getTacticalEventDebugTimeMs(int luaIndex) const
{
	return m_queryService != nullptr ? m_queryService->GetEventDebugTimeMs(luaIndex) : 0;
}

int TacticalService::getTacticalEventDebugRemainingTtlMs(int luaIndex) const
{
	return m_queryService != nullptr ? m_queryService->GetEventDebugRemainingTtlMs(luaIndex) : 0;
}

int TacticalService::rebuildTacticalDangerLayer(int perspectiveTeamId, float dangerStrength, float bulletShotRadius, float bulletImpactRadius, float deadFriendlyRadius, float enemySightingRadius, int spreadPasses)
{
	if (m_queryService == nullptr)
		return 0;
	return m_queryService->RebuildDangerLayer(perspectiveTeamId, dangerStrength, bulletShotRadius, bulletImpactRadius, deadFriendlyRadius, enemySightingRadius, spreadPasses);
}

int TacticalService::rebuildTacticalTeamLayer(int positiveTeamId, float teamStrength, float radius, int spreadPasses)
{
	if (m_queryService == nullptr || m_objectManager == nullptr)
		return 0;
	return m_queryService->RebuildTeamLayer(m_objectManager->getAllAgents(), positiveTeamId, teamStrength, radius, spreadPasses);
}

int TacticalService::rebuildTacticalObjectiveLayer(const Ogre::Vector3& center, float strength, float radius, int spreadPasses)
{
	if (m_queryService == nullptr)
		return 0;
	return m_queryService->RebuildObjectiveLayer(center, strength, radius, spreadPasses);
}

int TacticalService::getTacticalInfluenceLayerActiveCellCount(const std::string& layerName) const
{
	return m_queryService != nullptr ? m_queryService->GetInfluenceLayerActiveCellCount(layerName) : 0;
}

int TacticalService::getTacticalInfluenceLayerCellWriteCount(const std::string& layerName) const
{
	return m_queryService != nullptr ? m_queryService->GetInfluenceLayerCellWriteCount(layerName) : 0;
}

int TacticalService::getTacticalInfluenceLayerDebugCellCount(const std::string& layerName, float threshold, int maxCells) const
{
	return m_queryService != nullptr ? m_queryService->GetInfluenceLayerDebugCellCount(layerName, threshold, maxCells) : 0;
}

Ogre::Vector3 TacticalService::getTacticalInfluenceLayerDebugCellPosition(const std::string& layerName, int luaIndex, float threshold) const
{
	return m_queryService != nullptr ? m_queryService->GetInfluenceLayerDebugCellPosition(layerName, luaIndex, threshold) : Ogre::Vector3::ZERO;
}

float TacticalService::getTacticalInfluenceLayerDebugCellValue(const std::string& layerName, int luaIndex, float threshold) const
{
	return m_queryService != nullptr ? m_queryService->GetInfluenceLayerDebugCellValue(layerName, luaIndex, threshold) : 0.0f;
}

int TacticalService::drawTacticalInfluenceLayer(const std::string& layerName, float yOffset, const Ogre::ColourValue& positiveValue, const Ogre::ColourValue& zeroValue, const Ogre::ColourValue& negativeValue, const Ogre::ColourValue& gridColor, float threshold, int maxCells, bool drawNeutralCells, bool projectToNav, float maxProjectionDistance, const Ogre::String& navMeshName)
{
	if (m_queryService == nullptr || m_debugDrawService == nullptr)
		return 0;
	const NavigationMesh* navMesh = projectToNav ? ResolveNavigationMesh(navMeshName) : nullptr;
	return m_debugDrawService->DrawLayer(m_queryService->GetInfluenceMapSystem(), layerName, yOffset, positiveValue, zeroValue, negativeValue, gridColor, threshold, maxCells, drawNeutralCells, projectToNav, maxProjectionDistance, navMesh, navMeshName);
}

int TacticalService::rebuildTacticalInfluenceLayerDebugVisual(const std::string& layerName, float yOffset, const Ogre::ColourValue& positiveValue, const Ogre::ColourValue& zeroValue, const Ogre::ColourValue& negativeValue, const Ogre::ColourValue& gridColor, float threshold, int maxCells, bool drawNeutralCells, bool projectToNav, float maxProjectionDistance, const Ogre::String& navMeshName)
{
	if (m_queryService == nullptr || m_debugDrawService == nullptr)
		return 0;
	const NavigationMesh* navMesh = projectToNav ? ResolveNavigationMesh(navMeshName) : nullptr;
	return m_debugDrawService->RebuildLayerDebugVisual(m_queryService->GetInfluenceMapSystem(), layerName, yOffset, positiveValue, zeroValue, negativeValue, gridColor, threshold, maxCells, drawNeutralCells, projectToNav, maxProjectionDistance, navMesh, navMeshName);
}

void TacticalService::configureTacticalInfluenceLayerDebug(const std::string& layerName, float yOffset, float threshold, int maxCells, bool drawNeutralCells, bool projectToNav, float maxProjectionDistance, const Ogre::String& navMeshName, int drawOrder)
{
	if (m_debugDrawService != nullptr)
		m_debugDrawService->ConfigureLayer(layerName, yOffset, threshold, maxCells, drawNeutralCells, projectToNav, maxProjectionDistance, navMeshName, drawOrder);
}

void TacticalService::setTacticalInfluenceLayerDebugOrder(const std::string& layerName, int drawOrder)
{
	if (m_debugDrawService != nullptr)
		m_debugDrawService->SetLayerDrawOrder(layerName, drawOrder);
}

void TacticalService::setTacticalInfluenceDebugVisible(bool visible)
{
	if (m_debugDrawService != nullptr)
		m_debugDrawService->SetVisible(visible);
}

void TacticalService::clearTacticalInfluenceDebugVisuals()
{
	if (m_debugDrawService != nullptr)
		m_debugDrawService->ClearVisuals();
}

int TacticalService::getTacticalInfluenceActiveCellCount() const
{
	return m_queryService != nullptr ? m_queryService->GetInfluenceActiveCellCount() : 0;
}

int TacticalService::getTacticalInfluenceCellWriteCount() const
{
	return m_queryService != nullptr ? m_queryService->GetInfluenceCellWriteCount() : 0;
}

int TacticalService::getTacticalInfluenceQueryCount() const
{
	return m_queryService != nullptr ? m_queryService->GetInfluenceQueryCount() : 0;
}

std::string TacticalService::buildTacticalInfluenceDebugSummary() const
{
	if (m_queryService == nullptr)
		return "[TacticalQueryService] unavailable";
	if (m_debugDrawService == nullptr)
		return m_queryService->BuildDebugSummary();

	std::ostringstream stream;
	stream << m_queryService->BuildDebugSummary()
		<< "\n" << m_debugDrawService->BuildDebugSummary();
	return stream.str();
}
