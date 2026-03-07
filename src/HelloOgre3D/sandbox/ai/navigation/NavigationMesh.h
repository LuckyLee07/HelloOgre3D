#ifndef __NAVIGATION_MESH_H__
#define __NAVIGATION_MESH_H__

#include <string>
#include <vector>
#include "OgreVector3.h"

struct rcConfig;
class BlockObject;
class dtNavMesh;
class dtNavMeshQuery;
struct dtMeshTile;

namespace Ogre
{
	class SceneNode;
	class ManualObject;
}

class NavigationMesh //tolua_exports
{ //tolua_exports
public:
	NavigationMesh(const rcConfig& config, const std::vector<BlockObject*> objects);
	~NavigationMesh();

	//tolua_begin
	bool IsValid() const;

	Ogre::Vector3 FindClosestPoint(const Ogre::Vector3& point) const;
	bool FindPath(const Ogre::Vector3& start, const Ogre::Vector3& end, std::vector<Ogre::Vector3>& outPath) const;
	Ogre::Vector3 RandomPoint() const;

	bool HasDebugVisual() const;
	void SetDebugVisible(bool visible);
	
	//tolua_end

	bool RebuildDebugVisual();
	Ogre::SceneNode* GetDebugNode() const;
	
	static void DrawMeshTile(Ogre::ManualObject& manualObject, const dtNavMesh& navMesh, const dtMeshTile& tile);
	static void DrawMeshOutline(Ogre::ManualObject& manualObject, const dtNavMesh& navMesh, const dtMeshTile& tile);

private:
	void DestroyDebugVisual();

private:
	dtNavMesh* m_navMesh;
	dtNavMeshQuery* m_navQuery;

	Ogre::SceneNode* m_debugNode;
	std::string m_debugMeshName;
}; //tolua_exports

#endif  // __NAVIGATION_MESH_H__
