#include "PursueState.h"

#include "GameDefine.h"
#include "ai/fsm/AgentActionContext.h"
#include "ai/fsm/AgentStateController.h"
#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"

namespace
{
	const float kPursueProgressEpsilon = 0.08f;
	const float kPursueStuckRepathMs = 450.0f;
	const float kPursueStuckTerminateMs = 1500.0f;
	const float kPursueReachDistance = 2.0f;
}

PursueState::PursueState(AgentObject* pAgent)
	: AgentState(pAgent)
	, m_repathAccumMs(0.0f)
	, m_stuckMs(0.0f)
	, m_stuckRepathMs(0.0f)
	, m_lastPosition(Ogre::Vector3::ZERO)
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
	m_stuckMs = 0.0f;
	m_stuckRepathMs = 0.0f;
	m_lastPosition = m_pAgent ? m_pAgent->GetPosition() : Ogre::Vector3::ZERO;

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
	actions->DrawPath(Ogre::ColourValue(1.0f, 0.0f, 0.0f), Ogre::Vector3(0.0f, 0.1f, 0.0f), kPursueReachDistance);

	if (actions->IsTargetReached(kPursueReachDistance, false))
	{
		SetTerminated(true);
		return "";
	}

	const Ogre::Vector3 currPosition = m_pAgent->GetPosition();
	if ((currPosition - m_lastPosition).squaredLength() > (kPursueProgressEpsilon * kPursueProgressEpsilon))
	{
		m_lastPosition = currPosition;
		m_stuckMs = 0.0f;
		m_stuckRepathMs = 0.0f;
	}
	else if (!actions->HasPendingAnimation())
	{
		m_stuckMs += dt;
		m_stuckRepathMs += dt;
	}

	if (m_stuckRepathMs >= kPursueStuckRepathMs)
	{
		actions->PlanPathToEnemy();
		m_stuckRepathMs = 0.0f;
	}

	if (!m_pAgent->HasPath() && !actions->HasPendingAnimation())
	{
		SetTerminated(true);
		return "";
	}

	if (m_stuckMs >= kPursueStuckTerminateMs)
	{
		SetTerminated(true);
	}

	return "";
}