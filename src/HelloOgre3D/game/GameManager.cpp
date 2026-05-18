#include "GameManager.h"
#include "Ogre.h"
#include "OgreDpiHelper.h"
#include "ScriptLuaVM.h"
#include "tolua++.h"
#include "LuaInterface.h"
#include <algorithm>
#include "GlobalFuncs.h"
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include <winsock.h>
#else
#include <sys/time.h>
#endif
#include "ClientManager.h"
#if defined(HELLO_ENABLE_FGUI)
#include "ui/fairygui/FairyGuiSystem.h"
#endif
#include "systems/ui/UIManager.h"
#include "systems/manager/SandboxMgr.h"
#include "systems/manager/ObjectManager.h"
#include "debug/DebugDrawer.h"
#include "systems/physics/PhysicsWorld.h"
#include "core/SandboxMacros.h"
#include "profiling/Profile.h"

using namespace Ogre;

extern int tolua_SandboxToLua_open(lua_State* tolua_S);
extern int tolua_GameToLua_open(lua_State* tolua_S);
extern int tolua_SandboxToLua_Manual(lua_State* tolua_S);

GameManager* g_GameManager = nullptr;
GameManager* GetGameManager()
{
	return g_GameManager;
}

GameManager::GameManager(ClientManager* pClientMgr)
	: m_pClientManager(pClientMgr), m_SimulationTime(0), m_pScriptVM(nullptr),
	m_pPhysicsWorld(nullptr), m_pSandboxMgr(nullptr), m_pObjectManager(nullptr), m_pUIManager(nullptr),
	m_fairyGuiLastPackageName()
{
	
}

GameManager::~GameManager()
{
	SAFE_DELETE(m_pSandboxMgr);
	SAFE_DELETE(m_pObjectManager);
	SAFE_DELETE(m_pPhysicsWorld);
	SAFE_DELETE(m_pUIManager);

	g_SandboxMgr = nullptr;
	g_ObjectManager = nullptr;
	m_pClientManager = nullptr;
}

GameManager* GameManager::GetInstance()
{
	return g_GameManager;
}

bool GameManager::isFairyGuiAvailable()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->IsInitialized();
#else
	return false;
#endif
}

const char* GameManager::loadFairyGuiPackage(const char* packagePath)
{
	m_fairyGuiLastPackageName.clear();
#if defined(HELLO_ENABLE_FGUI)
	if (packagePath == nullptr)
		return "";

	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system == nullptr)
		return "";

	m_fairyGuiLastPackageName = system->LoadPackageAndGetName(packagePath);
#endif
	return m_fairyGuiLastPackageName.c_str();
}

int GameManager::createFairyGuiObject(const char* packageName, const char* objectName)
{
#if defined(HELLO_ENABLE_FGUI)
	if (packageName == nullptr || objectName == nullptr)
		return 0;

	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system == nullptr)
		return 0;

	return system->CreateObjectHandle(packageName, objectName);
#else
	return 0;
#endif
}

bool GameManager::addFairyGuiObjectToRoot(int objectHandle)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->AddObjectHandleToRoot(objectHandle);
#else
	return false;
#endif
}

bool GameManager::setFairyGuiObjectPosition(int objectHandle, Ogre::Real x, Ogre::Real y)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandlePosition(objectHandle, x, y);
#else
	return false;
#endif
}

bool GameManager::setFairyGuiObjectSize(int objectHandle, Ogre::Real width, Ogre::Real height)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleSize(objectHandle, width, height);
#else
	return false;
#endif
}

bool GameManager::setFairyGuiObjectVisible(int objectHandle, bool visible)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleVisible(objectHandle, visible);
#else
	return false;
#endif
}

bool GameManager::centerFairyGuiObject(int objectHandle, bool restraint)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->CenterObjectHandle(objectHandle, restraint);
#else
	return false;
#endif
}

bool GameManager::removeFairyGuiObject(int objectHandle)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->RemoveObjectHandle(objectHandle);
#else
	return false;
#endif
}

void GameManager::clearFairyGuiObjects()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system != nullptr)
		system->ClearObjectHandles();
#endif
}

void GameManager::Initialize()
{
	m_pScriptVM = GetScriptLuaVM();

	m_pUIManager = new UIManager(this);
	m_pUIManager->InitConfig();

	m_pPhysicsWorld = new PhysicsWorld();
	m_pPhysicsWorld->initilize();

	m_pObjectManager = new ObjectManager(m_pPhysicsWorld);
	g_ObjectManager = m_pObjectManager;

	UIService uiservice(m_pUIManager);
	ObjectFactory objfactory(m_pObjectManager);
	CameraService camservice(m_pClientManager);

	Ogre::SceneManager* pSceneManager = m_pClientManager->getSceneManager();
	m_pSandboxMgr = new SandboxMgr(uiservice, camservice, objfactory, pSceneManager);
	g_SandboxMgr = m_pSandboxMgr;

	this->InitLuaEnv();
}

void GameManager::InitLuaEnv()
{
	// 设置ToLua对象 
	tolua_SandboxToLua_open(m_pScriptVM->getLuaState());
	tolua_GameToLua_open(m_pScriptVM->getLuaState());
	tolua_SandboxToLua_Manual(m_pScriptVM->getLuaState());

	// 设置lua可用的c++对象 
	m_pScriptVM->setUserTypePointer("Sandbox", "SandboxMgr", m_pSandboxMgr);
	m_pScriptVM->setUserTypePointer("GameManager", "GameManager", this);
	m_pScriptVM->setUserTypePointer("ObjectManager", "ObjectManager", m_pObjectManager);
	m_pScriptVM->setUserTypePointer("DebugDrawer", "DebugDrawer", DebugDrawer::GetInstance());
	m_pScriptVM->setUserTypePointer("LuaInterface", "LuaInterface", LuaInterface::GetInstance());
	
	m_pScriptVM->callFile("res/scripts/script_init.lua");

	struct timeval stNow;
	gettimeofday(&stNow, NULL);
	m_pScriptVM->callFunction("__init__", "ii", stNow.tv_sec, stNow.tv_usec / 1000);
}

void GameManager::Update(int deltaMilliseconds)
{
	H3D_PROFILE_SCOPE("GameManager::UpdateStages");
	//struct timeval stNow;
	//gettimeofday(&stNow, NULL);
	{
		H3D_PROFILE_SCOPE("Lua::__tick__");
		m_pScriptVM->callFunction("__tick__", "i", deltaMilliseconds);
	}

	m_SimulationTime += deltaMilliseconds;

	{
		H3D_PROFILE_SCOPE("ObjectManager::Update");
		m_pObjectManager->Update(deltaMilliseconds);
	}

	{
		H3D_PROFILE_SCOPE("PhysicsWorld::Step");
		m_pPhysicsWorld->stepWorld();
	}

	{
		H3D_PROFILE_SCOPE("Lua::Sandbox_Update");
		m_pScriptVM->callFunction("Sandbox_Update", "i", deltaMilliseconds);
	}
}

Ogre::Camera* GameManager::getCamera()
{
	return m_pClientManager->getCamera();
}

Ogre::SceneNode* GameManager::getRootSceneNode()
{
	return m_pClientManager->getRootSceneNode();
}

Ogre::SceneManager* GameManager::getSceneManager()
{
	return m_pClientManager->getSceneManager();
}

Ogre::Real GameManager::getScreenWidth()
{
	if (m_pUIManager != nullptr)
	{
		Ogre::Real uiWidth = m_pUIManager->GetScreenWidth();
		if (uiWidth > 0.0f)
			return uiWidth;
	}

	Ogre::RenderWindow* renderWindow = m_pClientManager->getRenderWindow();
	if (renderWindow != nullptr)
	{
		unsigned int width = 0, height = 0, depth = 0;
		int left = 0, top = 0;
		renderWindow->getMetrics(width, height, depth, left, top);
		return Ogre::Real(Ogre::DpiHelper::toLogicalPixels(width));
	}

	Ogre::Camera* camera = m_pClientManager->getCamera();
	if (camera && camera->getViewport())
		return Ogre::Real(Ogre::DpiHelper::toLogicalPixels(static_cast<unsigned int>(camera->getViewport()->getActualWidth())));
	return 0.0f;
}

Ogre::Real GameManager::getScreenHeight()
{
	if (m_pUIManager != nullptr)
	{
		Ogre::Real uiHeight = m_pUIManager->GetScreenHeight();
		if (uiHeight > 0.0f)
			return uiHeight;
	}

	Ogre::RenderWindow* renderWindow = m_pClientManager->getRenderWindow();
	if (renderWindow != nullptr)
	{
		unsigned int width = 0, height = 0, depth = 0;
		int left = 0, top = 0;
		renderWindow->getMetrics(width, height, depth, left, top);
		return Ogre::Real(Ogre::DpiHelper::toLogicalPixels(height));
	}

	Ogre::Camera* camera = m_pClientManager->getCamera();
	if (camera && camera->getViewport())
		return Ogre::Real(Ogre::DpiHelper::toLogicalPixels(static_cast<unsigned int>(camera->getViewport()->getActualHeight())));
	return 0.0f;
}

InputManager* GameManager::getInputManager()
{
	return m_pClientManager->getInputManager();
}

void GameManager::HandleWindowClosed()
{
	//m_pScriptVM->callFunction("EventHandle_WindowClosed", "");
}

void GameManager::HandleWindowResized(unsigned int width, unsigned int height)
{
	if (m_pUIManager != nullptr)
		m_pUIManager->HandleWindowResized(width, height);
	m_pScriptVM->callFunction("EventHandle_WindowResized", "ii", width, height);
}

void GameManager::OnKeyPressed(OIS::KeyCode keycode, unsigned int key)
{
	m_pScriptVM->callFunction("EventHandle_Keyboard", "ib", keycode, true);
}

void GameManager::OnKeyReleased(OIS::KeyCode keycode, unsigned int key)
{
	m_pScriptVM->callFunction("EventHandle_Keyboard", "ib", keycode, false);

	m_pObjectManager->HandleKeyEvent(keycode, key);
}

void GameManager::OnMouseMoved(const OIS::MouseEvent& evt)
{

}

void GameManager::OnMousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID btn)
{

}

void GameManager::OnMouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID btn)
{

}

long long GameManager::getTimeInMillis()
{
	return m_SimulationTime;
}

Ogre::Real GameManager::getTimeInSeconds()
{
	return (Ogre::Real)(getTimeInMillis() / 1000);
}
