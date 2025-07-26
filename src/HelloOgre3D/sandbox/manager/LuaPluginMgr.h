#ifndef __LUA_PLUGIN_MGR_H__  
#define __LUA_PLUGIN_MGR_H__

#include <regex> //C++11ÒýÈë
#include <string>
#include <assert.h>
#include <typeinfo>
#include "ScriptLuaVM.h"
#include "LuaClassNameTraits.h"

class LuaPluginMgr
{
public:
	template<typename T>
	static bool BindLuaPluginByFile(T* bindObj, const std::string& fileName)
	{
		const char* className = LuaClassNameTraits<T>::value;

		char formatStr[128];
		::sprintf(formatStr, "u[%s]s>b", className);
		bool result = false;
		GetScriptLuaVM()->callModuleFunc("LuaPluginMgr", "BindByLuaFile", formatStr, bindObj, fileName.c_str(), &result);

		assert(result && "Failed on bind lua plugin to obj"); // ±¨´íassert

		return result;
	}

	static void RemoveLocalEnvForObject(void* obj)
	{
		GetScriptLuaVM()->callModuleFunc("LuaPluginMgr", "RemLocalGt", "u[LuaEnvObject]", obj);
	}
};

#endif; // __LUA_PLUGIN_MGR_H__