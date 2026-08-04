#include "CameraService.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreCamera.h"
#include "OgreViewport.h"
#include "OgrePlane.h"
#include "OgreRay.h"
#include "GameFunction.h"
#include "ogre/OgreCameraController.h"

CameraService::CameraService(Ogre::Camera* camera, Ogre::SceneManager* sceneManager, OgreCameraController* cameraController, const ProfileTimeGetter& profileTimeGetter)
	: m_camera(camera), m_sceneManager(sceneManager), m_cameraController(cameraController), m_profileTimeGetter(profileTimeGetter)
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

// 世界坐标 -> 屏幕像素。相机后方或无 viewport 时返回 (-1,-1)，调用方据此丢弃。
Ogre::Vector2 CameraService::WorldToScreen(const Ogre::Vector3& world)
{
	Ogre::Camera* pCamera = GetCamera();
	if (pCamera == nullptr)
		return Ogre::Vector2(-1.0f, -1.0f);

	Ogre::Viewport* pViewport = pCamera->getViewport();
	if (pViewport == nullptr)
		return Ogre::Vector2(-1.0f, -1.0f);

	// 视图空间 z >= 0 表示点在相机后方（Ogre 右手系，相机朝 -z）。
	const Ogre::Vector3 eyeSpace = pCamera->getViewMatrix(true) * world;
	if (eyeSpace.z >= 0.0f)
		return Ogre::Vector2(-1.0f, -1.0f);

	// Ogre 的 Matrix4 * Vector3 已含透视除法，结果是 [-1,1] 的 NDC。
	const Ogre::Vector3 ndc = pCamera->getProjectionMatrix() * eyeSpace;
	const Ogre::Real width = static_cast<Ogre::Real>(pViewport->getActualWidth());
	const Ogre::Real height = static_cast<Ogre::Real>(pViewport->getActualHeight());
	return Ogre::Vector2((ndc.x * 0.5f + 0.5f) * width, (0.5f - ndc.y * 0.5f) * height);
}

// 屏幕像素 -> 与水平面 y=groundY 的交点。射线与平面平行/背离时返回相机位置，调用方据此兜底。
Ogre::Vector3 CameraService::ScreenToGroundPoint(Ogre::Real screenX, Ogre::Real screenY, Ogre::Real groundY)
{
	Ogre::Camera* pCamera = GetCamera();
	if (pCamera == nullptr)
		return Ogre::Vector3::ZERO;

	Ogre::Viewport* pViewport = pCamera->getViewport();
	if (pViewport == nullptr)
		return pCamera->getDerivedPosition();

	const Ogre::Real width = static_cast<Ogre::Real>(pViewport->getActualWidth());
	const Ogre::Real height = static_cast<Ogre::Real>(pViewport->getActualHeight());
	if (width <= 0.0f || height <= 0.0f)
		return pCamera->getDerivedPosition();

	const Ogre::Ray ray = pCamera->getCameraToViewportRay(screenX / width, screenY / height);
	const Ogre::Plane ground(Ogre::Vector3::UNIT_Y, Ogre::Vector3(0.0f, groundY, 0.0f));
	const std::pair<bool, Ogre::Real> hit = ray.intersects(ground);
	if (!hit.first)
		return pCamera->getDerivedPosition();

	return ray.getPoint(hit.second);
}

void CameraService::TranslateCameraWorld(const Ogre::Vector3& delta)
{
	Ogre::Camera* pCamera = GetCamera();
	if (pCamera == nullptr || delta.isNaN())
		return;
	pCamera->setPosition(pCamera->getPosition() + delta);
}

void CameraService::EnterFollowMode(float horz, float vert, float target, float eye, float spring)
{
	if (m_cameraController == nullptr)
		return;
	m_cameraController->setFollowParams(horz, vert, target, eye, spring);
	m_cameraController->enterFollow();
}

void CameraService::ExitFollowMode()
{
	if (m_cameraController != nullptr)
		m_cameraController->exitFollow();
}

void CameraService::UpdateFollow(const Ogre::Vector3& targetPos, const Ogre::Vector3& forwardXZ, float dtSec)
{
	if (m_cameraController != nullptr)
		m_cameraController->updateFollow(targetPos, forwardXZ, dtSec);
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
