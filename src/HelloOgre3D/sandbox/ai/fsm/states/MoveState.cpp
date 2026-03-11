#include "MoveState.h"

#include "GameDefine.h"
#include "ai/fsm/AgentActionContext.h"
#include "ai/fsm/AgentStateController.h"
#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"

MoveState::MoveState(AgentObject* pAgent)
	: AgentState(pAgent)
	, m_elapsedMs(0.0f)
{
	m_stateId = "MoveState";
}

MoveState::~MoveState()
{
}

void MoveState::OnEnter()
{
	SetTerminated(false);
	m_elapsedMs = 0.0f;

	AgentActionContext* actions = m_controller ? m_controller->GetActionContext() : nullptr;
	if (actions)
	{
		actions->EnterMove(false);
	}
	else if (m_pAgent)
	{
		m_pAgent->RequestState(SSTATE_RUN_FORWARD);
	}
}

void MoveState::OnLeave()
{
}

std::string MoveState::OnUpdate(float dt)
{
	if (!m_pAgent)
		return "";

	if (m_pAgent->GetHealth() <= 0.0f)
	{
		SetTerminated(true);
		return "";
	}

	m_elapsedMs += dt;

	AgentActionContext* actions = m_controller ? m_controller->GetActionContext() : nullptr;
	if (actions)
	{
		actions->TickMovement(dt, false);
		actions->DrawPath(Ogre::ColourValue(1.0f, 0.5f, 0.0f), Ogre::Vector3(0.0f, 0.05f, 0.0f), 1.5f);
	}

	SoldierObject* soldier = dynamic_cast<SoldierObject*>(m_pAgent);
	const bool timeout = (m_elapsedMs >= 500.0f);
	const bool reached = actions ? actions->IsTargetReached(1.5f, true) : (soldier && soldier->IsTargetReached(1.5f));
	if (timeout || reached)
	{
		if (reached && !actions && soldier)
		{
			soldier->ClearMovePosition();
		}
		SetTerminated(true);
	}

	return "";
}