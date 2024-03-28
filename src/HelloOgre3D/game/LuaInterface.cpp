#include "LuaInterface.h"
#include "LogSystem.h"
#include <windows.h>
#include <tchar.h>
#include <codecvt>
#include <assert.h>

LuaInterface::LuaInterface()
{
}

LuaInterface::~LuaInterface()
{
}

void LuaInterface::log(const char* msg)
{
	if (msg == NULL) return;
	
	CCLUA_INFO("@lua:%s", msg);
}

