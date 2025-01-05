#include "GameManager.h"
#include "Ogre.h"
#include "SandboxDef.h"
#include "ScriptLuaVM.h"
#include "tolua++.h"
#include "LuaInterface.h"
#include <algorithm>
#include "GlobalFuncs.h"
#include <winsock.h>
#include "object/UIComponent.h"
#include "manager/ClientManager.h"
#include "manager/SandboxMgr.h"
#include "manager/ObjectManager.h"
#include "debug/DebugDrawer.h"
#include "play/PhysicsWorld.h"

using namespace Ogre;

extern int tolua_ClientToLua_open(lua_State* tolua_S);

GameManager* g_GameManager = nullptr;
GameManager* GetGameManager()
{
	return g_GameManager;
}

GameManager::GameManager() : m_pUIScene(nullptr), m_SimulationTime(0), m_pScriptVM(nullptr), 
	m_pPhysicsWorld(nullptr), m_pSandboxMgr(nullptr), m_pObjectManager(nullptr), m_pMarkupText(nullptr)
{
	std::fill_n(m_pUILayers, UI_LAYER_COUNT, nullptr);
}

GameManager::~GameManager()
{
	SAFE_DELETE(m_pObjectManager);
	SAFE_DELETE(m_pSandboxMgr);
	SAFE_DELETE(m_pPhysicsWorld);
	g_SandboxMgr = nullptr;
	g_ObjectManager = nullptr;

	for (size_t index = 0; index < UI_LAYER_COUNT; index++)
	{
		if (m_pUILayers[index] != nullptr)
			m_pUIScene->destroy(m_pUILayers[index]);
		m_pUILayers[index] = nullptr;
	}
	Gorilla::Silverback* pSilverback = Gorilla::Silverback::getSingletonPtr();
	pSilverback->destroyScreen(m_pUIScene);
	m_pUIScene = nullptr;
}

GameManager* GameManager::GetInstance()
{
	return g_GameManager;
}

void GameManager::Initialize(SceneManager* sceneManager)
{
	this->InitUIConfig();

	m_pScriptVM = GetScriptLuaVM();

	m_pPhysicsWorld = new PhysicsWorld();
	m_pPhysicsWorld->initilize();

	m_pSandboxMgr = new SandboxMgr(sceneManager);
	g_SandboxMgr = m_pSandboxMgr;

	m_pObjectManager = new ObjectManager(m_pPhysicsWorld);
	g_ObjectManager = m_pObjectManager;

	this->InitLuaEnv();
}

Gorilla::Layer* GameManager::getUILayer(unsigned int index)
{
	if (index >= UI_LAYER_COUNT) return nullptr;
	
	if (m_pUILayers[index] == nullptr)
	{
		m_pUILayers[index] = m_pUIScene->createLayer(index);
	}
	return m_pUILayers[index];
}

void GameManager::InitUIConfig()
{
	Gorilla::Silverback* pSilverback = Gorilla::Silverback::getSingletonPtr();
	Ogre::Camera* pCamera = GetClientMgr()->getCamera();
	m_pUIScene = pSilverback->createScreen(pCamera->getViewport(), DEFAULT_ATLAS);
	
	m_pMarkupText = getUILayer()->createMarkupText(
		91, m_pUIScene->getWidth(), m_pUIScene->getHeight(),
		"Learning Game AI Programming. " __TIMESTAMP__);

	Ogre::Real leftPos = m_pUIScene->getWidth() - m_pMarkupText->maxTextWidth() - 4;
	Ogre::Real topPos = m_pUIScene->getHeight() - m_pUIScene->getAtlas()->getGlyphData(9)->mLineHeight - 4;
	m_pMarkupText->left(leftPos);
	m_pMarkupText->top(topPos);
}

void GameManager::InitLuaEnv()
{
	// 设置ToLua对象 
	tolua_ClientToLua_open(m_pScriptVM->getLuaState());

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
	return m_pUIScene->getWidth();
}

Ogre::Real GameManager::getScreenHeight()
{
	return m_pUIScene->getHeight();
}

void GameManager::HandleWindowResized(unsigned int width, unsigned int height)
{
	Ogre::Real leftPos = width - m_pMarkupText->maxTextWidth() - 4;
	Ogre::Real topPos = height - m_pUIScene->getAtlas()->getGlyphData(9)->mLineHeight - 4;
	m_pMarkupText->left(leftPos);
	m_pMarkupText->top(topPos);

	m_pScriptVM->callFunction("EventHandle_WindowResized", "ii", width, height);
}

void GameManager::HandleWindowClosed()
{
	//m_pScriptVM->callFunction("EventHandle_WindowClosed", "");
}

void GameManager::HandleKeyPress(OIS::KeyCode keycode, unsigned int key)
{
	m_pScriptVM->callFunction("EventHandle_Keyboard", "ib", keycode, true);
}

void GameManager::HandleKeyRelease(OIS::KeyCode keycode, unsigned int key)
{
	m_pScriptVM->callFunction("EventHandle_Keyboard", "ib", keycode, false);

	m_pObjectManager->HandleKeyEvent(keycode, key);
}

void GameManager::HandleMouseMove(int width, int height)
{

}

void GameManager::HandleMousePress(int width, int height, OIS::MouseButtonID buttonId)
{

}

void GameManager::HandleMouseRelease(int width, int height, OIS::MouseButtonID buttonId)
{

}

UIComponent* GameManager::createUIComponent(unsigned int index)
{
	if (index < UI_LAYER_COUNT)
	{
		UIComponent* pComponent = new UIComponent(getUILayer(index));
		m_pObjectManager->addUIObject(pComponent);
		return pComponent;
	}
	return nullptr;
}

void GameManager::setMarkupColor(unsigned int index, const Ogre::ColourValue& color)
{
	for (size_t layerIndex = 0; layerIndex < UI_LAYER_COUNT; layerIndex++)
	{
		getUILayer(layerIndex)->_getAtlas()->setMarkupColour(index, color);
	}
}

long long GameManager::getTimeInMillis()
{
	return m_SimulationTime;
}

Ogre::Real GameManager::getTimeInSeconds()
{
	return (Ogre::Real)(getTimeInMillis() / 1000);
}
