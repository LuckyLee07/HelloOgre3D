#ifndef __GAME_MANAGER_H__  
#define __GAME_MANAGER_H__

#include <map>
#include <string>
#include <vector>
#include "OgrePrerequisites.h"
#include "systems/input/IInputHandler.h"

class SandboxMgr;
class ScriptLuaVM;
class PhysicsWorld;
class UIManager;
class InputManager;
class ObjectManager;
class ObjectFactory;
class ClientManager;
class FairyGuiLuaApi;
class CameraService;

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
	std::string buildRuntimeResourceDump(int maxEntriesPerType);
	//tolua_end

	Ogre::Camera* getCamera();
	Ogre::SceneNode* getRootSceneNode();
	Ogre::SceneManager* getSceneManager();
	
	InputManager* getInputManager();
	PhysicsWorld* getPhysicsWorld() { return m_pPhysicsWorld; }

public:
	void HandleWindowClosed();
	void HandleWindowResized(unsigned int width, unsigned int height);

	virtual bool OnKeyPressed(OIS::KeyCode keycode, unsigned int key);
	virtual bool OnKeyReleased(OIS::KeyCode keycode, unsigned int key);

	virtual bool OnMouseMoved(const OIS::MouseEvent& evt);
	virtual bool OnMousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID btn);
	virtual bool OnMouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID btn);

private:
	ClientManager* m_pClientManager;

	ScriptLuaVM* m_pScriptVM;
	SandboxMgr* m_pSandboxMgr;
	PhysicsWorld* m_pPhysicsWorld;

	CameraService* m_pCameraService;
	UIManager* m_pUIManager;
	ObjectManager* m_pObjectManager;
	ObjectFactory* m_pObjectFactory;

	long long m_SimulationTime; // 运行时间

	FairyGuiLuaApi* m_pFairyGuiLuaApi;

}; //tolua_exports

extern GameManager* g_GameManager;
GameManager* GetGameManager();

#endif; // __GAME_MANAGER_H__
