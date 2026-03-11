#ifndef __AGENT_FMS_H__
#define __AGENT_FMS_H__

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class AgentState;
class AgentFSM //tolua_exports
{ //tolua_exports
public:
	using TransitionEvaluator = std::function<bool()>;

	AgentFSM();
	~AgentFSM();

	void Update(float dt);

	//tolua_begin
	void AddState(AgentState* state);
	void RequestState(const std::string& stateName);
	void AddTransition(const std::string& from, const std::string& to);
	//tolua_end

	void AddTransitionEvaluator(const std::string& from, const std::string& to, TransitionEvaluator evaluator);
	void ClearTransitionEvaluators(const std::string& fromState = "");

	void SetCurrentState(const std::string& stateName);

	AgentState* GetCurrState() { return m_currState; }
	std::string GetCurrStateName() const { return m_currStateName; }

private:
	struct TransitionRule
	{
		std::string toState;
		TransitionEvaluator evaluator;
	};

	bool ContainsState(const std::string& stateName);
	bool ContainsTransition(const std::string& from, const std::string& to);

	bool EvaluateTransitionRules();

private:
	// 所有注册的状态：状态名 → 状态对象
	std::unordered_map<std::string, AgentState*> m_states;

	// 状态跳转表：从状态名 → 可跳转的目标状态名集合
	std::unordered_map<std::string, std::unordered_set<std::string>> m_transitions;
	std::unordered_map<std::string, std::vector<TransitionRule>> m_transitionRules;

	// 当前状态
	AgentState* m_currState = nullptr;
	std::string m_currStateName;

}; //tolua_exports
#endif  // __AGENT_FMS_H__
