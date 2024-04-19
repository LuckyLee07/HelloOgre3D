#ifndef __SCRIPT_LUA_VM_H__
#define __SCRIPT_LUA_VM_H__

#include "Singleton.h"

#define LUA_OK	0
#define LUA_VER	"Lua 5.1.4"

extern "C" {
#include <lua.h>
}
//struct lua_State;
class ScriptLuaVM : public Fancy::Singleton<ScriptLuaVM>
{
public:
	ScriptLuaVM(void);
	~ScriptLuaVM();

	bool callFile(const char *fpath);

	bool callString(const char *szLua_code);

	bool callFunction(const char* funcname, const char* format, ...);

	void setUserTypePointer(const char* name, const char* clsname, void* ptr);

	lua_State* getLuaState() { return m_pState; }

	bool setLuaLogfunc(const char* funcname, lua_CFunction logfunc);

	static void showLuaError(lua_State* L, const char* msg);

private:
	lua_State *m_pState;
};

ScriptLuaVM* GetScriptLuaVM(); // 获取单例

//------------require自定义loader_Lua------------
extern int myLuaLoader(lua_State * m_state);
extern void addLuaLoader(lua_State* L, lua_CFunction func);

//extern int proxy_log(lua_State *L); // lua端的打印接口

#endif //__SCRIPT_LUA_VM_H__