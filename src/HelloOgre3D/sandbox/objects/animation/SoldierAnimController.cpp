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
	, m_shootPresentationReady(false)
	, m_reloadPresentationStarted(false)
	, m_reloadPresentationFinished(false)
{
}

void SoldierAnimController::Update(float deltaTimeInMillis)
{
	(void)deltaTimeInMillis;

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
		m_reloadPresentationFinished = m_reloadPresentationStarted && bodyFinished && weaponFinished;
	}
}

void SoldierAnimController::OnBodyStateChanged(int stateId)
{
	if (m_actionIntent == SoldierActionIntent::Shoot)
	{
		const int shootStateId = ConvertAnimID(SSTATE_FIRE, m_owner->getStanceType());
		if (stateId == shootStateId)
		{
			m_shootPresentationReady = true;
		}
	}
	else if (m_actionIntent == SoldierActionIntent::Reload)
	{
		const int reloadStateId = ConvertAnimID(SSTATE_RELOAD, m_owner->getStanceType());
		if (stateId == reloadStateId)
		{
			m_reloadPresentationStarted = true;
		}
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
	m_reloadPresentationStarted = false;
	m_reloadPresentationFinished = false;
}

void SoldierAnimController::ClearAllActions()
{
	ClearAction(m_actionIntent);
}

bool SoldierAnimController::IsMovePresentationReady() const
{
	return m_actionIntent == SoldierActionIntent::None && m_locomotionIntent == SoldierLocomotionIntent::Move;
}

bool SoldierAnimController::IsShootPresentationReady() const
{
	return m_shootPresentationReady;
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
	m_reloadPresentationStarted = false;
	m_reloadPresentationFinished = false;

	if (intent == SoldierActionIntent::Shoot)
	{
		m_reloadPresentationStarted = false;
		m_reloadPresentationFinished = false;
	}
}