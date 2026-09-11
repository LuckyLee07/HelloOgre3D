#include "ClientManager.h"
#include "AppConfig.h"
#include "GameManager.h"
#include "ObfuscatedZip.h"
#include "ogre/OgreCameraController.h"
#include "debug/DebugDrawer.h"
#include "systems/input/InputManager.h"
#include "core/SandboxMacros.h"
#include "profiling/Profile.h"
#include "profiling/RuntimeProfileCounters.h"
#if defined(HELLO_ENABLE_FGUI)
#include "ui/fairygui/FairyGuiSystem.h"
#endif
#include "Ogre.h"
#include "OgreDpiHelper.h"
#if defined(_WIN32)
#include "OgreD3D9Plugin.h"
#elif defined(__APPLE__)
#include "OgreGL3PlusPlugin.h"
#else
#include "OgreGLPlugin.h"
#endif
#include "OgreParticleFXPlugin.h"
#include "ogre3d_gorilla/include/Gorilla.h"

#include <cstdlib>
#include <cstring>

using namespace Ogre;
namespace
{
    int ReadSimulationHz()
    {
        // 默认 30Hz（与历史硬编码一致）；HELLO_SIM_FPS / HELLO_SIM_HZ 可覆盖。
        // 此前首行硬编码 return 30 把下面的读取逻辑短路成死代码，导致环境变量失效。
        const char* value = std::getenv("HELLO_SIM_FPS");
        if (value == nullptr || value[0] == '\0')
            value = std::getenv("HELLO_SIM_HZ");

        if (value == nullptr || value[0] == '\0')
            return 30;

        const int hz = std::atoi(value);
        if (hz <= 0)
            return 30;
        if (hz < 15)
            return 15;
        if (hz > 240)
            return 240;
        return hz;
    }

    const char* ReadEnvValue(const char* name)
    {
        const char* value = std::getenv(name);
        return value != nullptr && value[0] != '\0' ? value : nullptr;
    }

    Ogre::String ReadStringEnvValue(const char* name, const Ogre::String& defaultValue)
    {
        const char* value = ReadEnvValue(name);
        return value != nullptr ? Ogre::String(value) : defaultValue;
    }

    bool ReadBoolEnvValue(const char* name, bool defaultValue)
    {
        const char* value = ReadEnvValue(name);
        if (value == nullptr)
            return defaultValue;

        if (std::strcmp(value, "1") == 0 ||
            std::strcmp(value, "true") == 0 ||
            std::strcmp(value, "yes") == 0 ||
            std::strcmp(value, "on") == 0)
        {
            return true;
        }

        if (std::strcmp(value, "0") == 0 ||
            std::strcmp(value, "false") == 0 ||
            std::strcmp(value, "no") == 0 ||
            std::strcmp(value, "off") == 0)
        {
            return false;
        }

        return defaultValue;
    }

    Ogre::ShadowTechnique ReadShadowTechnique(bool defaultEnabled)
    {
        return ReadBoolEnvValue("HELLO_RENDER_SHADOWS", defaultEnabled) ?
            Ogre::SHADOWTYPE_STENCIL_ADDITIVE :
            Ogre::SHADOWTYPE_NONE;
    }

    const char* GetShadowTechniqueName(Ogre::ShadowTechnique technique)
    {
        return technique == Ogre::SHADOWTYPE_NONE ? "none" : "stencil additive";
    }
}

#if defined(_WIN32)
static Ogre::String BuildDpiScaledVideoMode(unsigned int baseWidth, unsigned int baseHeight)
{
    const unsigned int scaledWidth = Ogre::DpiHelper::toPhysicalPixels(baseWidth);
    const unsigned int scaledHeight = Ogre::DpiHelper::toPhysicalPixels(baseHeight);

    return Ogre::StringConverter::toString(scaledWidth) + " x " +
           Ogre::StringConverter::toString(scaledHeight) + " @ 32-bit colour";
}

static unsigned int ReadBackgroundDimension(const char* name, unsigned int fallback, unsigned int minimum, unsigned int maximum)
{
	const char* value = std::getenv(name);
	if (value == nullptr || value[0] == 0) return fallback;
	char* end = nullptr;
	const long size = std::strtol(value, &end, 10);
	return end != value && *end == 0 && size >= static_cast<long>(minimum) && size <= static_cast<long>(maximum)
		? static_cast<unsigned int>(size) : fallback;
}
#endif


static ClientManager* s_ClientMgr = nullptr;
ClientManager* GetClientMgr()
{
    if (s_ClientMgr == nullptr)
    {
        s_ClientMgr = new ClientManager();
    }
    return s_ClientMgr;
}

ClientManager::ClientManager()
    : m_pRoot(nullptr), m_pCamera(nullptr), m_pSceneManager(nullptr),
    m_pRenderWindow(nullptr), m_pObfuscatedZipFactory(nullptr), m_pCameraController(nullptr),
    m_pDebugDrawer(nullptr), m_pGameManager(nullptr),
#if defined(HELLO_ENABLE_FGUI)
    m_pFairyGuiSystem(nullptr),
#endif
    m_shutdown(false)
{
    m_Timer.reset();

    m_lastUpdateTimeInMicro = m_Timer.getMicroseconds();//微秒级
    //m_lastUpdateTimeInMicro = m_Timer.getMilliseconds();//毫秒级
    m_lastDrawTimeInMicro = m_Timer.getMicroseconds();   //微秒级
    m_lastViewportWidth = 0;
    m_lastViewportHeight = 0;
}

ClientManager::~ClientManager()
{
#if defined(HELLO_ENABLE_FGUI)
    SAFE_DELETE(m_pFairyGuiSystem);
#endif
    SAFE_DELETE(m_pGameManager);
    g_GameManager = nullptr;
    SAFE_DELETE(m_pDebugDrawer);
    SAFE_DELETE(m_pInputManager);
    SAFE_DELETE(m_pCameraController);

    Gorilla::Silverback* pSilverback = Gorilla::Silverback::getSingletonPtr();
    if (pSilverback != nullptr) delete pSilverback;

    delete m_pRoot;
}

Ogre::Camera* ClientManager::getCamera()
{
	return m_pCamera;
}

OgreCameraController* ClientManager::getCameraController()
{
    return m_pCameraController;
}

RenderWindow* ClientManager::getRenderWindow()
{
	return m_pRenderWindow;
}

SceneManager* ClientManager::getSceneManager()
{
	return m_pSceneManager;
}

SceneNode* ClientManager::getRootSceneNode()
{
    return m_pSceneManager->getRootSceneNode();
}

void ClientManager::SetAppTitle(const String& appTitle)
{
    m_applicationTitle = appTitle;
}

void ClientManager::CreateFrameListener(Ogre::FrameListener* newListener)
{
    m_pRoot->addFrameListener(newListener);
}

void ClientManager::CreateWindowEventListener(Ogre::WindowEventListener* newListener)
{
    Ogre::WindowEventUtilities::addWindowEventListener(m_pRenderWindow, newListener);
}

void ClientManager::SetWindowActive(bool state)
{
    m_pRenderWindow->setActive(state);
}

void ClientManager::SetupResources()
{
    // Load resource paths from config file
    Ogre::ConfigFile cfg;
    cfg.load(APPLICATION_RESOURCES);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator secIter = cfg.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (secIter.hasMoreElements())
    {
        secName = secIter.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap* settings = secIter.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName, true);
        }
    }
}

bool ClientManager::Configure()
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    const Ogre::RenderSystemList& renderers = m_pRoot->getAvailableRenderers();

    if (renderers.empty())
    {
        Ogre::LogManager::getSingleton().logMessage("No render systems available. Check OpenGL/GL3Plus plugin linkage.");
        return false;
    }

#if defined(__APPLE__)
    Ogre::RenderSystem* selected = 0;
    Ogre::RenderSystem* glFallback = 0;
    for (Ogre::RenderSystemList::const_iterator it = renderers.begin(); it != renderers.end(); ++it)
    {
        if (!*it)
            continue;

        const Ogre::String& rsName = (*it)->getName();
        if (rsName.find("GL 3+") != Ogre::String::npos || rsName.find("OpenGL 3+") != Ogre::String::npos)
        {
            selected = *it;
            break;
        }

        if (rsName.find("OpenGL Rendering Subsystem") != Ogre::String::npos)
            glFallback = *it;
    }

    if (!selected)
        selected = glFallback ? glFallback : renderers.front();

    m_pRoot->setRenderSystem(selected);

    const Ogre::ConfigOptionMap& configOptions = selected->getConfigOptions();
    const bool isGL3 = selected->getName().find("GL 3+") != Ogre::String::npos ||
        selected->getName().find("OpenGL 3+") != Ogre::String::npos;

    if (configOptions.find("Video Mode") != configOptions.end())
        try { selected->setConfigOption("Video Mode", "1280 x 720"); } catch (...) {}
    if (configOptions.find("Full Screen") != configOptions.end())
        try { selected->setConfigOption("Full Screen", "No"); } catch (...) {}
    if (configOptions.find("FSAA") != configOptions.end())
    {
        const Ogre::String fsaa = ReadStringEnvValue("HELLO_RENDER_FSAA", "0");
        try { selected->setConfigOption("FSAA", fsaa); } catch (...) {}
        Ogre::LogManager::getSingleton().logMessage("Mac render FSAA: " + fsaa);
    }
    if (configOptions.find("VSync") != configOptions.end())
    {
        const Ogre::String vsync = ReadBoolEnvValue("HELLO_RENDER_VSYNC", true) ? "Yes" : "No";
        try { selected->setConfigOption("VSync", vsync); } catch (...) {}
        Ogre::LogManager::getSingleton().logMessage("Mac render VSync: " + vsync);
    }
    if (configOptions.find("Colour Depth") != configOptions.end())
        try { selected->setConfigOption("Colour Depth", "32"); } catch (...) {}
    if (configOptions.find("RTT Preferred Mode") != configOptions.end())
        try { selected->setConfigOption("RTT Preferred Mode", "FBO"); } catch (...) {}
    if (configOptions.find("sRGB Gamma Conversion") != configOptions.end())
        try { selected->setConfigOption("sRGB Gamma Conversion", "No"); } catch (...) {}
    if (configOptions.find("macAPI") != configOptions.end())
        try { selected->setConfigOption("macAPI", "cocoa"); } catch (...) {}
    if (configOptions.find("Content Scaling Factor") != configOptions.end())
    {
        const Ogre::String contentScale = ReadStringEnvValue("HELLO_RENDER_CONTENT_SCALE", "1.0");
        try { selected->setConfigOption("Content Scaling Factor", contentScale); } catch (...) {}
        Ogre::LogManager::getSingleton().logMessage("Mac render content scale: " + contentScale);
    }
    if (configOptions.find("contextProfile") != configOptions.end())
        try { selected->setConfigOption("contextProfile", isGL3 ? "1" : "0"); } catch (...) {}

    m_pRenderWindow = m_pRoot->initialise(true, m_applicationTitle);
    if (m_pRenderWindow)
    {
        m_pRenderWindow->setVisible(true);
        m_pRenderWindow->setActive(true);
    }
    return m_pRenderWindow != 0;
#elif defined(_WIN32)
    Ogre::RenderSystem* selected = 0;
    for (Ogre::RenderSystemList::const_iterator it = renderers.begin(); it != renderers.end(); ++it)
    {
        if (!*it)
            continue;

        const Ogre::String& rsName = (*it)->getName();
        Ogre::LogManager::getSingleton().logMessage("Detected renderer: " + rsName);
        if (rsName.find("Direct3D9") != Ogre::String::npos || rsName.find("D3D9") != Ogre::String::npos)
        {
            selected = *it;
            break;
        }
    }

    if (!selected)
    {
        Ogre::LogManager::getSingleton().logMessage("D3D9 renderer not found on Windows. Abort initialise to avoid shader/profile mismatch.");
        return false;
    }

    m_pRoot->setRenderSystem(selected);
    try { selected->setConfigOption("Full Screen", "No"); } catch (...) {}
    const Ogre::String dpiScaledVideoMode = BuildDpiScaledVideoMode(1280, 800);
    Ogre::LogManager::getSingleton().logMessage("Windows DPI-scaled video mode: " + dpiScaledVideoMode);
    try { selected->setConfigOption("Video Mode", dpiScaledVideoMode); } catch (...) {}
    try { selected->setConfigOption("VSync", "Yes"); } catch (...) {}
    try { selected->setConfigOption("FSAA", "0"); } catch (...) {}

    if (ReadBoolEnvValue("HELLO_WINDOW_BACKGROUND", false))
    {
		// A hidden HWND is unreliable for D3D9 device creation. Keep the render
		// window alive beyond the virtual desktop without activating or listing it.
		const HWND foregroundBefore = GetForegroundWindow();
		const unsigned int width = ReadBackgroundDimension("HELLO_WINDOW_WIDTH", 1280, 640, 3840);
		const unsigned int height = ReadBackgroundDimension("HELLO_WINDOW_HEIGHT", 800, 360, 2160);
		const int offscreenLeft = GetSystemMetrics(SM_XVIRTUALSCREEN) - static_cast<int>(width) - 64;
		const int offscreenTop = GetSystemMetrics(SM_YVIRTUALSCREEN) - static_cast<int>(height) - 64;
		m_pRoot->initialise(false, m_applicationTitle);
		Ogre::NameValuePairList parameters;
		parameters["hidden"] = "false";
		parameters["noActivate"] = "true";
		parameters["border"] = "none";
		parameters["outerDimensions"] = "true";
		parameters["left"] = Ogre::StringConverter::toString(offscreenLeft);
		parameters["top"] = Ogre::StringConverter::toString(offscreenTop);
		parameters["colourDepth"] = "32";
		parameters["FSAA"] = "0";
		parameters["vsync"] = "true";
		m_pRenderWindow = m_pRoot->createRenderWindow(m_applicationTitle, width, height, false, &parameters);
		m_pRenderWindow->setActive(true);
		unsigned int actualWidth = 0;
		unsigned int actualHeight = 0;
		unsigned int actualColourDepth = 0;
		int actualLeft = 0;
		int actualTop = 0;
		m_pRenderWindow->getMetrics(actualWidth, actualHeight, actualColourDepth, actualLeft, actualTop);
		const int virtualLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
		const int virtualTop = GetSystemMetrics(SM_YVIRTUALSCREEN);
		const int virtualRight = virtualLeft + GetSystemMetrics(SM_CXVIRTUALSCREEN);
		const int virtualBottom = virtualTop + GetSystemMetrics(SM_CYVIRTUALSCREEN);
		const bool isOffscreen = actualLeft + static_cast<int>(actualWidth) <= virtualLeft
			|| actualLeft >= virtualRight
			|| actualTop + static_cast<int>(actualHeight) <= virtualTop
			|| actualTop >= virtualBottom;
		Ogre::LogManager::getSingleton().logMessage("[WindowMode] background=true hidden="
			+ Ogre::StringConverter::toString(m_pRenderWindow->isHidden())
			+ " noActivate=true"
			+ " offscreen=" + Ogre::StringConverter::toString(isOffscreen)
			+ " foregroundUnchanged=" + Ogre::StringConverter::toString(foregroundBefore == GetForegroundWindow())
			+ " pixels=" + Ogre::StringConverter::toString(actualWidth) + "x" + Ogre::StringConverter::toString(actualHeight)
			+ " position=" + Ogre::StringConverter::toString(actualLeft) + "," + Ogre::StringConverter::toString(actualTop));
    }
    else
        m_pRenderWindow = m_pRoot->initialise(true, m_applicationTitle);
    return m_pRenderWindow != 0;
#else
    if (renderers.size() == 1)
    {
        m_pRoot->setRenderSystem(renderers.front());
        m_pRenderWindow = m_pRoot->initialise(true, m_applicationTitle);
        return true;
    }

    if (m_pRoot->restoreConfig() || m_pRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        m_pRenderWindow = m_pRoot->initialise(true, m_applicationTitle);

        return true;
    }

    return false;
#endif
}

void ClientManager::ChooseSceneManager()
{
    // The sandbox is only built with the generic scene manager.
    m_pSceneManager = m_pRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE);
}

void ClientManager::CreateCamera()
{
    m_pCamera = m_pSceneManager->createCamera("PlayerCamera");

    m_pCamera->setPosition(Ogre::Vector3(0, 1.0f, 0));

    // Look back along -Z
    m_pCamera->lookAt(Ogre::Vector3(0, 0, -1.0f));
    m_pCamera->setNearClipDistance(0.001f);

    m_pCamera->setAutoAspectRatio(true);

    m_pCameraController = new OgreCameraController(m_pCamera);
    m_pCameraController->setTopSpeed(5.0f);
}

void ClientManager::CreateViewports()
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = m_pRenderWindow->addViewport(m_pCamera);
    vp->setDimensions(0.0f, 0.0f, 1.0f, 1.0f);
    vp->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));

    UpdateViewportLayout(true);
}

void ClientManager::UpdateViewportLayout(bool force)
{
    if (m_pRenderWindow == nullptr || m_pRenderWindow->getNumViewports() == 0)
        return;

    Ogre::Viewport* vp = m_pRenderWindow->getViewport(0);
    if (vp == nullptr)
        return;

    const unsigned int currentWidth = vp->getActualWidth();
    const unsigned int currentHeight = vp->getActualHeight();
    if (!force && currentWidth == m_lastViewportWidth && currentHeight == m_lastViewportHeight)
        return;

    vp->setDimensions(0.0f, 0.0f, 1.0f, 1.0f);
    const unsigned int viewportWidth = vp->getActualWidth();
    const unsigned int viewportHeight = vp->getActualHeight();
    if (m_pCamera != nullptr && viewportHeight > 0)
        m_pCamera->setAspectRatio(Ogre::Real(viewportWidth) / Ogre::Real(viewportHeight));

    m_lastViewportWidth = viewportWidth;
    m_lastViewportHeight = viewportHeight;
}

void ClientManager::LoadResources(void)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

bool ClientManager::Setup(void)
{
    m_pRoot = new Ogre::Root("", APPLICATION_CONFIG, APPLICATION_LOG);
#if defined(_WIN32)
    m_pRoot->installPlugin(new Ogre::D3D9Plugin());
#elif defined(__APPLE__)
    m_pRoot->installPlugin(new Ogre::GL3PlusPlugin());
#else
    m_pRoot->installPlugin(new Ogre::GLPlugin());
#endif
    m_pRoot->installPlugin(new Ogre::ParticleFXPlugin());

    m_pObfuscatedZipFactory = new ObfuscatedZipFactory();
    Ogre::ArchiveManager::getSingleton().addArchiveFactory(m_pObfuscatedZipFactory);

    SetupResources();

    bool carryOn = Configure();
    if (!carryOn) return false;

    ChooseSceneManager();
    CreateCamera();
    CreateViewports();

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // Load resources
    LoadResources();

    Initialize();

    return true;
}

void ClientManager::Cleanup()
{

}

void ClientManager::Initialize()
{
    H3D_PROFILE_THREAD("MainThread");

    const Ogre::ColourValue ambient(0.0f, 0.0f, 0.0f);

    m_pRenderWindow->getViewport(0)->setBackgroundColour(ambient);
    m_pRenderWindow->setDeactivateOnFocusChange(false);

    m_pSceneManager->setAmbientLight(ambient);
#if defined(__APPLE__)
    const bool defaultShadowsEnabled = false;
#else
    const bool defaultShadowsEnabled = true;
#endif
    const Ogre::ShadowTechnique shadowTechnique = ReadShadowTechnique(defaultShadowsEnabled);
    m_pSceneManager->setShadowTechnique(shadowTechnique);
    Ogre::LogManager::getSingleton().logMessage(
        Ogre::String("Render shadows: ") + GetShadowTechniqueName(shadowTechnique));

    m_pCamera->setFarClipDistance(1000.0f);
    m_pCamera->setNearClipDistance(0.1f);
    m_pCamera->setAutoAspectRatio(true);

    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(4);
    
    if (Gorilla::Silverback::getSingletonPtr() == nullptr)
    {
        Gorilla::Silverback* pSilverback = new Gorilla::Silverback();
        pSilverback->loadAtlas(DEFAULT_ATLAS);
    }

    // Initialize InputManager
    m_pInputManager = new InputManager(
        m_pRenderWindow,
        m_pCameraController,
        [this](bool state) { SetShutdown(state); },
        [this](bool state) { SetWindowActive(state); });
    m_pInputManager->Initialize();

#if defined(HELLO_ENABLE_FGUI)
    FairyGuiSystemStartupContext fairyGuiContext;
    fairyGuiContext.renderWindow = m_pRenderWindow;
    fairyGuiContext.sceneManager = m_pSceneManager;
    m_pFairyGuiSystem = new FairyGuiSystem();
    m_pFairyGuiSystem->Initialize(fairyGuiContext);
#endif

    m_pDebugDrawer = new DebugDrawer();
    m_pDebugDrawer->Initialize();

    m_pGameManager = new GameManager(this);
    m_pGameManager->Initialize();
    g_GameManager = m_pGameManager;

    m_pInputManager->registerHandler(m_pGameManager);

    m_lastUpdateTimeInMicro = m_Timer.getMicroseconds();
    UpdateViewportLayout(true);
}

void ClientManager::Run()
{
    m_pRoot->startRendering();
}

void ClientManager::Draw()
{
    H3D_PROFILE_SCOPE("ClientManager::Draw");
    long long currTimeInMicro = m_Timer.getMicroseconds();
    SetProfileTime(P_RENDER_TIME, currTimeInMicro - m_lastDrawTimeInMicro);

#if defined(HELLO_ENABLE_FGUI)
    if (m_pFairyGuiSystem)
        m_pFairyGuiSystem->Render();
#endif

    m_lastDrawTimeInMicro = currTimeInMicro;
}

long long ClientManager::GetProfileTime(ProfileTimeType profile)
{
    if (profile < PROFILE_TIME_COUNT)
    {
        return m_profileTimes[profile];
    }
    return 0;
}

void ClientManager::SetProfileTime(ProfileTimeType profile, long long time)
{
    if (profile < PROFILE_TIME_COUNT)
    {
        m_profileTimes[profile] = time;
    }
}

long long ClientManager::GetSimulationTimeInMillis() const
{
    return m_pGameManager != nullptr ? m_pGameManager->getTimeInMillis() : 0;
}

void ClientManager::Update()
{
    static int simulationHz = ReadSimulationHz();
    static long long updatePerSecondInMicros = 1000000 / simulationHz;
    static int deltaMilliseconds = int(updatePerSecondInMicros / 1000);

    long long currTimeInMicros = m_Timer.getMicroseconds();
    long long timeDeltaInMicros = currTimeInMicros - m_lastUpdateTimeInMicro;

    if (m_pGameManager && timeDeltaInMicros >= updatePerSecondInMicros)
    {
        H3D_PROFILE_SCOPE("ClientManager::Update");
        const bool perfEnabled = RuntimeStallProfiler::IsEnabled();
        RuntimeSimulationTiming perfTiming;
        perfTiming.simulateDeltaMs = double(timeDeltaInMicros) / 1000.0;
        long long stageStartMicros = 0;
        SetProfileTime(P_TOTAL_SIMULATE_TIME, timeDeltaInMicros);
        H3D_PROFILE_PLOT("SimulateDeltaMs", double(timeDeltaInMicros) / 1000.0);

        {
            H3D_PROFILE_SCOPE("DebugDrawer::Clear");
            if (perfEnabled)
                stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
            m_pDebugDrawer->clear();
            if (perfEnabled)
                perfTiming.debugClearMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
        }

        {
            H3D_PROFILE_SCOPE("GameManager::Update");
            if (perfEnabled)
                stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
            m_pGameManager->Update(deltaMilliseconds);
            if (perfEnabled)
                perfTiming.gameUpdateMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
        }
        // 累加器步进：用固定间隔推进 lastUpdate，保留时间余数，避免在 render 帧率
        // 量化（VSync 锁 60fps、每帧 16.67ms）下丢节拍——此前 = currTimeInMicros
        // 丢弃余数，使 30Hz 目标实测掉到 ~24Hz。偶发长卡顿致积压超过 4 步时直接
        // 对齐当前时刻，防止追赶滞后（spiral of death）。
        m_lastUpdateTimeInMicro += updatePerSecondInMicros;
        if (currTimeInMicros - m_lastUpdateTimeInMicro >= updatePerSecondInMicros * 4)
            m_lastUpdateTimeInMicro = currTimeInMicros;

        {
            H3D_PROFILE_SCOPE("DebugDrawer::Build");
            if (perfEnabled)
                stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
            m_pDebugDrawer->build();
            if (perfEnabled)
                perfTiming.debugBuildMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
        }

        long long newTimeInMicros = m_Timer.getMicroseconds() - currTimeInMicros;
        SetProfileTime(P_SIMULATE_TIME, newTimeInMicros);
        perfTiming.simulateCostMs = double(newTimeInMicros) / 1000.0;
        H3D_PROFILE_PLOT("SimulateCostMs", double(newTimeInMicros) / 1000.0);

        {
            H3D_PROFILE_SCOPE("InputManager::Update");
            if (perfEnabled)
                stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
            m_pInputManager->update(deltaMilliseconds);
            if (perfEnabled)
                perfTiming.inputUpdateMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
        }
        if (perfEnabled)
            RuntimeStallProfiler::SetSimulationTiming(perfTiming);
    }
}

void ClientManager::InputCapture()
{
    H3D_PROFILE_SCOPE("ClientManager::InputCapture");
    m_pInputManager->capture();
}

void ClientManager::FrameRendering(const Ogre::FrameEvent& event)
{
    H3D_PROFILE_SCOPE("ClientManager::FrameRendering");

    {
        H3D_PROFILE_SCOPE("CameraController::frameRenderingQueued");
        m_pCameraController->frameRenderingQueued(event);
    }

#if defined(HELLO_ENABLE_FGUI)
    if (m_pFairyGuiSystem)
        m_pFairyGuiSystem->Update(event.timeSinceLastFrame);
#endif

    UpdateViewportLayout(false);
}

void ClientManager::WindowClosed()
{
    m_shutdown = true;

    if (m_pRoot)
        m_pRoot->queueEndRendering();

    // Input teardown is owned by the destructor. On Cocoa this callback can run
    // inside OIS keyboard capture, so destroying OIS here would delete the active stack object.
    if (m_pGameManager)
        m_pGameManager->HandleWindowClosed();
}

void ClientManager::WindowResized(unsigned int width, unsigned int height)
{
    m_pInputManager->resizeMouseState(width, height);

    Ogre::Viewport* vp = nullptr;
    if (m_pRenderWindow && m_pRenderWindow->getNumViewports() > 0)
        vp = m_pRenderWindow->getViewport(0);

    const unsigned int physicalWidth = vp ? vp->getActualWidth() : width;
    const unsigned int physicalHeight = vp ? vp->getActualHeight() : height;

    unsigned int uiWidth = Ogre::DpiHelper::toLogicalPixels(physicalWidth);
    unsigned int uiHeight = Ogre::DpiHelper::toLogicalPixels(physicalHeight);
    if (m_pRenderWindow)
    {
        const float pointToPixelScale = m_pRenderWindow->getViewPointToPixelScale();
        if (pointToPixelScale > 1.0f)
        {
            uiWidth = static_cast<unsigned int>(Ogre::Real(physicalWidth) / pointToPixelScale + 0.5f);
            uiHeight = static_cast<unsigned int>(Ogre::Real(physicalHeight) / pointToPixelScale + 0.5f);
        }
    }
    m_pGameManager->HandleWindowResized(uiWidth, uiHeight);

#if defined(HELLO_ENABLE_FGUI)
    if (m_pFairyGuiSystem)
        m_pFairyGuiSystem->HandleWindowResized(uiWidth, uiHeight);
#endif

    UpdateViewportLayout(true);
}
