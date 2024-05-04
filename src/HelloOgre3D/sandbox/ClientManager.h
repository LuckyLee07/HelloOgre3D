#ifndef __CLIENT_MANAGER__
#define __CLIENT_MANAGER__

#include "Singleton.h"
#include <unordered_map>
#include "OgreString.h"
#include "OgreTimer.h"

namespace Ogre
{
	class Camera;
	class RenderWindow;
	class Root;
	class SceneManager;
	class FrameListener;
}
namespace OgreBites
{
	class SdkCameraMan;
}

class InputManager;
class ObfuscatedZipFactory;
class ClientManager : public Fancy::Singleton<ClientManager>
{
public:
	enum ProfileTimeType
	{
		P_RENDER_TIME,
		P_SIMULATE_TIME,
		P_TOTAL_SIMULATE_TIME,

		PROFILE_TIME_COUNT
	};
public:
	ClientManager(void);
	~ClientManager();
	
	bool Setup();
	void Cleanup();
	void Initialize();

	void Run();
	void Draw();
	void Update();

	void InputCapture();
	void FrameRendering(const Ogre::FrameEvent& event);

	void SetAppTitle(const Ogre::String& appTitle);

	void CreateFrameListener(Ogre::FrameListener* newListener);

	void SetWindowActive(bool state);

	void WindowClosed();
	void WindowResized(unsigned int width, unsigned int height);

	long long GetProfileTime(ProfileTimeType profile);
	void SetProfileTime(ProfileTimeType profile, long long time);
	

public:
	Ogre::Camera* getCamera();
	Ogre::SceneManager* getSceneManager();
	Ogre::RenderWindow* getRenderWindow();
	Ogre::SceneNode* getRootSceneNode();
	OgreBites::SdkCameraMan* getCameraMan();

private:
	void ChooseSceneManager();
	bool Configure();
	void CreateCamera();
	void CreateViewports();
	void LoadResources();
	void SetupResources();
	
private:
	Ogre::Root* m_pRoot;
	Ogre::Camera* m_pCamera;
	Ogre::SceneManager* m_pSceneManager;
	Ogre::RenderWindow* m_pRenderWindow;
	ObfuscatedZipFactory* m_pObfuscatedZipFactory;

	Ogre::String m_applicationTitle;
	Ogre::String m_resourceConfig;

	Ogre::Timer m_Timer;
	long long m_lastUpdateTimeInMicro;
	long long m_lastDrawTimeInMicro;
	long long m_profileTimes[PROFILE_TIME_COUNT];
	
	InputManager* m_pInputManager;
	OgreBites::SdkCameraMan* m_pCameraMan;
};

ClientManager* GetClientMgr();

#endif  // __CLIENT_MANAGER__
