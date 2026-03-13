#include "FleeState.h"

#include "GameDefine.h"
#include "ai/fsm/AgentActionContext.h"
#include "ai/fsm/AgentStateController.h"
#include "objects/AgentObject.h"

namespace
{
	const float kFleeProgressEpsilon = 0.08f;
	const float kFleeStuckRetryMs = 700.0f;
}

FleeState::FleeState(AgentObject* pAgent)
	: AgentState(pAgent)
	, m_stuckMs(0.0f)
	, m_lastPosition(Ogre::Vector3::ZERO)
{
	m_stateId = "FleeState";
}

FleeState::~FleeState()
{
}

void FleeState::OnEnter()
{
	SetTerminated(false);
	m_stuckMs = 0.0f;
	m_lastPosition = m_pAgent ? m_pAgent->GetPosition() : Ogre::Vector3::ZERO;

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
		return "";
	}

	const Ogre::Vector3 currPosition = m_pAgent->GetPosition();
	if ((currPosition - m_lastPosition).squaredLength() > (kFleeProgressEpsilon * kFleeProgressEpsilon))
	{
		m_lastPosition = currPosition;
		m_stuckMs = 0.0f;
	}
	else if (!actions->HasPendingAnimation())
	{
		m_stuckMs += dt;
	}

	if (m_stuckMs >= kFleeStuckRetryMs)
	{
		if (actions->SelectFleeDestination())
		{
			actions->EnterMove(true);
			m_stuckMs = 0.0f;
		}
		else
		{
			SetTerminated(true);
		}
	}

	return "";
}