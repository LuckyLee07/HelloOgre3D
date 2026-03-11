#include "AgentState.h"
#include "objects/AgentObject.h"

AgentState::AgentState(AgentObject* pAgent)
	: m_pAgent(pAgent)
	, m_controller(nullptr)
	, m_terminated(false)
{
	Event()->CreateDispatcher("FSM_STATE_CHANGE");
}

AgentState::~AgentState()
{
	m_pAgent = nullptr;
	m_controller = nullptr;
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
