#include "ScriptLuaVM.h"
#include "FileManager.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luasocket.h>
}
#include <assert.h>
#include <iostream>
#include "tolua++.h"
#include "LogSystem.h"

// LuaStack自动清理
class LuaStackBackup
{
public:
	LuaStackBackup(lua_State* vpls) : ls(vpls)
	{
		top = lua_gettop(vpls);
	}

	~LuaStackBackup()
	{
		lua_settop(ls, top);
	}

private:
	lua_State* ls;
	int	top;
};

static const char* lua_readfile(lua_State* L, void* data, size_t* size)
{
	FILE* pfile = (FILE*)data;

	if (!feof(pfile))
	{
		static char buffer[1024]; //缓冲区大小
		size_t bytesRead = fread(buffer, 1, sizeof(buffer), pfile);
		if (bytesRead > 0)
		{
			*size = bytesRead;
			return buffer;
		}
	}
	return NULL;
}

// 加入LuaSocket
//extern int luaopen_socket_core(lua_State* L);
static const luaL_Reg lualibext[] = {
	{"socket.core", luaopen_socket_core},
	{NULL, NULL}
};

LUALIB_API void luaL_openextlibs(lua_State* L) {
	const luaL_Reg* lib = lualibext;
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");
	for (; lib->func; lib++)
	{
		lua_pushcfunction(L, lib->func);
		lua_setfield(L, -2, lib->name);
	}
	lua_pop(L, 2);
}

ScriptLuaVM::ScriptLuaVM()
	:m_pState(nullptr)
{
	m_pState = luaL_newstate();

	luaL_openlibs(m_pState);
	luaL_openextlibs(m_pState);

	// 添加require loader
	addLuaLoader(m_pState, myLuaLoader);

	// 设置lua打印接口
	//setLuaLogfunc("proxy_log", proxy_log);
}

ScriptLuaVM::~ScriptLuaVM()
{
	if (m_pState) lua_close(m_pState);
}

bool ScriptLuaVM::callFile(const char *fpath)
{
	LuaStackBackup stackbackup(m_pState);

	std::string fullPath = GetFileManager()->getFullPath(fpath);

	FILE* pfile = fopen(fullPath.c_str(), "rb");
	assert(pfile != NULL);
	if (lua_load(m_pState, (lua_Reader)lua_readfile, pfile, fullPath.c_str()) != LUA_OK)
	{
		const char *perr = lua_tostring(m_pState, -1);
		CCLUA_ERROR("call_file error: %s | error: %s", fpath, perr);
		lua_pop(m_pState, 1);
		//showLuaError(m_pState, perr);
		return false;
	}

	if (lua_pcall(m_pState, 0, 0, 0) != LUA_OK)
	{
		const char *perr = lua_tostring(m_pState, -1);
		CCLUA_ERROR("lua_pcall path: %s | error: %s", fpath, perr);
		lua_pop(m_pState, 1);
		//showLuaError(m_pState, perr);
		return false;
	}

	return true;
}

bool ScriptLuaVM::loadFile(const char* module)
{
	// 1. 清除缓存
	lua_getglobal(m_pState, "package");
	lua_getfield(m_pState, -1, "loaded"); // package.loaded
	lua_pushnil(m_pState); // 清除缓存
	lua_setfield(m_pState, -2, module);
	lua_pop(m_pState, 2); // 弹出 package 和 loaded

	// 2. 重新加载模块
	std::string filepath = "res/scripts/samples/game_init.lua";
	if (luaL_dofile(m_pState, filepath.c_str()) != LUA_OK)
	{
		const char* perr = lua_tostring(m_pState, -1);
		CCLUA_ERROR("lua_dofile path : %s | error: %s", filepath.c_str(), perr);
		return false;
	}

	return true;
}

bool ScriptLuaVM::callString(const char *szLua_code)
{
	if (szLua_code == nullptr) return false;

	LuaStackBackup stackbackup(m_pState);
	
	int result = luaL_dostring(m_pState, szLua_code);
	if (result != LUA_OK)
	{	
		const char *perr = lua_tostring(m_pState, -1);
		CCLUA_ERROR("call_string error: %s-%s", perr, szLua_code);
		lua_pop(m_pState, 1);
		//showLuaError(m_pState, perr);
		return false;
	}
	return true;
}

bool ScriptLuaVM::callFunction(const char* funcname, const char* format, ...)
{
	va_list vl;
	va_start(vl, format);
	bool result = callFunctionV(funcname, format, vl);
	va_end(vl);

	return result;
}

bool ScriptLuaVM::callFunctionV(const char* funcname, const char* format, va_list vl)
{
	if (!m_pState || !funcname || !funcname[0])
	{
		return false;
	}
	LuaStackBackup stackbackup(m_pState);

	lua_getglobal(m_pState, funcname);
	if (!lua_isfunction(m_pState, -1))
	{
		CCLUA_ERROR("call_func error: %s is not a function", funcname);
		lua_pop(m_pState, 1);
		return false;
	}

	const char *sig = format;
	if (format == NULL) sig = "";
	int narg = 0;
	while (*sig)
	{
		switch (*sig)
		{
		case 'd': /* double	argument */
			lua_pushnumber(m_pState, va_arg(vl, double));
			break;
		case 'f':	//用float有问题,改成double自测没问题
			lua_pushnumber(m_pState, va_arg(vl, double));
			break;
		case 'i': /* int argument */
			lua_pushnumber(m_pState, va_arg(vl, int));
			break;
		case 's':
		{
			lua_pushstring(m_pState, va_arg(vl, char *));
		}
		break;
		case 'S':
		{
			char *s = va_arg(vl, char *);
			int len = va_arg(vl, int);
			lua_pushlstring(m_pState, s, len);
		}
		break;
		case 'w': // long long to number
			lua_pushnumber(m_pState, (lua_Number)va_arg(vl, long long));
			break;
		case 'b': /* boolean argument */
			//lua_pushboolean(m_pState, va_arg(vl, bool)); 安卓下会崩溃
			lua_pushboolean(m_pState, va_arg(vl, int));
			break;
		case 'u': /* user data */
		{
			if (sig[1] == '[')
			{
				char classname[256];
				sig++;
				const char *pend = strchr(sig, ']');
				assert(pend != NULL);
				size_t len = pend - sig - 1;
				memcpy(classname, sig + 1, len);
				classname[len] = 0;

				tolua_pushusertype(m_pState, va_arg(vl, void *), classname);
				sig = pend;
			}
			else
			{
				tolua_pushuserdata(m_pState, va_arg(vl, void *));
			}
		}
		break;
		case '>':
			sig++;
			goto endwhile;
		default:
			assert(0);
			lua_pop(m_pState, 1);
			return false;
		}
		sig++;
		narg++;
		luaL_checkstack(m_pState, 1, "too many arguments");
	}

endwhile:
	int nres = (int)strlen(sig);	/* number of expected results */
	if (lua_pcall(m_pState, narg, nres, 0) != 0) /* do	the	call */
	{
		CCLUA_ERROR("call_func error: %s === %s", funcname, lua_tostring(m_pState, -1));
		lua_pop(m_pState, 2); // 弹出 function 和错误信息
		return false;
	}

	/* retrieve	results	*/
	nres = -nres; /* 第一个返回值在栈中的索引 */
	while (*sig)
	{
		switch (*sig++)
		{
		case 'd': /* double	result */
			assert(lua_isnumber(m_pState, nres));
			*va_arg(vl, double *) = lua_tonumber(m_pState, nres);
			break;
		case 'f':
			assert(lua_isnumber(m_pState, nres));
			*va_arg(vl, float*) = (float)lua_tonumber(m_pState, nres);
			break;
		case 'i': /* int result	*/
			assert(lua_isnumber(m_pState, nres));
			*va_arg(vl, int	*) = (int)lua_tonumber(m_pState, nres);
			break;
		case 's':
			assert(lua_isstring(m_pState, nres));
			strcpy(va_arg(vl, char *), lua_tostring(m_pState, nres));
			break;
		case 'b': /* boolean argument */
			assert(lua_isboolean(m_pState, nres));
			*va_arg(vl, bool *) = (0 != lua_toboolean(m_pState, nres));
			break;
		case 'u': /* light user data */
			assert(lua_isuserdata(m_pState, nres));
			*va_arg(vl, void **) = tolua_tousertype(m_pState, nres, NULL);
			break;
		default:
			assert(0);
			break;
		}
		nres++;
	}
	lua_pop(m_pState, 1); // 弹出 function
	return true;
}

void ScriptLuaVM::setUserTypePointer(const char* name, const char* clsname, void* ptr)
{
	LuaStackBackup stackbackup(m_pState);
	tolua_pushusertype(m_pState, ptr, clsname);
	lua_setglobal(m_pState, name);
}

bool ScriptLuaVM::setLuaLogfunc(const char* funcname, lua_CFunction logfunc)
{
	if (m_pState != NULL)
		lua_register(m_pState, funcname, logfunc);

	return true;
}

void ScriptLuaVM::showLuaError(lua_State* L, const char* msg)
{
	//send_LuaErrMsg(msg); //上传lua报错信息
	
	const char* luaTrack = lua_get_traceback(L);
	if (!luaTrack) return;

	const char* title = "lua error";
	std::string errStr = msg;
	errStr.append("\nluaTraceBack:\n");
	errStr.append(luaTrack);
	
	//弹出lua报错弹窗
	//pop_MessageBox(errStr.c_str(), title);
}

bool ScriptLuaVM::callFunctionV1(const char* funcname, const char* format, bool needSelf, va_list vl)
{
	if (funcname == nullptr || funcname[0] == '\0')
	{
		return false;
	}

	// 平栈操作
	LuaStackBackup stackbackup(m_pState);

	// 1. 从该表中获取函数
	lua_getfield(m_pState, -1, funcname);
	if (!lua_isfunction(m_pState, -1))
	{
		CCLUA_INFO("CallFuncV1 error: %s is not a function", funcname);
		return false;
	}

	// 2. 将 table 设置为该函数的环境（注意：lua_setfenv 在 Lua5.2+ 中已废弃）
	lua_pushvalue(m_pState, -2);  // 将 table 复制一份压栈（self）
	if (!needSelf)
		lua_setfenv(m_pState, -2);    // 设置函数环境为 table

	// 3. 根据 format 字符串解析并压入参数（遇到 '>' 表示参数结束）
	const char* sig = format;
	if (format == NULL) sig = "";
	int narg = needSelf ? 1 : 0; //self是第一个参数
	while (*sig)
	{
		switch (*sig)
		{
		case 'd': /* double	argument */
			lua_pushnumber(m_pState, va_arg(vl, double));
			break;
		case 'f':	//用float有问题,改成double自测没问题
			lua_pushnumber(m_pState, va_arg(vl, double));
			break;
		case 'i': /* int argument */
			lua_pushnumber(m_pState, va_arg(vl, int));
			break;
		case 's':
		{
			lua_pushstring(m_pState, va_arg(vl, char*));
		}
		break;
		case 'S':
		{
			char* s = va_arg(vl, char*);
			int len = va_arg(vl, int);
			lua_pushlstring(m_pState, s, len);
		}
		break;
		case 'w': // long long to number
			lua_pushnumber(m_pState, (lua_Number)va_arg(vl, long long));
			break;
		case 'b': /* boolean argument */
			//lua_pushboolean(m_pState, va_arg(vl, bool)); 安卓下会崩溃
			lua_pushboolean(m_pState, va_arg(vl, int));
			break;
		case 'u': /* user data */
		{
			if (sig[1] == '[')
			{
				char classname[256];
				sig++;
				const char* pend = strchr(sig, ']');
				assert(pend != NULL);
				size_t len = pend - sig - 1;
				memcpy(classname, sig + 1, len);
				classname[len] = 0;

				tolua_pushusertype(m_pState, va_arg(vl, void*), classname);
				sig = pend;
			}
			else
			{
				tolua_pushuserdata(m_pState, va_arg(vl, void*));
			}
		}
		break;
		case '>':
			sig++;
			goto endwhile;
		default:
			assert(0);
			return false;
		}
		sig++;
		narg++;
		luaL_checkstack(m_pState, 1, "too many arguments");
	}

endwhile:
	// 4.调用函数
	// 此时栈结构：[table, function, arg1, arg2, ..., argN]
	// lua_pcall 会把 function 和参数弹出，将返回值压入栈中
	int nres = (int)strlen(sig);	/* number of expected results */
	if (lua_pcall(m_pState, narg, nres, 0) != 0) /* do	the	call */
	{
		CCLUA_ERROR("CallFuncV1 lua_pcall error: %s === %s", funcname, lua_tostring(m_pState, -1));
		return false;
	}

	// 5. 解析返回值，并将结果写入调用者提供的指针中
	int resIndex = -nres; /* 第一个返回值在栈中的索引 */
	while (*sig)
	{
		switch (*sig++)
		{
		case 'd': /* double	result */
			assert(lua_isnumber(m_pState, resIndex));
			*va_arg(vl, double*) = lua_tonumber(m_pState, resIndex);
			break;
		case 'f':
			assert(lua_isnumber(m_pState, resIndex));
			*va_arg(vl, float*) = (float)lua_tonumber(m_pState, resIndex);
			break;
		case 'i': /* int result	*/
			assert(lua_isnumber(m_pState, resIndex));
			*va_arg(vl, int*) = (int)lua_tonumber(m_pState, resIndex);
			break;
		case 's':
			assert(lua_isstring(m_pState, resIndex));
			strcpy(va_arg(vl, char*), lua_tostring(m_pState, resIndex));
			break;
		case 'b': /* boolean argument */
			assert(lua_isboolean(m_pState, resIndex));
			*va_arg(vl, bool*) = (0 != lua_toboolean(m_pState, resIndex));
			break;
		case 'u': /* light user data */
			assert(lua_isuserdata(m_pState, resIndex));
			*va_arg(vl, void**) = tolua_tousertype(m_pState, resIndex, NULL);
			break;
		default:
			assert(0);
			break;
		}
		resIndex++;
	}

	return true;
}

bool ScriptLuaVM::callModuleFuncV(int luaRef, const char* funcname, const char* format, va_list vl)
{
	if (luaRef <= 0) // 取不到表 直接调用global的
	{
		return callFunctionV(funcname, format, vl);
	}

	// 根据 tableRef 从注册表中获取 Lua 表
	lua_rawgeti(m_pState, LUA_REGISTRYINDEX, luaRef);
	if (!lua_istable(m_pState, -1))
	{
		CCLUA_INFO("callModuleFunc error: ref %d is not a table", luaRef);
		lua_pop(m_pState, 1);
		return false;
	}

	bool result = callFunctionV1(funcname, format, false, vl);
	lua_pop(m_pState, 1); // 弹出 table

	return result;
}

bool ScriptLuaVM::callModuleFunc(int luaRef, const char* funcname, const char* format, ...)
{
	va_list vl;
	va_start(vl, format);
	bool result = callModuleFuncV(luaRef, funcname, format, vl);
	va_end(vl);
	return result;
}

bool ScriptLuaVM::callModuleFunc(const char* tableName, const char* funcname, const char* format, ...)
{
	// 取不到表 直接调用global的
	if (tableName == nullptr || tableName[0] == '\0')
	{
		va_list vl;
		va_start(vl, format);
		bool result = callFunctionV(funcname, format, vl);
		va_end(vl);
		return result;
	}

	// 从全局环境中获取 Lua 表
	lua_getglobal(m_pState, tableName);
	if (!lua_istable(m_pState, -1))
	{
		CCLUA_INFO("callModuleFunc error: %s is not a table", tableName);
		lua_pop(m_pState, 1);
		return false;
	}

	va_list vl;
	va_start(vl, format);
	bool result = callFunctionV1(funcname, format, true, vl);
	// 弹出 table
	lua_pop(m_pState, 1);
	va_end(vl);

	return result;
}


static ScriptLuaVM* s_ScriptLuaVM = nullptr;
ScriptLuaVM* GetScriptLuaVM()
{
	if (s_ScriptLuaVM == nullptr)
	{
		s_ScriptLuaVM = new ScriptLuaVM();
	}
	return s_ScriptLuaVM;
}


//------------require自定义loader_Lua------------
int myLuaLoader(lua_State * m_state)
{
	const std::string LuaExt = ".lua";
	const char* luapath = lua_tostring(m_state, 1);

	//兼容"socket.core"，这种写法
	std::string filename = luapath;
	size_t pos = filename.rfind(LuaExt);
	if (pos != std::string::npos && pos == (filename.length() - LuaExt.length()))
	{
		filename = filename.substr(0, pos);
	}

	pos = filename.find_first_of('.');
	while (pos != std::string::npos)
	{
		filename.replace(pos, 1, "/");
		pos = filename.find_first_of('.');
	}

	filename += LuaExt; //重新补上Lua后缀
	std::string fullPath = GetFileManager()->getFullPath(filename.c_str());

	FILE* pfile = fopen(fullPath.c_str(), "rb");
	if (pfile != NULL)
	{
		//if (luaL_loadfile(m_state, fullpath) != LUA_OK)
		if (lua_load(m_state, (lua_Reader)lua_readfile, pfile, fullPath.c_str()) != LUA_OK)
		{
			const char *perr = lua_tostring(m_state, -1);
			CCLOG_ERROR("require lua %s error: %s", filename.c_str(), perr);
			lua_pop(m_state, 1);
		}
	}
	else
	{
		CCLOG_ERROR("requrie not find [%s]", filename.c_str());
	}
	
	return 1;
}

//参考：https://blog.csdn.net/Yueya_Shanhua/article/details/52241544
void addLuaLoader(lua_State* m_state, lua_CFunction func)
{
	if (func == NULL) return;
	
	// stackcontent after the invoking of the function
	// getloader table（获得lua预加载好的package这个table，并将这个table压桟）
	lua_getglobal(m_state, "package"); /* L: package */
	// 获得package这个table里面的loaders元素，并将其压桟，这个loaders也是一个table，table里的元素全是方法
	lua_getfield(m_state, -1, "loaders"); /* L: package, loaders */
	// insertloader into index 2（将自己的loader方法压桟）
	lua_pushcfunction(m_state, func); /* L: package, loaders, func */
	// 遍历loaders这个table，将其下标为2的元素替换为自己的loader方法
	for (int i = (int)lua_objlen(m_state, -2) + 1; i > 2; --i)
	{
		lua_rawgeti(m_state, -2, i - 1); /* L: package, loaders, func, function */
		//we call lua_rawgeti, so the loader table now is at -3
		lua_rawseti(m_state, -3, i); /* L: package, loaders, func */
	}
	lua_rawseti(m_state, -2, 2); /* L: package, loaders */
	// setloaders into package
	lua_setfield(m_state, -2, "loaders"); /* L: package */
	lua_pop(m_state, 1);
}

/*
// Lua端的打印接口
int proxy_log(lua_State *L)
{
	const char* msg = luaL_checkstring(L, 1);
	if (msg == NULL) return 0;

	//CCLUA_INFO("@lua:%s\n", msg);

	return 0;
}*/