#include "NavBuilder.h"
#include "detour/include/DetourNavMeshBuilder.h"
#include "objects/BlockObject.h"
#include "Ogre.h"

NavBuilder::NavBuilder() : m_ctx()
{

}

NavBuilder::~NavBuilder()
{

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

	const unsigned short numSubMeshes = mesh->getNumSubMeshes();
	for (unsigned short si = 0; si < numSubMeshes; ++si)
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

	int baseVertex = 0;
	int sharedBaseVertex = 0;
	addedShared = false;

	for (unsigned short si = 0; si < numSubMeshes; ++si)
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

		int subMeshBaseVertex = baseVertex;
		const bool needCopyVertices = (!sm->useSharedVertices) || (sm->useSharedVertices && !addedShared);

		if (needCopyVertices)
		{
			if (sm->useSharedVertices)
			{
				addedShared = true;
				sharedBaseVertex = baseVertex;
				subMeshBaseVertex = sharedBaseVertex;
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

			baseVertex += static_cast<int>(vd->vertexCount);
		}
		else
		{
			subMeshBaseVertex = sharedBaseVertex;
		}
		
		// ----- read indices -----
		Ogre::HardwareIndexBufferSharedPtr ibuf = id->indexBuffer;
		const bool use32bit = ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT;

		void* idxPtr = ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);
		if (use32bit)
		{
			unsigned int* pIdx = static_cast<unsigned int*>(idxPtr);
			for (size_t k = 0; k < id->indexCount; ++k)
				outTris.push_back(static_cast<int>(pIdx[k]) + subMeshBaseVertex);
		}
		else
		{
			unsigned short* pIdx = static_cast<unsigned short*>(idxPtr);
			for (size_t k = 0; k < id->indexCount; ++k)
				outTris.push_back(static_cast<int>(pIdx[k]) + subMeshBaseVertex);
		}
		ibuf->unlock();
	}

	// 必须是三角形索引
	if (outTris.size() % 3 != 0) 
		return false;

	return true;
}
