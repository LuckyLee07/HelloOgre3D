#ifndef __NAV_BUILDER_H__
#define __NAV_BUILDER_H__

#include <string>
#include <vector>
#include "OgreVector3.h"
#include "recast/include/Recast.h"
#include "detour/include/DetourNavMesh.h"
#include "detour/include/DetourNavMeshQuery.h"

class BlockObject;

class NavBuilder
{
public:
	NavBuilder() = default;
	~NavBuilder() = default;

	// Build dtNavMesh + dtNavMeshQuery from scene objects and recast config.
	bool Build(const rcConfig& cfg,
		const std::vector<BlockObject*>& objects,
		dtNavMesh*& outNavMesh,
		dtNavMeshQuery*& outQuery);

	// Build Recast poly/detail mesh only.
	bool BuildPolyMeshes(const rcConfig& cfg,
		const std::vector<BlockObject*>& objects,
		rcPolyMesh*& outPolyMesh,
		rcPolyMeshDetail*& outPolyMeshDetail);

	// Build Detour navmesh from Recast poly/detail mesh.
	bool BuildDetourNavMesh(const rcConfig& cfg,
		rcPolyMesh& polyMesh,
		rcPolyMeshDetail& polyMeshDetail,
		dtNavMesh*& outNavMesh);

	// Build navmesh query from Detour navmesh.
	bool BuildDetourQuery(const dtNavMesh& navMesh,
		dtNavMeshQuery*& outQuery,
		const int maxNodes = 2048);

	// Destroy helpers.
	static void DestroyPolyMesh(rcPolyMesh*& mesh);
	static void DestroyPolyMeshDetail(rcPolyMeshDetail*& mesh);
	static void DestroyNavMesh(dtNavMesh*& navMesh);
	static void DestroyNavMeshQuery(dtNavMeshQuery*& query);

private:
	rcHeightfield* CreateHeightfield(const rcConfig& cfg);
	void FilterWalkableSurfaces(const rcConfig& cfg, rcHeightfield& hf);
	rcCompactHeightfield* CreateCompactHeightfield(const rcConfig& cfg, rcHeightfield& hf);
	bool ErodeWalkableArea(const rcConfig& cfg, rcCompactHeightfield& chf);
	bool BuildDistanceField(const rcConfig& cfg, rcCompactHeightfield& chf);
	bool BuildRegions(const rcConfig& cfg, rcCompactHeightfield& chf);
	rcContourSet* CreateContourSet(const rcConfig& cfg, rcCompactHeightfield& chf);
	rcPolyMesh* CreatePolyMesh(const rcConfig& cfg, rcContourSet& cset);
	rcPolyMeshDetail* CreatePolyMeshDetail(const rcConfig& cfg, rcCompactHeightfield& chf, rcPolyMesh& pmesh);

	static void DestroyHeightfield(rcHeightfield*& hf);
	static void DestroyCompactHeightfield(rcCompactHeightfield*& chf);
	static void DestroyContourSet(rcContourSet*& cset);

	struct TriangleSoup
	{
		const float* verts;
		int vertCount;
		const int* tris;
		int triCount;
	};

	bool ExtractTriangleSoup(const BlockObject& obj,
		std::vector<float>& outVerts,
		std::vector<int>& outTris);

	bool RasterizeObjects(const rcConfig& cfg,
		const std::vector<BlockObject*>& objects,
		rcHeightfield& hf);

	bool RasterizeTriangleSoup(const rcConfig& cfg,
		const TriangleSoup& soup,
		rcHeightfield& hf,
		std::vector<unsigned char>& areaBuff);

private:
	rcContext m_ctx;
};

#endif  // __NAV_BUILDER_H__
