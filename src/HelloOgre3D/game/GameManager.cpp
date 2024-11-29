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
#include "debug/DebugDrawer.h"

using namespace Ogre;

extern int tolua_ClientToLua_open(lua_State* tolua_S);

GameManager* g_GameManager = nullptr;
GameManager* GetGameManager()
{
	return g_GameManager;
}

GameManager::GameManager() : m_objectIndex(0), m_pUIScene(nullptr),
	m_pScriptVM(nullptr), m_pPhysicsWorld(nullptr), m_pSandboxMgr(nullptr), m_pMarkupText(nullptr)
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
	m_pScriptVM->setUserTypePointer("DebugDrawer", "DebugDrawer", DebugDrawer::GetInstance());
	m_pScriptVM->setUserTypePointer("LuaInterface", "LuaInterface", LuaInterface::GetInstance());
	
	m_pScriptVM->callFile("res/scripts/script_init.lua");

	struct timeval stNow;
	gettimeofday(&stNow, NULL);
	m_pScriptVM->callFunction("__init__", "ii", stNow.tv_sec, stNow.tv_usec / 1000);
}

void GameManager::Update(int deltaMilliseconds)
{
	struct timeval stNow;
	gettimeofday(&stNow, NULL);
	m_pScriptVM->callFunction("__tick__", "ii", stNow.tv_sec, stNow.tv_usec / 1000);

	for (auto iter = m_pEntitys.begin(); iter != m_pEntitys.end(); iter++)
	{
		if (EntityObject* pObject = *iter)
			pObject->update(deltaMilliseconds);
	}
	for (auto iter = m_pObjects.begin(); iter != m_pObjects.end(); iter++)
	{
		if (SandboxObject* pObject = *iter) 
			pObject->update(deltaMilliseconds);
	}
	for (auto iter = m_pAgents.begin(); iter != m_pAgents.end(); iter++)
	{
		if (AgentObject* pAgent = *iter) 
			pAgent->update(deltaMilliseconds);
	}
	for (auto iter = m_pUIComps.begin(); iter != m_pUIComps.end(); iter++)
	{
		if (UIComponent* pComponent = *iter) 
			pComponent->update(deltaMilliseconds);
	}

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

unsigned int GameManager::getObjectCount()
{
	return m_pObjects.size();
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

	auto rigidBody = pAgentObject->getRigidBody();
	if (rigidBody != nullptr)
	{
		m_pPhysicsWorld->addRigidBody(rigidBody);
	}
}

void GameManager::addSandboxObject(SandboxObject* pSandboxObject)
{
	unsigned int objectId = getNextObjectId();

	pSandboxObject->setObjId(objectId);
	pSandboxObject->Initialize();

	m_pObjects.push_back(pSandboxObject);

	m_pPhysicsWorld->addRigidBody(pSandboxObject->getRigidBody());
}

void GameManager::addEntityObject(EntityObject* pEntityObject)
{
	unsigned int objectId = getNextObjectId();

	pEntityObject->setObjId(objectId);
	pEntityObject->Initialize();

	m_pEntitys.push_back(pEntityObject);
}

UIComponent* GameManager::createUIComponent(unsigned int index)
{
	if (index < UI_LAYER_COUNT)
	{
		UIComponent* pComponent = new UIComponent(getUILayer(index));
		pComponent->setObjId(getNextObjectId());
		m_pUIComps.push_back(pComponent);

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

std::vector<AgentObject*> GameManager::getSpecifyAgents(AGENT_OBJ_TYPE agentType)
{
	if (agentType == AGENT_OBJ_NONE)
	{
		return m_pAgents;
	}

	std::vector<AgentObject*> specifyAgents;
	
	std::vector<AgentObject*>::iterator iter;
	for (iter = m_pAgents.begin(); iter != m_pAgents.end(); iter++)
	{
		AgentObject* pAgentObject = *iter;
		if (pAgentObject->getAgentType() == agentType)
		{
			specifyAgents.push_back(pAgentObject);
		}
	}

	return specifyAgents;
}