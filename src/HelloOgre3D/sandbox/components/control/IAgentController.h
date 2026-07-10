#ifndef __I_AGENT_CONTROLLER_H__
#define __I_AGENT_CONTROLLER_H__

enum AgentControllerKind
{
	AGENT_CONTROLLER_AI = 0,
	AGENT_CONTROLLER_PLAYER,
};

class IAgentController
{
public:
	virtual ~IAgentController() {}

	virtual AgentControllerKind GetControllerKind() const = 0;
	virtual const char* GetControllerName() const = 0;
};

#endif // __I_AGENT_CONTROLLER_H__
