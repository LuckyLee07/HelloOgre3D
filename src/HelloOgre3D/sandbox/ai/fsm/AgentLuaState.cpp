#include "AgentLuaState.h"
#include "objects/AgentObject.h"
#include "systems/manager/LuaPluginMgr.h"

AgentLuaState::AgentLuaState(AgentObject* pAgent) : AgentState(pAgent)
{

}

AgentLuaState::~AgentLuaState()
{
}

void AgentLuaState::doBeforeEntering()
{
	callFunction("AgentState_OnEnter", "u[AgentObject]", m_pAgent);
}

void AgentLuaState::doBeforeLeaving()
{
	callFunction("AgentState_OnLeave", "u[AgentObject]", m_pAgent);
}

std::string AgentLuaState::OnUpdate(float dt)
{
	char outState[64] = { 0 };
	callFunction("AgentState_OnUpdate", "u[AgentObject]i>s", m_pAgent, (int)dt, outState);

	return outState;
}

bool AgentLuaState::BindToScript(const std::string& filepath)
{
	return LuaPluginMgr::BindLuaPluginByFile(this, filepath);
}