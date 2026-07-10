#include "PlayerController.h"

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

	bool NormalizeHorizontal(Ogre::Vector3& direction)
	{
		direction.y = 0.0f;
		if (direction.isNaN() || direction.squaredLength() <= kDirectionEpsilon)
			return false;
		direction.normalise();
		return true;
	}

	void ResolveCameraGroundBasis(CameraService* camera, const Ogre::Vector3& fallbackForward,
		Ogre::Vector3& forward, Ogre::Vector3& right)
	{
		right = camera != nullptr ? -camera->GetCameraLeft() : Ogre::Vector3::ZERO;
		const bool hasCameraRight = NormalizeHorizontal(right);

		forward = camera != nullptr ? camera->GetCameraForward() : Ogre::Vector3::ZERO;
		bool hasCameraForward = NormalizeHorizontal(forward);
		if (!hasCameraForward && hasCameraRight)
		{
			forward = Ogre::Vector3::UNIT_Y.crossProduct(right);
			hasCameraForward = NormalizeHorizontal(forward);
		}
		if (!hasCameraForward)
		{
			forward = fallbackForward;
			if (!NormalizeHorizontal(forward))
				forward = Ogre::Vector3::UNIT_Z;
		}

		if (!hasCameraRight)
		{
			right = forward.crossProduct(Ogre::Vector3::UNIT_Y);
			if (!NormalizeHorizontal(right))
				right = Ogre::Vector3::UNIT_X;
		}
	}
}

PlayerController::PlayerController(BaseObject* owner)
	: m_registeredInput(nullptr)
	, m_combatState(COMBAT_READY)
	, m_aimDirection(Ogre::Vector3::UNIT_Z)
	, m_lastFollowPosition(Ogre::Vector3::ZERO)
	, m_hasFollowPosition(false)
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
	IComponent::onDetach();
}

void PlayerController::onSandboxServicesChanged(const SandboxServices* services)
{
	RegisterInput(services != nullptr ? services->input : nullptr);
}

int PlayerController::getUpdateOrder() const
{
	return ComponentUpdateOrder::AI;
}

void PlayerController::update(int deltaMs)
{
	(void)deltaMs;
	if (!IsAlive())
	{
		EnterDeadState();
		return;
	}

	UpdateCameraFollow();
	UpdateAimDirection();
	UpdateCombat();
	UpdateMovement();
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

bool PlayerController::OnMousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID btn)
{
	(void)evt;
	if (btn != OIS::MB_Left || !IsAlive())
		return false;
	m_firePressed = true;
	return true;
}

bool PlayerController::OnMouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID btn)
{
	(void)evt;
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
	m_lastFollowPosition = Ogre::Vector3::ZERO;
	m_hasFollowPosition = false;
}

void PlayerController::UpdateCameraFollow()
{
	SoldierObject* owner = GetSoldierOwner();
	const SandboxServices* services = GetSandboxServices();
	CameraService* camera = services != nullptr ? services->camera : nullptr;
	if (owner == nullptr || camera == nullptr)
	{
		ResetCameraFollow();
		return;
	}

	const Ogre::Vector3 ownerPosition = owner->GetPosition();
	if (ownerPosition.isNaN())
	{
		ResetCameraFollow();
		return;
	}
	if (!m_hasFollowPosition)
	{
		m_lastFollowPosition = ownerPosition;
		m_hasFollowPosition = true;
		return;
	}

	Ogre::Vector3 delta = ownerPosition - m_lastFollowPosition;
	m_lastFollowPosition = ownerPosition;
	delta.y = 0.0f;
	if (delta.squaredLength() > kDirectionEpsilon)
		camera->TranslateCameraWorld(delta);
}

void PlayerController::UpdateAimDirection()
{
	SoldierObject* owner = GetSoldierOwner();
	const SandboxServices* services = GetSandboxServices();
	CameraService* camera = services != nullptr ? services->camera : nullptr;
	const Ogre::Vector3 fallbackForward = owner != nullptr ? owner->GetForward() : Ogre::Vector3::UNIT_Z;
	Ogre::Vector3 unusedRight;
	ResolveCameraGroundBasis(camera, fallbackForward, m_aimDirection, unusedRight);
	if (owner != nullptr)
		owner->SetForward(m_aimDirection);
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

	const SandboxServices* services = GetSandboxServices();
	CameraService* camera = services != nullptr ? services->camera : nullptr;
	Ogre::Vector3 forward;
	Ogre::Vector3 right;
	ResolveCameraGroundBasis(camera, m_aimDirection, forward, right);

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
