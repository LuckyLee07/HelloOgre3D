#include "DeathState.h"
#include "object/AgentObject.h"
#include "GameDefine.h"

DeathState::DeathState(AgentObject* pAgent) : AgentState(pAgent)
{
	m_stateId = "DeathState";
}

DeathState::~DeathState()
{

}

void DeathState::OnEnter()
{
	m_pAgent->RequestState(SSTATE_DEAD);
}

void DeathState::OnLeave()
{

}

std::string DeathState::OnUpdate(float dt)
{
	if (m_pAgent->IsMoving())
	{
		m_pAgent->SlowMoving(2.0f);
	}
	else
	{
		if (m_pAgent->GetCurStateId() != SSTATE_DEAD)
		{
			m_pAgent->RequestState(SSTATE_DEAD);
		}
		else
		{
			m_pAgent->SetHealth(0.0f);
		}
	}

	return "";
}
