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
	int createFairyGuiContainer(const char* name);
	int createFairyGuiChildContainer(int ownerHandle, const char* name);
	int createFairyGuiLoader(int ownerHandle, const char* name, const char* url);
	int createFairyGuiText(int ownerHandle, const char* name, const char* text, Ogre::Real fontSize, Ogre::Real red, Ogre::Real green, Ogre::Real blue);
	int createFairyGuiTextInput(int ownerHandle, const char* name, const char* text, Ogre::Real fontSize, Ogre::Real red, Ogre::Real green, Ogre::Real blue);
	int createFairyGuiModalMask(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha);
	int getFairyGuiLastRenderCommandCount();
	int getFairyGuiLastTriangleCount();
	int getFairyGuiScreenWidth();
	int getFairyGuiScreenHeight();
	int getFairyGuiRuntimeObjectHandleCount();
	int getFairyGuiRuntimeListenerBindingCount();
	int getFairyGuiMaterialCount();
	int getFairyGuiTextureCount();
	int getFairyGuiMaterialAliasCount();
	int getFairyGuiTextureAliasCount();
	bool plotFairyGuiServiceStats(int serviceOpenTotal, int serviceKindCount, int toastQueueCount, int loadingRefTotal, int serviceCreatedTotal, int serviceClosedTotal, int serviceFailedTotal, int servicePeakOpen);
	int getFairyGuiChild(int objectHandle, const char* childPath);
	int getFairyGuiListItem(int objectHandle, int itemIndex);
	int getFairyGuiListItemCount(int objectHandle);
	bool addFairyGuiObjectToRoot(int objectHandle);
	bool addFairyGuiObjectToParent(int objectHandle, int parentHandle);
	bool setFairyGuiObjectPosition(int objectHandle, Ogre::Real x, Ogre::Real y);
	bool setFairyGuiObjectSize(int objectHandle, Ogre::Real width, Ogre::Real height);
	bool setFairyGuiObjectVisible(int objectHandle, bool visible);
	bool setFairyGuiObjectAlpha(int objectHandle, Ogre::Real alpha);
	bool setFairyGuiObjectTouchable(int objectHandle, bool touchable);
	bool setFairyGuiObjectMask(int objectHandle, int maskHandle, bool inverted);
	bool setFairyGuiObjectSortingOrder(int objectHandle, int sortingOrder);
	bool setFairyGuiObjectText(int objectHandle, const char* text);
	const char* getFairyGuiObjectText(int objectHandle);
	bool setFairyGuiObjectIcon(int objectHandle, const char* icon);
	bool setFairyGuiObjectLoaderUrl(int objectHandle, const char* url);
	bool setFairyGuiObjectControllerIndex(int objectHandle, const char* controllerName, int selectedIndex);
	bool setFairyGuiObjectValue(int objectHandle, Ogre::Real value);
	Ogre::Real getFairyGuiObjectValue(int objectHandle);
	bool setFairyGuiObjectMin(int objectHandle, Ogre::Real minValue);
	Ogre::Real getFairyGuiObjectMin(int objectHandle);
	bool setFairyGuiObjectMax(int objectHandle, Ogre::Real maxValue);
	Ogre::Real getFairyGuiObjectMax(int objectHandle);
	bool setFairyGuiComboBoxSelectedIndex(int objectHandle, int selectedIndex);
	int getFairyGuiComboBoxSelectedIndex(int objectHandle);
	bool setFairyGuiComboBoxValue(int objectHandle, const char* value);
	const char* getFairyGuiComboBoxValue(int objectHandle);
	bool playFairyGuiTransition(int objectHandle, const char* transitionName, int times, Ogre::Real delay, int callbackId);
	bool stopFairyGuiTransition(int objectHandle, const char* transitionName, bool setToComplete, bool processCallback);
	bool focusFairyGuiObject(int objectHandle);
	bool clearFairyGuiFocus();
	int getFairyGuiFocusedObject();
	bool setFairyGuiListItemCount(int objectHandle, int itemCount);
	bool setFairyGuiListSelectedIndex(int objectHandle, int selectedIndex);
	int getFairyGuiListSelectedIndex(int objectHandle);
		bool scrollFairyGuiListToView(int objectHandle, int itemIndex);
		bool centerFairyGuiObject(int objectHandle, bool restraint);
		bool injectFairyGuiMouseMove(int x, int y);
		bool injectFairyGuiMouseDown(int x, int y, int button);
		bool injectFairyGuiMouseUp(int x, int y, int button);
		bool injectFairyGuiMouseWheel(int x, int y, int wheelDelta);
		bool injectFairyGuiClick(int x, int y, int button);
		bool injectFairyGuiKeyPressed(int keyCode, int keyText);
		bool injectFairyGuiKeyReleased(int keyCode, int keyText);
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

	UIManager* m_pUIManager;
	ObjectManager* m_pObjectManager;

	long long m_SimulationTime; // 运行时间

	std::string m_fairyGuiLastPackageName;
	std::string m_fairyGuiLastObjectText;
	std::string m_fairyGuiLastObjectValue;

}; //tolua_exports

extern GameManager* g_GameManager;
GameManager* GetGameManager();

#endif; // __GAME_MANAGER_H__
