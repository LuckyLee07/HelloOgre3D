#ifndef __GAME_MANAGER_H__  
#define __GAME_MANAGER_H__

#include <map>
#include <vector>
#include "OgrePrerequisites.h"
#include "input/IInputHandler.h"

class SandboxMgr;
class ScriptLuaVM;
class PhysicsWorld;
class UIManager;
class InputManager;
class ObjectManager;
class ClientManager;

class GameManager : public IInputHandler //tolua_exports
{ //tolua_exports
public:
	GameManager(ClientManager*);
	~GameManager();

	static GameManager* GetInstance();

	void Initialize();
	void Update(int deltaMilliseconds);

	void InitLuaEnv();
	
public:
	//tolua_begin
	Ogre::Real getScreenWidth();
	Ogre::Real getScreenHeight();

	long long getTimeInMillis();
	Ogre::Real getTimeInSeconds();
	//tolua_end

	Ogre::Camera* getCamera();
	Ogre::SceneNode* getRootSceneNode();
	Ogre::SceneManager* getSceneManager();

	InputManager* getInputManager() { return m_pInputManager; }
	PhysicsWorld* getPhysicsWorld() { return m_pPhysicsWorld; }

public:
	void InputCapture();
	void HandleWindowClosed();
	void HandleWindowResized(unsigned int width, unsigned int height);

	virtual void OnKeyPressed(OIS::KeyCode keycode, unsigned int key);
	virtual void OnKeyReleased(OIS::KeyCode keycode, unsigned int key);

	virtual void OnMouseMoved(const OIS::MouseEvent& evt);
	virtual void OnMousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID btn);
	virtual void OnMouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID btn);

private:
	ClientManager* m_pClientManager;

	ScriptLuaVM* m_pScriptVM;
	SandboxMgr* m_pSandboxMgr;
	PhysicsWorld* m_pPhysicsWorld;
	InputManager* m_pInputManager;

	UIManager* m_pUIManager;
	ObjectManager* m_pObjectManager;

	long long m_SimulationTime; // 运行时间

}; //tolua_exports

extern GameManager* g_GameManager;
GameManager* GetGameManager();

#endif; // __GAME_MANAGER_H__