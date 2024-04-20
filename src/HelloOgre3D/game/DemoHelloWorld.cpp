
#include "DemoHelloWorld.h"
#include "Ogre.h"
#include "Procedural.h"

#include "ScriptLuaVM.h"
#include "LuaInterface.h"
#include "LogSystem.h"
#include "SandboxDef.h"
#include "GameManager.h"
#include "ClientManager.h"

extern int tolua_open_all(lua_State* tolua_S);


DemoHelloWorld::DemoHelloWorld()
	: Application("Learning Game AI Programming")
{
}

DemoHelloWorld::~DemoHelloWorld()
{
}

void DemoHelloWorld::Update()
{
    Application::Update();
}

void DemoHelloWorld::Run()
{
	if (!Application::Setup()) 
		return;
	
	this->InitLuaEnv();

	this->InitGameScene();

	Application::Run();
}

void DemoHelloWorld::InitLuaEnv()
{
	// 设置ToLua对象 
	ScriptLuaVM* pScriptVM = GetScriptLuaVM();
	tolua_open_all(pScriptVM->getLuaState());

	// 设置lua可用的c++对象 
	pScriptVM->setUserTypePointer("Sandbox", "GameManager", GameManager::GetInstance());
	pScriptVM->setUserTypePointer("LuaInterface", "LuaInterface", LuaInterface::GetInstance());

	pScriptVM->callFile("res/scripts/script_init.lua");
}

void DemoHelloWorld::InitGameScene()
{
	GetScriptLuaVM()->callFunction("Sandbox_Initialize", ">");
}