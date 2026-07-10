#include "DeathState.h"

#include "GameDefine.h"
#include "components/anim/AnimComponent.h"
#include "components/anim/IAnimContextProvider.h"
#include "components/anim/IAnimController.h"
#include "objects/AgentObject.h"
#include "objects/animation/AgentAnimStateMachine.h"
#include "objects/animation/SoldierAnimProfile.h"

namespace
{
	const float kDeathCleanupDelaySec = 3.5f;
	const float kDeathFallbackMs = 2000.0f;

	IAnimController* GetAnimController(AgentObject* agent)
	{
		AnimComponent* animComp = agent != nullptr ? agent->GetAnimComponent() : nullptr;
		return animComp != nullptr ? animComp->GetController() : nullptr;
	}
}

DeathState::DeathState(AgentObject* pAgent)
	: AgentState(pAgent)
	, m_elapsedMs(0.0f)
	, m_cleanupQueued(false)
{
	m_stateId = "DeathState";
}

DeathState::~DeathState()
{
}

void DeathState::OnEnter()
{
	SetTerminated(false);
	m_elapsedMs = 0.0f;
	m_cleanupQueued = false;

	IAnimController* animController = GetAnimController(m_pAgent);
	if (animController != nullptr)
	{
		animController->ClearAllActions();
		animController->RequestAction(SoldierActionIntent::Death, true);
	}
	else
	{
		m_pAgent->RequestState(SSTATE_DEAD, true);
	}

	if (m_pAgent->getRigidBody() != nullptr)
	{
		m_pAgent->ResetRigidBody(nullptr);
	}
}

void DeathState::OnLeave()
{
}

std::string DeathState::OnUpdate(float dt)
{
	m_elapsedMs += dt;

	IAnimContextProvider* animContext = dynamic_cast<IAnimContextProvider*>(m_pAgent);
	AgentAnimStateMachine* bodyAsm = nullptr;
	int deathStateId = SSTATE_DEAD;
	if (animContext != nullptr)
	{
		deathStateId = SoldierAnimProfile::ResolveBodyActionState(animContext->GetAnimStanceType(), SoldierActionIntent::Death);
		bodyAsm = animContext->GetBodyAnimStateMachine();

		IAnimController* animController = GetAnimController(m_pAgent);
		if (animController != nullptr)
		{
			animController->RequestAction(SoldierActionIntent::Death, false);
		}
	}

	const bool deathAnimFinished =
		bodyAsm != nullptr &&
		bodyAsm->GetCurrStateID() == deathStateId &&
		bodyAsm->GetCurrStateProgress() >= 0.98f &&
		!bodyAsm->HasNextState();

	if (!m_cleanupQueued && (deathAnimFinished || m_elapsedMs >= kDeathFallbackMs))
	{
		m_pAgent->OnDeath(kDeathCleanupDelaySec);
		m_cleanupQueued = true;
	}

	return "";
}
