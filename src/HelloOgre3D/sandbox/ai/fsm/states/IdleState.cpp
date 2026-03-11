#include "IdleState.h"
#include "GameDefine.h"
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

	if (m_pAgent)
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

	if (m_pAgent->IsMoving())
	{
		m_pAgent->SlowMoving(2.0f);
	}

	m_elapsedMs += dt;
	if (m_elapsedMs >= 2000.0f)
	{
		SetTerminated(true);
	}

	return "";
}
