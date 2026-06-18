#include "MoveState.h"

#include "GameDefine.h"
#include "ai/fsm/AgentActionContext.h"
#include "ai/fsm/AgentStateController.h"
#include "objects/AgentObject.h"

namespace
{
	const float kMoveProgressEpsilon = 0.08f;
	const float kMoveStuckTimeoutMs = 900.0f;
}

MoveState::MoveState(AgentObject* pAgent)
	: AgentState(pAgent)
	, m_stuckMs(0.0f)
	, m_lastPosition(Ogre::Vector3::ZERO)
{
	m_stateId = "MoveState";
}

MoveState::~MoveState()
{
}

void MoveState::OnEnter()
{
	SetTerminated(false);
	m_stuckMs = 0.0f;
	m_lastPosition = m_pAgent ? m_pAgent->GetPosition() : Ogre::Vector3::ZERO;

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

	AgentActionContext* actions = m_controller ? m_controller->GetActionContext() : nullptr;
	if (actions)
	{
		actions->TickMovement(dt, false);
		actions->DrawPath(Ogre::ColourValue(1.0f, 0.5f, 0.0f), Ogre::Vector3(0.0f, 0.05f, 0.0f), 1.5f);
	}

	const bool reached = actions ? actions->IsTargetReached(1.5f, true) : false;
	if (reached)
	{
		SetTerminated(true);
		return "";
	}

	const Ogre::Vector3 currPosition = m_pAgent->GetPosition();
	if ((currPosition - m_lastPosition).squaredLength() > (kMoveProgressEpsilon * kMoveProgressEpsilon))
	{
		m_lastPosition = currPosition;
		m_stuckMs = 0.0f;
	}
	else if (!(actions && actions->HasPendingAnimation()))
	{
		m_stuckMs += dt;
	}

	if (m_stuckMs >= kMoveStuckTimeoutMs)
	{
		SetTerminated(true);
	}

	return "";
}
