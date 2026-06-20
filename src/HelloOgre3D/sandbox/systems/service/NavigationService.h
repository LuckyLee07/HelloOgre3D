#ifndef __NAVIGATION_SERVICE_H__
#define __NAVIGATION_SERVICE_H__

#include <memory>
#include <unordered_map>
#include <vector>
#include "OgreString.h"
#include "OgreVector3.h"

class ObjectManager;
class NavigationMesh;
struct rcConfig;

class NavigationService //tolua_exports
{ //tolua_exports
public:
	explicit NavigationService(ObjectManager* objectManager);
	~NavigationService();

	void SetObjectManager(ObjectManager* objectManager);
	NavigationMesh* GetNavigationMesh(const Ogre::String& navMeshName) const;
	// Takes ownership of navMesh when returning true.
	bool AddNavigationMesh(const Ogre::String& navMeshName, NavigationMesh* navMesh);
	int GetNavigationMeshCount() const;

	//tolua_begin
	void DefaultConfig(rcConfig& config);
	void ApplySettingConfig(rcConfig& config, float height, float radius, float climb);
	NavigationMesh* CreateNavigationMesh(const rcConfig& config, const Ogre::String& navMeshName);

	Ogre::Vector3 RandomPoint(const Ogre::String& navMeshName) const;
	Ogre::Vector3 FindClosestPoint(const Ogre::String& navMeshName, const Ogre::Vector3& point) const;
	bool FindPath(const Ogre::String& navMeshName, const Ogre::Vector3& start, const Ogre::Vector3& end, std::vector<Ogre::Vector3>& outPath) const;
	//tolua_end

private:
	ObjectManager* m_objectManager = nullptr; // non-owning; injected by GameManager/ObjectManager wiring
	std::unordered_map<Ogre::String, std::unique_ptr<NavigationMesh>> m_navMeshes;
}; //tolua_exports

#endif // __NAVIGATION_SERVICE_H__
