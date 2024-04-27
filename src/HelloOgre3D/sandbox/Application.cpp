#include "Application.h"
#include "SandboxDef.h"
#include "ClientManager.h"

Application::Application(const std::string& appTitle)
{
    m_pClientManager = GetClientMgr();
    m_pClientManager->SetAppTitle(appTitle);
}

Application::~Application()
{
    delete m_pClientManager;
}

bool Application::Setup()
{
    bool setupSucc = m_pClientManager->Setup();
    if (!setupSucc) return false;

    CreateFrameListener();

    return true;
}

void Application::Cleanup()
{
    m_pClientManager->Cleanup();
}

void Application::Run()
{
    m_pClientManager->Run();
}

void Application::Draw()
{
    m_pClientManager->Draw();
}

void Application::Update()
{
    m_pClientManager->Update();
}

void Application::CreateFrameListener()
{
    m_pClientManager->CreateFrameListener(this);
}

bool Application::frameEnded(const Ogre::FrameEvent& event)
{
    return true;
}

bool Application::frameRenderingQueued(const Ogre::FrameEvent& event)
{
    this->Update();
    return true;
}

bool Application::frameStarted(const Ogre::FrameEvent& event)
{
    this->Draw();
    return true;
}