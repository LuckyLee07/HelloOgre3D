#ifndef __CAMERA_SERVICE_H__
#define __CAMERA_SERVICE_H__

#include "OgreVector3.h"
#include "OgreQuaternion.h"
#include <functional>

namespace Ogre {
	class Camera;
	class SceneManager;
}

class OgreCameraController;

class CameraService //tolua_exports
{ //tolua_exports
public:
	enum ProfileTimeKind
	{
		PROFILE_RENDER_TIME,
		PROFILE_SIMULATE_TIME,
		PROFILE_TOTAL_SIMULATE_TIME
	};

	typedef std::function<long long(ProfileTimeKind)> ProfileTimeGetter;

	CameraService(Ogre::Camera* camera, Ogre::SceneManager* sceneManager, OgreCameraController* cameraController, const ProfileTimeGetter& profileTimeGetter);
	~CameraService()
	{
		m_camera = nullptr;
		m_sceneManager = nullptr;
		m_cameraController = nullptr;
		m_profileTimeGetter = nullptr;
	}

	Ogre::SceneManager* GetSceneManager();

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
	//tolua_end

	void TranslateCameraWorld(const Ogre::Vector3& delta);

	// 第三人称跟随门面（非 tolua，C++ 内部用；转发到 OgreCameraController CS_FOLLOW）。
	void EnterFollowMode(float horz, float vert, float target, float eye, float spring);
	void ExitFollowMode();
	void UpdateFollow(const Ogre::Vector3& targetPos, const Ogre::Vector3& forwardXZ, float dtSec);

private:
	long long GetProfileTime(ProfileTimeKind kind) const;

	Ogre::Camera* m_camera; // non-owning; injected by GameManager
	Ogre::SceneManager* m_sceneManager; // non-owning; injected by GameManager
	OgreCameraController* m_cameraController; // non-owning; injected by GameManager
	ProfileTimeGetter m_profileTimeGetter;
}; //tolua_exports

#endif // __CAMERA_SERVICE_H__
