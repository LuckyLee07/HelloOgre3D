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
	if (m_pAgent)
	{
		AgentCommandType commandType = AGENT_COMMAND_NONE;
		bool preferImmediate = false;
		bool hasCommandMapping = TryGetCommandTypeByStateId(m_stateId, commandType, preferImmediate);
		bool queued = false;
		if (hasCommandMapping)
		{
			if (preferImmediate)
			{
				queued = m_pAgent->ImmediateCommand(commandType, 1000, 0, "FSM_ON_ENTER");
			}
			else
			{
				queued = m_pAgent->QueueCommand(commandType, 0, 0, "FSM_ON_ENTER");
			}
		}

		if (!queued)
		{
			// Fallback path for agents that do not use command scheduling.
			m_pAgent->RequestAnimByFsmState(m_stateId);
		}
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
