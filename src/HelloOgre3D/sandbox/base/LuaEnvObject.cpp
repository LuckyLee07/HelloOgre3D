#include "LuaEnvObject.h"
#include "ScriptLuaVM.h"
#include "LogSystem.h"
#include "manager/LuaPluginMgr.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

LuaEnvObject::LuaEnvObject()
{
	m_pScriptVM = GetScriptLuaVM();
}

LuaEnvObject::~LuaEnvObject()
{
	if (m_pScriptVM)
	{
		lua_State* L = m_pScriptVM->getLuaState();
		luaL_unref(L, LUA_REGISTRYINDEX, m_luaRef);

		LuaPluginMgr::RemoveLocalEnvForObject(this); // ��������ֲ���
	}
}

bool LuaEnvObject::setPluginEnv(lua_State* L)
{
	if (!lua_istable(L, 2)) {
		CCLUA_INFO("BindLuaEnv Error: param is not a table");
		return false;
	}

	lua_pushvalue(L, 2); //self->Ϊ1 �˴���table����ѹ��

	// Ϊ��ֹ���������� �� Lua ע����д���һ������
	m_luaRef = luaL_ref(L, LUA_REGISTRYINDEX);

	return true;
}

bool LuaEnvObject::callFunction(const char* funcname, const char* format, ...)
{
	va_list vl;
	va_start(vl, format);
	bool result = m_pScriptVM->callModuleFuncV(m_luaRef, funcname, format, vl);
	va_end(vl);
	return result;
}