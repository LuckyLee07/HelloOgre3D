#ifndef __PLAYER_CONTROLLER_H__
#define __PLAYER_CONTROLLER_H__

#include "OgreVector3.h"
#include "component/IComponent.h"
#include "components/control/IAgentController.h"
#include "systems/input/IInputHandler.h"

class AnimComponent;
class InputManager;
class SoldierObject;
class WeaponComponent;

class PlayerController : public IComponent, public IAgentController, public IInputHandler
{
public:
	explicit PlayerController(BaseObject* owner = nullptr);
	virtual ~PlayerController();

	virtual void onAttach(BaseObject* owner) override;
	virtual void onDetach() override;
	virtual void onSandboxServicesChanged(const SandboxServices* services) override;
	virtual int getUpdateOrder() const override;
	virtual void update(int deltaMs) override;

	virtual AgentControllerKind GetControllerKind() const override { return AGENT_CONTROLLER_PLAYER; }
	virtual const char* GetControllerName() const override { return "PlayerController"; }

	virtual bool OnKeyPressed(OIS::KeyCode keycode, unsigned int key) override;
	virtual bool OnKeyReleased(OIS::KeyCode keycode, unsigned int key) override;
	virtual bool OnMouseMoved(const OIS::MouseEvent& evt) override;
	virtual bool OnMousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID btn) override;
	virtual bool OnMouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID btn) override;

	void ExecuteAnimShoot();

private:
	enum CombatState
	{
		COMBAT_READY = 0,
		COMBAT_SHOOTING,
		COMBAT_RELOADING,
		COMBAT_DEAD,
	};

	SoldierObject* GetSoldierOwner() const;
	AnimComponent* GetAnimComponent() const;
	WeaponComponent* GetWeaponComponent() const;
	void RegisterInput(InputManager* inputManager);
	void UnregisterInput();
	void ResetInputState();
	void ResetCameraFollow();
	void UpdateCameraFollow(int deltaMs);
	void UpdateAimDirection();
	void UpdateFacingForward();
	void UpdateMovement();
	void UpdateCombat();
	void StopHorizontalMovement();
	void BeginShoot();
	void BeginReload();
	void EnterDeadState();
	bool IsAlive() const;

private:
	InputManager* m_registeredInput;
	CombatState m_combatState;
	Ogre::Vector3 m_aimDirection;
	Ogre::Real m_yaw;         // 角色偏航（弧度），由 RMB 拖动鼠标 X 驱动
	bool m_hasYaw;
	bool m_rmbHeld;
	bool m_forwardPressed;
	bool m_backPressed;
	bool m_leftPressed;
	bool m_rightPressed;
	bool m_sprintPressed;
	bool m_firePressed;
	bool m_reloadRequested;
	bool m_deathIntentIssued;
};

#endif // __PLAYER_CONTROLLER_H__
