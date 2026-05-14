#include "LuaCondition.h"

#include "ScriptLuaVM.h"

extern "C" {
#include "lauxlib.h"
}

LuaCondition::LuaCondition()
	: m_evalLuaRef(LUA_NOREF)
{
}

LuaCondition::~LuaCondition()
{
	if (m_evalLuaRef != LUA_NOREF)
	{
		lua_State* L = GetScriptLuaVM()->getLuaState();
		if (L) luaL_unref(L, LUA_REGISTRYINDEX, m_evalLuaRef);
	}
}

void LuaCondition::SetEvaluatorRef(int luaRef)
{
	if (m_evalLuaRef != LUA_NOREF)
	{
		lua_State* L = GetScriptLuaVM()->getLuaState();
		if (L) luaL_unref(L, LUA_REGISTRYINDEX, m_evalLuaRef);
	}
	m_evalLuaRef = luaRef;
}

BehaviorNode::Status LuaCondition::Tick(float /*deltaMs*/)
{
	if (m_evalLuaRef == LUA_NOREF) return TraceStatus(STATUS_FAILURE);

	lua_State* L = GetScriptLuaVM()->getLuaState();
	if (!L) return TraceStatus(STATUS_FAILURE);

	lua_rawgeti(L, LUA_REGISTRYINDEX, m_evalLuaRef);
	if (lua_pcall(L, 0, 1, 0) != 0)
	{
		// pcall 出错：错误消息已在栈顶，pop 掉
		lua_pop(L, 1);
		return TraceStatus(STATUS_FAILURE);
	}

	const bool result = lua_toboolean(L, -1) != 0;
	lua_pop(L, 1);
	return TraceStatus(result ? STATUS_SUCCESS : STATUS_FAILURE);
}
