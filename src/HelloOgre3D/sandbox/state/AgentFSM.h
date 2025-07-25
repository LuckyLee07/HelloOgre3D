#ifndef __AGENT_FMS_H__
#define __AGENT_FMS_H__

#include <string>
#include <unordered_map>
#include <unordered_set>

class AgentState;
class AgentFSM //tolua_exports
{ //tolua_exports
public:
	AgentFSM();
	~AgentFSM();

	void Update(float dt);

	//tolua_begin
	void AddState(AgentState* state);
	void RequestState(const std::string& stateName);
	void AddTransition(const std::string& from, const std::string& to);
	//tolua_end
	void SetCurrentState(const std::string& stateName);

	std::string GetCurrStateName() const { return m_currStateName; }

private:
	bool ContainsState(const std::string& stateName);
	bool ContainsTransition(const std::string& from, const std::string& to);

private:
	// 所有注册的状态：状态名 → 状态对象
	std::unordered_map<std::string, AgentState*> m_states;

	// 状态跳转表：从状态名 → 可跳转的目标状态名集合
	std::unordered_map<std::string, std::unordered_set<std::string>> m_transitions;

	// 当前状态
	AgentState* m_currState = nullptr;
	std::string m_currStateName;

}; //tolua_exports
#endif  // __AGENT_FMS_H__
