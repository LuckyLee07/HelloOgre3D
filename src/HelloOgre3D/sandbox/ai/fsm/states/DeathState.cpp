#include "DeathState.h"

#include "GameDefine.h"
#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"
#include "objects/animation/AgentAnimStateMachine.h"
#include "objects/animation/SoldierAnimController.h"
#include "objects/animation/SoldierAnimProfile.h"

namespace
{
	const float kDeathCleanupDelaySec = 3.5f;
	const float kDeathFallbackMs = 2000.0f;
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

	SoldierObject* soldier = dynamic_cast<SoldierObject*>(m_pAgent);
	if (soldier && soldier->GetAnimController())
	{
		soldier->GetAnimController()->ClearAllActions();
		soldier->GetAnimController()->RequestAction(SoldierActionIntent::Death, true);
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

	SoldierObject* soldier = dynamic_cast<SoldierObject*>(m_pAgent);
	AgentAnimStateMachine* bodyAsm = nullptr;
	int deathStateId = SSTATE_DEAD;
	if (soldier)
	{
		deathStateId = SoldierAnimProfile::ResolveBodyActionState(soldier->getStanceType(), SoldierActionIntent::Death);
		bodyAsm = soldier->GetObjectASM();
		if (soldier->GetAnimController())
		{
			soldier->GetAnimController()->RequestAction(SoldierActionIntent::Death, false);
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
