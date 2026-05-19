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

	bool isFairyGuiAvailable();
	const char* loadFairyGuiPackage(const char* packagePath);
	bool removeFairyGuiPackage(const char* packageName);
	int createFairyGuiObject(const char* packageName, const char* objectName);
	int createFairyGuiModalMask(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha);
	int getFairyGuiChild(int objectHandle, const char* childPath);
	bool addFairyGuiObjectToRoot(int objectHandle);
	bool setFairyGuiObjectPosition(int objectHandle, Ogre::Real x, Ogre::Real y);
	bool setFairyGuiObjectSize(int objectHandle, Ogre::Real width, Ogre::Real height);
	bool setFairyGuiObjectVisible(int objectHandle, bool visible);
	bool setFairyGuiObjectSortingOrder(int objectHandle, int sortingOrder);
	bool setFairyGuiObjectText(int objectHandle, const char* text);
	bool setFairyGuiObjectIcon(int objectHandle, const char* icon);
	bool setFairyGuiObjectLoaderUrl(int objectHandle, const char* url);
	bool setFairyGuiObjectControllerIndex(int objectHandle, const char* controllerName, int selectedIndex);
	bool centerFairyGuiObject(int objectHandle, bool restraint);
	int addFairyGuiEventListener(int objectHandle, const char* childPath, int eventType, int callbackId);
	int addFairyGuiClickListener(int objectHandle, const char* childPath, int callbackId);
	bool removeFairyGuiListener(int bindingId);
	bool removeFairyGuiObject(int objectHandle);
	void clearFairyGuiObjects();
	//tolua_end

	Ogre::Camera* getCamera();
	Ogre::SceneNode* getRootSceneNode();
	Ogre::SceneManager* getSceneManager();
	
	InputManager* getInputManager();
	PhysicsWorld* getPhysicsWorld() { return m_pPhysicsWorld; }

public:
	void HandleWindowClosed();
	void HandleWindowResized(unsigned int width, unsigned int height);

	virtual void OnKeyPressed(OIS::KeyCode keycode, unsigned int key);
	virtual void OnKeyReleased(OIS::KeyCode keycode, unsigned int key);

	virtual bool OnMouseMoved(const OIS::MouseEvent& evt);
	virtual bool OnMousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID btn);
	virtual bool OnMouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID btn);

private:
	ClientManager* m_pClientManager;

	ScriptLuaVM* m_pScriptVM;
	SandboxMgr* m_pSandboxMgr;
	PhysicsWorld* m_pPhysicsWorld;

	UIManager* m_pUIManager;
	ObjectManager* m_pObjectManager;

	long long m_SimulationTime; // 运行时间

	std::string m_fairyGuiLastPackageName;

}; //tolua_exports

extern GameManager* g_GameManager;
GameManager* GetGameManager();

#endif; // __GAME_MANAGER_H__
