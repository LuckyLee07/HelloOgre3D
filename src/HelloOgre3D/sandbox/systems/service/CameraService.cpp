#include "CameraService.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreCamera.h"
#include "GameFunction.h"

CameraService::CameraService(Ogre::Camera* camera, Ogre::SceneManager* sceneManager, const ProfileTimeGetter& profileTimeGetter)
	: m_camera(camera), m_sceneManager(sceneManager), m_profileTimeGetter(profileTimeGetter)
{
}

Ogre::Camera* CameraService::GetCamera()
{
	return m_camera;
}

Ogre::SceneManager* CameraService::GetSceneManager()
{
	return m_sceneManager;
}

Ogre::Vector3 CameraService::GetCameraUp()
{
	Ogre::Camera* pCamera = GetCamera();
	return pCamera != nullptr ? -1.0f * pCamera->getDerivedUp() : Ogre::Vector3::ZERO;
}

Ogre::Vector3 CameraService::GetCameraLeft()
{
	Ogre::Camera* pCamera = GetCamera();
	return pCamera != nullptr ? -1.0f * pCamera->getDerivedRight() : Ogre::Vector3::ZERO;
}

Ogre::Vector3 CameraService::GetCameraForward()
{
	Ogre::Camera* pCamera = GetCamera();
	return pCamera != nullptr ? pCamera->getDerivedDirection() : Ogre::Vector3::ZERO;
}

Ogre::Vector3 CameraService::GetCameraPosition()
{
	Ogre::Camera* pCamera = GetCamera();
	return pCamera != nullptr ? pCamera->getDerivedPosition() : Ogre::Vector3::ZERO;
}

Ogre::Vector3 CameraService::GetCameraRotation()
{
	return QuaternionToRotationDegrees(GetCameraOrientation());
}

Ogre::Quaternion CameraService::GetCameraOrientation()
{
	Ogre::Camera* pCamera = GetCamera();
	return pCamera != nullptr ? pCamera->getDerivedOrientation() : Ogre::Quaternion::IDENTITY;
}

long long CameraService::GetRenderTime()
{
	return GetProfileTime(PROFILE_RENDER_TIME);
}

long long CameraService::GetSimulateTime()
{
	return GetProfileTime(PROFILE_SIMULATE_TIME);
}

long long CameraService::GetTotalSimulateTime()
{
	return GetProfileTime(PROFILE_TOTAL_SIMULATE_TIME);
}

long long CameraService::GetProfileTime(ProfileTimeKind kind) const
{
	return m_profileTimeGetter != nullptr ? m_profileTimeGetter(kind) : 0;
}
