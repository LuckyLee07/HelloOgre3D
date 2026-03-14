#include "DeathState.h"
#include "objects/AgentObject.h"
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
	AgentState::OnEnter();
}

void DeathState::OnLeave()
{
	AgentState::OnLeave();
}

std::string DeathState::OnUpdate(float dt)
{
	if (m_pAgent->IsMoving())
	{
		m_pAgent->SlowMoving(2.0f);
	}
	else
	{
		if (!m_pAgent->IsAnimReadyByFsmState(m_stateId))
		{
			m_pAgent->RequestAnimByFsmState(m_stateId);
		}
		else
		{
			m_pAgent->SetHealth(0.0f);
		}
	}

	return "";
}
