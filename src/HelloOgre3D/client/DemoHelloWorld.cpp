
#include "DemoHelloWorld.h"
#include "ScriptLuaVM.h"
#include "LogSystem.h"

DemoHelloWorld::DemoHelloWorld()
	: Application("Learning Game AI Programming")
{
}

DemoHelloWorld::~DemoHelloWorld()
{
}

void DemoHelloWorld::Run()
{
	if (!Application::Setup()) 
		return;
	
	this->InitGameScene();

	Application::Run();
}

void DemoHelloWorld::InitGameScene()
{
	GetScriptLuaVM()->callFunction("Sandbox_Initialize", ">");
}