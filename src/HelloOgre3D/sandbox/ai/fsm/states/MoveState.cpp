#include "MoveState.h"

#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"
#include "GameDefine.h"
#include "ai/fsm/AgentStateController.h"
#include "debug/DebugDrawer.h"

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

	if (m_pAgent)
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

	if (m_controller && m_pAgent->IsAnimReadyForMove())
	{
		m_controller->ApplySteering(dt * 0.001f, false);
	}

	DebugDrawer* debugDrawer = DebugDrawer::GetInstance();
	if (debugDrawer)
	{
		const std::vector<Ogre::Vector3>& path = m_pAgent->GetPath();
		if (!path.empty())
		{
			const Ogre::Vector3 offset(0.0f, 0.05f, 0.0f);
			const Ogre::ColourValue color(1.0f, 0.5f, 0.0f);
			debugDrawer->drawPath(path, color, false, offset);
			debugDrawer->drawCircle(path.back() + offset, 1.5f, 20, color, false);
		}
	}

	SoldierObject* soldier = dynamic_cast<SoldierObject*>(m_pAgent);
	const bool timeout = (m_elapsedMs >= 500.0f);
	const bool reached = (soldier && soldier->IsTargetReached(1.5f));
	if (timeout || reached)
	{
		if (reached && soldier)
		{
			soldier->ClearMovePosition();
		}
		SetTerminated(true);
	}

	return "";
}
