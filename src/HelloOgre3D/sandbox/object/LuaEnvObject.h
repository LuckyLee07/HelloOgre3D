#ifndef __LUAENV_OBJECT__
#define __LUAENV_OBJECT__

struct lua_State;
class ScriptLuaVM;

class LuaEnvObject
{
public:
	LuaEnvObject();
	virtual ~LuaEnvObject();

	bool setPluginEnv(lua_State* L); // 手动tolua

	bool callFunction(const char* funcname, const char* format, ...);

private:
	int m_luaRef = 0; // 绑定的Lua表

	ScriptLuaVM* m_pScriptVM;
};

#endif // __LUAENV_OBJECT__