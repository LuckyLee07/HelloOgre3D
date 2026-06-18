#ifndef __CAMERA_SERVICE_H__
#define __CAMERA_SERVICE_H__

#include "OgreVector3.h"
#include "OgreQuaternion.h"
#include <functional>

namespace Ogre {
	class Camera;
	class SceneManager;
}

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

	CameraService(Ogre::Camera* camera, Ogre::SceneManager* sceneManager, const ProfileTimeGetter& profileTimeGetter);
	~CameraService()
	{
		m_camera = nullptr;
		m_sceneManager = nullptr;
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

private:
	long long GetProfileTime(ProfileTimeKind kind) const;

	Ogre::Camera* m_camera;
	Ogre::SceneManager* m_sceneManager;
	ProfileTimeGetter m_profileTimeGetter;
}; //tolua_exports

#endif // __CAMERA_SERVICE_H__
