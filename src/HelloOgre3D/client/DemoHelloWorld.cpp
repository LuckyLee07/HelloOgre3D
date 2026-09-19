
#include "DemoHelloWorld.h"
#include "ScriptLuaVM.h"
#include "LogSystem.h"
#include <cstdlib>
#include <cstring>

namespace
{
	const char* SampleWindowTitle()
	{
		const char* sample = std::getenv("HELLO_SANDBOX_SAMPLE");
		return sample != nullptr && (std::strcmp(sample, "Sandbox20") == 0 || std::strcmp(sample, "20") == 0)
			? "CROSS / FIRE - Relay Operations" : "Learning Game AI Programming";
	}
}

DemoHelloWorld::DemoHelloWorld()
	: Application(SampleWindowTitle())
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
