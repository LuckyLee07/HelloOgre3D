#include "ClientManager.h"
#include "SandboxDef.h"
#include "GameManager.h"
#include "ObfuscatedZip.h"

#include "Ogre.h"
#include "OgreD3D9Plugin.h"
#include "OgreParticleFXPlugin.h"
#include "ogre3d_gorilla/include/Gorilla.h"

using namespace Ogre;

ClientManager::ClientManager()
    : m_pRoot(nullptr), m_pCamera(nullptr), m_pSceneManager(nullptr), 
    m_pRenderWindow(nullptr), m_pObfuscatedZipFactory(nullptr)
{
    m_Timer.reset();

    m_lastUpdateTimeInMicro = m_Timer.getMilliseconds();
}

ClientManager::~ClientManager()
{
    delete g_GameManager;

    Gorilla::Silverback* pSilverback = Gorilla::Silverback::getSingletonPtr();
    if (pSilverback != nullptr) delete pSilverback;

    delete m_pRoot;
}

Ogre::Camera* ClientManager::getCamera()
{
	return m_pCamera;
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
    if (m_pRoot->restoreConfig() || m_pRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        m_pRenderWindow = m_pRoot->initialise(true, m_applicationTitle);

        return true;
    }
    else
    {
        return false;
    }
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
}

void ClientManager::CreateViewports()
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = m_pRenderWindow->addViewport(m_pCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));

    // Alter the camera aspect ratio to match the viewport
    m_pCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

void ClientManager::LoadResources(void)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

bool ClientManager::Setup(void)
{
    m_pRoot = new Ogre::Root("", APPLICATION_CONFIG, APPLICATION_LOG);
    m_pRoot->installPlugin(new Ogre::D3D9Plugin());
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
    
    Gorilla::Silverback* pSilverback = new Gorilla::Silverback();
    pSilverback->loadAtlas(DEFAULT_ATLAS);

    g_GameManager = new GameManager();
    g_GameManager->Initialize(m_pSceneManager);

    m_lastUpdateTimeInMicro = m_Timer.getMilliseconds();
}

void ClientManager::Run()
{
    m_pRoot->startRendering();
}

void ClientManager::Draw()
{
    
}

void ClientManager::Update()
{
    static long long updatePerSecondInMicros = 1000000 / 30;
    static int deltaMilliseconds = int(updatePerSecondInMicros / 1000);

    long long currTimeInMicros = m_Timer.getMicroseconds();
    long long timeDeltaInMicros = currTimeInMicros - m_lastUpdateTimeInMicro;

    if (g_GameManager && timeDeltaInMicros >= updatePerSecondInMicros)
    {
        g_GameManager->Update(deltaMilliseconds);
        m_lastUpdateTimeInMicro = currTimeInMicros;
    }
}

static ClientManager* s_ClientMgr = nullptr;
ClientManager* GetClientMgr()
{
	if (s_ClientMgr == nullptr)
	{
        s_ClientMgr = new ClientManager();
	}
	return s_ClientMgr;
}