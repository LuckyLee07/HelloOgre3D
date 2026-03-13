#ifndef __SOLDIER_ANIM_CONTROLLER_H__
#define __SOLDIER_ANIM_CONTROLLER_H__

#include <string>
#include "GameDefine.h"

class SoldierObject;
class AgentAnimStateMachine;

enum class SoldierLocomotionIntent
{
	None = 0,
	Idle,
	Move,
	Fall,
};

enum class SoldierActionIntent
{
	None = 0,
	Shoot,
	Reload,
	Death,
};

class SoldierAnimController
{
public:
	explicit SoldierAnimController(SoldierObject& owner);

	void Update(float deltaTimeInMillis);
	void OnBodyStateChanged(int stateId);

	void SetLocomotionIntent(SoldierLocomotionIntent intent);
	SoldierLocomotionIntent GetLocomotionIntent() const { return m_locomotionIntent; }

	bool RequestAction(SoldierActionIntent intent, bool forceRestart = false);
	void ClearAction(SoldierActionIntent intent);
	void ClearAllActions();
	SoldierActionIntent GetActionIntent() const { return m_actionIntent; }

	bool IsMovePresentationReady() const;
	bool IsShootPresentationReady() const;
	bool IsReloadPresentationFinished() const;
	bool HasPendingPresentation() const;

private:
	SoldierObject* GetOwner() const { return m_owner; }
	AgentAnimStateMachine* GetBodyAsm() const;
	AgentAnimStateMachine* GetWeaponAsm() const;

	int ResolveBodyLocomotionState() const;
	int ResolveBodyActionState() const;
	std::string ResolveWeaponState() const;

	void ApplyBodyState(int stateId, bool forceRestart);
	void ApplyWeaponState(const std::string& stateName, bool forceRestart);
	void ResetActionRuntime(SoldierActionIntent intent);

private:
	SoldierObject* m_owner;
	SoldierLocomotionIntent m_locomotionIntent;
	SoldierActionIntent m_actionIntent;
	bool m_forceActionRestart;
	bool m_shootPresentationReady;
	bool m_reloadPresentationStarted;
	bool m_reloadPresentationFinished;
};

#endif  // __SOLDIER_ANIM_CONTROLLER_H__