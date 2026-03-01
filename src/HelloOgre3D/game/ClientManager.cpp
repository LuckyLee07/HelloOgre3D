#include "ClientManager.h"
#include "AppConfig.h"
#include "GameManager.h"
#include "ObfuscatedZip.h"
#include "Samples/SdkCameraMan.h"
#include "debug/DebugDrawer.h"
#include "systems/input/InputManager.h"
#include "core/SandboxMacros.h"
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

using namespace Ogre;
#if defined(_WIN32)
static Ogre::String BuildDpiScaledVideoMode(unsigned int baseWidth, unsigned int baseHeight)
{
    const unsigned int scaledWidth = Ogre::DpiHelper::toPhysicalPixels(baseWidth);
    const unsigned int scaledHeight = Ogre::DpiHelper::toPhysicalPixels(baseHeight);

    return Ogre::StringConverter::toString(scaledWidth) + " x " +
           Ogre::StringConverter::toString(scaledHeight) + " @ 32-bit colour";
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
    m_pRenderWindow(nullptr), m_pObfuscatedZipFactory(nullptr), m_pCameraMan(nullptr), 
    m_pDebugDrawer(nullptr), m_pGameManager(nullptr), m_shutdown(false)
{
    m_Timer.reset();

    m_lastUpdateTimeInMicro = m_Timer.getMicroseconds();//微秒级
    //m_lastUpdateTimeInMicro = m_Timer.getMilliseconds();//毫秒级
    m_lastDrawTimeInMicro = m_Timer.getMicroseconds();   //微秒级
}

ClientManager::~ClientManager()
{
    SAFE_DELETE(m_pGameManager);
    g_GameManager = nullptr;
    SAFE_DELETE(m_pDebugDrawer);
    SAFE_DELETE(m_pInputManager);

    Gorilla::Silverback* pSilverback = Gorilla::Silverback::getSingletonPtr();
    if (pSilverback != nullptr) delete pSilverback;

    delete m_pRoot;
}

Ogre::Camera* ClientManager::getCamera()
{
	return m_pCamera;
}

OgreBites::SdkCameraMan* ClientManager::getCameraMan()
{
    return m_pCameraMan;
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
        try { selected->setConfigOption("FSAA", "0"); } catch (...) {}
    if (configOptions.find("VSync") != configOptions.end())
        try { selected->setConfigOption("VSync", "Yes"); } catch (...) {}
    if (configOptions.find("Colour Depth") != configOptions.end())
        try { selected->setConfigOption("Colour Depth", "32"); } catch (...) {}
    if (configOptions.find("RTT Preferred Mode") != configOptions.end())
        try { selected->setConfigOption("RTT Preferred Mode", "FBO"); } catch (...) {}
    if (configOptions.find("sRGB Gamma Conversion") != configOptions.end())
        try { selected->setConfigOption("sRGB Gamma Conversion", "No"); } catch (...) {}
    if (configOptions.find("macAPI") != configOptions.end())
        try { selected->setConfigOption("macAPI", "cocoa"); } catch (...) {}
    if (configOptions.find("Content Scaling Factor") != configOptions.end())
        try { selected->setConfigOption("Content Scaling Factor", "1.0"); } catch (...) {}
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
    const Ogre::String dpiScaledVideoMode = BuildDpiScaledVideoMode(1280, 720);
    Ogre::LogManager::getSingleton().logMessage("Windows DPI-scaled video mode: " + dpiScaledVideoMode);
    try { selected->setConfigOption("Video Mode", dpiScaledVideoMode); } catch (...) {}
    try { selected->setConfigOption("VSync", "Yes"); } catch (...) {}
    try { selected->setConfigOption("FSAA", "0"); } catch (...) {}

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

    m_pCameraMan = new OgreBites::SdkCameraMan(m_pCamera);
    m_pCameraMan->setTopSpeed(5.0f);
}

void ClientManager::CreateViewports()
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = m_pRenderWindow->addViewport(m_pCamera);
    vp->setDimensions(0.0f, 0.0f, 1.0f, 1.0f);
    vp->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));

    // Alter the camera aspect ratio to match the viewport
    m_pCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));}

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
    const Ogre::ColourValue ambient(0.0f, 0.0f, 0.0f);

    m_pRenderWindow->getViewport(0)->setBackgroundColour(ambient);
    m_pRenderWindow->setDeactivateOnFocusChange(false);

    m_pSceneManager->setAmbientLight(ambient);
    m_pSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

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
    m_pInputManager = new InputManager();
    m_pInputManager->Initialize();

    m_pDebugDrawer = new DebugDrawer();
    m_pDebugDrawer->Initialize();

    m_pGameManager = new GameManager(this);
    m_pGameManager->Initialize();
    g_GameManager = m_pGameManager;

    m_pInputManager->registerHandler(m_pGameManager);

    m_lastUpdateTimeInMicro = m_Timer.getMicroseconds();
}

void ClientManager::Run()
{
    m_pRoot->startRendering();
}

void ClientManager::Draw()
{
    long long currTimeInMicro = m_Timer.getMicroseconds();
    SetProfileTime(P_RENDER_TIME, currTimeInMicro - m_lastDrawTimeInMicro);

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

void ClientManager::Update()
{
    static long long updatePerSecondInMicros = 1000000 / 30;
    static int deltaMilliseconds = int(updatePerSecondInMicros / 1000);

    long long currTimeInMicros = m_Timer.getMicroseconds();
    long long timeDeltaInMicros = currTimeInMicros - m_lastUpdateTimeInMicro;

    if (m_pGameManager && timeDeltaInMicros >= updatePerSecondInMicros)
    {
        SetProfileTime(P_TOTAL_SIMULATE_TIME, timeDeltaInMicros);

        m_pDebugDrawer->clear();

        m_pGameManager->Update(deltaMilliseconds);
        m_lastUpdateTimeInMicro = currTimeInMicros;

        m_pDebugDrawer->build();

        long long newTimeInMicros = m_Timer.getMicroseconds() - currTimeInMicros;
        SetProfileTime(P_SIMULATE_TIME, newTimeInMicros);

        m_pInputManager->update(deltaMilliseconds);
    }
}

void ClientManager::InputCapture()
{
    m_pInputManager->capture();
}

void ClientManager::FrameRendering(const Ogre::FrameEvent& event)
{
    m_pCameraMan->frameRenderingQueued(event);

    if (m_pRenderWindow && m_pRenderWindow->getNumViewports() > 0)
    {
        Ogre::Viewport* vp = m_pRenderWindow->getViewport(0);
        if (vp)
        {
            vp->setDimensions(0.0f, 0.0f, 1.0f, 1.0f);
            const unsigned int vw = vp->getActualWidth();
            const unsigned int vh = vp->getActualHeight();
            if (m_pCamera && vh > 0)
                m_pCamera->setAspectRatio(Ogre::Real(vw) / Ogre::Real(vh));
        }
    }
}

void ClientManager::WindowClosed()
{
    m_pInputManager->closeWindow();
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

    const unsigned int uiWidth = Ogre::DpiHelper::toLogicalPixels(physicalWidth);
    const unsigned int uiHeight = Ogre::DpiHelper::toLogicalPixels(physicalHeight);
    m_pGameManager->HandleWindowResized(uiWidth, uiHeight);

    if (vp)
    {
        vp->setDimensions(0.0f, 0.0f, 1.0f, 1.0f);
        const unsigned int vw = vp->getActualWidth();
        const unsigned int vh = vp->getActualHeight();
        if (m_pCamera && vh > 0)
            m_pCamera->setAspectRatio(Ogre::Real(vw) / Ogre::Real(vh));
    }
}
