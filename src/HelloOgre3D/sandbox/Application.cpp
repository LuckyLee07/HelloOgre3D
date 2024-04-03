#include "Application.h"
#include "SandboxDef.h"
#include "ClientManager.h"
#include "GameManager.h"

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

    g_GameManager = new GameManager(m_pClientManager->getSceneManager());

    return true;
}

void Application::Cleanup()
{
    m_pClientManager->Cleanup();
}

void Application::Draw()
{
    m_pClientManager->Draw();
}

void Application::Update()
{
    m_pClientManager->Update();
}
