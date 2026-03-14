#ifndef __AGENT_COMMAND_SCHEDULER__
#define __AGENT_COMMAND_SCHEDULER__

#include <deque>
#include <string>

enum AgentCommandType
{
	AGENT_COMMAND_NONE = 0,
	AGENT_COMMAND_IDLE,
	AGENT_COMMAND_MOVE,
	AGENT_COMMAND_SHOOT,
	AGENT_COMMAND_DIE,
	AGENT_COMMAND_CHANGE_STANCE
};

struct AgentCommandRequest
{
	AgentCommandRequest() = default;

	AgentCommandRequest(AgentCommandType commandType,
		int commandPriority = 0,
		bool canInterrupt = false,
		int lifeTimeMs = -1,
		int commandArg = 0,
		const std::string& commandSource = "");

	bool IsValid() const;
	void ResetElapsed();
	void StepElapsed(int deltaMs);
	bool IsExpired() const;

	AgentCommandType type = AGENT_COMMAND_NONE;
	int priority = 0;
	bool interrupt = false;
	int ttlMs = -1;
	int arg = 0;
	int elapsedMs = 0;
	std::string source;
};

class AgentCommandScheduler
{
public:
	void Clear();

	void QueueCommand(const AgentCommandRequest& request);
	void ImmediateCommand(const AgentCommandRequest& request);

	void Update(int deltaMs);

	bool TryStartNextCommand();
	void FinishCurrentCommand();

	bool HasCurrentCommand() const;
	bool HasPendingCommands() const;
	bool HasCurrentOrPending() const;
	int GetPendingCommandCount() const;

	const AgentCommandRequest* GetCurrentCommand() const;
	const AgentCommandRequest* GetPreviousCommand() const;

private:
	std::deque<AgentCommandRequest> m_pendingCommands;
	AgentCommandRequest m_currentCommand;
	AgentCommandRequest m_previousCommand;
	bool m_hasCurrentCommand = false;
	bool m_hasPreviousCommand = false;
};

#endif // __AGENT_COMMAND_SCHEDULER__
