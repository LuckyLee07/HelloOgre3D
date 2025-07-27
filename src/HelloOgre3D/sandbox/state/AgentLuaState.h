#ifndef __AGENT_LUA_STATE_H__
#define __AGENT_LUA_STATE_H__

#include "AgentState.h"
#include "LuaEnvObject.h"
#include "LuaClassNameTraits.h"

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

	bool BindToScript(const std::string& filepath);

}; //tolua_exports

REGISTER_LUA_CLASS_NAME(AgentLuaState);

#endif  // __AGENT_LUA_STATE_H__
