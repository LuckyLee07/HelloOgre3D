#ifndef __AGENT_LUA_STATE_H__
#define __AGENT_LUA_STATE_H__

#include "AgentState.h"
#include "LuaEnvObject.h"

class AgentObject;

class AgentLuaState : public AgentState //tolua_exports
	, public LuaEnvObject
{ //tolua_exports
public:
	AgentLuaState(AgentObject* pAgent);
	virtual ~AgentLuaState();

	virtual void doBeforeEntering();
	virtual void doBeforeLeaving();

	virtual std::string OnUpdate(float dt);
}; //tolua_exports

#endif  // __AGENT_LUA_STATE_H__
