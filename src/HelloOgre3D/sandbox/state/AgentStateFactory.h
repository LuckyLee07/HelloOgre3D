#ifndef __AGENT_STATE_FACTORY_H__
#define __AGENT_STATE_FACTORY_H__

#include <map>
#include <string>
#include <functional>

class AgentState;
class AgentObject;
class AgentStateFactory
{
public:
	static bool Init();

	static AgentState* Create(const char* type, AgentObject* agent);

	template<typename T>
	static bool RegisterCreator(const char* type);

private:
	static std::map<std::string, std::function<AgentState*(AgentObject* agent)>> m_registers;
};

template<typename T>
bool AgentStateFactory::RegisterCreator(const char* type)
{
	if (type == nullptr) 
		return false;

	m_registers[type] = [](AgentObject* agent) -> AgentState* {
		auto stateIns = new T(agent);
		return static_cast<AgentState*>(stateIns);
	};
	return true;
}

#endif  // __ANIMATION_STATE_H__
