#ifndef __HELLO_FAIRY_GUI_SYSTEM_H__
#define __HELLO_FAIRY_GUI_SYSTEM_H__

#include <string>

namespace Ogre
{
	class RenderWindow;
	class SceneManager;
}

namespace cocos2d
{
	class Scene;
}

namespace fairygui
{
	class GObject;
	class GRoot;
}

class FairyGuiSystem
{
public:
	FairyGuiSystem();
	~FairyGuiSystem();

	bool Initialize(Ogre::RenderWindow* renderWindow, Ogre::SceneManager* sceneManager);
	void Shutdown();

	void Update(float deltaSeconds);
	void Render();
	void HandleWindowResized(unsigned int width, unsigned int height);

	bool LoadPackage(const std::string& packagePath);
	fairygui::GObject* CreateObject(const std::string& packageName, const std::string& objectName);
	bool AddToRoot(fairygui::GObject* object);

	bool IsInitialized() const { return m_initialized; }
	fairygui::GRoot* GetRoot() const { return m_pRoot; }

private:
	Ogre::RenderWindow* m_pRenderWindow;
	Ogre::SceneManager* m_pSceneManager;
	cocos2d::Scene* m_pScene;
	fairygui::GRoot* m_pRoot;
	bool m_initialized;
	unsigned int m_screenWidth;
	unsigned int m_screenHeight;
};

#endif // __HELLO_FAIRY_GUI_SYSTEM_H__
