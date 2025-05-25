#ifndef __AGENT_FMS_H__
#define __AGENT_FMS_H__

#include <string>
#include <unordered_map>

class AgentState;
class AgentFSM //tolua_exports
{ //tolua_exports
public:
	AgentFSM();
	~AgentFSM();

	void Update(float deltaMilliseconds);

	//tolua_begin
	void RequestState(const std::string& stateName);
	void AddState(AgentState* animState);
	void AddTransition(const std::string& fromState, const std::string& toState);

	void SetCurrentState(const std::string& stateName);

private:
	bool ContainsState(const std::string& stateName);
	bool ContainsTransition(const std::string& fromState, const std::string& toState);

private:
	std::unordered_map<std::string, AgentState*> m_animStates;
	typedef std::unordered_map<std::string, std::string*> TransitionMap;
	std::unordered_map<std::string, TransitionMap> m_animTransitions;

	AgentState* m_pCurrentState;
	AgentState* m_pNextState;
	float m_TransitionStartTime;
}; //tolua_exports
#endif  // __AGENT_FMS_H__
