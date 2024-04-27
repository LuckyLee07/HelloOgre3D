#include "GameManager.h"
#include "Ogre.h"
#include "SandboxDef.h"
//#include <algorithm>
#include "SandboxMgr.h"
#include "ScriptLuaVM.h"
#include "tolua++.h"
#include "LuaInterface.h"
#include "UIComponent.h"

using namespace Ogre;

extern int tolua_ClientToLua_open(lua_State* tolua_S);

GameManager* g_GameManager = nullptr;

GameManager::GameManager()
	: m_pPhysicsWorld(nullptr), m_objectIndex(0)
{
}

GameManager::~GameManager()
{
}

GameManager* GameManager::GetInstance()
{
	return g_GameManager;
}

void GameManager::Initialize(SceneManager* sceneManager)
{
	m_pSandboxMgr = new SandboxMgr(sceneManager);

	m_pPhysicsWorld = new PhysicsWorld();
	m_pPhysicsWorld->initilize();

	this->InitLuaEnv();
}

void GameManager::InitLuaEnv()
{
	// 设置ToLua对象 
	ScriptLuaVM* pScriptVM = GetScriptLuaVM();
	tolua_ClientToLua_open(pScriptVM->getLuaState());

	// 设置lua可用的c++对象 
	pScriptVM->setUserTypePointer("Sandbox", "SandboxMgr", m_pSandboxMgr);
	pScriptVM->setUserTypePointer("LuaInterface", "LuaInterface", LuaInterface::GetInstance());

	pScriptVM->callFile("res/scripts/script_init.lua");
}

void GameManager::Update(int deltaMilliseconds)
{
	std::map<unsigned int, SandboxObject*>::iterator objectIter;
	for (objectIter = m_pObjects.begin(); objectIter != m_pObjects.end(); objectIter++)
	{
		objectIter->second->Update(deltaMilliseconds);
	}

	m_pPhysicsWorld->stepWorld();
}

void GameManager::AddSandboxObject(SandboxObject* pSandboxObject)
{
	unsigned int objectId = getNextObjectId();

	pSandboxObject->setObjId(objectId);
	m_pObjects[objectId] = pSandboxObject;

	pSandboxObject->Initialize();

	m_pPhysicsWorld->addRigidBody(pSandboxObject->getRigidBody());
}

GameManager* GetGameManager()
{
	return g_GameManager;
}