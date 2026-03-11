#include "FleeState.h"

#include "GameDefine.h"
#include "ai/fsm/AgentActionContext.h"
#include "ai/fsm/AgentStateController.h"
#include "objects/AgentObject.h"

FleeState::FleeState(AgentObject* pAgent)
	: AgentState(pAgent)
{
	m_stateId = "FleeState";
}

FleeState::~FleeState()
{
}

void FleeState::OnEnter()
{
	SetTerminated(false);

	AgentActionContext* actions = m_controller ? m_controller->GetActionContext() : nullptr;
	if (!actions || !actions->SelectFleeDestination())
	{
		SetTerminated(true);
		return;
	}

	actions->EnterMove(true);
}

void FleeState::OnLeave()
{
}

std::string FleeState::OnUpdate(float dt)
{
	if (!m_pAgent)
		return "";

	if (m_pAgent->GetHealth() <= 0.0f)
	{
		SetTerminated(true);
		return "";
	}

	AgentActionContext* actions = m_controller ? m_controller->GetActionContext() : nullptr;
	if (!actions)
	{
		SetTerminated(true);
		return "";
	}

	actions->TickMovement(dt, false);
	actions->DrawPath(Ogre::ColourValue(0.0f, 0.0f, 1.0f), Ogre::Vector3::ZERO, 1.5f);

	if (actions->IsTargetReached(1.5f, true))
	{
		SetTerminated(true);
	}

	return "";
}