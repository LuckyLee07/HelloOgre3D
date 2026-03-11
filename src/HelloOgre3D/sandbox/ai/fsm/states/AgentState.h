#ifndef __AGENT_STATE_H__
#define __AGENT_STATE_H__

#include "object/SandboxObject.h"
#include <string>

class AgentObject;
class AgentStateController;

class AgentState : public SandboxObject
{
public:
	AgentState(AgentObject* pAgent);
	virtual ~AgentState();

	virtual void OnEnter();
	virtual void OnLeave();

	virtual std::string OnUpdate(float dt) = 0;

	virtual void doBeforeEntering() {}
	virtual void doBeforeLeaving() {}

	void SetStateId(const std::string& stateId);
	std::string GetStateId() const { return m_stateId; }

	void SetController(AgentStateController* controller) { m_controller = controller; }
	AgentStateController* GetController() const { return m_controller; }

	void SetTerminated(bool terminated) { m_terminated = terminated; }
	bool IsTerminated() const { return m_terminated; }

protected:
	std::string m_stateId;

	AgentObject* m_pAgent;
	AgentStateController* m_controller;
	bool m_terminated;
};

#endif  // __ANIMATION_STATE_H__
