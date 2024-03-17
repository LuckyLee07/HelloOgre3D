#ifndef __LUA_INTERFACE_H__  
#define __LUA_INTERFACE_H__

#include <string>
#include "Singleton.h"

class LuaInterface : //tolua_exports
	public Singleton<LuaInterface>
{ //tolua_exports
public:
	LuaInterface();
	~LuaInterface();
	
public:
	//tolua_begin
	void log(const char* msg);

	//tolua_end

private:
	std::string m_moduleFileName;
}; //tolua_exports

#endif;