#ifndef __CLIENT_MANAGER__
#define __CLIENT_MANAGER__

#include "Singleton.h"
#include <unordered_map>
#include "OgreString.h"

namespace Ogre
{
	class Camera;
	class RenderWindow;
	class Root;
	class SceneManager;
}
class ObfuscatedZipFactory;

class ClientManager : public Fancy::Singleton<ClientManager>
{
public:
	ClientManager(void);
	~ClientManager();
	
	bool Setup();
	void Initialize();

	void Draw();
	void Update();
	void Cleanup();
	void SetAppTitle(const Ogre::String& appTitle);

public:
	Ogre::Camera* getCamera();
	Ogre::SceneManager* getSceneManager();
	Ogre::RenderWindow* getRenderWindow();
	Ogre::SceneNode* getRootSceneNode();

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
};

ClientManager* GetClientMgr();

#endif  // __CLIENT_MANAGER__
