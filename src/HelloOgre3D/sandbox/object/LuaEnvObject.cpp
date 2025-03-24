#include "LuaEnvObject.h"
#include "ScriptLuaVM.h"
#include "LogSystem.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

LuaEnvObject::LuaEnvObject()
{
}

LuaEnvObject::~LuaEnvObject()
{
	lua_State* L = GetScriptLuaVM()->getLuaState();
	luaL_unref(L, LUA_REGISTRYINDEX, m_luaRef);
}

void LuaEnvObject::setPluginEnv(lua_State* L)
{
	if (!lua_istable(L, 2)) {
		CCLUA_INFO("BindLuaEnv Error: param is not a table");
		return;
	}

	lua_pushvalue(L, 2); //self->为1 此处把table参数压入

	// 为防止被垃圾回收 在 Lua 注册表中创建一个引用
	m_luaRef = luaL_ref(L, LUA_REGISTRYINDEX);
}