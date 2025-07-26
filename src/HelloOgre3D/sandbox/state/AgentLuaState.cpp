#include "AgentLuaState.h"
#include "object/AgentObject.h"

AgentLuaState::AgentLuaState(AgentObject* pAgent) : AgentState(pAgent)
{

}

AgentLuaState::~AgentLuaState()
{
}

void AgentLuaState::doBeforeEntering()
{
	
}

void AgentLuaState::doBeforeLeaving()
{

}

std::string AgentLuaState::OnUpdate(float dt)
{
	return "";
}
