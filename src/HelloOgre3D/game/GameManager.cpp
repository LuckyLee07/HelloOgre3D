#include "GameManager.h"
#include "Ogre.h"
#include "SandboxDef.h"
#include "SandboxMgr.h"
#include "ScriptLuaVM.h"
#include "tolua++.h"
#include "LuaInterface.h"
#include "UIComponent.h"
#include "ClientManager.h"
#include <algorithm>
#include "DebugDrawer.h"

using namespace Ogre;

extern int tolua_ClientToLua_open(lua_State* tolua_S);

GameManager* g_GameManager = nullptr;
GameManager* GetGameManager()
{
	return g_GameManager;
}

GameManager::GameManager() : m_objectIndex(0),
	m_pScriptVM(nullptr), m_pPhysicsWorld(nullptr), m_pSandboxMgr(nullptr)
{
	std::fill_n(m_pUILayers, UI_LAYER_COUNT, nullptr);
}

GameManager::~GameManager()
{
	delete m_pSandboxMgr;

	delete m_pPhysicsWorld;

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
	
	Gorilla::MarkupText* pUIText = getUILayer()->createMarkupText(
		91, m_pUIScene->getWidth(), m_pUIScene->getHeight(),
		"Learning Game AI Programming. " __TIMESTAMP__);

	Ogre::Real leftPos = m_pUIScene->getWidth() - pUIText->maxTextWidth() - 4;
	Ogre::Real topPos = m_pUIScene->getHeight() - m_pUIScene->getAtlas()->getGlyphData(9)->mLineHeight - 4;
	pUIText->left(leftPos);
	pUIText->top(topPos);
}

void GameManager::InitLuaEnv()
{
	// 设置ToLua对象 
	tolua_ClientToLua_open(m_pScriptVM->getLuaState());

	// 设置lua可用的c++对象 
	m_pScriptVM->setUserTypePointer("Sandbox", "SandboxMgr", m_pSandboxMgr);
	m_pScriptVM->setUserTypePointer("DebugDrawer", "DebugDrawer", DebugDrawer::GetInstance());
	m_pScriptVM->setUserTypePointer("LuaInterface", "LuaInterface", LuaInterface::GetInstance());
	
	m_pScriptVM->callFile("res/scripts/script_init.lua");
}

void GameManager::Update(int deltaMilliseconds)
{
	std::map<unsigned int, BaseObject*>::iterator objectIter;
	for (objectIter = m_pObjects.begin(); objectIter != m_pObjects.end(); objectIter++)
	{
		BaseObject* pObject = objectIter->second;
		if (pObject != nullptr) pObject->update(deltaMilliseconds);
	}

	std::vector<AgentObject*>::iterator agentIter;
	for (agentIter = m_pAgents.begin(); agentIter != m_pAgents.end(); agentIter++)
	{
		AgentObject* pAgent = *agentIter;
		pAgent->update(deltaMilliseconds);
	}

	m_pPhysicsWorld->stepWorld();

	m_pScriptVM->callFunction("Sandbox_Update", ">");
}

Ogre::Real GameManager::getScreenWidth()
{
	return m_pUIScene->getWidth();
}

Ogre::Real GameManager::getScreenHeight()
{
	return m_pUIScene->getHeight();
}

unsigned int GameManager::getObjectCount()
{
	return m_pObjects.size();
}

void GameManager::HandleKeyPress(OIS::KeyCode keycode, unsigned int key)
{
	if (keycode == OIS::KC_F10) // reload script
	{
		m_pScriptVM->callFile("res/scripts/script_init.lua");
	}

	m_pScriptVM->callFunction("EventHandle_Keyboard", "ib", keycode, true);
}

void GameManager::HandleKeyRelease(OIS::KeyCode keycode, unsigned int key)
{
	m_pScriptVM->callFunction("EventHandle_Keyboard", "ib", keycode, false);
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

void GameManager::addAgentObject(AgentObject* pAgentObject)
{
	unsigned int objectId = getNextObjectId();

	pAgentObject->setObjId(objectId);
	pAgentObject->Initialize();

	m_pAgents.push_back(pAgentObject);

	m_pPhysicsWorld->addRigidBody(pAgentObject->getRigidBody());
}

void GameManager::addSandboxObject(SandboxObject* pSandboxObject)
{
	unsigned int objectId = getNextObjectId();

	pSandboxObject->setObjId(objectId);
	m_pObjects[objectId] = pSandboxObject;

	pSandboxObject->Initialize();

	m_pPhysicsWorld->addRigidBody(pSandboxObject->getRigidBody());
}

UIComponent* GameManager::createUIComponent(unsigned int index)
{
	if (index < UI_LAYER_COUNT)
	{
		UIComponent* pComponent = new UIComponent(getUILayer(index));
		pComponent->setObjId(getNextObjectId());
		m_pObjects[pComponent->getObjId()] = pComponent;
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