#include "SoldierCommandController.h"

#include <deque>
#include "objects/SoldierObject.h"
#include "objects/RenderableObject.h"
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

bool SoldierCommandController::TryGetBodyAnimStateByCommandType(AgentCommandType commandType, int& outAnimState)
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
	case AGENT_COMMAND_RELOAD:
		outAnimState = SSTATE_RELOAD;
		return true;
	case AGENT_COMMAND_WEAPON_TO_SMG:
		outAnimState = SSTATE_SMG_TRANSFORM;
		return true;
	case AGENT_COMMAND_WEAPON_TO_SNIPER:
		outAnimState = SSTATE_SNIPER_TRANSFORM;
		return true;
	default:
		return false;
	}
}

bool SoldierCommandController::TryResolveWeaponAnimStateByCommandType(SoldierObject& owner, const AgentCommandType commandType, std::string& outWeaponStateName)
{
	RenderableObject* weapon = owner.getWeapon();
	if (weapon == nullptr)
	{
		return false;
	}

	AgentAnimStateMachine* weaponAsm = weapon->GetObjectASM();
	if (weaponAsm == nullptr)
	{
		return false;
	}

	const std::string currentState = weaponAsm->GetCurrStateName();
	const bool isSmgMode = (currentState == "smg_idle" || currentState == "smg_transform");
	switch (commandType)
	{
	case AGENT_COMMAND_IDLE:
	case AGENT_COMMAND_MOVE:
	case AGENT_COMMAND_SHOOT:
	case AGENT_COMMAND_DIE:
	case AGENT_COMMAND_FALLING:
	case AGENT_COMMAND_CHANGE_STANCE:
		outWeaponStateName = isSmgMode ? "smg_idle" : "sniper_idle";
		return true;
	case AGENT_COMMAND_RELOAD:
		outWeaponStateName = isSmgMode ? "smg_idle" : "sniper_reload";
		return true;
	case AGENT_COMMAND_WEAPON_TO_SMG:
		outWeaponStateName = isSmgMode ? "smg_idle" : "smg_transform";
		return true;
	case AGENT_COMMAND_WEAPON_TO_SNIPER:
		outWeaponStateName = isSmgMode ? "sniper_transform" : "sniper_idle";
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
	if (animStateId == SSTATE_FALL_IDLE || animStateId == SSTATE_FALL_DEAD)
	{
		outCommandType = AGENT_COMMAND_FALLING;
		return true;
	}
	if (animStateId == SSTATE_RELOAD)
	{
		outCommandType = AGENT_COMMAND_RELOAD;
		return true;
	}
	if (animStateId == SSTATE_SMG_TRANSFORM)
	{
		outCommandType = AGENT_COMMAND_WEAPON_TO_SMG;
		return true;
	}
	if (animStateId == SSTATE_SNIPER_TRANSFORM)
	{
		outCommandType = AGENT_COMMAND_WEAPON_TO_SNIPER;
		return true;
	}

	return false;
}

bool SoldierCommandController::IsShootAnimStateId(int animStateId)
{
	return animStateId == SSTATE_FIRE || animStateId == CROUCH_SSTATE_FIRE;
}

namespace
{
	bool CanUseRealtimeHandoff(AgentCommandType commandType)
	{
		return commandType == AGENT_COMMAND_IDLE ||
			commandType == AGENT_COMMAND_MOVE ||
			commandType == AGENT_COMMAND_SHOOT;
	}
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
	const bool isEnterEvent = (eventType == AgentAnimStateMachine::ASM_EVENT_ENTER || eventType == 0);
	const bool isLoopEvent = (eventType == AgentAnimStateMachine::ASM_EVENT_LOOP);

	// Keep shoot side effects in command controller so SoldierObject stays a pure bridge.
	if ((isEnterEvent || isLoopEvent) && IsShootAnimStateId(stateId))
	{
		m_owner.ShootBullet();
	}

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

	if (command->type == AGENT_COMMAND_DIE)
	{
		if (isEnterEvent && m_owner.GetHealth() > 0.0f)
		{
			m_owner.SetHealth(0.0f);
		}
		return;
	}
	if (command->type == AGENT_COMMAND_FALLING)
	{
		if (isEnterEvent && stateId == SSTATE_FALL_DEAD && m_owner.GetHealth() > 0.0f)
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
	const CommandRequest* currentBeforeUpdate = m_impl->scheduler.GetCurrentCommand();
	const bool isDeadBeforeUpdate = (currentBeforeUpdate && currentBeforeUpdate->type == AGENT_COMMAND_DIE);
	const bool isFallingBeforeUpdate = (currentBeforeUpdate && currentBeforeUpdate->type == AGENT_COMMAND_FALLING);
	if (m_owner.GetHealth() > 0.0f &&
		!isDeadBeforeUpdate &&
		m_owner.IsFalling() &&
		!isFallingBeforeUpdate)
	{
		CommandRequest request(AGENT_COMMAND_FALLING, 900, true, -1, 0, "AUTO_FALLING");
		m_impl->scheduler.ImmediateCommand(request);
	}

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

	// Old-controller style handoff: once current animation state is already serving
	// a loopable command and a newer command is waiting, hand off immediately.
	if (m_impl->scheduler.HasPendingCommands() && CanUseRealtimeHandoff(command->type))
	{
		AgentCommandType currentStateCommandType = AGENT_COMMAND_NONE;
		const int currentBodyState = m_owner.GetCurStateId();
		if (TryGetCommandTypeByAnimStateId(currentBodyState, currentStateCommandType) &&
			currentStateCommandType == command->type)
		{
			m_impl->scheduler.FinishCurrentCommand();
			m_impl->scheduler.TryStartNextCommand();
			command = m_impl->scheduler.GetCurrentCommand();
			if (command == nullptr)
			{
				return;
			}
		}
	}

	if (command->type == AGENT_COMMAND_IDLE)
	{
		if (m_owner.IsMoving())
		{
			m_owner.SlowMoving(2.0f);
		}
	}
	else if (command->type == AGENT_COMMAND_MOVE)
	{
		// If body still stays on shoot pose during fire->run transition,
		// keep damping residual velocity to avoid visible static sliding.
		const int currentBodyState = m_owner.GetCurStateId();
		const bool hasPendingAnimTransition = m_owner.HasNextAnim();
		if (IsShootAnimStateId(currentBodyState) &&
			!hasPendingAnimTransition &&
			m_owner.IsMoving())
		{
			m_owner.SlowMoving(1.5f);
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
	else if (command->type == AGENT_COMMAND_RELOAD ||
		command->type == AGENT_COMMAND_WEAPON_TO_SMG ||
		command->type == AGENT_COMMAND_WEAPON_TO_SNIPER)
	{
		if (m_owner.IsMoving())
		{
			m_owner.SlowMoving(1.0f);
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
	if (command->type == AGENT_COMMAND_FALLING)
	{
		const int currStateId = m_owner.GetCurStateId();
		if (m_owner.IsFalling())
		{
			if (currStateId != SSTATE_FALL_IDLE && currStateId != SSTATE_FALL_DEAD)
			{
				m_owner.RequestState(SSTATE_FALL_IDLE, false);
			}
		}
		else
		{
			if (currStateId != SSTATE_FALL_DEAD)
			{
				m_owner.RequestState(SSTATE_FALL_DEAD, false);
			}
			else if (m_owner.GetHealth() > 0.0f)
			{
				// Event-driven path is primary. This keeps behavior forward-progressing if events are lost.
				m_owner.SetHealth(0.0f);
			}
		}

		std::string weaponStateName;
		if (TryResolveWeaponAnimStateByCommandType(m_owner, command->type, weaponStateName))
		{
			m_owner.RequestWeaponState(weaponStateName, false);
		}
		return;
	}

	int requestState = SSTATE_MAXCOUNT;
	if (!TryGetBodyAnimStateByCommandType(command->type, requestState))
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

	std::string weaponStateName;
	if (TryResolveWeaponAnimStateByCommandType(m_owner, command->type, weaponStateName))
	{
		m_owner.RequestWeaponState(weaponStateName, false);
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
