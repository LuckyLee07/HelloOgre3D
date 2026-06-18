#ifndef __SANDBOX_MGR_H__  
#define __SANDBOX_MGR_H__

#include "OgreMesh.h"
#include "OgreVector3.h"
#include "OgreString.h"

namespace Ogre {
	class Camera;
	class SceneNode;
	class SceneManager;
}

class ObjectManager;
class CameraService;
class BlockObject;
struct rcConfig;
class NavigationMesh;

class SandboxMgr //tolua_exports
{ //tolua_exports
public:
	SandboxMgr(CameraService* cameraService,
		ObjectManager* objectManager,
		Ogre::SceneManager* sceneManager);
	~SandboxMgr();

	//tolua_begin
	void CallFile(const Ogre::String& filepath);
	//tolua_end
	
	Ogre::SceneManager* GetSceneCreator();
	bool GetUseCppFsmFlag() { return m_useCppFsmFlag; }

	//tolua_begin
	void SetSkyBox(const Ogre::String materialName, const Ogre::Vector3& rotation);
	void SetAmbientLight(const Ogre::Vector3& colourValue);

	Ogre::Light* CreateDirectionalLight(const Ogre::Vector3& rotation);

	void setMaterial(Ogre::SceneNode* pNode, const Ogre::String& materialName);
	void setMaterial(BlockObject* pObject, const Ogre::String& materialName);
	
	void SetUseCppFsmFlag(bool value); // 使用CppFSM标记

	void UpdateSceneGraph(); // 强制刷新场景图
	void DefaultConfig(rcConfig& config); // 获取默认的rcConfig
	void ApplySettingConfig(rcConfig& config, float height, float radius, float climb);
	NavigationMesh* CreateNavigationMesh(const rcConfig& config, const Ogre::String& navMeshName);

	Ogre::Vector3 RandomPoint(const Ogre::String& navMeshName) const;
	Ogre::Vector3 FindClosestPoint(const Ogre::String& navMeshName, const Ogre::Vector3& point) const;
	bool FindPath(const Ogre::String& navMeshName, const Ogre::Vector3& start, const Ogre::Vector3& end, std::vector<Ogre::Vector3>& outPath) const;
	int RayCastObjectId(const Ogre::Vector3& from, const Ogre::Vector3& to) const;
	//tolua_end

private:
	Ogre::Camera* GetSceneGraphCamera();

	bool m_useCppFsmFlag = true;
	Ogre::SceneNode* m_pRootSceneNode = nullptr;

	CameraService* m_cameraService = nullptr;
	ObjectManager* m_objectManager = nullptr;

}; //tolua_exports

#endif; // __SANDBOX_MGR_H__
