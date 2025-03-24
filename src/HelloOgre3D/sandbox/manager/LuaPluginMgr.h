#ifndef __LUA_PLUGIN_MGR_H__  
#define __LUA_PLUGIN_MGR_H__

#include <regex> //C++11ÒýÈë
#include <string>
#include <assert.h>
#include <typeinfo>
#include "ScriptLuaVM.h"

class LuaPluginMgr
{
public:
	template<typename T>
	static bool BindLuaPluginByFile(T* bindObj, const std::string& fileName)
	{
		std::string clsinfo = typeid(bindObj).name();

		std::smatch matches;
		std::regex patten("class\\s(\\w+)\\s\\*");
		bool found = std::regex_match(clsinfo, matches, patten);
		assert(found && "BindLuaPlugin can't find class name!");

		std::string clsname = found ? std::string(matches[1]) : "";
		char formatStr[128];
		::sprintf(formatStr, "u[%s]s>b", clsname.c_str());
		bool result = false;
		GetScriptLuaVM()->callModuleFunc("LuaPluginMgr", "BindByLuaFile", formatStr, bindObj, fileName.c_str(), &result);
		return result;
	}
};

#endif; // __LUA_PLUGIN_MGR_H__