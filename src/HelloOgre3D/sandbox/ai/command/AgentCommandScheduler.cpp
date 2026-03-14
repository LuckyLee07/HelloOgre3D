#include "AgentCommandScheduler.h"

AgentCommandRequest::AgentCommandRequest(AgentCommandType commandType,
	int commandPriority /*= 0*/,
	bool canInterrupt /*= false*/,
	int lifeTimeMs /*= -1*/,
	int commandArg /*= 0*/,
	const std::string& commandSource /*= ""*/)
	: type(commandType)
	, priority(commandPriority)
	, interrupt(canInterrupt)
	, ttlMs(lifeTimeMs)
	, arg(commandArg)
	, source(commandSource)
{
}

bool AgentCommandRequest::IsValid() const
{
	return type != AGENT_COMMAND_NONE;
}

void AgentCommandRequest::ResetElapsed()
{
	elapsedMs = 0;
}

void AgentCommandRequest::StepElapsed(int deltaMs)
{
	if (deltaMs <= 0) return;
	elapsedMs += deltaMs;
}

bool AgentCommandRequest::IsExpired() const
{
	return ttlMs > 0 && elapsedMs >= ttlMs;
}

void AgentCommandScheduler::Clear()
{
	m_pendingCommands.clear();
	m_hasCurrentCommand = false;
	m_hasPreviousCommand = false;
}

void AgentCommandScheduler::QueueCommand(const AgentCommandRequest& request)
{
	if (!request.IsValid()) return;

	m_pendingCommands.push_back(request);
}

void AgentCommandScheduler::ImmediateCommand(const AgentCommandRequest& request)
{
	if (!request.IsValid()) return;

	AgentCommandRequest immediateRequest = request;
	immediateRequest.ResetElapsed();

	if (m_hasCurrentCommand)
	{
		m_pendingCommands.push_front(m_currentCommand);
	}

	m_currentCommand = immediateRequest;
	m_hasCurrentCommand = true;
}

void AgentCommandScheduler::Update(int deltaMs)
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

bool AgentCommandScheduler::TryStartNextCommand()
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

void AgentCommandScheduler::FinishCurrentCommand()
{
	if (!m_hasCurrentCommand)
	{
		return;
	}

	m_previousCommand = m_currentCommand;
	m_hasPreviousCommand = true;
	m_hasCurrentCommand = false;
}

bool AgentCommandScheduler::HasCurrentCommand() const
{
	return m_hasCurrentCommand;
}

bool AgentCommandScheduler::HasPendingCommands() const
{
	return !m_pendingCommands.empty();
}

bool AgentCommandScheduler::HasCurrentOrPending() const
{
	return m_hasCurrentCommand || !m_pendingCommands.empty();
}

int AgentCommandScheduler::GetPendingCommandCount() const
{
	return static_cast<int>(m_pendingCommands.size());
}

const AgentCommandRequest* AgentCommandScheduler::GetCurrentCommand() const
{
	if (!m_hasCurrentCommand)
	{
		return nullptr;
	}
	return &m_currentCommand;
}

const AgentCommandRequest* AgentCommandScheduler::GetPreviousCommand() const
{
	if (!m_hasPreviousCommand)
	{
		return nullptr;
	}
	return &m_previousCommand;
}
