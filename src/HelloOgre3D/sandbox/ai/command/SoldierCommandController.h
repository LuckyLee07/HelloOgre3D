#ifndef __SOLDIER_COMMAND_CONTROLLER__
#define __SOLDIER_COMMAND_CONTROLLER__

#include <memory>
#include <string>
#include "AgentCommandTypes.h"

class SoldierObject;

class SoldierCommandController
{
public:
	explicit SoldierCommandController(SoldierObject& owner);
	~SoldierCommandController();

	bool QueueCommand(AgentCommandType commandType, int priority = 0, int arg = 0, const std::string& source = "");
	bool ImmediateCommand(AgentCommandType commandType, int priority = 100, int arg = 0, const std::string& source = "");

	void ClearCommands();
	bool HasPendingCommands() const;
	bool HasCurrentCommand() const;
	int GetPendingCommandCount() const;
	AgentCommandType GetCurrentCommandType() const;
	AgentCommandType GetPreviousCommandType() const;

	void Update(int deltaMs);
	void HandleAsmStateChange(int stateId, int eventType);

private:
	static bool TryGetBodyAnimStateByCommandType(AgentCommandType commandType, int& outAnimState);
	static bool TryResolveWeaponAnimStateByCommandType(SoldierObject& owner, const AgentCommandType commandType, std::string& outWeaponStateName);
	static bool TryGetCommandTypeByAnimStateId(int animStateId, AgentCommandType& outCommandType);
	static bool IsShootAnimStateId(int animStateId);

	SoldierObject& m_owner;
	struct Impl;
	std::unique_ptr<Impl> m_impl;
};

#endif  // __SOLDIER_COMMAND_CONTROLLER__
