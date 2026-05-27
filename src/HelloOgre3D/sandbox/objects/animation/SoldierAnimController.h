#ifndef __SOLDIER_ANIM_CONTROLLER_H__
#define __SOLDIER_ANIM_CONTROLLER_H__

#include "GameDefine.h"
#include "components/anim/IAnimController.h"

class SoldierObject;
class AgentAnimStateMachine;

class SoldierAnimController : public IAnimController
{
public:
	explicit SoldierAnimController(SoldierObject& owner);

	virtual void Update(float deltaTimeInMillis) override;
	virtual void OnBodyStateChanged(int stateId) override;
	virtual void OnBodyNotify(const std::string& eventName, int stateId, float normalizedTime) override;

	virtual void SetLocomotionIntent(SoldierLocomotionIntent intent) override;
	virtual SoldierLocomotionIntent GetLocomotionIntent() const override { return m_locomotionIntent; }

	virtual bool RequestAction(SoldierActionIntent intent, bool forceRestart = false) override;
	virtual void ClearAction(SoldierActionIntent intent) override;
	virtual void ClearAllActions() override;
	virtual SoldierActionIntent GetActionIntent() const override { return m_actionIntent; }

	virtual bool ConsumeShootExecution() override;
	virtual bool IsMovePresentationReady() const override;
	virtual bool IsShootPresentationReady() const override;
	virtual bool IsShootPresentationFinished() const override;
	virtual bool IsReloadPresentationFinished() const override;
	virtual bool HasPendingPresentation() const override;

private:
	SoldierObject* GetOwner() const { return m_owner; }
	AgentAnimStateMachine* GetBodyAsm() const;
	AgentAnimStateMachine* GetWeaponAsm() const;

	void EnsureBodyNotifiesRegistered();
	void ApplyBodyState(int stateId, bool forceRestart);
	void ApplyWeaponState(const std::string& stateName, bool forceRestart);
	void ResetActionRuntime(SoldierActionIntent intent);

private:
	SoldierObject* m_owner;
	SoldierLocomotionIntent m_locomotionIntent;
	SoldierActionIntent m_actionIntent;
	bool m_forceActionRestart;
	bool m_bodyNotifiesRegistered;
	bool m_shootPresentationReady;
	bool m_shootExecutionTriggered;
	bool m_shootPresentationFinished;
	bool m_reloadPresentationStarted;
	bool m_reloadPresentationFinished;
};

#endif  // __SOLDIER_ANIM_CONTROLLER_H__
