#ifndef __GAME_MANAGER_H__  
#define __GAME_MANAGER_H__

#include <map>
#include <vector>
#include "OgrePrerequisites.h"
#include "input/IInputHandler.h"

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

class GameManager : public IInputHandler //tolua_exports
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

	virtual void OnKeyPressed(OIS::KeyCode keycode, unsigned int key);
	virtual void OnKeyReleased(OIS::KeyCode keycode, unsigned int key);

	virtual void OnMouseMoved(const OIS::MouseEvent& evt);
	virtual void OnMousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID btn);
	virtual void OnMouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID btn);

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