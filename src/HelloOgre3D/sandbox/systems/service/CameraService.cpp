#include "CameraService.h"
#include "RaycastService.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreCamera.h"
#include "OgreViewport.h"
#include "OgrePlane.h"
#include "OgreRay.h"
#include "GameFunction.h"
#include "ogre/OgreCameraController.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "OgreLogManager.h"
#include "OgreStringConverter.h"

CameraService::CameraService(Ogre::Camera* camera, Ogre::SceneManager* sceneManager, OgreCameraController* cameraController, const ProfileTimeGetter& profileTimeGetter)
	: m_camera(camera), m_sceneManager(sceneManager), m_cameraController(cameraController), m_profileTimeGetter(profileTimeGetter),
	m_followDistance(8.0f), m_followHeight(4.0f), m_followLookAhead(3.0f), m_followEyeHeight(1.5f),
	m_followMinDistance(8.0f), m_followMaxDistance(8.0f), m_followConfigured(false), m_cameraRelativeMovement(false)
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

bool CameraService::ConfigureFollowCamera(float distance, float height, float lookAhead, float eyeHeight, float minDistance, float maxDistance)
{
	if (!std::isfinite(distance) || !std::isfinite(height) || !std::isfinite(lookAhead)
		|| !std::isfinite(eyeHeight) || !std::isfinite(minDistance) || !std::isfinite(maxDistance)
		|| minDistance < 2.0f || maxDistance > 80.0f || minDistance > maxDistance
		|| height < 0.5f || height > 60.0f || lookAhead < 0.0f || lookAhead > 40.0f
		|| eyeHeight < 0.0f || eyeHeight > 10.0f)
		return false;
	m_followDistance = std::max(minDistance, std::min(maxDistance, distance));
	m_followHeight = height;
	m_followLookAhead = lookAhead;
	m_followEyeHeight = eyeHeight;
	m_followMinDistance = minDistance;
	m_followMaxDistance = maxDistance;
	m_followConfigured = true;
	if (m_cameraController != nullptr)
		m_cameraController->setFollowParams(m_followDistance, m_followHeight, m_followLookAhead, m_followEyeHeight, 64.0f);
	return true;
}

void CameraService::ResetFollowCamera()
{
	EndFollowOrbit();
	m_followTurnInput = 0.0f;
	m_obstructionDistance = 0.0f;
	if (m_cameraController != nullptr) m_cameraController->resetFollowView();
	m_followDistance = 8.0f;
	m_followHeight = 4.0f;
	m_followLookAhead = 3.0f;
	m_followEyeHeight = 1.5f;
	m_followMinDistance = 8.0f;
	m_followMaxDistance = 8.0f;
	m_followConfigured = false;
	m_cameraRelativeMovement = false;
	if (m_cameraController != nullptr)
		m_cameraController->setFollowParams(m_followDistance, m_followHeight, m_followLookAhead, m_followEyeHeight, 64.0f);
}

void CameraService::SetCameraRelativeMovement(bool enabled)
{
	m_cameraRelativeMovement = enabled;
	if (!enabled) { EndFollowOrbit(); m_followTurnInput = 0.0f; }
}

bool CameraService::IsCameraRelativeMovement() const
{
	return m_cameraRelativeMovement;
}

float CameraService::GetFollowDistance() const
{
	return m_followDistance;
}

void CameraService::SnapFollowTarget(const Ogre::Vector3& position, const Ogre::Vector3& forward)
{
	if (!std::isfinite(position.x) || !std::isfinite(position.y) || !std::isfinite(position.z)
		|| !std::isfinite(forward.x) || !std::isfinite(forward.y) || !std::isfinite(forward.z))
		return;
	m_followTurnInput = 0.0f;
	m_obstructionDistance = 0.0f;
	UpdateFollow(position, forward, 0.0f);
}

bool CameraService::ZoomFollowCamera(float distanceDelta)
{
	if (!m_followConfigured || m_cameraController == nullptr
		|| m_cameraController->getStyle() != OgreCameraController::CS_FOLLOW || !std::isfinite(distanceDelta))
		return false;
	const float distance = std::max(m_followMinDistance, std::min(m_followMaxDistance, m_followDistance + distanceDelta));
	// Keep the elevation angle stable as the sample zooms.
	m_followHeight *= distance / m_followDistance;
	m_followDistance = distance;
	m_cameraController->setFollowParams(m_followDistance, m_followHeight, m_followLookAhead, m_followEyeHeight, 64.0f);
	return true;
}

void CameraService::EnterFollowMode()
{
	if (m_cameraController == nullptr)
		return;
	m_cameraController->setFollowParams(m_followDistance, m_followHeight, m_followLookAhead, m_followEyeHeight, 64.0f);
	m_cameraController->enterFollow();
}

void CameraService::ExitFollowMode()
{
	ResetFollowCamera();
	if (m_cameraController != nullptr)
		m_cameraController->exitFollow();
}

void CameraService::UpdateFollow(const Ogre::Vector3& targetPos, const Ogre::Vector3& forwardXZ, float dtSec)
{
	if (m_cameraController != nullptr)
		m_cameraController->updateFollow(targetPos,
			m_cameraRelativeMovement && dtSec > 0.0f ? GetFollowForward() : forwardXZ, dtSec);
}

bool CameraService::IsFollowing() const
{
	return m_cameraController != nullptr && m_cameraController->getStyle() == OgreCameraController::CS_FOLLOW;
}

bool CameraService::BeginFollowOrbit()
{
	if (!m_cameraRelativeMovement || m_cameraController == nullptr
		|| m_cameraController->getStyle() != OgreCameraController::CS_FOLLOW) return false;
	m_followOrbiting = true;
	return true;
}

void CameraService::EndFollowOrbit()
{
	m_followOrbiting = false;
}

void CameraService::DragFollowOrbit(float dx, float dy)
{
	if (!m_followOrbiting || m_cameraController == nullptr || !std::isfinite(dx) || !std::isfinite(dy)) return;
	// Native relative units, independent of Retina backing pixels and window size.
	const float radiansPerUnit = Ogre::Degree(0.12f).valueRadians();
	m_cameraController->rotateFollowView(-dx * radiansPerUnit, dy * radiansPerUnit);
}

Ogre::Vector3 CameraService::GetFollowForward() const
{
	return m_cameraController != nullptr ? m_cameraController->getFollowForward() : Ogre::Vector3::UNIT_Z;
}

void CameraService::SetFollowTurnInput(bool left, bool right)
{
	m_followTurnInput = m_cameraRelativeMovement ? static_cast<float>(left) - static_cast<float>(right) : 0.0f;
}

void CameraService::RenderFollow(const Ogre::Vector3& displayedTarget, float dtSec)
{
	if (m_cameraController == nullptr || m_cameraController->getStyle() != OgreCameraController::CS_FOLLOW) return;
	// Keyboard orbit advances on the display clock, never in 30 Hz simulation steps.
	// Clamp only stalls; releasing a key sets the rate to zero without a smoothing tail.
	if (dtSec > 0.0f && m_followTurnInput != 0.0f)
		m_cameraController->rotateFollowView(m_followTurnInput * Ogre::Degree(75.0f).valueRadians()
			* std::min(dtSec, 0.1f), 0.0f);
	m_cameraController->renderFollow(displayedTarget, dtSec);
	const Ogre::Vector3 look = m_cameraController->getFollowLook();
	const Ogre::Vector3 ideal = m_cameraController->getFollowIdealPosition();
	const Ogre::Vector3 pivot = displayedTarget + Ogre::Vector3::UNIT_Y * m_followEyeHeight;
	const Ogre::Vector3 boom = ideal - pivot;
	const float fullDistance = boom.length();
	if (fullDistance < 0.01f) return;
	const Ogre::Vector3 safe = m_raycast != nullptr ? m_raycast->SweepCamera(pivot, ideal, 0.18f) : ideal;
	const float clearDistance = (safe - pivot).length();
	// Retract immediately; ease the return after a wall stops obstructing the view.
	if (m_obstructionDistance <= 0.0f || clearDistance < m_obstructionDistance)
		m_obstructionDistance = clearDistance;
	else if (dtSec > 0.0f)
		m_obstructionDistance += (clearDistance - m_obstructionDistance) * (1.0f - std::exp(-12.0f * dtSec));
	m_camera->setPosition(pivot + boom * (std::min(clearDistance, m_obstructionDistance) / fullDistance));
	// Retraction changes distance only. A filtered look point can fall behind a
	// fully retracted camera and otherwise flip the view toward the obstacle.
	const Ogre::Vector3 view = look - ideal;
	if (!view.isZeroLength()) m_camera->setDirection(view);
	static const bool trace = std::getenv("HELLO_CAMERA_TRACE") != nullptr;
	if (trace)
	{
		Ogre::LogManager::getSingleton().logMessage("[FollowCameraTrace] pivot=" + Ogre::StringConverter::toString(pivot)
			+ " ideal=" + Ogre::StringConverter::toString(ideal)
			+ " actual=" + Ogre::StringConverter::toString(m_camera->getPosition())
			+ " forward=" + Ogre::StringConverter::toString(m_camera->getDirection())
			+ " clear=" + Ogre::StringConverter::toString(clearDistance)
			+ " orbit=" + Ogre::StringConverter::toString(m_followOrbiting));
	}
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
