#include "IdleState.h"

#include "GameDefine.h"
#include "ai/fsm/AgentActionContext.h"
#include "ai/fsm/AgentStateController.h"
#include "objects/AgentObject.h"

IdleState::IdleState(AgentObject* pAgent)
	: AgentState(pAgent)
	, m_elapsedMs(0.0f)
{
	m_stateId = "IdleState";
}

IdleState::~IdleState()
{
}

void IdleState::OnEnter()
{
	SetTerminated(false);
	m_elapsedMs = 0.0f;

	AgentActionContext* actions = m_controller ? m_controller->GetActionContext() : nullptr;
	if (actions)
	{
		actions->EnterIdle();
	}
	else if (m_pAgent)
	{
		m_pAgent->RequestState(SSTATE_IDLE_AIM);
	}
}

void IdleState::OnLeave()
{
}

std::string IdleState::OnUpdate(float dt)
{
	if (!m_pAgent)
		return "";

	AgentActionContext* actions = m_controller ? m_controller->GetActionContext() : nullptr;
	if (actions)
	{
		actions->StopMovement();
	}
	else
	{
		m_pAgent->SetVelocity(Ogre::Vector3(0.0f, m_pAgent->GetVelocity().y, 0.0f));
	}

	m_elapsedMs += dt;
	if (m_elapsedMs >= 2000.0f)
	{
		SetTerminated(true);
	}

	return "";
}
