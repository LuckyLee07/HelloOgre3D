#include "AgentState.h"
#include "objects/AgentObject.h"

namespace
{
	bool TryGetCommandTypeByStateId(const std::string& stateId, AgentCommandType& outCommandType, bool& outPreferImmediate)
	{
		outPreferImmediate = false;
		if (stateId == "IdleState")
		{
			outCommandType = AGENT_COMMAND_IDLE;
			return true;
		}
		if (stateId == "MoveState")
		{
			outCommandType = AGENT_COMMAND_MOVE;
			return true;
		}
		if (stateId == "ShootState")
		{
			outCommandType = AGENT_COMMAND_SHOOT;
			return true;
		}
		if (stateId == "DeathState")
		{
			outCommandType = AGENT_COMMAND_DIE;
			outPreferImmediate = true;
			return true;
		}
		return false;
	}
}

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
	if (!m_pAgent)
	{
		return;
	}

	AgentCommandType commandType = AGENT_COMMAND_NONE;
	bool preferImmediate = false;
	bool hasCommandMapping = TryGetCommandTypeByStateId(m_stateId, commandType, preferImmediate);
	if (!hasCommandMapping)
	{
		return;
	}

	if (preferImmediate)
	{
		m_pAgent->ImmediateCommand(commandType, 1000, 0, "FSM_ON_ENTER");
	}
	else
	{
		m_pAgent->QueueCommand(commandType, 0, 0, "FSM_ON_ENTER");
	}
}

void AgentState::OnLeave()
{
	doBeforeLeaving();
}

void AgentState::SetStateId(const std::string& stateId)
{
	m_stateId = stateId;
}
