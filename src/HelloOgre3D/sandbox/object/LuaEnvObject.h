#ifndef __LUAENV_OBJECT__
#define __LUAENV_OBJECT__

struct lua_State;

class LuaEnvObject
{
public:
	LuaEnvObject();
	virtual ~LuaEnvObject();

	void setPluginEnv(lua_State* L); // 手动tolua

protected:
	int m_luaRef = 0; // 绑定的Lua表
};

#endif