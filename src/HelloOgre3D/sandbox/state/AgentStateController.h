#ifndef __AGENT_STATE_CONTROLLER_H__
#define __AGENT_STATE_CONTROLLER_H__

#include <string>

class AgentFSM;
class SoldierObject;
class AgentStateController //tolua_exports
{ //tolua_exports
public:
	AgentStateController(SoldierObject* soldier);
	~AgentStateController();

	void Init();				// 注册所有状态 + 跳转
	void Update(float dtime);	// 每帧驱动 FSM
	void ChangeState(const std::string& stateName); // 手动切换

	std::string GetCurrState() const;

	//tolua_begin
	bool AddState(const std::string& name);
	void AddTransition(const std::string& from, const std::string& to);
	//tolua_end

private:
	AgentFSM* m_fsm;
	SoldierObject* m_agent;

}; //tolua_exports

#endif  // __AGENT_STATE_CONTROLLER_H__
