#ifndef __NAVIGATION_MESH_H__
#define __NAVIGATION_MESH_H__

#include <string>
#include <vector>
#include "OgreVector3.h"

struct rcConfig;
class BaseObject;
class dtNavMesh;
class dtNavMeshQuery;

namespace Ogre
{
	class SceneNode;
}

class NavigationMesh //tolua_exports
{ //tolua_exports
public:
	NavigationMesh(const rcConfig& config, const std::vector<BaseObject*> objects);
	~NavigationMesh();

	bool IsValid() const;

	Ogre::Vector3 FindClosestPoint(const Ogre::Vector3& point) const;
	bool FindPath(const Ogre::Vector3& start, const Ogre::Vector3& end, std::vector<Ogre::Vector3>& outPath) const;
	Ogre::Vector3 RandomPoint() const;

	bool RebuildDebugVisual();
	bool HasDebugVisual() const;
	void SetDebugVisible(bool visible);
	Ogre::SceneNode* GetDebugNode() const;

	//tolua_begin
	static bool CreateNavMesh(const rcConfig& cfg, const std::string& navMeshName);
	//tolua_end

private:
	void DestroyDebugVisual();

private:
	dtNavMesh* m_navMesh;
	dtNavMeshQuery* m_navQuery;

	Ogre::SceneNode* m_debugNode;
	std::string m_debugMeshName;
}; //tolua_exports

#endif  // __NAVIGATION_MESH_H__
