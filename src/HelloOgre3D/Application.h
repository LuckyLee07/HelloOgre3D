#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "OgreString.h"

class ObfuscatedZipFactory;

namespace Ogre
{
    class Camera;
    class RenderWindow;
    class Root;
    class SceneManager;
}

#ifdef _DEBUG
#define APPLICATION_LOG         "Sandbox_d.log"
#define APPLICATION_CONFIG      "Sandbox_d.cfg"
#define APPLICATION_RESOURCES	"SandboxResources_d.cfg"
#else
#define APPLICATION_LOG         "Sandbox.log"
#define APPLICATION_CONFIG      "Sandbox.cfg"
#define APPLICATION_RESOURCES	"SandboxResources.cfg"
#endif

class Application
{
public:
	Application(const Ogre::String& applicationTitle = "");

    virtual ~Application();

    virtual void Cleanup();

    virtual void Draw();

    Ogre::Camera* GetCamera();

    Ogre::SceneManager* GetSceneManager();

    Ogre::RenderWindow* GetRenderWindow();
	
    virtual void Initialize();

    void Run();

    virtual void Update();

private:
    ObfuscatedZipFactory* obfuscatedZipFactory_;

    Ogre::String applicationTitle_;
    Ogre::Root* root_;
    Ogre::Camera* camera_;
    Ogre::SceneManager* sceneManager_;
    Ogre::RenderWindow* renderWindow_;
    Ogre::String resourceConfig_;

    void ChooseSceneManager();
    bool Configure();
    void CreateCamera();
    void CreateResourceListener();
    void CreateViewports();
    void LoadResources();
    bool Setup();
    void SetupResources();

};  // class Application

#endif  // __APPLICATION_H__
