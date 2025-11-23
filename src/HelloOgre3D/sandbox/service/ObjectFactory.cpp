#include "ObjectFactory.h"
#include "SceneFactory.h"
#include "PhysicsFactory.h"
#include "object/BlockObject.h"
#include "object/AgentObject.h"
#include "object/SoldierObject.h"
#include "object/RenderableObject.h"
#include "manager/ObjectManager.h"
#include "manager/LuaPluginMgr.h"
#include "Ogre.h"
#include "Procedural.h"
#include "btBulletDynamicsCommon.h"

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
	Ogre::Real height = AgentObject::DEFAULT_AGENT_HEIGHT;
	Ogre::Real radius = AgentObject::DEFAULT_AGENT_RADIUS;

	Ogre::SceneNode* capsuleNode = SceneFactory::CreateNodeCapsule(height, radius);
	RenderableObject* pEntityObj = new RenderableObject(capsuleNode);
	pEntityObj->SetMaterial("Ground2");

	btRigidBody* capsuleRigidBody = PhysicsFactory::CreateRigidBodyCapsule(height, radius);
	capsuleRigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	AgentObject* pObject = new AgentObject(pEntityObj, capsuleRigidBody);
	pObject->setAgentType(agentType);
	if (filepath != nullptr)
		LuaPluginMgr::BindLuaPluginByFile<AgentObject>(pObject, filepath);

	m_objectManager->addNewObject(pObject);

	return pObject;
}

SoldierObject* ObjectFactory::CreateSoldier(const Ogre::String& meshFile, const char* filepath)
{
	Ogre::Real height = AgentObject::DEFAULT_AGENT_HEIGHT;
	Ogre::Real radius = AgentObject::DEFAULT_AGENT_RADIUS;

	RenderableObject* pEntityObj = new RenderableObject(meshFile);
	pEntityObj->SetOriginPos(Ogre::Vector3(0.0f, -height * 0.5f, 0.0f));

	btRigidBody* capsuleRigidBody = PhysicsFactory::CreateRigidBodyCapsule(height, radius);
	capsuleRigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	SoldierObject* pObject = new SoldierObject(pEntityObj, capsuleRigidBody);
	if (filepath != nullptr)
		LuaPluginMgr::BindLuaPluginByFile<SoldierObject>(pObject, filepath);

	m_objectManager->addNewObject(pObject);

	return pObject;
}

