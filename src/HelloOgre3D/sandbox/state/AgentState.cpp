#include "AgentState.h"
#include "object/AgentObject.h"

AgentState::AgentState(AgentObject* pAgent) : m_pAgent(pAgent)
{
	Event()->CreateDispatcher("FSM_STATE_CHANGE");
}

AgentState::~AgentState()
{
	m_pAgent = nullptr;
	Event()->RemoveDispatcher("FSM_STATE_CHANGE");
}

void AgentState::OnEnter()
{
	doBeforeEntering();
}

void AgentState::OnLeave()
{
	doBeforeLeaving();
}

void AgentState::SetStateId(const std::string& stateId)
{
	m_stateId = stateId;
}
