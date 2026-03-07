#include "NavigationMesh.h"
#include "object/BaseObject.h"
#include "systems/manager/ObjectManager.h"
#include "recast/include/Recast.h"
#include "detour/include/DetourNavMesh.h"
#include "detour/include/DetourNavMeshQuery.h"
#include "objects/BlockObject.h"
#include "NavBuilder.h"
#include "ClientManager.h"
#include "Ogre.h"
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <unordered_set>
#include "systems/manager/SandboxMgr.h"

namespace
{
	static void DefaultConfig(rcConfig& cfg)
	{
		memset(&cfg, 0, sizeof(cfg));

		g_SandboxMgr->DefaultConfig(cfg);
	}

	static void EnsureConfig(rcConfig& cfg)
	{
		if (cfg.cs <= 0.0f || cfg.ch <= 0.0f)
		{
			DefaultConfig(cfg);
			return;
		}

		if (cfg.walkableHeight <= 0)
			cfg.walkableHeight = static_cast<int>(ceilf(2.0f / cfg.ch));
		if (cfg.walkableClimb < 0)
			cfg.walkableClimb = static_cast<int>(floorf(0.9f / cfg.ch));
		if (cfg.walkableRadius < 0)
			cfg.walkableRadius = static_cast<int>(ceilf(0.6f / cfg.cs));
		if (cfg.maxEdgeLen <= 0)
			cfg.maxEdgeLen = static_cast<int>(20.0f / cfg.cs);
		if (cfg.maxSimplificationError <= 0.0f)
			cfg.maxSimplificationError = 1.0f;
		if (cfg.minRegionArea <= 0)
			cfg.minRegionArea = static_cast<int>(powf(8.0f, 2));
		if (cfg.mergeRegionArea <= 0)
			cfg.mergeRegionArea = static_cast<int>(powf(20.0f, 2));
		if (cfg.maxVertsPerPoly <= 0)
			cfg.maxVertsPerPoly = 3;
		if (cfg.detailSampleDist <= 0.0f)
			cfg.detailSampleDist = 5.0f * cfg.cs;
		if (cfg.detailSampleMaxError <= 0.0f)
			cfg.detailSampleMaxError = 1.0f * cfg.ch;
		if (cfg.walkableSlopeAngle <= 0.0f)
			cfg.walkableSlopeAngle = 45.0f;
	}

	static bool SetBoundsFromObjects(rcConfig& cfg, const std::vector<BlockObject*>& objects)
	{
		bool hasBounds = false;
		Ogre::Vector3 minv(1e9f, 1e9f, 1e9f);
		Ogre::Vector3 maxv(-1e9f, -1e9f, -1e9f);

		for (size_t i = 0; i < objects.size(); ++i)
		{
			BlockObject* obj = objects[i];
			if (!obj)
				continue;

			Ogre::SceneNode* node = obj->GetSceneNode();
			if (!node)
				continue;

			const Ogre::AxisAlignedBox aabb = node->_getWorldAABB();
			if (aabb.isNull() || !aabb.isFinite())
				continue;

			if (!hasBounds)
			{
				minv = aabb.getMinimum();
				maxv = aabb.getMaximum();
				hasBounds = true;
			}
			else
			{
				minv.makeFloor(aabb.getMinimum());
				maxv.makeCeil(aabb.getMaximum());
			}
		}

		if (!hasBounds)
			return false;

		const Ogre::Vector3 padding(cfg.cs * 2.0f, cfg.ch * 2.0f, cfg.cs * 2.0f);
		minv -= padding;
		maxv += padding;

		cfg.bmin[0] = minv.x;
		cfg.bmin[1] = minv.y;
		cfg.bmin[2] = minv.z;
		cfg.bmax[0] = maxv.x;
		cfg.bmax[1] = maxv.y;
		cfg.bmax[2] = maxv.z;

		rcCalcGridSize(cfg.bmin, cfg.bmax, cfg.cs, &cfg.width, &cfg.height);
		return (cfg.width > 0 && cfg.height > 0);
	}

	static bool FindNearestPoly(const dtNavMeshQuery& query,
		const Ogre::Vector3& point,
		dtPolyRef& outPoly,
		float outNearest[3])
	{
		const float center[3] = { point.x, point.y, point.z };
		const float extents[3] = { 2.0f, 5.0f, 2.0f };
		dtQueryFilter filter;

		const dtStatus status = query.findNearestPoly(center, extents, &filter, &outPoly, outNearest);
		return dtStatusSucceed(status) && outPoly != 0;
	}

	static float NavRand()
	{
		return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}

	static Ogre::ColourValue AreaColor(unsigned char area)
	{
		if (area == RC_WALKABLE_AREA)
			return Ogre::ColourValue(0.1f, 0.75f, 0.95f, 0.40f);

		const float c = static_cast<float>(area) / 255.0f;
		return Ogre::ColourValue(c, 0.5f * c + 0.2f, 1.0f - 0.5f * c, 0.35f);
	}

	static Ogre::ManualObject* BuildDebugManualObject(Ogre::SceneManager& sceneMgr, const dtNavMesh& navMesh)
	{
		Ogre::ManualObject* manualObj = sceneMgr.createManualObject();
		if (!manualObj)
			return NULL;

		manualObj->setCastShadows(false);

		manualObj->begin("debug_draw", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		for (int ti = 0; ti < navMesh.getMaxTiles(); ++ti)
		{
			const dtMeshTile* tile = navMesh.getTile(ti);
			if (!tile || !tile->header)
				continue;

			NavigationMesh::DrawMeshTile(*manualObj, navMesh, *tile);
		}
		manualObj->end();

		manualObj->begin("debug_opaque_draw", Ogre::RenderOperation::OT_LINE_LIST);
		for (int ti = 0; ti < navMesh.getMaxTiles(); ++ti)
		{
			const dtMeshTile* tile = navMesh.getTile(ti);
			if (!tile || !tile->header)
				continue;

			NavigationMesh::DrawMeshOutline(*manualObj, navMesh, *tile);
		}
		manualObj->end();

		return manualObj;
	}
}

void NavigationMesh::DrawMeshTile(Ogre::ManualObject& manualObject, const dtNavMesh& navMesh, const dtMeshTile& tile)
{
	(void)navMesh;
	if (!tile.header) return;

	for (int pi = 0; pi < tile.header->polyCount; ++pi)
	{
		const dtPoly* poly = &tile.polys[pi];
		if (!poly || poly->vertCount < 3)
			continue;
		if (poly->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)
			continue;

		const dtPolyDetail* pd = &tile.detailMeshes[pi];
		if (!pd || pd->triCount <= 0)
			continue;

		const Ogre::ColourValue color = AreaColor(poly->getArea());

		for (int tri = 0; tri < pd->triCount; ++tri)
		{
			const unsigned int index = static_cast<unsigned int>(manualObject.getCurrentVertexCount());
			const unsigned char* t = &tile.detailTris[(pd->triBase + tri) * 4];

			for (int corner = 0; corner < 3; ++corner)
			{
				const float* vertex = 0;
				if (t[corner] < poly->vertCount)
					vertex = &tile.verts[poly->verts[t[corner]] * 3];
				else
					vertex = &tile.detailVerts[(pd->vertBase + t[corner] - poly->vertCount) * 3];

				manualObject.position(vertex[0], vertex[1], vertex[2]);
				manualObject.colour(color);
			}

			manualObject.triangle(index, index + 1, index + 2);
		}
	}
}

void NavigationMesh::DrawMeshOutline(Ogre::ManualObject& manualObject, const dtNavMesh& navMesh, const dtMeshTile& tile)
{
	(void)navMesh;
	if (!tile.header)
		return;

	// 外边界颜色
	const Ogre::ColourValue boundaryColor(
		0.0f / 255.0f,
		128.0f / 255.0f,
		128.0f / 255.0f);

	// 内部连接颜色
	const Ogre::ColourValue internalColor(
		0.0f / 255.0f,
		150.0f / 255.0f,
		150.0f / 255.0f);
	const float yOffset = 0.08f;
	std::unordered_set<std::uint64_t> internalEdgeKeys;

	for (int pi = 0; pi < tile.header->polyCount; ++pi)
	{
		const dtPoly* poly = &tile.polys[pi];
		if (!poly || poly->vertCount < 2)
			continue;
		if (poly->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)
			continue;

		const int polyVertCount = poly->vertCount;
		const dtPolyDetail* pd = &tile.detailMeshes[pi];
		if (!pd || pd->triCount <= 0)
			continue;

		// Draw detail edges that map to original polygon edges.
		for (int tri = 0; tri < pd->triCount; ++tri)
		{
			const unsigned char* t = &tile.detailTris[(pd->triBase + tri) * 4];
			const float* tv[3];
			for (int corner = 0; corner < 3; ++corner)
			{
				if (t[corner] < polyVertCount)
					tv[corner] = &tile.verts[poly->verts[t[corner]] * 3];
				else
					tv[corner] = &tile.detailVerts[(pd->vertBase + (t[corner] - polyVertCount)) * 3];
			}

			for (int m = 0, n = 2; m < 3; n = m++)
			{
				const unsigned char edgeFlags = static_cast<unsigned char>((t[3] >> (n * 2)) & 0x3);
				if (edgeFlags == 0)
					continue; // Skip inner detail edges.

				if (t[n] >= polyVertCount || t[m] >= polyVertCount)
					continue; // Not an edge on original polygon vertices.

				const int a = static_cast<int>(t[n]);
				const int b = static_cast<int>(t[m]);
				int polyEdge = -1;
				if (((a + 1) % polyVertCount) == b)
					polyEdge = a;
				else if (((b + 1) % polyVertCount) == a)
					polyEdge = b;

				if (polyEdge < 0)
					continue;

				const bool isBoundary = (poly->neis[polyEdge] == 0);
				if (!isBoundary)
				{
					const unsigned int ia = static_cast<unsigned int>(poly->verts[a]);
					const unsigned int ib = static_cast<unsigned int>(poly->verts[b]);
					const unsigned int lo = (ia < ib) ? ia : ib;
					const unsigned int hi = (ia < ib) ? ib : ia;
					const std::uint64_t edgeKey = (static_cast<std::uint64_t>(lo) << 32) | static_cast<std::uint64_t>(hi);
					if (!internalEdgeKeys.insert(edgeKey).second)
						continue; // Shared edge already drawn by neighbor polygon.
				}

				const Ogre::ColourValue& lineColor = isBoundary ? boundaryColor : internalColor;
				manualObject.position(tv[n][0], tv[n][1] + yOffset, tv[n][2]);
				manualObject.colour(lineColor);
				manualObject.position(tv[m][0], tv[m][1] + yOffset, tv[m][2]);
				manualObject.colour(lineColor);
			}
		}
	}
}
NavigationMesh::NavigationMesh(const rcConfig& config, const std::vector<BlockObject*> blocks)
	: m_navMesh(NULL)
	, m_navQuery(NULL)
	, m_debugNode(NULL)
{
	rcConfig buildCfg = config;
	//EnsureConfig(buildCfg);
	if (!SetBoundsFromObjects(buildCfg, blocks))
		return;

	NavBuilder builder;
	if (!builder.Build(buildCfg, blocks, m_navMesh, m_navQuery))
	{
		m_navMesh = NULL;
		m_navQuery = NULL;
		return;
	}

	RebuildDebugVisual();
	SetDebugVisible(true);
}

NavigationMesh::~NavigationMesh()
{
	DestroyDebugVisual();
	NavBuilder::DestroyNavMeshQuery(m_navQuery);
	NavBuilder::DestroyNavMesh(m_navMesh);
}

bool NavigationMesh::IsValid() const
{
	return m_navMesh != NULL && m_navQuery != NULL;
}

Ogre::Vector3 NavigationMesh::FindClosestPoint(const Ogre::Vector3& point) const
{
	if (!m_navQuery)
		return point;

	dtPolyRef poly = 0;
	float nearest[3] = { point.x, point.y, point.z };
	if (!FindNearestPoly(*m_navQuery, point, poly, nearest))
		return point;

	return Ogre::Vector3(nearest[0], nearest[1], nearest[2]);
}

bool NavigationMesh::FindPath(const Ogre::Vector3& start,
	const Ogre::Vector3& end,
	std::vector<Ogre::Vector3>& outPath) const
{
	outPath.clear();
	if (!m_navQuery)
		return false;

	dtPolyRef startPoly = 0;
	dtPolyRef endPoly = 0;
	float startNearest[3] = { start.x, start.y, start.z };
	float endNearest[3] = { end.x, end.y, end.z };

	if (!FindNearestPoly(*m_navQuery, start, startPoly, startNearest))
		return false;
	if (!FindNearestPoly(*m_navQuery, end, endPoly, endNearest))
		return false;

	dtQueryFilter filter;
	const int kMaxPath = 256;
	dtPolyRef path[kMaxPath];
	int pathCount = 0;

	dtStatus status = m_navQuery->findPath(
		startPoly,
		endPoly,
		startNearest,
		endNearest,
		&filter,
		path,
		&pathCount,
		kMaxPath);

	if (dtStatusFailed(status) || pathCount <= 0)
		return false;

	const int kMaxStraightPath = 512;
	float straightPath[kMaxStraightPath * 3];
	unsigned char straightFlags[kMaxStraightPath];
	dtPolyRef straightRefs[kMaxStraightPath];
	int straightCount = 0;

	status = m_navQuery->findStraightPath(
		startNearest,
		endNearest,
		path,
		pathCount,
		straightPath,
		straightFlags,
		straightRefs,
		&straightCount,
		kMaxStraightPath,
		DT_STRAIGHTPATH_ALL_CROSSINGS);

	if (dtStatusFailed(status) || straightCount <= 0)
		return false;

	outPath.reserve(static_cast<size_t>(straightCount));
	for (int i = 0; i < straightCount; ++i)
	{
		outPath.push_back(Ogre::Vector3(
			straightPath[i * 3],
			straightPath[i * 3 + 1],
			straightPath[i * 3 + 2]));
	}

	return true;
}

Ogre::Vector3 NavigationMesh::RandomPoint() const
{
	if (!m_navQuery)
		return Ogre::Vector3::ZERO;

	dtQueryFilter filter;
	dtPolyRef randomPoly = 0;
	float randomPt[3] = { 0.0f, 0.0f, 0.0f };

	const dtStatus status = m_navQuery->findRandomPoint(&filter, NavRand, &randomPoly, randomPt);
	if (dtStatusFailed(status) || randomPoly == 0)
		return Ogre::Vector3::ZERO;

	return Ogre::Vector3(randomPt[0], randomPt[1], randomPt[2]);
}

bool NavigationMesh::RebuildDebugVisual()
{
	DestroyDebugVisual();

	if (!m_navMesh)
		return false;

	ClientManager* clientMgr = GetClientMgr();
	if (!clientMgr)
		return false;

	Ogre::SceneManager* sceneMgr = clientMgr->getSceneManager();
	if (!sceneMgr)
		return false;

	Ogre::ManualObject* manual = BuildDebugManualObject(*sceneMgr, *m_navMesh);
	if (!manual)
		return false;

	static unsigned int s_debugMeshCounter = 0;
	m_debugMeshName = "NavMeshDebug_" + Ogre::StringConverter::toString(++s_debugMeshCounter);

	Ogre::MeshPtr debugMesh = manual->convertToMesh(m_debugMeshName);
	sceneMgr->destroyManualObject(manual);

	Ogre::Entity* debugEntity = sceneMgr->createEntity(debugMesh);
	debugEntity->setCastShadows(false);

	m_debugNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
	m_debugNode->attachObject(debugEntity);
	m_debugNode->setVisible(false);
	m_debugNode->_updateBounds();

	return true;
}

bool NavigationMesh::HasDebugVisual() const
{
	return m_debugNode != NULL;
}

void NavigationMesh::SetDebugVisible(bool visible)
{
	if (m_debugNode)
	{
		m_debugNode->setVisible(visible);
	}
}

Ogre::SceneNode* NavigationMesh::GetDebugNode() const
{
	return m_debugNode;
}

void NavigationMesh::DestroyDebugVisual()
{
	if (!m_debugNode)
		return;

	Ogre::SceneManager* sceneMgr = m_debugNode->getCreator();
	if (sceneMgr)
	{
		while (m_debugNode->numAttachedObjects() > 0)
		{
			Ogre::MovableObject* obj = m_debugNode->getAttachedObject(static_cast<unsigned short>(0));
			m_debugNode->detachObject(static_cast<unsigned short>(0));
			if (obj && obj->getMovableType() == Ogre::EntityFactory::FACTORY_TYPE_NAME)
			{
				sceneMgr->destroyEntity(static_cast<Ogre::Entity*>(obj));
			}
			else if (obj)
			{
				sceneMgr->destroyMovableObject(obj);
			}
		}

		sceneMgr->destroySceneNode(m_debugNode);
	}

	m_debugNode = NULL;

	if (!m_debugMeshName.empty())
	{
		Ogre::MeshManager::getSingleton().remove(m_debugMeshName);
		m_debugMeshName.clear();
	}
}



