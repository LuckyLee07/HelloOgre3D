#include "LuaScriptComponent.h"
#include "ScriptLuaVM.h"
#include "LogSystem.h"
#include "object/BaseObject.h"
#include "scripting/LuaPluginMgr.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

LuaScriptComponent::LuaScriptComponent()
	: m_pScriptVM(GetScriptLuaVM())
{
}

LuaScriptComponent::~LuaScriptComponent()
{
	ReleaseLuaEnv();
}

void LuaScriptComponent::onAttach(GameObject* owner)
{
	IComponent::onAttach(owner);

	if (m_pScriptVM == nullptr)
	{
		m_pScriptVM = GetScriptLuaVM();
	}

	if (m_localEnvOwner == nullptr)
	{
		SetLocalEnvOwner(getOwner(), "BaseObject");
	}
}

void LuaScriptComponent::onDetach()
{
	ReleaseLuaEnv();
	IComponent::onDetach();
}

void LuaScriptComponent::SetLocalEnvOwner(void* object, const char* luaClassName)
{
	m_localEnvOwner = object;
	m_localEnvClassName = luaClassName != nullptr ? luaClassName : "";
}

bool LuaScriptComponent::setPluginEnv(lua_State* L)
{
	if (!lua_istable(L, 2)) {
		CCLUA_INFO("BindLuaEnv Error: param is not a table");
		return false;
	}

	ReleaseLuaRef();

	lua_pushvalue(L, 2); //self->为1 此处把table参数压入

	// 为防止被垃圾回收 在 Lua 注册表中创建一个引用
	m_luaRef = luaL_ref(L, LUA_REGISTRYINDEX);

	return true;
}

bool LuaScriptComponent::callFunction(const char* funcname, const char* format, ...)
{
	va_list vl;
	va_start(vl, format);
	bool result = callFunctionV(funcname, format, vl);
	va_end(vl);
	return result;
}

bool LuaScriptComponent::callFunctionV(const char* funcname, const char* format, va_list vl)
{
	if (m_pScriptVM == nullptr)
	{
		m_pScriptVM = GetScriptLuaVM();
	}
	if (m_pScriptVM == nullptr)
	{
		return false;
	}

	return m_pScriptVM->callModuleFuncV(m_luaRef, funcname, format, vl);
}

void LuaScriptComponent::ReleaseLuaRef()
{
	if (m_pScriptVM == nullptr || m_luaRef <= 0)
	{
		m_luaRef = 0;
		return;
	}

	lua_State* L = m_pScriptVM->getLuaState();
	if (L != nullptr)
	{
		luaL_unref(L, LUA_REGISTRYINDEX, m_luaRef);
	}
	m_luaRef = 0;
}

void LuaScriptComponent::ReleaseLuaEnv()
{
	const bool hadLuaEnv = m_luaRef > 0;
	ReleaseLuaRef();

	if (hadLuaEnv && m_pScriptVM != nullptr && m_localEnvOwner != nullptr && !m_localEnvClassName.empty())
	{
		LuaPluginMgr::RemoveLocalEnvForObject(m_localEnvOwner, m_localEnvClassName.c_str());
	}
}
