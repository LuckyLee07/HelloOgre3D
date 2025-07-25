#ifndef __AGENT_STATE_CONTROLLER_H__
#define __AGENT_STATE_CONTROLLER_H__

#include <string>

class AgentFSM;
class AgentObject;
class AgentStateController //tolua_exports
{ //tolua_exports
public:
	AgentStateController(AgentObject* soldier);
	~AgentStateController();

	void Init();				// ע������״̬ + ��ת
	void Update(float dtime);	// ÿ֡���� FSM
	void ChangeState(const std::string& stateName); // �ֶ��л�

	std::string GetCurrState() const;

	//tolua_begin
	bool AddState(const std::string& name);
	void AddTransition(const std::string& from, const std::string& to);
	//tolua_end

private:
	AgentFSM* m_fsm;
	AgentObject* m_agent;

}; //tolua_exports

#endif  // __AGENT_STATE_CONTROLLER_H__
