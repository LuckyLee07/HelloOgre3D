#include "PursueState.h"

#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"
#include "GameDefine.h"
#include "ai/fsm/AgentStateController.h"

PursueState::PursueState(AgentObject* pAgent)
	: AgentState(pAgent)
	, m_repathAccumMs(0.0f)
{
	m_stateId = "PursueState";
}

PursueState::~PursueState()
{
}

void PursueState::OnEnter()
{
	SetTerminated(false);
	m_repathAccumMs = 0.0f;

	if (m_controller)
	{
		m_controller->PlanPathToEnemy();
	}

	if (m_pAgent)
	{
		m_pAgent->RequestState(SSTATE_RUN_FORWARD, true);
	}
}

void PursueState::OnLeave()
{
}

std::string PursueState::OnUpdate(float dt)
{
	if (!m_pAgent)
		return "";

	if (m_pAgent->GetHealth() <= 0.0f)
	{
		SetTerminated(true);
		return "";
	}

	SoldierObject* soldier = dynamic_cast<SoldierObject*>(m_pAgent);
	if (!m_controller || !soldier || !soldier->HasEnemy(m_controller->GetNavMeshName()))
	{
		SetTerminated(true);
		return "";
	}

	m_repathAccumMs += dt;
	if (m_repathAccumMs >= 250.0f)
	{
		m_controller->PlanPathToEnemy();
		m_repathAccumMs = 0.0f;
	}

	if (m_pAgent->IsAnimReadyForMove())
	{
		m_controller->ApplySteering(dt * 0.001f, false);
	}

	if (soldier->IsTargetReached(3.0f))
	{
		SetTerminated(true);
	}

	return "";
}
