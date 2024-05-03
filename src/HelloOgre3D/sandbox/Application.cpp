#include "Application.h"
#include "SandboxDef.h"
#include "ClientManager.h"
#include "OgreRenderWindow.h"
#include <iostream>

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
    //Need to capture/update each device
    m_pClientManager->InputCapture();

    m_pClientManager->Update();

    m_pClientManager->FrameRendering(event);

    return true;
}

bool Application::frameStarted(const Ogre::FrameEvent& event)
{
    m_pClientManager->Draw();

    return true;
}

void Application::windowResized(Ogre::RenderWindow* renderWindow)
{
    unsigned int width, height, depth;
    int left, top;
    renderWindow->getMetrics(width, height, depth, left, top);

    m_pClientManager->WindowResized(width, height);
}

void Application::windowClosed(Ogre::RenderWindow* renderWindow)
{
    Ogre::RenderWindow* pRenderWindow = m_pClientManager->getRenderWindow();
    if (renderWindow == pRenderWindow)
    {
        m_pClientManager->WindowClosed();
    }
}