#include "PlayerController.h"

#include "OgreMath.h"
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
	const Ogre::Real kYawSensitivity = 0.005f; // 弧度/像素（RMB 拖动）

	// 第三人称跟随相机参数（Sandbox19 尺度；SoldierObject ~人高）。
	const float kFollowHorzDist = 8.0f;
	const float kFollowVertDist = 4.0f;
	const float kFollowTargetDist = 3.0f;
	const float kFollowEyeHeight = 1.5f;
	const float kFollowSpring = 64.0f;
}

PlayerController::PlayerController(BaseObject* owner)
	: m_registeredInput(nullptr)
	, m_combatState(COMBAT_READY)
	, m_aimDirection(Ogre::Vector3::UNIT_Z)
	, m_yaw(0.0f)
	, m_hasYaw(false)
	, m_rmbHeld(false)
	, m_forwardPressed(false)
	, m_backPressed(false)
	, m_leftPressed(false)
	, m_rightPressed(false)
	, m_sprintPressed(false)
	, m_firePressed(false)
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
		camera->EnterFollowMode(kFollowHorzDist, kFollowVertDist, kFollowTargetDist, kFollowEyeHeight, kFollowSpring);
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

	// 先定朝向（含首帧从 owner 初始化 yaw），再战斗/移动，最后相机跟随用最新 pos/forward。
	UpdateAimDirection();
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
	case OIS::KC_LSHIFT:
	case OIS::KC_RSHIFT:
		m_sprintPressed = true;
		return true;
	case OIS::KC_R:
		m_reloadRequested = true;
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
	case OIS::KC_LSHIFT:
	case OIS::KC_RSHIFT:
		m_sprintPressed = false;
		break;
	default:
		return false;
	}
	return IsAlive();
}

bool PlayerController::OnMouseMoved(const OIS::MouseEvent& evt)
{
	// 仅 RMB 按住时转向；消费事件避免相机控制器再做 FREELOOK。松开返回 false 不拦截。
	if (!IsAlive() || !m_rmbHeld)
		return false;
	m_yaw -= static_cast<Ogre::Real>(evt.state.X.rel) * kYawSensitivity;
	m_hasYaw = true;
	UpdateFacingForward();
	return true;
}

bool PlayerController::OnMousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID btn)
{
	(void)evt;
	if (btn == OIS::MB_Right)
	{
		m_rmbHeld = true;
		return true;
	}
	if (btn != OIS::MB_Left || !IsAlive())
		return false;
	m_firePressed = true;
	return true;
}

bool PlayerController::OnMouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID btn)
{
	(void)evt;
	if (btn == OIS::MB_Right)
	{
		m_rmbHeld = false;
		return true;
	}
	if (btn != OIS::MB_Left)
		return false;
	m_firePressed = false;
	return IsAlive();
}

void PlayerController::ExecuteAnimShoot()
{
	WeaponComponent* weapon = GetWeaponComponent();
	if (m_combatState != COMBAT_SHOOTING || weapon == nullptr || !weapon->HasAmmo())
		return;

	weapon->ConsumeAmmo(1);
	weapon->ShootBulletTowards(m_aimDirection);
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

void PlayerController::ResetInputState()
{
	m_forwardPressed = false;
	m_backPressed = false;
	m_leftPressed = false;
	m_rightPressed = false;
	m_sprintPressed = false;
	m_firePressed = false;
	m_reloadRequested = false;
}

void PlayerController::ResetCameraFollow()
{
	m_rmbHeld = false;
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

	camera->UpdateFollow(ownerPosition, m_aimDirection, static_cast<float>(deltaMs) / 1000.0f);
}

void PlayerController::UpdateAimDirection()
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
	UpdateFacingForward();
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

	if (m_combatState == COMBAT_SHOOTING || m_combatState == COMBAT_RELOADING)
	{
		StopHorizontalMovement();
		return;
	}

	// 移动基向量相对角色朝向（不再相对相机）：forward=角色朝向水平化，right=UNIT_Y×forward。
	Ogre::Vector3 forward = m_aimDirection;
	forward.y = 0.0f;
	if (forward.isZeroLength())
		forward = Ogre::Vector3::UNIT_Z;
	forward.normalise();
	Ogre::Vector3 right = Ogre::Vector3::UNIT_Y.crossProduct(forward);
	if (right.isZeroLength())
		right = Ogre::Vector3::UNIT_X;
	right.normalise();

	Ogre::Vector3 movement = Ogre::Vector3::ZERO;
	if (m_forwardPressed) movement += forward;
	if (m_backPressed) movement -= forward;
	if (m_rightPressed) movement += right;
	if (m_leftPressed) movement -= right;

	AnimComponent* anim = GetAnimComponent();
	if (movement.squaredLength() <= kDirectionEpsilon)
	{
		StopHorizontalMovement();
		if (anim != nullptr)
			anim->EnterIdleIntent();
		return;
	}

	movement.normalise();
	AgentLocomotion* locomotion = owner->GetLocomotionComponent();
	const Ogre::Real baseSpeed = locomotion != nullptr ? locomotion->GetMaxSpeed() : static_cast<Ogre::Real>(SOLDIER_STAND_SPEED);
	const Ogre::Real speed = baseSpeed * (m_sprintPressed ? kSprintMultiplier : 1.0f);
	Ogre::Vector3 velocity = owner->GetVelocity();
	velocity.x = movement.x * speed;
	velocity.z = movement.z * speed;
	owner->SetVelocity(velocity);
	if (anim != nullptr)
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

	if (m_firePressed)
	{
		if (weapon != nullptr && weapon->HasAmmo())
			BeginShoot();
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

bool PlayerController::IsAlive() const
{
	SoldierObject* owner = GetSoldierOwner();
	return owner != nullptr && owner->GetHealth() > 0.0f;
}
