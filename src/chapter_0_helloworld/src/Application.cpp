#include "Application.h"
#include "demo_framework/include/ObfuscatedZip.h"

#include "ogre3d/include/Ogre.h"
#include "ogre3d_direct3d9/include/OgreD3D9Plugin.h"
#include "ogre3d_particlefx/include/OgreParticleFXPlugin.h"

Application::Application(const Ogre::String& applicationTitle)
    : root_(NULL),
    camera_(NULL),
    sceneManager_(NULL),
    renderWindow_(NULL),
    resourceConfig_(Ogre::StringUtil::BLANK),
    applicationTitle_(applicationTitle),
    obfuscatedZipFactory_(NULL)
{
}

Application::~Application()
{
    delete root_;

    if(obfuscatedZipFactory_ != NULL)
    {
        delete obfuscatedZipFactory_;
        obfuscatedZipFactory_ = 0;
    }
}

void Application::ChooseSceneManager()
{
    // The sandbox is only built with the generic scene manager.
    sceneManager_ = root_->createSceneManager(Ogre::ST_EXTERIOR_CLOSE);
}

void Application::Cleanup()
{
}

bool Application::Configure()
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    if(root_->restoreConfig() || root_->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        renderWindow_ = root_->initialise(true, applicationTitle_);

        return true;
    }
    else
    {
        return false;
    }
}

void Application::CreateCamera()
{
    camera_ = sceneManager_->createCamera("PlayerCamera");

    camera_->setPosition(Ogre::Vector3(0, 1.0f, 0));

    // Look back along -Z
    camera_->lookAt(Ogre::Vector3(0, 0, -1.0f));
    camera_->setNearClipDistance(0.001f);

    camera_->setAutoAspectRatio(true);
}

void Application::CreateResourceListener()
{
}

void Application::CreateViewports()
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = renderWindow_->addViewport(camera_);
    vp->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));

    // Alter the camera aspect ratio to match the viewport
    camera_->setAspectRatio(
        Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

void Application::Draw()
{
}


Ogre::Camera* Application::GetCamera()
{
    return camera_;
}

Ogre::RenderWindow* Application::GetRenderWindow()
{
    return renderWindow_;
}

Ogre::SceneManager* Application::GetSceneManager()
{
    return sceneManager_;
}

void Application::Initialize()
{
    const Ogre::ColourValue ambient(0.0f, 0.0f, 0.0f);

    GetRenderWindow()->getViewport(0)->setBackgroundColour(ambient);
    GetSceneManager()->setAmbientLight(ambient);
    GetSceneManager()->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

    GetCamera()->setFarClipDistance(1000.0f);
    GetCamera()->setNearClipDistance(0.1f);
    GetCamera()->setAutoAspectRatio(true);

    GetRenderWindow()->setDeactivateOnFocusChange(false);

    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(4);
}

void Application::LoadResources(void)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void Application::Run()
{
    if (!Setup()) return;

    root_->startRendering();

    Cleanup();
}

bool Application::Setup(void)
{
	root_ = new Ogre::Root("", APPLICATION_CONFIG, APPLICATION_LOG);
    root_->installPlugin(new Ogre::D3D9Plugin());
    root_->installPlugin(new Ogre::ParticleFXPlugin());

    obfuscatedZipFactory_ = new ObfuscatedZipFactory();
    Ogre::ArchiveManager::getSingleton().addArchiveFactory(obfuscatedZipFactory_);

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

void Application::SetupResources()
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(APPLICATION_RESOURCES);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
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

void Application::Update()
{
}
