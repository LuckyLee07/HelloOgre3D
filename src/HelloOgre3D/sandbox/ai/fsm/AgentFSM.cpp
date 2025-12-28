#include "AgentFSM.h"
#include "states/AgentState.h"
#include "AgentStateFactory.h"
#include "SandboxMacros.h"

AgentFSM::AgentFSM() : m_currState(nullptr)
{
}

AgentFSM::~AgentFSM()
{
	for (auto& kv : m_states)
	{
		SAFE_DELETE(kv.second);
	}
	m_states.clear();
	m_transitions.clear();
}

void AgentFSM::Update(float dt)
{
	if (!m_currState) return;
	
	std::string nextState = m_currState->OnUpdate(dt);
	if (!nextState.empty())
	{
		PerformTransition(nextState);
	}
}

void AgentFSM::AddState(AgentState* state)
{
	if (!state) return;

	std::string stateId = state->GetStateId();
	m_states[stateId] = state;
}

void AgentFSM::AddTransition(const std::string& from, const std::string& to)
{
	if (!ContainsState(from) || !ContainsState(to)) 
		return;

	m_transitions[from].insert(to);
}

void AgentFSM::RequestState(const std::string& stateName)
{
	if (!ContainsState(stateName))
		return;

	if (ContainsTransition(m_currStateName, stateName))
	{
		SetCurrentState(stateName);
	}
}

void AgentFSM::SetCurrentState(const std::string& stateName)
{
	if (!ContainsState(stateName))
		return;

	if (m_currState)
		m_currState->OnLeave();

	m_currState = m_states[stateName];
	m_currStateName = stateName;

	if (m_currState)
		m_currState->OnEnter();
}

bool AgentFSM::ContainsState(const std::string& stateName)
{
	return m_states.find(stateName) != m_states.end();
}

bool AgentFSM::ContainsTransition(const std::string& from, const std::string& to)
{
	auto iter = m_transitions.find(from);
	if (iter == m_transitions.end()) return false;
	
	return iter->second.find(to) != iter->second.end();
}

bool AgentFSM::PerformTransition(const std::string& trans)
{
	std::string nextState = trans.substr(2) + "State";
	if (nextState != m_currStateName && ContainsTransition(m_currStateName, nextState))
	{
		SetCurrentState(nextState);
		return true;
	}

	return false;
}