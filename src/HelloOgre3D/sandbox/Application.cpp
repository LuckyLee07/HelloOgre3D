#include "Application.h"
#include "SandboxDef.h"
#include "SandboxManager.h"
#include "OgreWrapper.h"

Application::Application(const std::string& appTitle)
{
    m_pSandboxManager = GetSandboxMgr();
    m_pSandboxManager->SetAppTitle(appTitle);
}

Application::~Application()
{
    delete m_pSandboxManager;
}

bool Application::Setup()
{
    bool setupSucc = m_pSandboxManager->Setup();
    if (!setupSucc) return false;

    g_OgreWrapper = new OgreWrapper(m_pSandboxManager->getSceneManager());

    return true;
}

void Application::Cleanup()
{
    m_pSandboxManager->Cleanup();
}

void Application::Draw()
{
    m_pSandboxManager->Draw();
}

void Application::Update()
{
    m_pSandboxManager->Update();
}
