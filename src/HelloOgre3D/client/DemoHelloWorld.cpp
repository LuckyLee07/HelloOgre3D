
#include "DemoHelloWorld.h"
#include "ScriptLuaVM.h"
#include "LogSystem.h"

DemoHelloWorld::DemoHelloWorld()
	: Application("Learning Game AI Programming")
    , m_sceneInitDone(false)
{
}

DemoHelloWorld::~DemoHelloWorld()
{
}

void DemoHelloWorld::Run()
{
	if (!Application::Setup()) 
		return;

	Application::Run();
}

bool DemoHelloWorld::frameEnded(const Ogre::FrameEvent& event)
{
    // Let the first frame present the window first, then run heavy scene script init.
    if (!m_sceneInitDone)
    {
        this->InitGameScene();
        m_sceneInitDone = true;
    }

    return Application::frameEnded(event);
}

void DemoHelloWorld::InitGameScene()
{
	GetScriptLuaVM()->callFunction("Sandbox_Initialize", ">");
}
