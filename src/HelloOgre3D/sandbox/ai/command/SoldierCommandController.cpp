#include "SoldierCommandController.h"

#include <deque>
#include "objects/SoldierObject.h"
#include "objects/animation/AgentAnimStateMachine.h"

namespace
{
	constexpr int kCommandEventFallbackMs = 600;

	struct CommandRequest
	{
		CommandRequest() = default;

		CommandRequest(AgentCommandType commandType,
			int commandPriority = 0,
			bool canInterrupt = false,
			int lifeTimeMs = -1,
			int commandArg = 0,
			const std::string& commandSource = "")
			: type(commandType)
			, priority(commandPriority)
			, interrupt(canInterrupt)
			, ttlMs(lifeTimeMs)
			, arg(commandArg)
			, source(commandSource)
		{
		}

		bool IsValid() const
		{
			return type != AGENT_COMMAND_NONE;
		}

		void ResetElapsed()
		{
			elapsedMs = 0;
		}

		void StepElapsed(int deltaMs)
		{
			if (deltaMs <= 0) return;
			elapsedMs += deltaMs;
		}

		bool IsExpired() const
		{
			return ttlMs > 0 && elapsedMs >= ttlMs;
		}

		AgentCommandType type = AGENT_COMMAND_NONE;
		int priority = 0;
		bool interrupt = false;
		int ttlMs = -1;
		int arg = 0;
		int elapsedMs = 0;
		std::string source;
	};

	class CommandScheduler
	{
	public:
		void Clear()
		{
			m_pendingCommands.clear();
			m_hasCurrentCommand = false;
			m_hasPreviousCommand = false;
		}

		void QueueCommand(const CommandRequest& request)
		{
			if (!request.IsValid()) return;
			m_pendingCommands.push_back(request);
		}

		void ImmediateCommand(const CommandRequest& request)
		{
			if (!request.IsValid()) return;

			CommandRequest immediateRequest = request;
			immediateRequest.ResetElapsed();
			if (m_hasCurrentCommand)
			{
				m_pendingCommands.push_front(m_currentCommand);
			}
			m_currentCommand = immediateRequest;
			m_hasCurrentCommand = true;
		}

		void Update(int deltaMs)
		{
			if (!m_hasCurrentCommand)
			{
				TryStartNextCommand();
				return;
			}

			m_currentCommand.StepElapsed(deltaMs);
			if (m_currentCommand.IsExpired())
			{
				FinishCurrentCommand();
				TryStartNextCommand();
			}
		}

		bool TryStartNextCommand()
		{
			if (m_hasCurrentCommand || m_pendingCommands.empty())
			{
				return false;
			}

			m_currentCommand = m_pendingCommands.front();
			m_currentCommand.ResetElapsed();
			m_pendingCommands.pop_front();
			m_hasCurrentCommand = true;
			return true;
		}

		void FinishCurrentCommand()
		{
			if (!m_hasCurrentCommand)
			{
				return;
			}

			m_previousCommand = m_currentCommand;
			m_hasPreviousCommand = true;
			m_hasCurrentCommand = false;
		}

		bool HasCurrentCommand() const { return m_hasCurrentCommand; }
		bool HasPendingCommands() const { return !m_pendingCommands.empty(); }
		bool HasCurrentOrPending() const { return m_hasCurrentCommand || !m_pendingCommands.empty(); }
		int GetPendingCommandCount() const { return static_cast<int>(m_pendingCommands.size()); }

		const CommandRequest* GetCurrentCommand() const
		{
			return m_hasCurrentCommand ? &m_currentCommand : nullptr;
		}

		const CommandRequest* GetPreviousCommand() const
		{
			return m_hasPreviousCommand ? &m_previousCommand : nullptr;
		}

	private:
		std::deque<CommandRequest> m_pendingCommands;
		CommandRequest m_currentCommand;
		CommandRequest m_previousCommand;
		bool m_hasCurrentCommand = false;
		bool m_hasPreviousCommand = false;
	};
}

struct SoldierCommandController::Impl
{
	CommandScheduler scheduler;
};

SoldierCommandController::SoldierCommandController(SoldierObject& owner)
	: m_owner(owner)
	, m_impl(new Impl())
{
}

SoldierCommandController::~SoldierCommandController() = default;

bool SoldierCommandController::TryGetAnimStateByCommandType(AgentCommandType commandType, int& outAnimState)
{
	switch (commandType)
	{
	case AGENT_COMMAND_IDLE:
		outAnimState = SSTATE_IDLE_AIM;
		return true;
	case AGENT_COMMAND_MOVE:
		outAnimState = SSTATE_RUN_FORWARD;
		return true;
	case AGENT_COMMAND_SHOOT:
		outAnimState = SSTATE_FIRE;
		return true;
	case AGENT_COMMAND_DIE:
		outAnimState = SSTATE_DEAD;
		return true;
	default:
		return false;
	}
}

bool SoldierCommandController::TryGetCommandTypeByAnimStateId(int animStateId, AgentCommandType& outCommandType)
{
	if (animStateId == SSTATE_IDLE_AIM || animStateId == CROUCH_SSTATE_IDLE_AIM)
	{
		outCommandType = AGENT_COMMAND_IDLE;
		return true;
	}
	if (animStateId == SSTATE_RUN_FORWARD || animStateId == CROUCH_SSTATE_FORWARD)
	{
		outCommandType = AGENT_COMMAND_MOVE;
		return true;
	}
	if (animStateId == SSTATE_FIRE || animStateId == CROUCH_SSTATE_FIRE)
	{
		outCommandType = AGENT_COMMAND_SHOOT;
		return true;
	}
	if (animStateId == SSTATE_DEAD || animStateId == CROUCH_SSTATE_DEAD)
	{
		outCommandType = AGENT_COMMAND_DIE;
		return true;
	}

	return false;
}

bool SoldierCommandController::QueueCommand(AgentCommandType commandType, int priority /*= 0*/, int arg /*= 0*/, const std::string& source /*= ""*/)
{
	CommandRequest request(commandType, priority, false, -1, arg, source);
	m_impl->scheduler.QueueCommand(request);
	return request.IsValid();
}

bool SoldierCommandController::ImmediateCommand(AgentCommandType commandType, int priority /*= 100*/, int arg /*= 0*/, const std::string& source /*= ""*/)
{
	if (commandType == AGENT_COMMAND_DIE)
	{
		// Terminal command should preempt and discard queued intents.
		m_impl->scheduler.Clear();
	}

	CommandRequest request(commandType, priority, true, -1, arg, source);
	m_impl->scheduler.ImmediateCommand(request);
	return request.IsValid();
}

void SoldierCommandController::ClearCommands()
{
	m_impl->scheduler.Clear();
}

bool SoldierCommandController::HasPendingCommands() const
{
	return m_impl->scheduler.HasPendingCommands();
}

bool SoldierCommandController::HasCurrentCommand() const
{
	return m_impl->scheduler.HasCurrentCommand();
}

int SoldierCommandController::GetPendingCommandCount() const
{
	return m_impl->scheduler.GetPendingCommandCount();
}

AgentCommandType SoldierCommandController::GetCurrentCommandType() const
{
	const CommandRequest* current = m_impl->scheduler.GetCurrentCommand();
	return current ? current->type : AGENT_COMMAND_NONE;
}

AgentCommandType SoldierCommandController::GetPreviousCommandType() const
{
	const CommandRequest* previous = m_impl->scheduler.GetPreviousCommand();
	return previous ? previous->type : AGENT_COMMAND_NONE;
}

void SoldierCommandController::HandleAsmStateChange(int stateId, int eventType)
{
	const CommandRequest* command = m_impl->scheduler.GetCurrentCommand();
	if (command == nullptr)
	{
		return;
	}

	AgentCommandType eventCommandType = AGENT_COMMAND_NONE;
	if (!TryGetCommandTypeByAnimStateId(stateId, eventCommandType))
	{
		return;
	}

	if (eventCommandType != command->type)
	{
		return;
	}

	const bool isEnterEvent = (eventType == AgentAnimStateMachine::ASM_EVENT_ENTER || eventType == 0);
	const bool isLoopEvent = (eventType == AgentAnimStateMachine::ASM_EVENT_LOOP);

	if (command->type == AGENT_COMMAND_DIE)
	{
		if (isEnterEvent && m_owner.GetHealth() > 0.0f)
		{
			m_owner.SetHealth(0.0f);
		}
		return;
	}

	bool canAdvance = false;
	if (command->type == AGENT_COMMAND_SHOOT)
	{
		canAdvance = isEnterEvent || isLoopEvent;
	}
	else
	{
		canAdvance = isEnterEvent;
	}

	if (canAdvance && m_impl->scheduler.HasPendingCommands())
	{
		m_impl->scheduler.FinishCurrentCommand();
	}
}

void SoldierCommandController::Update(int deltaMs)
{
	if (!m_impl->scheduler.HasCurrentOrPending())
	{
		return;
	}

	m_impl->scheduler.Update(deltaMs);
	m_impl->scheduler.TryStartNextCommand();

	const CommandRequest* command = m_impl->scheduler.GetCurrentCommand();
	if (command == nullptr)
	{
		return;
	}

	if (command->type == AGENT_COMMAND_IDLE)
	{
		if (m_owner.IsMoving())
		{
			m_owner.SlowMoving(2.0f);
		}
	}
	else if (command->type == AGENT_COMMAND_SHOOT)
	{
		if (m_owner.IsMoving())
		{
			m_owner.SlowMoving(1.0f);
		}
	}
	else if (command->type == AGENT_COMMAND_DIE)
	{
		if (m_owner.IsMoving())
		{
			m_owner.SlowMoving(2.0f);
		}
	}

	if (command->type == AGENT_COMMAND_CHANGE_STANCE)
	{
		CommandRequest resumeRequest;
		bool hasResumeRequest = false;
		if (!m_impl->scheduler.HasPendingCommands())
		{
			const CommandRequest* previous = m_impl->scheduler.GetPreviousCommand();
			if (previous &&
				previous->type != AGENT_COMMAND_CHANGE_STANCE &&
				previous->type != AGENT_COMMAND_DIE &&
				previous->type != AGENT_COMMAND_NONE)
			{
				resumeRequest = *previous;
				resumeRequest.interrupt = true;
				resumeRequest.ResetElapsed();
				hasResumeRequest = true;
			}
		}

		m_owner.changeStanceType(command->arg);
		m_impl->scheduler.FinishCurrentCommand();
		if (hasResumeRequest)
		{
			m_impl->scheduler.ImmediateCommand(resumeRequest);
		}
		return;
	}

	int requestState = SSTATE_MAXCOUNT;
	if (!TryGetAnimStateByCommandType(command->type, requestState))
	{
		m_impl->scheduler.FinishCurrentCommand();
		return;
	}

	if (command->type == AGENT_COMMAND_SHOOT)
	{
		m_owner.RequestState(requestState, true);
	}
	else
	{
		m_owner.RequestState(requestState, false);
	}

	// Keep terminal command executing forever, aligned with old DIE command semantics.
	if (command->type == AGENT_COMMAND_DIE)
	{
		return;
	}

	// Fallback: only timeout-based advancement when expected ASM events are missing.
	if (m_impl->scheduler.HasPendingCommands() &&
		command->elapsedMs >= kCommandEventFallbackMs)
	{
		m_impl->scheduler.FinishCurrentCommand();
	}
}
