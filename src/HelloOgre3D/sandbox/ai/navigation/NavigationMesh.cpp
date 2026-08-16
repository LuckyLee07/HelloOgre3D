#include "NavigationMesh.h"
#include "object/BaseObject.h"
#include "systems/manager/ObjectManager.h"
#include "recast/include/Recast.h"
#include "detour/include/DetourNavMesh.h"
#include "detour/include/DetourNavMeshQuery.h"
#include "objects/BlockObject.h"
#include "NavBuilder.h"
#include "systems/service/SceneFactory.h"
#include "Ogre.h"
#include <cerrno>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unordered_set>
#include "profiling/Profile.h"

#if defined(_WIN32)
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

namespace
{
	const std::uint32_t NAV_CACHE_MAGIC = UINT32_C(0x48334E56); // H3NV
	const std::uint32_t NAV_CACHE_VERSION = 1;
	const std::uint32_t NAV_CACHE_MAX_DATA_SIZE = 64 * 1024 * 1024;
	const float NAV_BOUNDS_PADDING = 2.0f;

	bool IsFalseEnvValue(const char* value)
	{
		return value != NULL &&
			(std::strcmp(value, "0") == 0 ||
			 std::strcmp(value, "false") == 0 ||
			 std::strcmp(value, "off") == 0 ||
			 std::strcmp(value, "no") == 0);
	}

	bool IsNavMeshCacheEnabled()
	{
		return !IsFalseEnvValue(std::getenv("HELLO_NAVMESH_CACHE"));
	}

	bool MakeDirectory(const std::string& path)
	{
#if defined(_WIN32)
		const int result = _mkdir(path.c_str());
#else
		const int result = mkdir(path.c_str(), 0755);
#endif
		return result == 0 || errno == EEXIST;
	}

	bool ResolveNavMeshCacheDirectory(std::string& outDirectory)
	{
		const char* overrideDirectory = std::getenv("HELLO_NAVMESH_CACHE_DIR");
		if (overrideDirectory != NULL && overrideDirectory[0] != '\0')
		{
			outDirectory = overrideDirectory;
			return MakeDirectory(outDirectory);
		}

		if (!MakeDirectory("../tmp"))
			return false;
		outDirectory = "../tmp/navmesh-cache";
		return MakeDirectory(outDirectory);
	}

	std::string BuildNavMeshCachePath(const std::string& directory, std::uint64_t fingerprint)
	{
		std::ostringstream stream;
		stream << directory << "/nav_"
			<< std::hex << std::setw(16) << std::setfill('0') << fingerprint
			<< ".bin";
		return stream.str();
	}

	template <typename T>
	bool ReadCacheValue(std::ifstream& stream, T& value)
	{
		stream.read(reinterpret_cast<char*>(&value), sizeof(value));
		return stream.good();
	}

	template <typename T>
	bool WriteCacheValue(std::ofstream& stream, const T& value)
	{
		stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
		return stream.good();
	}

	bool LoadNavMeshCache(const std::string& path,
		std::uint64_t expectedFingerprint,
		dtNavMesh*& outNavMesh,
		dtNavMeshQuery*& outQuery)
	{
		outNavMesh = NULL;
		outQuery = NULL;
		std::ifstream stream(path.c_str(), std::ios::in | std::ios::binary);
		if (!stream.is_open())
			return false;

		std::uint32_t magic = 0;
		std::uint32_t cacheVersion = 0;
		std::uint32_t detourVersion = 0;
		std::uint64_t fingerprint = 0;
		std::uint32_t dataSize = 0;
		if (!ReadCacheValue(stream, magic) ||
			!ReadCacheValue(stream, cacheVersion) ||
			!ReadCacheValue(stream, detourVersion) ||
			!ReadCacheValue(stream, fingerprint) ||
			!ReadCacheValue(stream, dataSize) ||
			magic != NAV_CACHE_MAGIC ||
			cacheVersion != NAV_CACHE_VERSION ||
			detourVersion != static_cast<std::uint32_t>(DT_NAVMESH_VERSION) ||
			fingerprint != expectedFingerprint ||
			dataSize == 0 || dataSize > NAV_CACHE_MAX_DATA_SIZE)
		{
			return false;
		}

		unsigned char* navData = static_cast<unsigned char*>(dtAlloc(dataSize, DT_ALLOC_PERM));
		if (navData == NULL)
			return false;

		stream.read(reinterpret_cast<char*>(navData), dataSize);
		if (!stream.good())
		{
			dtFree(navData);
			return false;
		}

		dtNavMesh* navMesh = dtAllocNavMesh();
		if (navMesh == NULL)
		{
			dtFree(navData);
			return false;
		}

		const dtStatus initStatus = navMesh->init(navData, static_cast<int>(dataSize), DT_TILE_FREE_DATA);
		if (dtStatusFailed(initStatus))
		{
			dtFree(navData);
			dtFreeNavMesh(navMesh);
			return false;
		}

		NavBuilder builder;
		dtNavMeshQuery* query = NULL;
		if (!builder.BuildDetourQuery(*navMesh, query))
		{
			dtFreeNavMesh(navMesh);
			return false;
		}

		outNavMesh = navMesh;
		outQuery = query;
		return true;
	}

	bool SaveNavMeshCache(const std::string& path,
		std::uint64_t fingerprint,
		const dtNavMesh& navMesh)
	{
		const dtMeshTile* tile = NULL;
		int validTileCount = 0;
		for (int tileIndex = 0; tileIndex < navMesh.getMaxTiles(); ++tileIndex)
		{
			const dtMeshTile* candidate = navMesh.getTile(tileIndex);
			if (candidate != NULL && candidate->header != NULL &&
				candidate->data != NULL && candidate->dataSize > 0)
			{
				tile = candidate;
				++validTileCount;
			}
		}
		if (validTileCount != 1 || tile == NULL ||
			static_cast<std::uint32_t>(tile->dataSize) > NAV_CACHE_MAX_DATA_SIZE)
			return false;

		const std::string tempPath = path + ".tmp";
		std::ofstream stream(tempPath.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
		if (!stream.is_open())
			return false;

		const std::uint32_t detourVersion = static_cast<std::uint32_t>(DT_NAVMESH_VERSION);
		const std::uint32_t dataSize = static_cast<std::uint32_t>(tile->dataSize);
		const bool headerWritten =
			WriteCacheValue(stream, NAV_CACHE_MAGIC) &&
			WriteCacheValue(stream, NAV_CACHE_VERSION) &&
			WriteCacheValue(stream, detourVersion) &&
			WriteCacheValue(stream, fingerprint) &&
			WriteCacheValue(stream, dataSize);
		if (headerWritten)
			stream.write(reinterpret_cast<const char*>(tile->data), tile->dataSize);
		stream.close();
		if (!headerWritten || stream.fail())
		{
			std::remove(tempPath.c_str());
			return false;
		}

		std::remove(path.c_str());
		if (std::rename(tempPath.c_str(), path.c_str()) != 0)
		{
			std::remove(tempPath.c_str());
			return false;
		}
		return true;
	}

	double ElapsedMilliseconds(const std::chrono::steady_clock::time_point& start)
	{
		return std::chrono::duration<double, std::milli>(
			std::chrono::steady_clock::now() - start).count();
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

bool NavigationMesh::GetWalkableTriangles(std::vector<float>& outVerts, std::vector<int>& outIndices) const
{
	outVerts.clear();
	outIndices.clear();
	if (m_navMesh == NULL)
		return false;

	// 通过 const 指针调用，命中 dtNavMesh 的 public const 版 getTile/getMaxTiles。
	const dtNavMesh* const navMesh = m_navMesh;

	// 与 DrawMeshTile 相同的 detail-mesh 三角遍历，但收集到顶点/索引缓冲而非绘制。
	for (int ti = 0; ti < navMesh->getMaxTiles(); ++ti)
	{
		const dtMeshTile* tile = navMesh->getTile(ti);
		if (!tile || !tile->header)
			continue;

		for (int pi = 0; pi < tile->header->polyCount; ++pi)
		{
			const dtPoly* poly = &tile->polys[pi];
			if (!poly || poly->vertCount < 3)
				continue;
			if (poly->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)
				continue;

			const dtPolyDetail* pd = &tile->detailMeshes[pi];
			if (!pd || pd->triCount <= 0)
				continue;

			for (int tri = 0; tri < pd->triCount; ++tri)
			{
				const unsigned char* td = &tile->detailTris[(pd->triBase + tri) * 4];
				for (int corner = 0; corner < 3; ++corner)
				{
					const float* vertex = 0;
					if (td[corner] < poly->vertCount)
						vertex = &tile->verts[poly->verts[td[corner]] * 3];
					else
						vertex = &tile->detailVerts[(pd->vertBase + td[corner] - poly->vertCount) * 3];

					outIndices.push_back(static_cast<int>(outVerts.size() / 3));
					outVerts.push_back(vertex[0]);
					outVerts.push_back(vertex[1]);
					outVerts.push_back(vertex[2]);
				}
			}
		}
	}

	return !outIndices.empty();
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

		for (int e = 0; e < poly->vertCount; ++e)
		{
			const int ne = (e + 1) % poly->vertCount;
			const bool isBoundary = (poly->neis[e] == 0);
			if (!isBoundary)
			{
				const unsigned int ia = static_cast<unsigned int>(poly->verts[e]);
				const unsigned int ib = static_cast<unsigned int>(poly->verts[ne]);
				const unsigned int lo = (ia < ib) ? ia : ib;
				const unsigned int hi = (ia < ib) ? ib : ia;
				const std::uint64_t edgeKey = (static_cast<std::uint64_t>(lo) << 32) | static_cast<std::uint64_t>(hi);
				if (!internalEdgeKeys.insert(edgeKey).second)
					continue; // Shared edge already drawn by neighbor polygon.
			}

			const float* pa = &tile.verts[poly->verts[e] * 3];
			const float* pb = &tile.verts[poly->verts[ne] * 3];
			const Ogre::ColourValue& lineColor = isBoundary ? boundaryColor : internalColor;
			manualObject.position(pa[0], pa[1] + yOffset, pa[2]);
			manualObject.colour(lineColor);
			manualObject.position(pb[0], pb[1] + yOffset, pb[2]);
			manualObject.colour(lineColor);
		}
	}
}

NavigationMesh::NavigationMesh(const rcConfig& config, const std::vector<BlockObject*> blocks)
	: m_navMesh(NULL)
	, m_navQuery(NULL)
	, m_debugNode(NULL)
{
	const std::chrono::steady_clock::time_point totalStart = std::chrono::steady_clock::now();
	rcConfig buildCfg = config;
	NavBuilder builder;
	const int legacyWidth = buildCfg.width;
	const int legacyHeight = buildCfg.height;
	const bool fittedBounds = builder.FitBoundsToObjects(buildCfg, blocks, NAV_BOUNDS_PADDING);
	const std::uint64_t fingerprint = builder.ComputeInputFingerprint(buildCfg, blocks);

	std::string cacheDirectory;
	std::string cachePath;
	const bool cacheEnabled = IsNavMeshCacheEnabled() && ResolveNavMeshCacheDirectory(cacheDirectory);
	if (cacheEnabled)
		cachePath = BuildNavMeshCachePath(cacheDirectory, fingerprint);

	const std::chrono::steady_clock::time_point loadStart = std::chrono::steady_clock::now();
	const bool cacheHit = cacheEnabled && LoadNavMeshCache(cachePath, fingerprint, m_navMesh, m_navQuery);
	const double cacheLoadMs = ElapsedMilliseconds(loadStart);
	const std::chrono::steady_clock::time_point buildStart = std::chrono::steady_clock::now();
	if (!cacheHit && !builder.Build(buildCfg, blocks, m_navMesh, m_navQuery))
	{
		m_navMesh = NULL;
		m_navQuery = NULL;
		return;
	}
	const double buildMs = cacheHit ? 0.0 : ElapsedMilliseconds(buildStart);
	const bool cacheSaved = !cacheHit && cacheEnabled && m_navMesh != NULL &&
		SaveNavMeshCache(cachePath, fingerprint, *m_navMesh);

	RebuildDebugVisual();
	SetDebugVisible(true);

	std::ostringstream log;
	log << "[NavigationMesh] cache=" << (cacheHit ? "hit" : "miss")
		<< " saved=" << (cacheSaved ? "true" : "false")
		<< " fitted=" << (fittedBounds ? "true" : "false")
		<< " grid=" << legacyWidth << "x" << legacyHeight
		<< "->" << buildCfg.width << "x" << buildCfg.height
		<< " fingerprint=" << std::hex << fingerprint << std::dec
		<< std::fixed << std::setprecision(2)
		<< " cacheLoadMs=" << cacheLoadMs
		<< " buildMs=" << buildMs
		<< " totalMs=" << ElapsedMilliseconds(totalStart);
	Ogre::LogManager::getSingleton().logMessage(log.str());
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
	H3D_PROFILE_SCOPE("NavigationMesh::FindClosestPoint");
	if (!m_navQuery)
		return point;

	dtPolyRef poly = 0;
	float nearest[3] = { point.x, point.y, point.z };
	if (!FindNearestPoly(*m_navQuery, point, poly, nearest))
		return point;

	return Ogre::Vector3(nearest[0], nearest[1], nearest[2]);
}

bool NavigationMesh::FindPath(const Ogre::Vector3& start, const Ogre::Vector3& end,
	std::vector<Ogre::Vector3>& outPath) const
{
	H3D_PROFILE_SCOPE("NavigationMesh::FindPath");
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
	if (path[pathCount - 1] != endPoly)
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
	H3D_PROFILE_SCOPE("NavigationMesh::RandomPoint");
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

	Ogre::SceneManager* sceneMgr = SceneFactory::GetSceneManager();
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

