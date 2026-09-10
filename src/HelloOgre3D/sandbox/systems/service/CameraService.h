#ifndef __CAMERA_SERVICE_H__
#define __CAMERA_SERVICE_H__

#include "OgreVector2.h"
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

	// 屏幕坐标互转（供 sample 做单位点选 / 框选 / 地面点选）。
	// WorldToScreen 在相机后方或无 viewport 时返回 (-1,-1)，调用方据此丢弃。
	Ogre::Vector2 WorldToScreen(const Ogre::Vector3& world);
	Ogre::Vector3 ScreenToGroundPoint(Ogre::Real screenX, Ogre::Real screenY, Ogre::Real groundY);

	// A sample owns its profile until ResetFollowCamera or ExitFollowMode.
	bool ConfigureFollowCamera(float distance, float height, float lookAhead, float eyeHeight, float minDistance, float maxDistance);
	void ResetFollowCamera();
	void SetCameraRelativeMovement(bool enabled);
	bool IsCameraRelativeMovement() const;
	void SnapFollowTarget(const Ogre::Vector3& position, const Ogre::Vector3& forward);
	float GetFollowDistance() const;

	long long GetRenderTime();
	long long GetSimulateTime();
	long long GetTotalSimulateTime();
	//tolua_end

	void TranslateCameraWorld(const Ogre::Vector3& delta);

	// 第三人称跟随门面（非 tolua，C++ 内部用；转发到 OgreCameraController CS_FOLLOW）。
	void EnterFollowMode();
	bool ZoomFollowCamera(float distanceDelta);
	void ExitFollowMode();
	void UpdateFollow(const Ogre::Vector3& targetPos, const Ogre::Vector3& forwardXZ, float dtSec);

private:
	long long GetProfileTime(ProfileTimeKind kind) const;

	Ogre::Camera* m_camera; // non-owning; injected by GameManager
	Ogre::SceneManager* m_sceneManager; // non-owning; injected by GameManager
	OgreCameraController* m_cameraController; // non-owning; injected by GameManager
	ProfileTimeGetter m_profileTimeGetter;
	float m_followDistance;
	float m_followHeight;
	float m_followLookAhead;
	float m_followEyeHeight;
	float m_followMinDistance;
	float m_followMaxDistance;
	bool m_followConfigured;
	bool m_cameraRelativeMovement;
}; //tolua_exports

#endif // __CAMERA_SERVICE_H__
