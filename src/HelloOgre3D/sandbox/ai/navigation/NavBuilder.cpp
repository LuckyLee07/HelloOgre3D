#include "NavBuilder.h"
#include "detour/include/DetourNavMeshBuilder.h"
#include "objects/BlockObject.h"
#include "Ogre.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

namespace
{
	const std::uint64_t FNV_OFFSET_BASIS = UINT64_C(14695981039346656037);
	const std::uint64_t FNV_PRIME = UINT64_C(1099511628211);

	void HashBytes(std::uint64_t& hash, const void* data, size_t size)
	{
		const unsigned char* bytes = static_cast<const unsigned char*>(data);
		for (size_t index = 0; index < size; ++index)
		{
			hash ^= static_cast<std::uint64_t>(bytes[index]);
			hash *= FNV_PRIME;
		}
	}

	template <typename T>
	void HashValue(std::uint64_t& hash, const T& value)
	{
		HashBytes(hash, &value, sizeof(value));
	}

	void HashConfig(std::uint64_t& hash, const rcConfig& cfg)
	{
		HashValue(hash, cfg.width);
		HashValue(hash, cfg.height);
		HashValue(hash, cfg.tileSize);
		HashValue(hash, cfg.borderSize);
		HashValue(hash, cfg.cs);
		HashValue(hash, cfg.ch);
		HashBytes(hash, cfg.bmin, sizeof(cfg.bmin));
		HashBytes(hash, cfg.bmax, sizeof(cfg.bmax));
		HashValue(hash, cfg.walkableSlopeAngle);
		HashValue(hash, cfg.walkableHeight);
		HashValue(hash, cfg.walkableClimb);
		HashValue(hash, cfg.walkableRadius);
		HashValue(hash, cfg.maxEdgeLen);
		HashValue(hash, cfg.maxSimplificationError);
		HashValue(hash, cfg.minRegionArea);
		HashValue(hash, cfg.mergeRegionArea);
		HashValue(hash, cfg.maxVertsPerPoly);
		HashValue(hash, cfg.detailSampleDist);
		HashValue(hash, cfg.detailSampleMaxError);
	}
}

bool NavBuilder::Build(const rcConfig& cfg,
	const std::vector<BlockObject*>& objects,
	dtNavMesh*& outNavMesh,
	dtNavMeshQuery*& outQuery)
{
	outQuery = NULL;
	outNavMesh = NULL;

	rcPolyMesh* pmesh = NULL;
	rcPolyMeshDetail* dmesh = NULL;

	if (!BuildPolyMeshes(cfg, objects, pmesh, dmesh))
	{
		DestroyPolyMesh(pmesh);
		DestroyPolyMeshDetail(dmesh);
		return false;
	}

	dtNavMesh* nav = NULL;
	if (!BuildDetourNavMesh(cfg, *pmesh, *dmesh, nav))
	{
		DestroyPolyMesh(pmesh);
		DestroyPolyMeshDetail(dmesh);
		DestroyNavMesh(nav);
		return false;
	}

	dtNavMeshQuery* query = NULL;
	if (!BuildDetourQuery(*nav, query))
	{
		DestroyPolyMesh(pmesh);
		DestroyPolyMeshDetail(dmesh);
		DestroyNavMesh(nav);
		DestroyNavMeshQuery(query);
		return false;
	}

	// 中间产物释放
	DestroyPolyMesh(pmesh);
	DestroyPolyMeshDetail(dmesh);

	outNavMesh = nav;
	outQuery = query;
	return true;
}

bool NavBuilder::FitBoundsToObjects(rcConfig& cfg,
	const std::vector<BlockObject*>& objects,
	float padding)
{
	float boundsMin[3] = {
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max()
	};
	float boundsMax[3] = {
		-std::numeric_limits<float>::max(),
		-std::numeric_limits<float>::max(),
		-std::numeric_limits<float>::max()
	};
	bool foundGeometry = false;
	std::vector<float> verts;
	std::vector<int> tris;

	for (size_t objectIndex = 0; objectIndex < objects.size(); ++objectIndex)
	{
		BlockObject* object = objects[objectIndex];
		if (object == NULL || !ExtractTriangleSoup(*object, verts, tris))
			continue;

		for (size_t vertexIndex = 0; vertexIndex + 2 < verts.size(); vertexIndex += 3)
		{
			const float x = verts[vertexIndex];
			const float y = verts[vertexIndex + 1];
			const float z = verts[vertexIndex + 2];
			if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z))
				continue;

			boundsMin[0] = std::min(boundsMin[0], x);
			boundsMin[1] = std::min(boundsMin[1], y);
			boundsMin[2] = std::min(boundsMin[2], z);
			boundsMax[0] = std::max(boundsMax[0], x);
			boundsMax[1] = std::max(boundsMax[1], y);
			boundsMax[2] = std::max(boundsMax[2], z);
			foundGeometry = true;
		}
	}

	if (!foundGeometry)
		return false;

	const float safePadding = std::max(padding,
		static_cast<float>(cfg.walkableRadius) * cfg.cs + cfg.cs * 2.0f);
	for (int axis = 0; axis < 3; ++axis)
	{
		cfg.bmin[axis] = boundsMin[axis] - safePadding;
		cfg.bmax[axis] = boundsMax[axis] + safePadding;
	}
	rcCalcGridSize(cfg.bmin, cfg.bmax, cfg.cs, &cfg.width, &cfg.height);
	return cfg.width > 0 && cfg.height > 0;
}

std::uint64_t NavBuilder::ComputeInputFingerprint(const rcConfig& cfg,
	const std::vector<BlockObject*>& objects)
{
	std::uint64_t hash = FNV_OFFSET_BASIS;
	HashConfig(hash, cfg);

	std::vector<std::uint64_t> objectHashes;
	std::vector<float> verts;
	std::vector<int> tris;
	for (size_t objectIndex = 0; objectIndex < objects.size(); ++objectIndex)
	{
		BlockObject* object = objects[objectIndex];
		if (object == NULL || !ExtractTriangleSoup(*object, verts, tris))
			continue;

		std::uint64_t objectHash = FNV_OFFSET_BASIS;
		const std::uint64_t vertexCount = static_cast<std::uint64_t>(verts.size());
		const std::uint64_t indexCount = static_cast<std::uint64_t>(tris.size());
		HashValue(objectHash, vertexCount);
		HashValue(objectHash, indexCount);
		if (!verts.empty())
			HashBytes(objectHash, &verts[0], verts.size() * sizeof(float));
		if (!tris.empty())
			HashBytes(objectHash, &tris[0], tris.size() * sizeof(int));
		objectHashes.push_back(objectHash);
	}

	std::sort(objectHashes.begin(), objectHashes.end());
	const std::uint64_t objectCount = static_cast<std::uint64_t>(objectHashes.size());
	HashValue(hash, objectCount);
	if (!objectHashes.empty())
		HashBytes(hash, &objectHashes[0], objectHashes.size() * sizeof(std::uint64_t));

	return hash;
}

bool NavBuilder::BuildPolyMeshes(const rcConfig& cfg,
	const std::vector<BlockObject*>& objects,
	rcPolyMesh*& outPolyMesh,
	rcPolyMeshDetail*& outPolyMeshDetail)
{
	outPolyMesh = NULL;
	outPolyMeshDetail = NULL;

	// 1) heightfield
	rcHeightfield* hf = CreateHeightfield(cfg);
	if (!hf) return false;
	
	// 2) rasterize all objects into hf
	if (!RasterizeObjects(cfg, objects, *hf))
	{
		DestroyHeightfield(hf);
		return false;
	}

	// 3) filter
	FilterWalkableSurfaces(cfg, *hf);

	// 4) compact heightfield
	rcCompactHeightfield* chf = CreateCompactHeightfield(cfg, *hf);
	if (!chf)
	{
		DestroyHeightfield(hf);
		return false;
	}

	// 5) erode
	if (!ErodeWalkableArea(cfg, *chf))
	{
		DestroyHeightfield(hf);
		DestroyCompactHeightfield(chf);
		return false;
	}

	// 6) distance field
	if (!BuildDistanceField(cfg, *chf))
	{
		DestroyHeightfield(hf);
		DestroyCompactHeightfield(chf);
		return false;
	}

	// 7) regions
	if (!BuildRegions(cfg, *chf))
	{
		DestroyHeightfield(hf);
		DestroyCompactHeightfield(chf);
		return false;
	}

	// 8) contours
	rcContourSet* cset = CreateContourSet(cfg, *chf);
	if (!cset)
	{
		DestroyHeightfield(hf);
		DestroyCompactHeightfield(chf);
		return false;
	}

	// 9) poly mesh
	rcPolyMesh* pmesh = CreatePolyMesh(cfg, *cset);
	if (!pmesh)
	{
		DestroyHeightfield(hf);
		DestroyCompactHeightfield(chf);
		DestroyContourSet(cset);
		return false;
	}

	// 10) detail mesh
	rcPolyMeshDetail* dmesh = CreatePolyMeshDetail(cfg, *chf, *pmesh);
	if (!dmesh)
	{
		DestroyHeightfield(hf);
		DestroyCompactHeightfield(chf);
		DestroyContourSet(cset);
		DestroyPolyMesh(pmesh);
		return false;
	}

	// pipeline 中间产物释放
	DestroyHeightfield(hf);
	DestroyCompactHeightfield(chf);
	DestroyContourSet(cset);

	outPolyMesh = pmesh;
	outPolyMeshDetail = dmesh;
	return true;
}

bool NavBuilder::BuildDetourNavMesh(const rcConfig& cfg,
	rcPolyMesh& polyMesh,
	rcPolyMeshDetail& polyMeshDetail,
	dtNavMesh*& outNavMesh)
{
	outNavMesh = NULL;

	dtNavMeshCreateParams params;
	memset(&params, 0, sizeof(params));

	params.verts = polyMesh.verts;
	params.vertCount = polyMesh.nverts;
	params.polys = polyMesh.polys;
	params.polyAreas = polyMesh.areas;
	params.polyFlags = polyMesh.flags;
	params.polyCount = polyMesh.npolys;
	params.nvp = polyMesh.nvp;

	params.detailMeshes = polyMeshDetail.meshes;
	params.detailVerts = polyMeshDetail.verts;
	params.detailVertsCount = polyMeshDetail.nverts;
	params.detailTris = polyMeshDetail.tris;
	params.detailTriCount = polyMeshDetail.ntris;

	params.walkableHeight = cfg.walkableHeight * cfg.ch;
	params.walkableRadius = cfg.walkableRadius * cfg.cs;
	params.walkableClimb = cfg.walkableClimb * cfg.ch;

	rcVcopy(params.bmin, polyMesh.bmin);
	rcVcopy(params.bmax, polyMesh.bmax);

	params.cs = cfg.cs;
	params.ch = cfg.ch;

	params.buildBvTree = true;

	unsigned char* navData = NULL;
	int navDataSize = 0;

	if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
		return false;

	dtNavMesh* navMesh = dtAllocNavMesh();
	if (!navMesh)
	{
		dtFree(navData);
		return false;
	}

	const dtStatus status = navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
	if (dtStatusFailed(status))
	{
		dtFree(navData);
		dtFreeNavMesh(navMesh);
		return false;
	}

	outNavMesh = navMesh;
	return true;
}

bool NavBuilder::BuildDetourQuery(const dtNavMesh& navMesh,
	dtNavMeshQuery*& outQuery,
	const int maxNodes/* = 2048*/)
{
	outQuery = NULL;

	dtNavMeshQuery* query = dtAllocNavMeshQuery();
	if (!query) return false;

	const dtStatus status = query->init(&navMesh, maxNodes);
	if (dtStatusFailed(status))
	{
		dtFreeNavMeshQuery(query);
		return false;
	}

	outQuery = query;
	return true;
}

// ----------------- Destroy -----------------
void NavBuilder::DestroyPolyMesh(rcPolyMesh*& mesh)
{
	if (mesh)
	{
		rcFreePolyMesh(mesh);
		mesh = NULL;
	}
}

void NavBuilder::DestroyPolyMeshDetail(rcPolyMeshDetail*& mesh)
{
	if (mesh)
	{
		rcFreePolyMeshDetail(mesh);
		mesh = NULL;
	}
}

void NavBuilder::DestroyNavMesh(dtNavMesh*& navMesh)
{
	if (navMesh)
	{
		dtFreeNavMesh(navMesh);
		navMesh = NULL;
	}
}

void NavBuilder::DestroyNavMeshQuery(dtNavMeshQuery*& query)
{
	if (query)
	{
		dtFreeNavMeshQuery(query);
		query = NULL;
	}
}

void NavBuilder::DestroyHeightfield(rcHeightfield*& hf)
{
	if (hf)
	{
		rcFreeHeightField(hf);
		hf = NULL;
	}
}

void NavBuilder::DestroyCompactHeightfield(rcCompactHeightfield*& chf)
{
	if (chf)
	{
		rcFreeCompactHeightfield(chf);
		chf = NULL;
	}
}

void NavBuilder::DestroyContourSet(rcContourSet*& cset)
{
	if (cset)
	{
		rcFreeContourSet(cset);
		cset = NULL;
	}
}

// ----------------- Recast steps -----------------
rcHeightfield* NavBuilder::CreateHeightfield(const rcConfig& cfg)
{
	rcHeightfield* hf = rcAllocHeightfield();
	if (!hf) return NULL;
	
	if (!rcCreateHeightfield(&m_ctx, *hf, 
		cfg.width, cfg.height,
		cfg.bmin, cfg.bmax,
		cfg.cs, cfg.ch))
	{
		rcFreeHeightField(hf);
		return NULL;
	}

	return hf;
}

void NavBuilder::FilterWalkableSurfaces(const rcConfig& cfg, rcHeightfield& hf)
{
	rcFilterLowHangingWalkableObstacles(&m_ctx, cfg.walkableClimb, hf);
	rcFilterLedgeSpans(&m_ctx, cfg.walkableHeight, cfg.walkableClimb, hf);
	rcFilterWalkableLowHeightSpans(&m_ctx, cfg.walkableHeight, hf);
}

rcCompactHeightfield* NavBuilder::CreateCompactHeightfield(const rcConfig& cfg, rcHeightfield& hf)
{
	rcCompactHeightfield* chf = rcAllocCompactHeightfield();
	if (!chf) return NULL;

	if (!rcBuildCompactHeightfield(&m_ctx,
		cfg.walkableHeight, cfg.walkableClimb, hf, *chf))
	{
		rcFreeCompactHeightfield(chf);
		return NULL;
	}

	return chf;
}

bool NavBuilder::ErodeWalkableArea(const rcConfig& cfg, rcCompactHeightfield& chf)
{
	return rcErodeWalkableArea(&m_ctx, cfg.walkableRadius, chf) != 0;
}

bool NavBuilder::BuildDistanceField(const rcConfig&, rcCompactHeightfield& chf)
{
	return rcBuildDistanceField(&m_ctx, chf) != 0;
}

bool NavBuilder::BuildRegions(const rcConfig& cfg, rcCompactHeightfield& chf)
{
	return rcBuildRegions(&m_ctx, chf, 0, cfg.minRegionArea, cfg.mergeRegionArea) != 0;
}

rcContourSet* NavBuilder::CreateContourSet(const rcConfig& cfg, rcCompactHeightfield& chf)
{
	rcContourSet* cset = rcAllocContourSet();
	if (!cset) return NULL;
	
	if (!rcBuildContours(&m_ctx, chf, cfg.maxSimplificationError, cfg.maxEdgeLen, *cset))
	{
		rcFreeContourSet(cset);
		return NULL;
	}

	return cset;
}

rcPolyMesh* NavBuilder::CreatePolyMesh(const rcConfig& cfg, rcContourSet& cset)
{
	rcPolyMesh* pmesh = rcAllocPolyMesh();
	if (!pmesh) return NULL;
	
	if (!rcBuildPolyMesh(&m_ctx, cset, cfg.maxVertsPerPoly, *pmesh))
	{
		rcFreePolyMesh(pmesh);
		return NULL;
	}

	// 默认 flags: 全部可用
	for (int i = 0; i < pmesh->npolys; i++)
		pmesh->flags[i] = 1;

	return pmesh;
}

rcPolyMeshDetail* NavBuilder::CreatePolyMeshDetail(const rcConfig& cfg, rcCompactHeightfield& chf, rcPolyMesh& pmesh)
{
	rcPolyMeshDetail* dmesh = rcAllocPolyMeshDetail();
	if (!dmesh) return NULL;
	
	if (!rcBuildPolyMeshDetail(&m_ctx, pmesh, chf, 
		cfg.detailSampleDist, cfg.detailSampleMaxError, *dmesh))
	{
		rcFreePolyMeshDetail(dmesh);
		return NULL;
	}

	return dmesh;
}

// ----------------- Rasterize -----------------
bool NavBuilder::RasterizeObjects(const rcConfig& cfg,
	const std::vector<BlockObject*>& objects,
	rcHeightfield& hf)
{
	std::vector<float> verts;
	std::vector<int> tris;

	std::vector<unsigned char> areaBuff; // 每个三角一个byte
	for (size_t i = 0; i < objects.size(); ++i)
	{
		if (!objects[i])
			continue;

		verts.clear();
		tris.clear();

		if (!ExtractTriangleSoup(*objects[i], verts, tris))
			continue; // 某些对象没 mesh 则跳过

		if (verts.empty() || tris.empty())
			continue;

		const int vertCount = static_cast<int>(verts.size() / 3);
		const int triCount = static_cast<int>(tris.size() / 3);

		TriangleSoup soup;
		soup.verts = &verts[0];
		soup.vertCount = vertCount;
		soup.tris = &tris[0];
		soup.triCount = triCount;

		areaBuff.resize(triCount);
		memset(&areaBuff[0], 0, static_cast<size_t>(triCount) * sizeof(unsigned char));

		if (!RasterizeTriangleSoup(cfg, soup, hf, areaBuff))
			return false;
	}
	return true;
}

bool NavBuilder::RasterizeTriangleSoup(const rcConfig& cfg,
	const TriangleSoup& soup,
	rcHeightfield& hf,
	std::vector<unsigned char>& areaBuff)
{
	// 1) 标记可走三角形（按坡度）
	rcMarkWalkableTriangles(&m_ctx, cfg.walkableSlopeAngle,
		soup.verts, soup.vertCount,
		soup.tris, soup.triCount, &areaBuff[0]);

	// 2) 光栅化
	// walkableClimb 影响跨越高度处理
	rcRasterizeTriangles(&m_ctx, soup.verts, soup.vertCount, 
		soup.tris, &areaBuff[0], soup.triCount, hf, cfg.walkableClimb);

	return true;
}

bool NavBuilder::ExtractTriangleSoup(const BlockObject& obj,
	std::vector<float>& outVerts,
	std::vector<int>& outTris)
{
	Ogre::Entity* ent = obj.GetEntity();
	if (!ent)
		return false;

	Ogre::MeshPtr mesh = ent->getMesh();
	if (mesh.isNull())
		return false;

	Ogre::SceneNode* node = ent->getParentSceneNode();
	const Ogre::Vector3 worldPos = node ? node->_getDerivedPosition() : Ogre::Vector3::ZERO;
	const Ogre::Quaternion worldOrient = node ? node->_getDerivedOrientation() : Ogre::Quaternion::IDENTITY;
	const Ogre::Vector3 worldScale = node ? node->_getDerivedScale() : Ogre::Vector3::UNIT_SCALE;

	size_t totalVerts = 0;
	size_t totalIdx = 0;
	bool addedShared = false;

	const size_t numSubMeshes = mesh->getNumSubMeshes();
	for (size_t si = 0; si < numSubMeshes; ++si)
	{
		Ogre::SubMesh* sm = mesh->getSubMesh(si);
		if (!sm) continue;

		Ogre::RenderOperation op;
		sm->_getRenderOperation(op);
		if (op.operationType != Ogre::RenderOperation::OT_TRIANGLE_LIST)
			continue;

		Ogre::VertexData* vd = sm->useSharedVertices ? mesh->sharedVertexData : sm->vertexData;
		Ogre::IndexData* id = sm->indexData;
		if (!vd || !id) continue;

		if (sm->useSharedVertices)
		{
			if (!addedShared)
			{
				totalVerts += vd->vertexCount;
				addedShared = true;
			}
		}
		else
		{
			totalVerts += vd->vertexCount;
		}

		totalIdx += id->indexCount;
	}

	if (totalVerts == 0 || totalIdx == 0)
		return false;

	outVerts.clear();
	outTris.clear();
	outVerts.reserve(totalVerts * 3);
	outTris.reserve(totalIdx);

	int currentOffset = 0;
	int sharedOffset = 0;
	int nextOffset = 0;
	int indexOffset = 0;
	addedShared = false;

	for (size_t si = 0; si < numSubMeshes; ++si)
	{
		Ogre::SubMesh* sm = mesh->getSubMesh(si);
		if (!sm) continue;

		Ogre::RenderOperation op;
		sm->_getRenderOperation(op);
		if (op.operationType != Ogre::RenderOperation::OT_TRIANGLE_LIST)
			continue;

		Ogre::VertexData* vd = sm->useSharedVertices ? mesh->sharedVertexData : sm->vertexData;
		Ogre::IndexData* id = sm->indexData;
		if (!vd || !id)
			continue;

		if ((!sm->useSharedVertices) || (sm->useSharedVertices && !addedShared))
		{
			if (sm->useSharedVertices)
			{
				addedShared = true;
				sharedOffset = currentOffset;
			}

			// ----- read positions -----
			const Ogre::VertexElement* posElem = vd->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
			Ogre::HardwareVertexBufferSharedPtr vbuf = vd->vertexBufferBinding->getBuffer(posElem->getSource());
			unsigned char* pVertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

			float* pReal = 0;
			for (size_t v = 0; v < vd->vertexCount; ++v)
			{
				posElem->baseVertexPointerToElement(pVertex, &pReal);

				const Ogre::Vector3 local(pReal[0], pReal[1], pReal[2]);
				const Ogre::Vector3 world = (worldOrient * (local * worldScale)) + worldPos;

				outVerts.push_back(world.x);
				outVerts.push_back(world.y);
				outVerts.push_back(world.z);

				pVertex += vbuf->getVertexSize();
			}
			vbuf->unlock();
			nextOffset += static_cast<int>(vd->vertexCount);
		}

		// ----- read indices -----
		const int vertexOffset = sm->useSharedVertices ? sharedOffset : currentOffset;
		Ogre::HardwareIndexBufferSharedPtr ibuf = id->indexBuffer;
		const bool use32bit = ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT;
		void* idxPtr = ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);

		if (use32bit)
		{
			unsigned int* pIdx = static_cast<unsigned int*>(idxPtr);
			for (size_t k = 0; k < id->indexCount; ++k)
			{
				outTris.push_back(static_cast<int>(pIdx[k]) + vertexOffset);
				++indexOffset;
			}
		}
		else
		{
			unsigned short* pIdx = static_cast<unsigned short*>(idxPtr);
			for (size_t k = 0; k < id->indexCount; ++k)
			{
				outTris.push_back(static_cast<int>(pIdx[k]) + vertexOffset);
				++indexOffset;
			}
		}

		ibuf->unlock();
		currentOffset = nextOffset;
	}

	// 必须是三角形索引
	if (indexOffset == 0 || (outTris.size() % 3) != 0)
		return false;

	return true;
}
