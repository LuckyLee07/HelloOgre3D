#ifndef __AGENT_STATE_CONTROLLER_H__
#define __AGENT_STATE_CONTROLLER_H__

#include <string>

class AgentState;
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

	AgentState* GetCurrState() const; // ��ȡ��ǰState

	//tolua_begin
	bool AddState(const std::string& name);
	bool AddStateExByLua(const std::string& name, const std::string& filepath);

	void AddTransition(const std::string& from, const std::string& to);
	//tolua_end

private:
	AgentFSM* m_fsm;
	AgentObject* m_agent;

}; //tolua_exports

#endif  // __AGENT_STATE_CONTROLLER_H__
