#include "SoldierAnimController.h"

#include "AgentAnimStateMachine.h"
#include "SoldierAnimProfile.h"
#include "components/anim/IAnimContextProvider.h"
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

	std::string GetWeaponCurrentStateName(AgentAnimStateMachine* weaponAsm)
	{
		if (!weaponAsm) return "";
		const std::string desired = weaponAsm->GetDesiredStateName();
		if (!desired.empty()) return desired;
		const std::string next = weaponAsm->GetNextStateName();
		if (!next.empty()) return next;
		return weaponAsm->GetCurrStateName();
	}
}

SoldierAnimController::SoldierAnimController(IAnimContextProvider& context)
	: m_context(&context)
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

	const int stanceType = m_context != nullptr ? m_context->GetAnimStanceType() : SOLDIER_STAND;
	const int actionStateId = SoldierAnimProfile::ResolveBodyActionState(stanceType, m_actionIntent);
	if (actionStateId >= 0)
	{
		ApplyBodyState(actionStateId, m_forceActionRestart);
	}
	else
	{
		ApplyBodyState(SoldierAnimProfile::ResolveBodyLocomotionState(stanceType, m_locomotionIntent), false);
	}

	AgentAnimStateMachine* weaponAsmForResolve = GetWeaponAsm();
	const std::string currentWeaponState = GetWeaponCurrentStateName(weaponAsmForResolve);
	ApplyWeaponState(SoldierAnimProfile::ResolveWeaponState(m_actionIntent, currentWeaponState), m_forceActionRestart);
	m_forceActionRestart = false;

	AgentAnimStateMachine* bodyAsm = GetBodyAsm();
	AgentAnimStateMachine* weaponAsm = weaponAsmForResolve;
	if (m_actionIntent == SoldierActionIntent::Reload)
	{
		const std::string reloadState = SoldierAnimProfile::GetStateNameById(SoldierAnimProfile::ResolveBodyActionState(stanceType, SoldierActionIntent::Reload));
		const bool bodyReloadQueued = IsAsmStateQueued(bodyAsm, reloadState);
		const std::string reloadWeaponState = SoldierAnimProfile::ResolveWeaponState(SoldierActionIntent::Reload, currentWeaponState);
		const bool weaponReloadQueued = !reloadWeaponState.empty() && IsAsmStateQueued(weaponAsm, reloadWeaponState);
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
		const int stanceType = m_context != nullptr ? m_context->GetAnimStanceType() : SOLDIER_STAND;
		const int reloadStateId = SoldierAnimProfile::ResolveBodyActionState(stanceType, SoldierActionIntent::Reload);
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
	const bool bodyPending = bodyAsm && (bodyAsm->IsTransitioning() || !bodyAsm->GetDesiredStateName().empty());
	const bool weaponPending = weaponAsm && (weaponAsm->IsTransitioning() || !weaponAsm->GetDesiredStateName().empty());
	return bodyPending || weaponPending;
}

AgentAnimStateMachine* SoldierAnimController::GetBodyAsm() const
{
	return m_context ? m_context->GetBodyAnimStateMachine() : nullptr;
}

AgentAnimStateMachine* SoldierAnimController::GetWeaponAsm() const
{
	return m_context ? m_context->GetWeaponAnimStateMachine() : nullptr;
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

	SoldierAnimProfile::RegisterDefaultBodyNotifies(*bodyAsm);
	m_bodyNotifiesRegistered = true;
}

void SoldierAnimController::ApplyBodyState(int stateId, bool forceRestart)
{
	AgentAnimStateMachine* bodyAsm = GetBodyAsm();
	if (!bodyAsm || stateId < 0)
	{
		return;
	}

	const std::string stateName = SoldierAnimProfile::GetStateNameById(stateId);
	if (stateName.empty()) return;
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
