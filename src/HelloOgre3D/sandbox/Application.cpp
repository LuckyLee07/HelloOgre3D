#include "Application.h"
#include "SandboxDef.h"
#include "SandboxManager.h"

Application::Application(const Ogre::String& applicationTitle)
{
    GetSandboxMgr()->SetAppTitle(applicationTitle);
}

Application::~Application()
{
}

bool Application::Setup()
{
    return GetSandboxMgr()->Setup();
}

void Application::Cleanup()
{
    GetSandboxMgr()->Cleanup();
}

void Application::Draw()
{
    GetSandboxMgr()->Draw();
}

void Application::Update()
{
    GetSandboxMgr()->Update();
}
