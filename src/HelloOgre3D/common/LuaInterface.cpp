#include "LuaInterface.h"
#include "LogSystem.h"

LuaInterface::LuaInterface()
{
}

LuaInterface::~LuaInterface()
{
}

void LuaInterface::log(const char* msg)
{
	if (msg == NULL) return;
	
	CCLUA_INFO("Lua:%s", msg);
}
