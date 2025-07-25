#include "IdleState.h"
#include "AgentState.h"
#include "object/AgentObject.h"

IdleState::IdleState(AgentObject* pAgent) : AgentState(pAgent)
{
	m_stateId = "IdleState";
}

IdleState::~IdleState()
{
	m_pAgent = nullptr;
}

void IdleState::OnEnter()
{
	if (m_pAgent->IsMoving())
	{

	}
	m_pAgent->RequestState(0);
}

void IdleState::OnLeave()
{

}

std::string IdleState::OnUpdate(float dt)
{
	return "";
}
