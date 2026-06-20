#ifndef __SCRIPT_SERVICE_H__
#define __SCRIPT_SERVICE_H__

#include "OgreString.h"

class ScriptLuaVM;

class ScriptService //tolua_exports
{ //tolua_exports
public:
	explicit ScriptService(ScriptLuaVM* scriptVM);
	~ScriptService();

	void SetScriptLuaVM(ScriptLuaVM* scriptVM);

	//tolua_begin
	void CallFile(const Ogre::String& filepath);
	//tolua_end

private:
	ScriptLuaVM* m_scriptVM = nullptr; // non-owning; injected by GameManager
}; //tolua_exports

#endif // __SCRIPT_SERVICE_H__
