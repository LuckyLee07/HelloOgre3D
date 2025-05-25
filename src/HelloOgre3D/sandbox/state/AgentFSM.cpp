#include "AgentFSM.h"
#include "AgentState.h"
#include "AgentStateFactory.h"

AgentFSM::AgentFSM() : m_pCurrentState(nullptr), m_pNextState(nullptr), 
	m_TransitionStartTime(0.0f)
{
}

AgentFSM::~AgentFSM()
{

}

void AgentFSM::Update(float deltaMilliseconds)
{

}

void AgentFSM::RequestState(const std::string& stateName)
{

}

void AgentFSM::AddState(AgentState* animState)
{

}

void AgentFSM::AddTransition(const std::string& fromState, const std::string& toState)
{

}

void AgentFSM::SetCurrentState(const std::string& stateName)
{

}

bool AgentFSM::ContainsState(const std::string& stateName)
{

}

bool AgentFSM::ContainsTransition(const std::string& fromState, const std::string& toState)
{

}