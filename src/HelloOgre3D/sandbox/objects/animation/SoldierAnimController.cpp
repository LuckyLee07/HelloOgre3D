#include "SoldierAnimController.h"

#include "AgentAnimStateMachine.h"
#include "AgentAnimState.h"
#include "objects/RenderableObject.h"
#include "objects/SoldierObject.h"
#include "GameFunction.h"

namespace
{
	bool IsAsmStateQueued(AgentAnimStateMachine* asmMachine, const std::string& stateName)
	{
		if (!asmMachine || stateName.empty())
		{
			return false;
		}

		return asmMachine->IsCurrentState(stateName) || asmMachine->IsNextState(stateName) || asmMachine->GetDesiredStateName() == stateName;
	}
}

SoldierAnimController::SoldierAnimController(SoldierObject& owner)
	: m_owner(&owner)
	, m_locomotionIntent(SoldierLocomotionIntent::Idle)
	, m_actionIntent(SoldierActionIntent::None)
	, m_forceActionRestart(false)
	, m_bodyNotifiesRegistered(false)
	, m_shootPresentationReady(false)
	, m_shootExecutionTriggered(false)
	, m_shootPresentationFinished(false)
	, m_reloadPresentationStarted(false)
	, m_reloadPresentationFinished(false)
{
}

void SoldierAnimController::Update(float deltaTimeInMillis)
{
	(void)deltaTimeInMillis;
	EnsureBodyNotifiesRegistered();

	const int actionStateId = ResolveBodyActionState();
	if (actionStateId >= 0)
	{
		ApplyBodyState(actionStateId, m_forceActionRestart);
	}
	else
	{
		const int locomotionStateId = ResolveBodyLocomotionState();
		ApplyBodyState(locomotionStateId, false);
	}

	ApplyWeaponState(ResolveWeaponState(), m_forceActionRestart);
	m_forceActionRestart = false;

	AgentAnimStateMachine* bodyAsm = GetBodyAsm();
	AgentAnimStateMachine* weaponAsm = GetWeaponAsm();
	if (m_actionIntent == SoldierActionIntent::Reload)
	{
		const std::string reloadState = AgentAnimState::GetNameByID(ConvertAnimID(SSTATE_RELOAD, m_owner->getStanceType()));
		const bool bodyReloadQueued = IsAsmStateQueued(bodyAsm, reloadState);
		const bool weaponReloadQueued = IsAsmStateQueued(weaponAsm, "sniper_reload");
		if (bodyReloadQueued || weaponReloadQueued)
		{
			m_reloadPresentationStarted = true;
		}

		const bool bodyFinished = !bodyAsm || (bodyAsm->GetCurrStateProgress() >= 0.98f && !bodyAsm->HasNextState());
		const bool weaponFinished = !weaponAsm || (weaponAsm->GetCurrStateProgress() >= 0.98f && !weaponAsm->HasNextState());
		if (m_reloadPresentationStarted && bodyFinished && weaponFinished)
		{
			m_reloadPresentationFinished = true;
		}
	}
}

void SoldierAnimController::OnBodyStateChanged(int stateId)
{
	if (m_actionIntent == SoldierActionIntent::Reload)
	{
		const int reloadStateId = ConvertAnimID(SSTATE_RELOAD, m_owner->getStanceType());
		if (stateId == reloadStateId)
		{
			m_reloadPresentationStarted = true;
		}
	}
}

void SoldierAnimController::OnBodyNotify(const std::string& eventName, int stateId, float normalizedTime)
{
	(void)stateId;
	(void)normalizedTime;

	if (eventName == "shoot_fire")
	{
		m_shootPresentationReady = true;
		m_shootExecutionTriggered = true;
	}
	else if (eventName == "shoot_complete")
	{
		m_shootPresentationFinished = true;
	}
	else if (eventName == "reload_complete")
	{
		m_reloadPresentationFinished = true;
	}
}

void SoldierAnimController::SetLocomotionIntent(SoldierLocomotionIntent intent)
{
	m_locomotionIntent = intent;
}

bool SoldierAnimController::RequestAction(SoldierActionIntent intent, bool forceRestart)
{
	if (intent == SoldierActionIntent::None)
	{
		ClearAllActions();
		return true;
	}

	if (!forceRestart && m_actionIntent == intent)
	{
		return false;
	}

	m_actionIntent = intent;
	m_forceActionRestart = forceRestart;
	ResetActionRuntime(intent);
	return true;
}

void SoldierAnimController::ClearAction(SoldierActionIntent intent)
{
	if (intent == SoldierActionIntent::None || m_actionIntent != intent)
	{
		return;
	}

	m_actionIntent = SoldierActionIntent::None;
	m_forceActionRestart = false;
	m_shootPresentationReady = false;
	m_shootExecutionTriggered = false;
	m_shootPresentationFinished = false;
	m_reloadPresentationStarted = false;
	m_reloadPresentationFinished = false;
}

void SoldierAnimController::ClearAllActions()
{
	ClearAction(m_actionIntent);
}

bool SoldierAnimController::ConsumeShootExecution()
{
	const bool triggered = m_shootExecutionTriggered;
	m_shootExecutionTriggered = false;
	return triggered;
}

bool SoldierAnimController::IsMovePresentationReady() const
{
	return m_actionIntent == SoldierActionIntent::None && m_locomotionIntent == SoldierLocomotionIntent::Move;
}

bool SoldierAnimController::IsShootPresentationReady() const
{
	return m_shootPresentationReady;
}

bool SoldierAnimController::IsShootPresentationFinished() const
{
	return m_shootPresentationFinished;
}

bool SoldierAnimController::IsReloadPresentationFinished() const
{
	return m_reloadPresentationFinished;
}

bool SoldierAnimController::HasPendingPresentation() const
{
	AgentAnimStateMachine* bodyAsm = GetBodyAsm();
	AgentAnimStateMachine* weaponAsm = GetWeaponAsm();
	return (bodyAsm && bodyAsm->IsTransitioning()) || (weaponAsm && weaponAsm->IsTransitioning());
}

AgentAnimStateMachine* SoldierAnimController::GetBodyAsm() const
{
	RenderableObject* body = m_owner ? m_owner->getBody() : nullptr;
	return body ? body->GetObjectASM() : nullptr;
}

AgentAnimStateMachine* SoldierAnimController::GetWeaponAsm() const
{
	RenderableObject* weapon = m_owner ? m_owner->getWeapon() : nullptr;
	return weapon ? weapon->GetObjectASM() : nullptr;
}

int SoldierAnimController::ResolveBodyLocomotionState() const
{
	switch (m_locomotionIntent)
	{
	case SoldierLocomotionIntent::Move:
		return ConvertAnimID(SSTATE_RUN_FORWARD, m_owner->getStanceType());
	case SoldierLocomotionIntent::Fall:
		return SSTATE_FALL_IDLE;
	case SoldierLocomotionIntent::None:
	case SoldierLocomotionIntent::Idle:
	default:
		return ConvertAnimID(SSTATE_IDLE_AIM, m_owner->getStanceType());
	}
}

int SoldierAnimController::ResolveBodyActionState() const
{
	switch (m_actionIntent)
	{
	case SoldierActionIntent::Shoot:
		return ConvertAnimID(SSTATE_FIRE, m_owner->getStanceType());
	case SoldierActionIntent::Reload:
		return ConvertAnimID(SSTATE_RELOAD, m_owner->getStanceType());
	case SoldierActionIntent::Death:
		return ConvertAnimID(SSTATE_DEAD, m_owner->getStanceType());
	case SoldierActionIntent::None:
	default:
		return -1;
	}
}

std::string SoldierAnimController::ResolveWeaponState() const
{
	switch (m_actionIntent)
	{
	case SoldierActionIntent::Reload:
		return "sniper_reload";
	case SoldierActionIntent::None:
	case SoldierActionIntent::Shoot:
	case SoldierActionIntent::Death:
	default:
		return "sniper_idle";
	}
}

void SoldierAnimController::EnsureBodyNotifiesRegistered()
{
	if (m_bodyNotifiesRegistered)
	{
		return;
	}

	AgentAnimStateMachine* bodyAsm = GetBodyAsm();
	if (!bodyAsm)
	{
		return;
	}

	bodyAsm->AddNotify("fire", "shoot_fire", 0.18f, true);
	bodyAsm->AddNotify("fire", "shoot_complete", 0.90f, true);
	bodyAsm->AddNotify("crouch_fire", "shoot_fire", 0.18f, true);
	bodyAsm->AddNotify("crouch_fire", "shoot_complete", 0.90f, true);
	bodyAsm->AddNotify("reload", "reload_complete", 0.92f, true);
	m_bodyNotifiesRegistered = true;
}

void SoldierAnimController::ApplyBodyState(int stateId, bool forceRestart)
{
	AgentAnimStateMachine* bodyAsm = GetBodyAsm();
	if (!bodyAsm || stateId < 0)
	{
		return;
	}

	const std::string stateName = AgentAnimState::GetNameByID(stateId);
	if (!forceRestart && IsAsmStateQueued(bodyAsm, stateName))
	{
		return;
	}

	bodyAsm->RequestState(stateName);
}

void SoldierAnimController::ApplyWeaponState(const std::string& stateName, bool forceRestart)
{
	AgentAnimStateMachine* weaponAsm = GetWeaponAsm();
	if (!weaponAsm || stateName.empty())
	{
		return;
	}

	if (!forceRestart && IsAsmStateQueued(weaponAsm, stateName))
	{
		return;
	}

	weaponAsm->RequestState(stateName);
}

void SoldierAnimController::ResetActionRuntime(SoldierActionIntent intent)
{
	m_shootPresentationReady = false;
	m_shootExecutionTriggered = false;
	m_shootPresentationFinished = false;
	m_reloadPresentationStarted = false;
	m_reloadPresentationFinished = false;

	if (intent == SoldierActionIntent::Reload)
	{
		m_reloadPresentationStarted = false;
		m_reloadPresentationFinished = false;
	}
}