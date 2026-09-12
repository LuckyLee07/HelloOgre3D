#include "PlayerController.h"

#include "OgreMath.h"
#include <algorithm>
#include <cmath>
#include "GameDefine.h"
#include "components/agent/AgentLocomotion.h"
#include "components/anim/AnimComponent.h"
#include "components/anim/IAnimController.h"
#include "components/combat/WeaponComponent.h"
#include "core/SandboxServices.h"
#include "objects/SoldierObject.h"
#include "systems/input/InputManager.h"
#include "systems/service/CameraService.h"

namespace
{
	const Ogre::Real kSprintMultiplier = 1.75f;
	const Ogre::Real kDirectionEpsilon = 1e-6f;
	const Ogre::Real kTurnRate = 2.5f; // 弧度/秒（A/D 平滑转向速率，照搬 code-master 角速度思路）
}

PlayerController::PlayerController(BaseObject* owner)
	: m_registeredInput(nullptr)
	, m_combatState(COMBAT_READY)
	, m_aimDirection(Ogre::Vector3::UNIT_Z)
	, m_yaw(0.0f)
	, m_hasYaw(false)
	, m_forwardPressed(false)
	, m_backPressed(false)
	, m_leftPressed(false)
	, m_rightPressed(false)
	, m_rotateLeftPressed(false)
	, m_rotateRightPressed(false)
	, m_sprintPressed(false)
	, m_fireInputs(0)
	, m_reloadRequested(false)
	, m_deathIntentIssued(false)
{
	(void)owner;
}

PlayerController::~PlayerController()
{
	UnregisterInput();
}

void PlayerController::onAttach(BaseObject* owner)
{
	IComponent::onAttach(owner);
	m_combatState = COMBAT_READY;
	m_deathIntentIssued = false;
	ResetInputState();
	ResetCameraFollow();
}

void PlayerController::onDetach()
{
	StopHorizontalMovement();
	UnregisterInput();
	ResetInputState();
	ResetCameraFollow();
	// 退出前恢复相机为 FREELOOK，避免第三人称 FOLLOW 残留污染其它 sample。
	const SandboxServices* services = GetSandboxServices();
	CameraService* camera = services != nullptr ? services->camera : nullptr;
	if (camera != nullptr)
		camera->ExitFollowMode();
	IComponent::onDetach();
}

void PlayerController::onSandboxServicesChanged(const SandboxServices* services)
{
	RegisterInput(services != nullptr ? services->input : nullptr);
	CameraService* camera = services != nullptr ? services->camera : nullptr;
	if (camera != nullptr)
		camera->EnterFollowMode();
}

int PlayerController::getUpdateOrder() const
{
	return ComponentUpdateOrder::AI;
}

void PlayerController::update(int deltaMs)
{
	if (!IsAlive())
	{
		EnterDeadState();
		return;
	}

	// 先更新视线和身体朝向，再战斗/移动，最后提交跟随位置。
	UpdateTurning(deltaMs);
	UpdateCombat();
	UpdateMovement();
	UpdateCameraFollow(deltaMs);
}

bool PlayerController::OnKeyPressed(OIS::KeyCode keycode, unsigned int key)
{
	(void)key;
	if (!IsAlive())
		return false;

	switch (keycode)
	{
	case OIS::KC_W:
	case OIS::KC_UP:
		m_forwardPressed = true;
		return true;
	case OIS::KC_S:
	case OIS::KC_DOWN:
		m_backPressed = true;
		return true;
	case OIS::KC_A:
	case OIS::KC_LEFT:
		m_leftPressed = true;
		return true;
	case OIS::KC_D:
	case OIS::KC_RIGHT:
		m_rightPressed = true;
		return true;
	case OIS::KC_Q:
		if (!UsesCameraRelativeMovement()) return false;
		m_rotateLeftPressed = true;
		UpdateFollowTurnInput();
		return true;
	case OIS::KC_E:
		if (!UsesCameraRelativeMovement()) return false;
		m_rotateRightPressed = true;
		UpdateFollowTurnInput();
		return true;
	case OIS::KC_LSHIFT:
	case OIS::KC_RSHIFT:
		m_sprintPressed = true;
		return true;
	case OIS::KC_R:
		m_reloadRequested = true;
		return true;
	case OIS::KC_SPACE:
		m_fireInputs |= 1;
		return true;
	default:
		return false;
	}
}

bool PlayerController::OnKeyReleased(OIS::KeyCode keycode, unsigned int key)
{
	(void)key;
	switch (keycode)
	{
	case OIS::KC_W:
	case OIS::KC_UP:
		m_forwardPressed = false;
		break;
	case OIS::KC_S:
	case OIS::KC_DOWN:
		m_backPressed = false;
		break;
	case OIS::KC_A:
	case OIS::KC_LEFT:
		m_leftPressed = false;
		break;
	case OIS::KC_D:
	case OIS::KC_RIGHT:
		m_rightPressed = false;
		break;
	case OIS::KC_Q:
		m_rotateLeftPressed = false;
		UpdateFollowTurnInput();
		return UsesCameraRelativeMovement();
	case OIS::KC_E:
		m_rotateRightPressed = false;
		UpdateFollowTurnInput();
		return UsesCameraRelativeMovement();
	case OIS::KC_LSHIFT:
	case OIS::KC_RSHIFT:
		m_sprintPressed = false;
		break;
	case OIS::KC_SPACE:
		m_fireInputs &= ~1;
		break;
	default:
		return false;
	}
	return IsAlive();
}

bool PlayerController::OnMousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID btn)
{
	(void)evt;
	if (btn != OIS::MB_Left || !IsAlive())
		return false;
	m_fireInputs |= 2;
	return true;
}

bool PlayerController::OnMouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID btn)
{
	(void)evt;
	if (btn != OIS::MB_Left)
		return false;
	m_fireInputs &= ~2;
	return IsAlive();
}

void PlayerController::ExecuteAnimShoot()
{
	WeaponComponent* weapon = GetWeaponComponent();
	if (m_combatState != COMBAT_SHOOTING || weapon == nullptr || !weapon->HasAmmo())
		return;

	weapon->ConsumeAmmo(1);
	// Fire along the evaluated muzzle pose, including its authored cant.
	weapon->ShootBullet();
}

SoldierObject* PlayerController::GetSoldierOwner() const
{
	return dynamic_cast<SoldierObject*>(getOwner());
}

AnimComponent* PlayerController::GetAnimComponent() const
{
	SoldierObject* owner = GetSoldierOwner();
	return owner != nullptr ? owner->GetAnimComponent() : nullptr;
}

WeaponComponent* PlayerController::GetWeaponComponent() const
{
	SoldierObject* owner = GetSoldierOwner();
	return owner != nullptr ? owner->GetWeaponComponent() : nullptr;
}

void PlayerController::RegisterInput(InputManager* inputManager)
{
	if (m_registeredInput == inputManager)
		return;
	UnregisterInput();
	m_registeredInput = inputManager;
	if (m_registeredInput != nullptr)
		m_registeredInput->registerHandler(this);
}

void PlayerController::UnregisterInput()
{
	if (m_registeredInput != nullptr)
		m_registeredInput->unregisterHandler(this);
	m_registeredInput = nullptr;
}

void PlayerController::ResetTransientInput()
{
	ResetInputState();
	StopHorizontalMovement();
	const SandboxServices* services = GetSandboxServices();
	if (services != nullptr && services->camera != nullptr) services->camera->EndFollowOrbit();
}

void PlayerController::ResetInputState()
{
	m_forwardPressed = false;
	m_backPressed = false;
	m_leftPressed = false;
	m_rightPressed = false;
	m_rotateLeftPressed = false;
	m_rotateRightPressed = false;
	UpdateFollowTurnInput();
	m_sprintPressed = false;
	m_fireInputs = 0;
	m_reloadRequested = false;
}

void PlayerController::ResetCameraFollow()
{
	m_hasYaw = false;
}

void PlayerController::UpdateCameraFollow(int deltaMs)
{
	SoldierObject* owner = GetSoldierOwner();
	const SandboxServices* services = GetSandboxServices();
	CameraService* camera = services != nullptr ? services->camera : nullptr;
	if (owner == nullptr || camera == nullptr)
		return;

	const Ogre::Vector3 ownerPosition = owner->GetPosition();
	if (ownerPosition.isNaN())
		return;

	const Ogre::Vector3 cameraForward = UsesCameraRelativeMovement()
		? Ogre::Vector3(Ogre::Math::Sin(Ogre::Radian(m_yaw)), 0.0f, Ogre::Math::Cos(Ogre::Radian(m_yaw)))
		: m_aimDirection;
	camera->UpdateFollow(ownerPosition, cameraForward, static_cast<float>(deltaMs) / 1000.0f);
}

void PlayerController::UpdateTurning(int deltaMs)
{
	SoldierObject* owner = GetSoldierOwner();
	if (owner == nullptr)
		return;

	if (!m_hasYaw)
	{
		// 首帧从角色现有朝向初始化 yaw（atan2(x,z) 与 forward=(sin,0,cos) 互逆）。
		Ogre::Vector3 forward = owner->GetForward();
		forward.y = 0.0f;
		if (forward.isZeroLength())
			forward = Ogre::Vector3::UNIT_Z;
		m_yaw = Ogre::Math::ATan2(forward.x, forward.z).valueRadians();
		m_hasYaw = true;
	}

	if (UsesCameraRelativeMovement())
	{
		// The camera owns view yaw. Movement and fire read the same reference.
		m_aimDirection = GetSandboxServices()->camera->GetFollowForward();
		m_yaw = Ogre::Math::ATan2(m_aimDirection.x, m_aimDirection.z).valueRadians();
		const bool moving = m_forwardPressed != m_backPressed || m_leftPressed != m_rightPressed;
		if (moving || m_fireInputs || m_combatState == COMBAT_SHOOTING)
		{
			AgentLocomotion* locomotion = owner->GetLocomotionComponent();
			if (locomotion != nullptr) locomotion->FaceDirection(m_aimDirection, static_cast<float>(deltaMs));
		}
		return;
	}

	// Preserve tank controls used by existing samples.
	Ogre::Real angular = 0.0f;
	if (m_leftPressed) angular += kTurnRate;
	if (m_rightPressed) angular -= kTurnRate;
	m_yaw += angular * (static_cast<Ogre::Real>(deltaMs) / 1000.0f);
	UpdateFacingForward();
}

void PlayerController::UpdateFollowTurnInput()
{
	const SandboxServices* services = GetSandboxServices();
	if (services != nullptr && services->camera != nullptr)
		services->camera->SetFollowTurnInput(m_rotateLeftPressed, m_rotateRightPressed);
}

bool PlayerController::IsFacingAim() const
{
	const SoldierObject* owner = GetSoldierOwner();
	return !UsesCameraRelativeMovement() || owner == nullptr
		|| owner->GetForward().dotProduct(m_aimDirection) >= Ogre::Math::Cos(Ogre::Degree(8.0f));
}

void PlayerController::UpdateFacingForward()
{
	SoldierObject* owner = GetSoldierOwner();
	if (owner == nullptr)
		return;
	const Ogre::Vector3 forward(
		Ogre::Math::Sin(Ogre::Radian(m_yaw)), 0.0f, Ogre::Math::Cos(Ogre::Radian(m_yaw)));
	m_aimDirection = forward;
	owner->SetForward(forward);
}

void PlayerController::UpdateMovement()
{
	SoldierObject* owner = GetSoldierOwner();
	if (owner == nullptr)
		return;

	// Reloading still commits the player to the presentation, but firing must
	// not cancel locomotion. The old full stop made sustained fire reduce a
	// nominal 3 m/s run to a short shuffle between shoot animations.
	if (m_combatState == COMBAT_RELOADING)
	{
		StopHorizontalMovement();
		return;
	}
	const bool shooting = m_combatState == COMBAT_SHOOTING;

	// 默认保持 tank；sample 显式选择相机平面移动时，WASD 位移与镜头偏航分开。
	Ogre::Vector3 forward = m_aimDirection;
	const bool relativeMovement = UsesCameraRelativeMovement();
	// Camera rotation, movement and the weapon share the same active horizontal view.
	forward.y = 0.0f;
	if (forward.isZeroLength())
		forward = Ogre::Vector3::UNIT_Z;
	forward.normalise();

	Ogre::Vector3 movement = Ogre::Vector3::ZERO;
	if (m_forwardPressed) movement += forward;
	if (m_backPressed) movement -= forward;
	if (relativeMovement)
	{
		const Ogre::Vector3 right = forward.crossProduct(Ogre::Vector3::UNIT_Y);
		if (m_leftPressed) movement -= right;
		if (m_rightPressed) movement += right;
	}

	AnimComponent* anim = GetAnimComponent();
	if (movement.squaredLength() <= kDirectionEpsilon)
	{
		StopHorizontalMovement();
		if (anim != nullptr && !shooting)
			anim->EnterIdleIntent();
		return;
	}

	movement.normalise();
	// Strafe/backpedal retain view-facing posture, including when fire starts/stops.
	AgentLocomotion* locomotion = owner->GetLocomotionComponent();
	const Ogre::Real baseSpeed = locomotion != nullptr ? locomotion->GetMaxSpeed() : static_cast<Ogre::Real>(SOLDIER_STAND_SPEED);
	const Ogre::Real forwardSpeed = baseSpeed * (m_sprintPressed ? kSprintMultiplier : 1.0f);
	// The available sidestep is authored at 1 m/s; keep tactical movement within
	// 1.6x playback speed, including Shift, instead of accelerating it to 5.25x.
	const Ogre::Real along = movement.dotProduct(forward);
	const Ogre::Real lateral = relativeMovement ? std::sqrt(std::max(0.0f, 1.0f - along * along)) : 0.0f;
	const Ogre::Real longitudinalSpeed = along < 0 ? std::min(baseSpeed, 2.25f) : forwardSpeed;
	const Ogre::Real lateralSpeed = std::min(baseSpeed, 1.6f);
	const Ogre::Real speed = baseSpeed <= kDirectionEpsilon ? 0.0f : relativeMovement ? 1.0f / std::sqrt(
		along * along / (longitudinalSpeed * longitudinalSpeed) + lateral * lateral / (lateralSpeed * lateralSpeed)) : forwardSpeed;
	Ogre::Vector3 velocity = owner->GetVelocity();
	velocity.x = movement.x * speed;
	velocity.z = movement.z * speed;
	owner->SetVelocity(velocity);
	if (anim != nullptr && !shooting)
		anim->EnterMoveIntent();
}

void PlayerController::UpdateCombat()
{
	AnimComponent* anim = GetAnimComponent();
	IAnimController* animController = anim != nullptr ? anim->GetController() : nullptr;
	WeaponComponent* weapon = GetWeaponComponent();

	if (m_combatState == COMBAT_SHOOTING)
	{
		if (animController == nullptr || animController->IsShootPresentationFinished())
		{
			if (animController != nullptr)
				animController->ClearAction(SoldierActionIntent::Shoot);
			m_combatState = COMBAT_READY;
		}
		return;
	}

	if (m_combatState == COMBAT_RELOADING)
	{
		if (animController == nullptr || animController->IsReloadPresentationFinished())
		{
			if (weapon != nullptr)
				weapon->RestoreAmmo();
			if (animController != nullptr)
				animController->ClearAction(SoldierActionIntent::Reload);
			m_combatState = COMBAT_READY;
		}
		return;
	}

	if (m_reloadRequested)
	{
		m_reloadRequested = false;
		if (weapon != nullptr && weapon->GetAmmo() < weapon->GetMaxAmmo())
			BeginReload();
		return;
	}

	if (m_fireInputs)
	{
		if (weapon != nullptr && weapon->HasAmmo())
		{
			// A large free-look turn settles before the first shot. Once started,
			// the action still fires from its evaluated muzzle, with no redirected bullet.
			if (IsFacingAim()) BeginShoot();
		}
		else
			BeginReload();
	}
}

void PlayerController::StopHorizontalMovement()
{
	SoldierObject* owner = GetSoldierOwner();
	if (owner == nullptr)
		return;
	Ogre::Vector3 velocity = owner->GetVelocity();
	velocity.x = 0.0f;
	velocity.z = 0.0f;
	owner->SetVelocity(velocity);
}

void PlayerController::BeginShoot()
{
	AnimComponent* anim = GetAnimComponent();
	if (anim != nullptr && anim->EnterShootIntent())
	{
		m_combatState = COMBAT_SHOOTING;
		return;
	}

	m_combatState = COMBAT_SHOOTING;
	ExecuteAnimShoot();
	m_combatState = COMBAT_READY;
}

void PlayerController::BeginReload()
{
	WeaponComponent* weapon = GetWeaponComponent();
	if (weapon == nullptr || weapon->GetAmmo() >= weapon->GetMaxAmmo())
		return;

	AnimComponent* anim = GetAnimComponent();
	if (anim != nullptr && anim->EnterReloadIntent())
	{
		m_combatState = COMBAT_RELOADING;
		return;
	}

	weapon->RestoreAmmo();
	m_combatState = COMBAT_READY;
}

void PlayerController::EnterDeadState()
{
	if (m_combatState == COMBAT_DEAD && m_deathIntentIssued)
		return;
	m_combatState = COMBAT_DEAD;
	StopHorizontalMovement();
	ResetInputState();
	AnimComponent* anim = GetAnimComponent();
	if (anim != nullptr)
		anim->EnterDeathIntent();
	m_deathIntentIssued = true;
}

bool PlayerController::UsesCameraRelativeMovement() const
{
	const SandboxServices* services = GetSandboxServices();
	return services != nullptr && services->camera != nullptr && services->camera->IsCameraRelativeMovement();
}

bool PlayerController::IsAlive() const
{
	SoldierObject* owner = GetSoldierOwner();
	return owner != nullptr && owner->GetHealth() > 0.0f;
}
