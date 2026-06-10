#include "Application.h"
#include "OgreRenderWindow.h"
#include "ClientManager.h"
#include "profiling/Profile.h"
#include "profiling/RuntimeProfileCounters.h"
#include "diagnostics/RuntimeRenderCapture.h"

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

    CreateEventListener();
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

void Application::CreateEventListener()
{
    m_pClientManager->CreateFrameListener(this);
    m_pClientManager->CreateWindowEventListener(this);
}

bool Application::frameEnded(const Ogre::FrameEvent& event)
{
    H3D_PROFILE_SCOPE("Application::frameEnded");
    return true;
}

bool Application::frameRenderingQueued(const Ogre::FrameEvent& event)
{
    H3D_PROFILE_SCOPE("Application::frameRenderingQueued");
    if (m_pClientManager->GetShutdown()) 
        return false;

    const bool perfEnabled = RuntimeStallProfiler::IsEnabled();
    RuntimeClientFrameTiming frameTiming;
    long long stageStartMicros = 0;
    
    //Need to capture/update each device
    {
        H3D_PROFILE_SCOPE("Application::InputCapture");
        if (perfEnabled)
            stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
        m_pClientManager->InputCapture();
        if (perfEnabled)
            frameTiming.inputCaptureMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
    }

    {
        H3D_PROFILE_SCOPE("Application::Update");
        if (perfEnabled)
            stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
        m_pClientManager->Update();
        if (perfEnabled)
            frameTiming.updateCallMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
    }

    {
        H3D_PROFILE_SCOPE("Application::FrameRendering");
        if (perfEnabled)
            stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
        m_pClientManager->FrameRendering(event);
        if (perfEnabled)
            frameTiming.frameRenderingMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
    }

    if (perfEnabled)
        RuntimeStallProfiler::FinishFrame(frameTiming);

    RuntimeRenderCapture::Update(m_pClientManager->getRenderWindow(), event.timeSinceLastFrame);

    return true;
}

bool Application::frameStarted(const Ogre::FrameEvent& event)
{
    H3D_PROFILE_FRAME();
    H3D_PROFILE_SCOPE("Application::frameStarted");
    H3D_PROFILE_PLOT("FrameTimeMs", event.timeSinceLastFrame * 1000.0f);

    const bool perfEnabled = RuntimeStallProfiler::IsEnabled();
    if (perfEnabled)
        RuntimeStallProfiler::BeginFrame(event.timeSinceLastFrame * 1000.0f);

    {
        H3D_PROFILE_SCOPE("Application::Draw");
        const long long drawStartMicros = perfEnabled ? RuntimeStallProfiler::NowMicroseconds() : 0;
        m_pClientManager->Draw();
        if (perfEnabled)
            RuntimeStallProfiler::SetDrawTiming(RuntimeStallProfiler::ElapsedMsSince(drawStartMicros));
    }

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
