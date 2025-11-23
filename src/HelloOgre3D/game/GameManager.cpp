#include "GameManager.h"
#include "Ogre.h"
#include "ScriptLuaVM.h"
#include "tolua++.h"
#include "LuaInterface.h"
#include <algorithm>
#include "GlobalFuncs.h"
#include <winsock.h>
#include "ui/UIManager.h"
#include "manager/ClientManager.h"
#include "manager/SandboxMgr.h"
#include "manager/ObjectManager.h"
#include "debug/DebugDrawer.h"
#include "play/PhysicsWorld.h"

using namespace Ogre;

extern int tolua_SandboxToLua_open(lua_State* tolua_S);
extern int tolua_ClientToLua_open(lua_State* tolua_S);
extern int tolua_SandboxToLua_Manual(lua_State* tolua_S);

GameManager* g_GameManager = nullptr;
GameManager* GetGameManager()
{
	return g_GameManager;
}

GameManager::GameManager() : m_SimulationTime(0), m_pScriptVM(nullptr), 
	m_pPhysicsWorld(nullptr), m_pSandboxMgr(nullptr), m_pObjectManager(nullptr), m_pUIManager(nullptr)
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
}

GameManager* GameManager::GetInstance()
{
	return g_GameManager;
}

void GameManager::Initialize(SceneManager* sceneManager)
{
	m_pScriptVM = GetScriptLuaVM();

	m_pUIManager = new UIManager(GetClientMgr());
	m_pUIManager->InitConfig();

	m_pPhysicsWorld = new PhysicsWorld();
	m_pPhysicsWorld->initilize();

	m_pObjectManager = new ObjectManager(m_pPhysicsWorld);
	g_ObjectManager = m_pObjectManager;

	UIService uiservice(m_pUIManager);
	CameraService camservice(GetClientMgr());
	ObjectFactory objfactory(m_pObjectManager);

	m_pSandboxMgr = new SandboxMgr(uiservice, camservice, objfactory, sceneManager);
	g_SandboxMgr = m_pSandboxMgr;

	this->InitLuaEnv();
}

void GameManager::InitLuaEnv()
{
	// 设置ToLua对象 
	tolua_SandboxToLua_open(m_pScriptVM->getLuaState());
	tolua_ClientToLua_open(m_pScriptVM->getLuaState());
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
	//struct timeval stNow;
	//gettimeofday(&stNow, NULL);
	m_pScriptVM->callFunction("__tick__", "i", deltaMilliseconds);

	m_SimulationTime += deltaMilliseconds;

	m_pObjectManager->Update(deltaMilliseconds);

	m_pPhysicsWorld->stepWorld();

	m_pScriptVM->callFunction("Sandbox_Update", "i", deltaMilliseconds);
}

Ogre::Real GameManager::getScreenWidth()
{
	return m_pUIManager->GetScreenWidth();
}

Ogre::Real GameManager::getScreenHeight()
{
	return m_pUIManager->GetScreenHeight();
}

void GameManager::HandleWindowResized(unsigned int width, unsigned int height)
{
	m_pUIManager->HandleWindowResized(width, height);

	m_pScriptVM->callFunction("EventHandle_WindowResized", "ii", width, height);
}

void GameManager::HandleWindowClosed()
{
	//m_pScriptVM->callFunction("EventHandle_WindowClosed", "");
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
