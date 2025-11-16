#include "ObjectFactory.h"
#include "SceneFactory.h"
#include "PhysicsFactory.h"
#include "object/BlockObject.h"
#include "object/UIComponent.h"
#include "object/AgentObject.h"
#include "object/SoldierObject.h"
#include "manager/ObjectManager.h"
#include "manager/LuaPluginMgr.h"
#include "Procedural.h"
#include "btBulletDynamicsCommon.h"


BlockObject* ObjectFactory::CreatePlane(float length, float width)
{
	Ogre::SceneNode* planeNode = SceneFactory::CreateNodePlane(length, width);

	btRigidBody* planeRigidBody = PhysicsFactory::CreateRigidBodyPlane(btVector3(0, 1.0f, 0), 0);

	BlockObject* pObject = new BlockObject(planeNode, planeRigidBody);
	pObject->setObjType(BaseObject::OBJ_TYPE_PLANE);

	g_ObjectManager->addNewObject(pObject);

	return pObject;
}

BlockObject* ObjectFactory::CreateBlockObject(const Ogre::String& meshFilePath)
{
	BlockObject* pObject = new BlockObject(meshFilePath);
	pObject->setObjType(BaseObject::OBJ_TYPE_BLOCK);

	g_ObjectManager->addNewObject(pObject);

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
	pObject->setObjType(BaseObject::OBJ_TYPE_BLOCK);
	pObject->getEntity()->setMaterialName(DEFAULT_MATERIAL);

	g_ObjectManager->addNewObject(pObject);

	return pObject;
}

BlockObject* ObjectFactory::CreateBullet(Ogre::Real height, Ogre::Real radius)
{
	Ogre::SceneNode* pSceneNode = SceneFactory::CreateChildSceneNode();

	btRigidBody* capsuleRigidBody = PhysicsFactory::CreateRigidBodyCapsule(height, radius);

	BlockObject* pBullet = new BlockObject(pSceneNode, capsuleRigidBody);
	pBullet->setObjType(BaseObject::OBJ_TYPE_BULLET);

	g_ObjectManager->addNewObject(pBullet);

	return pBullet;
}

EntityObject* ObjectFactory::CreateEntityObject(const Ogre::String& meshFilePath)
{
	EntityObject* pObject = new EntityObject(meshFilePath);
	pObject->setObjType(BaseObject::OBJ_TYPE_ENTITY);

	g_ObjectManager->addNewObject(pObject);

	return pObject;
}

AgentObject* ObjectFactory::CreateAgent(AGENT_OBJ_TYPE agentType, const char* filepath)
{
	Ogre::Real height = AgentObject::DEFAULT_AGENT_HEIGHT;
	Ogre::Real radius = AgentObject::DEFAULT_AGENT_RADIUS;

	Ogre::SceneNode* capsuleNode = SceneFactory::CreateNodeCapsule(height, radius);
	EntityObject* pEntityObj = new EntityObject(capsuleNode);
	pEntityObj->setMaterial("Ground2");

	btRigidBody* capsuleRigidBody = PhysicsFactory::CreateRigidBodyCapsule(height, radius);
	capsuleRigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	AgentObject* pObject = new AgentObject(pEntityObj, capsuleRigidBody);
	pObject->setAgentType(agentType);
	if (filepath != nullptr)
		LuaPluginMgr::BindLuaPluginByFile<AgentObject>(pObject, filepath);

	g_ObjectManager->addNewObject(pObject);

	return pObject;
}

SoldierObject* ObjectFactory::CreateSoldier(const Ogre::String& meshFile, const char* filepath)
{
	Ogre::Real height = AgentObject::DEFAULT_AGENT_HEIGHT;
	Ogre::Real radius = AgentObject::DEFAULT_AGENT_RADIUS;

	EntityObject* pEntityObj = new EntityObject(meshFile);
	pEntityObj->setOriginPos(Ogre::Vector3(0.0f, -height * 0.5f, 0.0f));

	btRigidBody* capsuleRigidBody = PhysicsFactory::CreateRigidBodyCapsule(height, radius);
	capsuleRigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	SoldierObject* pObject = new SoldierObject(pEntityObj, capsuleRigidBody);
	if (filepath != nullptr)
		LuaPluginMgr::BindLuaPluginByFile<SoldierObject>(pObject, filepath);

	g_ObjectManager->addNewObject(pObject);

	return pObject;
}

