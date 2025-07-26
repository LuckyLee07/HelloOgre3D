#include "AgentState.h"
#include "object/AgentObject.h"

AgentState::AgentState(AgentObject* pAgent) : m_pAgent(pAgent)
{

}

AgentState::~AgentState()
{
	m_pAgent = nullptr;
}

void AgentState::OnEnter()
{
	doBeforeEntering();
}

void AgentState::OnLeave()
{
	doBeforeLeaving();
}

