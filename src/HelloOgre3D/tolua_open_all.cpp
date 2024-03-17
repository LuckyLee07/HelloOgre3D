#include "tolua++.h"

int tolua_ClientToLua_open(lua_State* tolua_S);

int tolua_open_all(lua_State* tolua_S)
{
	tolua_ClientToLua_open(tolua_S);

	return 1;
}