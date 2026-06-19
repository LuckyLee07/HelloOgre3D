#include "ScriptService.h"
#include "ScriptLuaVM.h"

ScriptService::ScriptService(ScriptLuaVM* scriptVM)
	: m_scriptVM(scriptVM)
{
}

ScriptService::~ScriptService()
{
	m_scriptVM = nullptr;
}

void ScriptService::SetScriptLuaVM(ScriptLuaVM* scriptVM)
{
	m_scriptVM = scriptVM;
}

void ScriptService::CallFile(const Ogre::String& filepath)
{
	if (m_scriptVM != nullptr)
		m_scriptVM->callFile(filepath.c_str());
}
