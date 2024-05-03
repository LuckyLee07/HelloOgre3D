#ifndef __GAME_MANAGER_H__  
#define __GAME_MANAGER_H__

#include <map>
#include "SandboxDef.h"
#include "SandboxObject.h"
#include "PhysicsWorld.h"
#include "SandboxMgr.h"
#include "UIComponent.h"
#include "OgrePrerequisites.h"
#include "OISMouse.h"
#include "OISKeyboard.h"

#define UI_LAYER_COUNT	16

namespace Gorilla
{
	class Layer;
	class Screen;
}

class ScriptLuaVM;
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
	Ogre::Real getScreenWidth();
	Ogre::Real getScreenHeight();

	UIComponent* createUIComponent(unsigned int index);
	void addSandboxObject(SandboxObject* pSandboxObject);

	void HandleKeyPress(OIS::KeyCode keycode, unsigned int key);
	void HandleKeyRelease(OIS::KeyCode keycode, unsigned int key);

	void HandleMouseMove(int width, int height);
	void HandleMousePress(int width, int height, OIS::MouseButtonID buttonId);
	void HandleMouseRelease(int width, int height, OIS::MouseButtonID buttonId);

private:
	Gorilla::Layer* getUILayer(unsigned int index = 0);

	unsigned int getNextObjectId() { return ++m_objectIndex; }

private:
	ScriptLuaVM* m_pScriptVM;
	SandboxMgr* m_pSandboxMgr;
	PhysicsWorld* m_pPhysicsWorld;
	
	unsigned int m_objectIndex;
	std::map<unsigned int, BaseObject*> m_pObjects;
	
	Gorilla::Screen* m_pUIScene;
	Gorilla::Layer* m_pUILayers[UI_LAYER_COUNT];

}; //tolua_exports

extern GameManager* g_GameManager;
GameManager* GetGameManager();

#endif; // __GAME_MANAGER_H__