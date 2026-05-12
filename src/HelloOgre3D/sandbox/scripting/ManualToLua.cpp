#include "tolua++.h"
#include "object/LuaEnvObject.h"
#include "objects/AgentObject.h"
#include "ai/fsm/states/AgentLuaState.h"
#include "ai/decision/DecisionBranch.h"
#include "ai/decision/LuaDecisionAction.h"
#include "ai/behavior/LuaBehaviorAction.h"
#include "ai/behavior/LuaCondition.h"

extern "C" {
#include "lauxlib.h"
}

/* Exported function */
TOLUA_API int tolua_SandboxToLua_Manual(lua_State* tolua_S);


/* method: setPluginEnv of class AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_setPluginEnv00
static int tolua_ClientToLua_AgentObject_setPluginEnv00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_istable(tolua_S, 2, 0, &tolua_err) ||   // 改为检查索引2是否为 table
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  lua_State* L =  tolua_S;
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setPluginEnv'", NULL);
#endif
  {
   self->setPluginEnv(L);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setPluginEnv'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setPluginEnv of class AgentLuaState */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentLuaState_setPluginEnv00
static int tolua_ClientToLua_AgentLuaState_setPluginEnv00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (
        !tolua_isusertype(tolua_S, 1, "AgentLuaState", 0, &tolua_err) ||
        !tolua_istable(tolua_S, 2, 0, &tolua_err) ||   // 改为检查索引2是否为 table
        !tolua_isnoobj(tolua_S, 3, &tolua_err)
        )
        goto tolua_lerror;
    else
#endif
    {
        AgentLuaState* self = (AgentLuaState*)tolua_tousertype(tolua_S, 1, 0);
        lua_State* L = tolua_S;
#ifndef TOLUA_RELEASE
        if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setPluginEnv'", NULL);
#endif
        {
            self->setPluginEnv(L);
        }
    }
    return 0;
#ifndef TOLUA_RELEASE
    tolua_lerror :
    tolua_error(tolua_S, "#ferror in function 'setPluginEnv'.", &tolua_err);
    return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE


/* method: SetEvaluator of class DecisionBranch
 * Captures a Lua function from the call site and stores its registry ref on the branch.
 * Usage in Lua: branch:SetEvaluator(function() ... return N end)
 */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DecisionBranch_SetEvaluator00
static int tolua_SandboxToLua_DecisionBranch_SetEvaluator00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (!tolua_isusertype(tolua_S, 1, "DecisionBranch", 0, &tolua_err) ||
	    !lua_isfunction(tolua_S, 2) ||
	    !tolua_isnoobj(tolua_S, 3, &tolua_err))
	{
		goto tolua_lerror;
	}
	else
#endif
	{
		DecisionBranch* self = (DecisionBranch*)tolua_tousertype(tolua_S, 1, 0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'SetEvaluator'", NULL);
#endif
		// Duplicate the function to the top of stack, then luaL_ref consumes it from there.
		lua_pushvalue(tolua_S, 2);
		int ref = luaL_ref(tolua_S, LUA_REGISTRYINDEX);
		self->SetEvaluatorRef(ref);
	}
	return 0;
#ifndef TOLUA_RELEASE
tolua_lerror:
	tolua_error(tolua_S, "#ferror in function 'SetEvaluator'.", &tolua_err);
	return 0;
#endif
}
#endif

/* method: setPluginEnv of class LuaDecisionAction (mirrors AgentLuaState pattern) */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_LuaDecisionAction_setPluginEnv00
static int tolua_SandboxToLua_LuaDecisionAction_setPluginEnv00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (!tolua_isusertype(tolua_S, 1, "LuaDecisionAction", 0, &tolua_err) ||
	    !tolua_istable(tolua_S, 2, 0, &tolua_err) ||
	    !tolua_isnoobj(tolua_S, 3, &tolua_err))
	{
		goto tolua_lerror;
	}
	else
#endif
	{
		LuaDecisionAction* self = (LuaDecisionAction*)tolua_tousertype(tolua_S, 1, 0);
		lua_State* L = tolua_S;
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setPluginEnv'", NULL);
#endif
		self->setPluginEnv(L);
	}
	return 0;
#ifndef TOLUA_RELEASE
tolua_lerror:
	tolua_error(tolua_S, "#ferror in function 'setPluginEnv'.", &tolua_err);
	return 0;
#endif
}
#endif

/* method: setPluginEnv of class LuaBehaviorAction (mirrors LuaDecisionAction pattern) */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_LuaBehaviorAction_setPluginEnv00
static int tolua_SandboxToLua_LuaBehaviorAction_setPluginEnv00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (!tolua_isusertype(tolua_S, 1, "LuaBehaviorAction", 0, &tolua_err) ||
	    !tolua_istable(tolua_S, 2, 0, &tolua_err) ||
	    !tolua_isnoobj(tolua_S, 3, &tolua_err))
	{
		goto tolua_lerror;
	}
	else
#endif
	{
		LuaBehaviorAction* self = (LuaBehaviorAction*)tolua_tousertype(tolua_S, 1, 0);
		lua_State* L = tolua_S;
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setPluginEnv'", NULL);
#endif
		self->setPluginEnv(L);
	}
	return 0;
#ifndef TOLUA_RELEASE
tolua_lerror:
	tolua_error(tolua_S, "#ferror in function 'setPluginEnv'.", &tolua_err);
	return 0;
#endif
}
#endif

/* method: SetEvaluator of class LuaCondition
 * 同 DecisionBranch.SetEvaluator：捕获 Lua 闭包到 registry，存 ref。
 * Usage: cond:SetEvaluator(function() return agent:HasEnemy() end)
 */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_LuaCondition_SetEvaluator00
static int tolua_SandboxToLua_LuaCondition_SetEvaluator00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (!tolua_isusertype(tolua_S, 1, "LuaCondition", 0, &tolua_err) ||
	    !lua_isfunction(tolua_S, 2) ||
	    !tolua_isnoobj(tolua_S, 3, &tolua_err))
	{
		goto tolua_lerror;
	}
	else
#endif
	{
		LuaCondition* self = (LuaCondition*)tolua_tousertype(tolua_S, 1, 0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'SetEvaluator'", NULL);
#endif
		lua_pushvalue(tolua_S, 2);
		int ref = luaL_ref(tolua_S, LUA_REGISTRYINDEX);
		self->SetEvaluatorRef(ref);
	}
	return 0;
#ifndef TOLUA_RELEASE
tolua_lerror:
	tolua_error(tolua_S, "#ferror in function 'SetEvaluator'.", &tolua_err);
	return 0;
#endif
}
#endif

/* function to register type */
static void tolua_reg_types(lua_State* tolua_S)
{
	tolua_usertype(tolua_S, "LuaEnvObject");
    tolua_usertype(tolua_S, "VehicleObject");
	tolua_usertype(tolua_S, "AgentObject");
    tolua_usertype(tolua_S, "AgentLuaState");
    tolua_usertype(tolua_S, "DecisionBranch");
    tolua_usertype(tolua_S, "LuaDecisionAction");
    tolua_usertype(tolua_S, "LuaBehaviorAction");
    tolua_usertype(tolua_S, "LuaCondition");
}

/* open function */
TOLUA_API int tolua_SandboxToLua_Manual(lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"AgentObject","AgentObject","VehicleObject",NULL);
  tolua_beginmodule(tolua_S,"AgentObject");
   tolua_function(tolua_S,"setPluginEnv",tolua_ClientToLua_AgentObject_setPluginEnv00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S, "AgentLuaState", "AgentLuaState", "", NULL);
   tolua_beginmodule(tolua_S, "AgentLuaState");
    tolua_function(tolua_S, "setPluginEnv", tolua_ClientToLua_AgentLuaState_setPluginEnv00);
   tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S, "DecisionBranch", "DecisionBranch", "", NULL);
   tolua_beginmodule(tolua_S, "DecisionBranch");
    tolua_function(tolua_S, "SetEvaluator", tolua_SandboxToLua_DecisionBranch_SetEvaluator00);
   tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S, "LuaDecisionAction", "LuaDecisionAction", "", NULL);
   tolua_beginmodule(tolua_S, "LuaDecisionAction");
    tolua_function(tolua_S, "setPluginEnv", tolua_SandboxToLua_LuaDecisionAction_setPluginEnv00);
   tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S, "LuaBehaviorAction", "LuaBehaviorAction", "", NULL);
   tolua_beginmodule(tolua_S, "LuaBehaviorAction");
    tolua_function(tolua_S, "setPluginEnv", tolua_SandboxToLua_LuaBehaviorAction_setPluginEnv00);
   tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S, "LuaCondition", "LuaCondition", "", NULL);
   tolua_beginmodule(tolua_S, "LuaCondition");
    tolua_function(tolua_S, "SetEvaluator", tolua_SandboxToLua_LuaCondition_SetEvaluator00);
   tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_SandboxToLua_Manual (lua_State* tolua_S) {
 return tolua_SandboxToLua_Manual(tolua_S);
};
#endif

