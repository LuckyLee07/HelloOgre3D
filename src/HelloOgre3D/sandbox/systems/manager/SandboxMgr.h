#ifndef __SANDBOX_MGR_H__  
#define __SANDBOX_MGR_H__

#include "OgreMesh.h"
#include "OgreVector3.h"
#include "OgreQuaternion.h"
#include "OgreString.h"

#include "systems/service/UIService.h"
#include "systems/service/CameraService.h"
#include "systems/service/ObjectFactory.h"

namespace Ogre {
	class Camera;
	class SceneNode;
	class SceneManager;
}

class UIFrame;
class BlockObject;
class AgentObject;
class SoldierObject;
struct rcConfig;
class NavigationMesh;

class SandboxMgr //tolua_exports
{ //tolua_exports
public:
	SandboxMgr(UIService& uiService, 
		CameraService& cameraService,
		ObjectFactory& objectFactory,
		Ogre::SceneManager* sceneManager);
	~SandboxMgr();

	static SandboxMgr* GetInstance();

	//tolua_begin
	Ogre::Camera* GetCamera();

	Ogre::Vector3 GetCameraUp();
	Ogre::Vector3 GetCameraLeft();
	Ogre::Vector3 GetCameraForward();
	Ogre::Vector3 GetCameraPosition();
	Ogre::Vector3 GetCameraRotation();
	Ogre::Quaternion GetCameraOrientation();

	long long GetRenderTime();
	long long GetSimulateTime();
	long long GetTotalSimulateTime();

	void CallFile(const Ogre::String& filepath);
	//tolua_end
	
	Ogre::SceneManager* GetSceneCreator();

	//tolua_begin
	void SetSkyBox(const Ogre::String materialName, const Ogre::Vector3& rotation);
	void SetAmbientLight(const Ogre::Vector3& colourValue);

	Ogre::Light* CreateDirectionalLight(const Ogre::Vector3& rotation);

	void setMaterial(Ogre::SceneNode* pNode, const Ogre::String& materialName);
	void setMaterial(BlockObject* pObject, const Ogre::String& materialName);
	
	UIFrame* CreateUIFrame(unsigned int index = 1);
	void SetMarkupColor(unsigned int index, const Ogre::ColourValue& color);

	BlockObject* CreatePlane(float length, float width);
	BlockObject* CreateBlockObject(const Ogre::String& meshfilePath);
	BlockObject* CreateBlockBox(float width, float height, float length, float uTile, float vTile);
	BlockObject* CreateBullet(Ogre::Real height, Ogre::Real radius);
	
	AgentObject* CreateAgent(AGENT_OBJ_TYPE agentType, const char* filepath = nullptr);
	SoldierObject* CreateSoldier(const Ogre::String& meshFile, const char* filepath = nullptr);
	
	void UpdateSceneGraph(); // 强制刷新场景图
	void DefaultConfig(rcConfig& config); // 获取默认的rcConfig
	void ApplySettingConfig(rcConfig& config, float height, float radius, float climb);
	NavigationMesh* CreateNavigationMesh(const rcConfig& config, const Ogre::String& navMeshName);

	Ogre::Vector3 RandomPoint(const Ogre::String& navMeshName) const;
	Ogre::Vector3 FindClosestPoint(const Ogre::String& navMeshName, const Ogre::Vector3& point) const;
	bool FindPath(const Ogre::String& navMeshName, const Ogre::Vector3& start, const Ogre::Vector3& end, std::vector<Ogre::Vector3>& outPath) const;
	//tolua_end

private:
	Ogre::SceneNode* m_pRootSceneNode = nullptr;

	UIService m_uiService;
	CameraService m_cameraService;
	ObjectFactory m_objectFactory;

}; //tolua_exports

extern SandboxMgr* g_SandboxMgr;

#endif; // __SANDBOX_MGR_H__