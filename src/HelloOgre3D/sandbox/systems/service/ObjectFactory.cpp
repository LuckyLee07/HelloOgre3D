#include "ObjectFactory.h"
#include "SceneFactory.h"
#include "PhysicsFactory.h"
#include "objects/BlockObject.h"
#include "objects/AgentObject.h"
#include "systems/manager/ObjectManager.h"
#include "systems/service/AgentFactory.h"
#include "systems/service/SoldierFactory.h"
#include "Ogre.h"
#include "Procedural.h"
#include "btBulletDynamicsCommon.h"
#include "AppConfig.h"

ObjectFactory::ObjectFactory(ObjectManager* pMananger)
	: m_objectManager(pMananger)
{
}

BlockObject* ObjectFactory::CreatePlane(float length, float width)
{
	Ogre::SceneNode* planeNode = SceneFactory::CreateNodePlane(length, width);

	btRigidBody* planeRigidBody = PhysicsFactory::CreateRigidBodyPlane(btVector3(0, 1.0f, 0), 0);

	BlockObject* pObject = new BlockObject(planeNode, planeRigidBody);
	pObject->SetObjType(BaseObject::OBJ_TYPE_PLANE);

	m_objectManager->addNewObject(pObject);

	return pObject;
}

BlockObject* ObjectFactory::CreateBlockObject(const Ogre::String& meshFilePath)
{
	BlockObject* pObject = new BlockObject(meshFilePath);
	pObject->SetObjType(BaseObject::OBJ_TYPE_BLOCK);

	m_objectManager->addNewObject(pObject);

	return pObject;
}

BlockObject* ObjectFactory::CreateBlockBox(float width, float height, float length, float uTile, float vTile)
{
	Procedural::BoxGenerator boxGenerator;
	boxGenerator.setSizeX(width);
	boxGenerator.setSizeY(height);
	boxGenerator.setSizeZ(length);
	boxGenerator.setUTile(uTile);
	boxGenerator.setVTile(vTile);

	const Ogre::MeshPtr meshPtr = boxGenerator.realizeMesh();

	btRigidBody* planeRigidBody = PhysicsFactory::CreateRigidBodyBox(width, height, length);

	BlockObject* pObject = new BlockObject(meshPtr, planeRigidBody);
	pObject->SetObjType(BaseObject::OBJ_TYPE_BLOCK);
	pObject->GetEntity()->setMaterialName(DEFAULT_MATERIAL);

	m_objectManager->addNewObject(pObject);

	return pObject;
}

BlockObject* ObjectFactory::CreateBullet(Ogre::Real height, Ogre::Real radius)
{
	Ogre::SceneNode* pSceneNode = SceneFactory::CreateChildSceneNode();

	btRigidBody* capsuleRigidBody = PhysicsFactory::CreateRigidBodyCapsule(height, radius);

	BlockObject* pBullet = new BlockObject(pSceneNode, capsuleRigidBody);
	pBullet->SetObjType(BaseObject::OBJ_TYPE_BULLET);

	m_objectManager->addNewObject(pBullet);

	return pBullet;
}

AgentObject* ObjectFactory::CreateAgent(AGENT_OBJ_TYPE agentType, const char* filepath)
{
	return AgentFactory::CreateAgent(m_objectManager, agentType, filepath);
}

SoldierObject* ObjectFactory::CreateSoldier(const Ogre::String& meshFile, const char* filepath)
{
	return SoldierFactory::CreateSoldier(m_objectManager, meshFile, filepath);
}

