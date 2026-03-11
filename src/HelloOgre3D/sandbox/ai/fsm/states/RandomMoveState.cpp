#include "RandomMoveState.h"

#include "ai/fsm/AgentActionContext.h"
#include "ai/fsm/AgentStateController.h"

RandomMoveState::RandomMoveState(AgentObject* pAgent)
	: AgentState(pAgent)
{
	m_stateId = "RandomMoveState";
}

RandomMoveState::~RandomMoveState()
{
}

void RandomMoveState::OnEnter()
{
	SetTerminated(false);

	AgentActionContext* actions = m_controller ? m_controller->GetActionContext() : nullptr;
	if (actions)
	{
		(void)actions->SelectRandomDestination(12);
	}

	SetTerminated(true);
}

void RandomMoveState::OnLeave()
{
}

std::string RandomMoveState::OnUpdate(float dt)
{
	(void)dt;
	return "";
}