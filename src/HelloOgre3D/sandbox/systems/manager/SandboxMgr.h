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

class CameraService;
class RaycastService;
class SceneService;
class ScriptService;
class BlockObject;

class SandboxMgr //tolua_exports
{ //tolua_exports
public:
	SandboxMgr(SceneService* sceneService,
		ScriptService* scriptService,
		RaycastService* raycastService);
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

	int RayCastObjectId(const Ogre::Vector3& from, const Ogre::Vector3& to) const;
	//tolua_end

private:
	bool m_useCppFsmFlag = true;

	SceneService* m_sceneService = nullptr;
	ScriptService* m_scriptService = nullptr;
	RaycastService* m_raycastService = nullptr;

}; //tolua_exports

#endif; // __SANDBOX_MGR_H__
