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

	void Init();				// 注册所有状态 + 跳转
	void Update(float dtime);	// 每帧驱动 FSM
	void ChangeState(const std::string& stateName); // 手动切换

	AgentState* GetCurrState() const; // 获取当前State

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
