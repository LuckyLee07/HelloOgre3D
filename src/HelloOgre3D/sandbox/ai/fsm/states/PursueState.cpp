#include "PursueState.h"

#include "GameDefine.h"
#include "ai/fsm/AgentActionContext.h"
#include "ai/fsm/AgentStateController.h"
#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"

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

	AgentActionContext* actions = m_controller ? m_controller->GetActionContext() : nullptr;
	if (actions)
	{
		actions->PlanPathToEnemy();
		actions->EnterMove(true);
	}
	else if (m_pAgent)
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

	AgentActionContext* actions = m_controller ? m_controller->GetActionContext() : nullptr;
	if (!actions || !actions->HasEnemy())
	{
		SetTerminated(true);
		return "";
	}

	m_repathAccumMs += dt;
	if (m_repathAccumMs >= 250.0f)
	{
		actions->PlanPathToEnemy();
		m_repathAccumMs = 0.0f;
	}

	actions->TickMovement(dt, false);
	actions->DrawPath(Ogre::ColourValue(1.0f, 0.0f, 0.0f), Ogre::Vector3(0.0f, 0.1f, 0.0f), 3.0f);

	if (actions->IsTargetReached(3.0f, false))
	{
		SetTerminated(true);
	}

	return "";
}