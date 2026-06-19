#ifndef __LUA_SCRIPT_COMPONENT_H__
#define __LUA_SCRIPT_COMPONENT_H__

#include <cstdarg>
#include <string>

#include "component/IComponent.h"

struct lua_State;
class ScriptLuaVM;

class LuaScriptComponent : public IComponent
{
public:
	LuaScriptComponent();
	virtual ~LuaScriptComponent();

	virtual void onAttach(BaseObject* owner) override;
	virtual void onDetach() override;

	void SetLocalEnvOwner(void* object, const char* luaClassName);

	bool setPluginEnv(lua_State* L);
	bool callFunction(const char* funcname, const char* format, ...);
	bool callFunctionV(const char* funcname, const char* format, va_list vl);

private:
	void ReleaseLuaRef();
	void ReleaseLuaEnv();

private:
	ScriptLuaVM* m_pScriptVM = nullptr; // non-owning global Lua VM
	int m_luaRef = 0;
	void* m_localEnvOwner = nullptr; // non-owning key used by LuaPluginMgr
	std::string m_localEnvClassName;
};

#endif // __LUA_SCRIPT_COMPONENT_H__
