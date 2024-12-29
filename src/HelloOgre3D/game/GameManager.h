#ifndef __GAME_MANAGER_H__  
#define __GAME_MANAGER_H__

#include <map>
#include <vector>
#include "OgrePrerequisites.h"
#include "OISMouse.h"
#include "OISKeyboard.h"
#include "SandboxDef.h"

#define UI_LAYER_COUNT	16

namespace Gorilla
{
	class Layer;
	class Screen;
	class MarkupText;
}

class UIComponent;
class SandboxMgr;
class ScriptLuaVM;
class PhysicsWorld;
class ObjectManager;

class GameManager //tolua_exports
{ //tolua_exports
public:
	GameManager();
	~GameManager();

	static GameManager* GetInstance();

	void Initialize(Ogre::SceneManager* sceneManager);
	void Update(int deltaMilliseconds);

	void InitLuaEnv();
	void InitUIConfig();
	
public:
	//tolua_begin
	Ogre::Real getScreenWidth();
	Ogre::Real getScreenHeight();

	long long getTimeInMillis();
	Ogre::Real getTimeInSeconds();
	//tolua_end

	PhysicsWorld* getPhysicsWorld() { return m_pPhysicsWorld; }

	UIComponent* createUIComponent(unsigned int index);
	void setMarkupColor(unsigned int index, const Ogre::ColourValue& color);
	
public:
	void HandleWindowResized(unsigned int width, unsigned int height);
	void HandleWindowClosed();

	void HandleKeyPress(OIS::KeyCode keycode, unsigned int key);
	void HandleKeyRelease(OIS::KeyCode keycode, unsigned int key);

	void HandleMouseMove(int width, int height);
	void HandleMousePress(int width, int height, OIS::MouseButtonID buttonId);
	void HandleMouseRelease(int width, int height, OIS::MouseButtonID buttonId);

private:
	Gorilla::Layer* getUILayer(unsigned int index = 0);

private:
	ScriptLuaVM* m_pScriptVM;
	SandboxMgr* m_pSandboxMgr;
	PhysicsWorld* m_pPhysicsWorld;
	ObjectManager* m_pObjectManager;

	Gorilla::Screen* m_pUIScene;
	Gorilla::MarkupText* m_pMarkupText;
	Gorilla::Layer* m_pUILayers[UI_LAYER_COUNT];

	long long m_SimulationTime; // 运行时间

}; //tolua_exports

extern GameManager* g_GameManager;
GameManager* GetGameManager();

#endif; // __GAME_MANAGER_H__