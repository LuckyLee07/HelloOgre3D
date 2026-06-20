#ifndef __SCENE_SERVICE_H__
#define __SCENE_SERVICE_H__

#include "OgreString.h"
#include "OgreVector3.h"

namespace Ogre {
	class Camera;
	class Light;
	class SceneManager;
	class SceneNode;
}

class BlockObject;
class CameraService;

class SceneService //tolua_exports
{ //tolua_exports
public:
	SceneService(Ogre::SceneManager* sceneManager, CameraService* cameraService);
	~SceneService();

	void SetSceneManager(Ogre::SceneManager* sceneManager);
	void SetCameraService(CameraService* cameraService);
	Ogre::SceneManager* GetSceneManager() const;
	Ogre::SceneNode* GetRootSceneNode() const;

	//tolua_begin
	void SetSkyBox(const Ogre::String materialName, const Ogre::Vector3& rotation);
	void SetAmbientLight(const Ogre::Vector3& colourValue);
	Ogre::Light* CreateDirectionalLight(const Ogre::Vector3& direction);
	void setMaterial(Ogre::SceneNode* pNode, const Ogre::String& materialName);
	void setMaterial(BlockObject* pObject, const Ogre::String& materialName);
	void UpdateSceneGraph();
	//tolua_end

private:
	Ogre::Camera* GetSceneGraphCamera() const;

	Ogre::SceneManager* m_sceneManager = nullptr; // non-owning; injected by GameManager
	CameraService* m_cameraService = nullptr; // non-owning; injected service facade
}; //tolua_exports

#endif // __SCENE_SERVICE_H__
