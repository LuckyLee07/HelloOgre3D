#include "NavigationService.h"

#include <math.h>

#include "SandboxMacros.h"
#include "ai/navigation/NavigationMesh.h"
#include "components/agent/AgentLocomotion.h"
#include "objects/BlockObject.h"
#include "recast/include/Recast.h"
#include "systems/manager/ObjectManager.h"

namespace
{
	const float DEFAULT_AGENT_WALKABLE_CLIMB = AgentLocomotion::DEFAULT_AGENT_RADIUS / 2.0f;
	const float DEFAULT_AGENT_WALKABLE_SLOPE = 45.0f;
}

NavigationService::NavigationService(ObjectManager* objectManager)
	: m_objectManager(objectManager)
{
}

NavigationService::~NavigationService()
{
	for (std::unordered_map<Ogre::String, NavigationMesh*>::iterator iter = m_navMeshes.begin(); iter != m_navMeshes.end(); ++iter)
	{
		SAFE_DELETE(iter->second);
	}
	m_navMeshes.clear();
	m_objectManager = nullptr;
}

void NavigationService::SetObjectManager(ObjectManager* objectManager)
{
	m_objectManager = objectManager;
}

NavigationMesh* NavigationService::GetNavigationMesh(const Ogre::String& navMeshName) const
{
	std::unordered_map<Ogre::String, NavigationMesh*>::const_iterator found = m_navMeshes.find(navMeshName);
	return found != m_navMeshes.end() ? found->second : nullptr;
}

bool NavigationService::AddNavigationMesh(const Ogre::String& navMeshName, NavigationMesh* navMesh)
{
	if (navMesh == nullptr)
		return false;

	std::unordered_map<Ogre::String, NavigationMesh*>::iterator found = m_navMeshes.find(navMeshName);
	if (found != m_navMeshes.end())
	{
		SAFE_DELETE(found->second);
		found->second = navMesh;
	}
	else
	{
		m_navMeshes[navMeshName] = navMesh;
	}

	if (m_objectManager != nullptr)
		m_objectManager->clearTacticalInfluenceDebugVisuals();

	return true;
}

int NavigationService::GetNavigationMeshCount() const
{
	return static_cast<int>(m_navMeshes.size());
}

NavigationMesh* NavigationService::CreateNavigationMesh(const rcConfig& config, const Ogre::String& navMeshName)
{
	ObjectManager* objectManager = m_objectManager;
	if (objectManager == nullptr) return nullptr;

	const std::vector<BlockObject*> objects = objectManager->getFixedObjects();
	NavigationMesh* pNavMesh = new NavigationMesh(config, objects);

	if (!pNavMesh->IsValid())
	{
		SAFE_DELETE(pNavMesh);
		return nullptr;
	}

	bool result = AddNavigationMesh(navMeshName, pNavMesh);
	if (!result)
	{
		SAFE_DELETE(pNavMesh);
		return nullptr;
	}

	return pNavMesh;
}

void NavigationService::DefaultConfig(rcConfig& config)
{
	config.cs = 0.1f;
	config.ch = 0.1f;
	config.walkableSlopeAngle = DEFAULT_AGENT_WALKABLE_SLOPE;
	config.walkableHeight = static_cast<int>(
		ceilf(AgentLocomotion::DEFAULT_AGENT_HEIGHT / config.ch));
	config.walkableClimb = static_cast<int>(
		floorf(DEFAULT_AGENT_WALKABLE_CLIMB / config.ch));
	config.walkableRadius = static_cast<int>(
		ceilf(AgentLocomotion::DEFAULT_AGENT_RADIUS * 1.25f / config.cs));
	config.maxEdgeLen = static_cast<int>(20.0f / config.cs);
	config.maxSimplificationError = 1.0f;
	config.minRegionArea = static_cast<int>(pow(50.0f, 2));
	config.mergeRegionArea = static_cast<int>(pow(100.0f, 2));
	config.maxVertsPerPoly = 3;
	config.detailSampleDist = 5.0f * config.cs;
	config.detailSampleMaxError = 1.0f * config.ch;

	config.bmin[0] = -100.05f;
	config.bmin[1] = -100.05f;
	config.bmin[2] = -100.05f;

	config.bmax[0] = 100.05f;
	config.bmax[1] = 100.05f;
	config.bmax[2] = 100.05f;

	rcCalcGridSize(config.bmin, config.bmax, config.cs, &config.width, &config.height);
}

void NavigationService::ApplySettingConfig(rcConfig& config, float height, float radius, float climb)
{
	if (height > 0.0f)
		config.walkableHeight = static_cast<int>(ceilf(height / config.ch));
	if (radius > 0.0f)
		config.walkableRadius = static_cast<int>(ceilf(radius / config.cs));
	if (climb > 0.0f)
		config.walkableClimb = static_cast<int>(floorf(climb / config.ch));
}

Ogre::Vector3 NavigationService::RandomPoint(const Ogre::String& navMeshName) const
{
	NavigationMesh* pNavmesh = GetNavigationMesh(navMeshName);
	if (pNavmesh != nullptr)
	{
		return pNavmesh->RandomPoint();
	}
	return Ogre::Vector3::ZERO;
}

Ogre::Vector3 NavigationService::FindClosestPoint(const Ogre::String& navMeshName, const Ogre::Vector3& point) const
{
	NavigationMesh* pNavmesh = GetNavigationMesh(navMeshName);
	if (pNavmesh != nullptr)
	{
		return pNavmesh->FindClosestPoint(point);
	}
	return Ogre::Vector3::ZERO;
}

bool NavigationService::FindPath(const Ogre::String& navMeshName, const Ogre::Vector3& start, const Ogre::Vector3& end, std::vector<Ogre::Vector3>& outPath) const
{
	NavigationMesh* pNavmesh = GetNavigationMesh(navMeshName);
	if (pNavmesh != nullptr)
	{
		return pNavmesh->FindPath(start, end, outPath);
	}
	return false;
}
