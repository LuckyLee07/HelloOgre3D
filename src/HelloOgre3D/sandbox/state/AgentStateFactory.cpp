#include "AgentStateFactory.h"
#include "IdleState.h"
#include "MoveState.h"
#include "DeathState.h"
#include "ShootState.h"
#include "AgentLuaState.h"

std::map<std::string, std::function<AgentState*(AgentObject* agent)>> AgentStateFactory::m_registers;

bool AgentStateFactory::Init()
{
	RegisterCreator<IdleState>("IdleState");
	//RegisterCreator<MoveState>("MoveState");
	//RegisterCreator<FallState>("FallState");
	RegisterCreator<DeathState>("DeathState");
	RegisterCreator<ShootState>("ShootState");

	RegisterCreator<AgentLuaState>("AgentLuaState");

	return true;
}

AgentState* AgentStateFactory::Create(const char* type, AgentObject* agent)
{
	if (type == nullptr)
		return nullptr;

	auto iter = m_registers.find(type);
	if (iter == m_registers.end())
		return nullptr;

	return iter->second(agent);
}