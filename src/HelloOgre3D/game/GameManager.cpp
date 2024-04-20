#include "GameManager.h"
#include "Ogre.h"
#include "SandboxDef.h"
//#include <algorithm>
#include "ClientManager.h"
#include "SandboxMgr.h"
#include "ScriptLuaVM.h"
#include "tolua++.h"
#include "LuaInterface.h"

using namespace Ogre;

extern int tolua_ClientToLua_open(lua_State* tolua_S);

GameManager* g_GameManager = nullptr;

GameManager::GameManager(SceneManager* sceneManager)
	: m_pSceneManager(sceneManager), m_pPhysicsWorld(nullptr), m_objectIndex(0)
{
	m_pRootSceneNode = m_pSceneManager->getRootSceneNode();
}

GameManager::~GameManager()
{
	m_pSceneManager = nullptr;
	m_pRootSceneNode = nullptr;
}

GameManager* GameManager::GetInstance()
{
	return g_GameManager;
}

void GameManager::Initialize()
{
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
	pScriptVM->setUserTypePointer("Sandbox", "GameManager", GameManager::GetInstance());
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

Camera* GameManager::GetCamera()
{
	return GetClientMgr()->getCamera();
}

SceneManager* GameManager::GetSceneManager()
{
	return m_pRootSceneNode->getCreator();
}

SandboxObject* GameManager::CreatePlane(float length, float width)
{
	Ogre::SceneNode* planeNode = SandboxMgr::CreatePlane(length, width);

	btRigidBody* planeRigidBody = SandboxMgr::CreatePlane(btVector3(0, 1.0f, 0), 0);

	SandboxObject* pObject = new SandboxObject(getNextObjectId(), planeNode, planeRigidBody);

	m_pObjects[pObject->getObjId()] = pObject;

	pObject->Initialize();

	m_pPhysicsWorld->addRigidBody(pObject->getRigidBody());

	return pObject;
}

void GameManager::SetSkyBox(const Ogre::String materialName, const Ogre::Vector3& rotation)
{
	const Ogre::Quaternion& newOrientation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
	GetSceneManager()->setSkyBox(true, materialName, 5000.0f, true, newOrientation);
}

void GameManager::SetAmbientLight(const Ogre::Vector3& colourValue)
{
	Ogre::ColourValue tempColor(colourValue.x, colourValue.y, colourValue.z);
	GetSceneManager()->setAmbientLight(tempColor);
}

Ogre::Light* GameManager::CreateDirectionalLight(const Ogre::Vector3& direction)
{
	Ogre::SceneNode* const lightNode = m_pRootSceneNode->createChildSceneNode();

	Ogre::Light* const lightEntity = GetSceneManager()->createLight();

	lightEntity->setCastShadows(true);
	lightEntity->setType(Ogre::Light::LT_DIRECTIONAL);

	lightEntity->setDiffuseColour(1.0f, 1.0f, 1.0f);
	lightEntity->setSpecularColour(0, 0, 0);
	lightEntity->setDirection(direction);

	lightNode->attachObject(lightEntity);

	return lightEntity;
}

void GameManager::setMaterial(SandboxObject* pObject, const Ogre::String& materialName)
{
	assert(pObject != nullptr);
	setMaterial(pObject->getSceneNode(), materialName);
}

void GameManager::setMaterial(Ogre::SceneNode* pNode, const Ogre::String& materialName)
{
	Ogre::SceneNode::ObjectIterator it = pNode->getAttachedObjectIterator();

	while (it.hasMoreElements())
	{
		const Ogre::String movableType =
			it.current()->second->getMovableType();

		if (movableType == Ogre::EntityFactory::FACTORY_TYPE_NAME)
		{
			Ogre::Entity* const entity =
				static_cast<Ogre::Entity*>(it.current()->second);
			entity->setMaterialName(materialName);
		}
		else if (movableType ==
			Ogre::ManualObjectFactory::FACTORY_TYPE_NAME)
		{
			Ogre::ManualObject* const entity =
				static_cast<Ogre::ManualObject*>(it.current()->second);
			unsigned int sections = entity->getNumSections();

			for (unsigned int id = 0; id < sections; ++id)
			{
				entity->setMaterialName(id, materialName);
			}
		}

		it.getNext();
	}
}

SandboxObject* GameManager::CreateSandboxObject(const Ogre::String& meshFilePath)
{
	SandboxObject* pObject = new SandboxObject(getNextObjectId(), meshFilePath);

	m_pObjects[pObject->getObjId()] = pObject;

	pObject->Initialize();

	m_pPhysicsWorld->addRigidBody(pObject->getRigidBody());

	return pObject;
}