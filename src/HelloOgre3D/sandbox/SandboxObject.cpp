#include "SandboxObject.h"
#include "OgreSceneNode.h"
#include "OgreEntity.h"
#include "OgreSceneManager.h"
#include "ClientManager.h"
#include "btBulletDynamicsCommon.h"
#include "SandboxMgr.h"
#include "SandboxDef.h"

using namespace Ogre;

SandboxObject::SandboxObject(const Ogre::String& meshFile)
	: BaseObject(0, BaseObject::OBJ_SANDBOX_OBJ)
{
	SceneNode* pRootScene = GetClientMgr()->getRootSceneNode();

	m_pSceneNode = pRootScene->createChildSceneNode();
	m_pEntity = m_pSceneNode->getCreator()->createEntity(meshFile);
	m_pSceneNode->attachObject(m_pEntity);

	Ogre::Mesh* meshPtr = m_pEntity->getMesh().getPointer();
	m_pRigidBody = SandboxMgr::CreateRigidBody(meshPtr, 1.0f);

	m_pRigidBody->setUserPointer(this);
}

SandboxObject::SandboxObject(Ogre::SceneNode* pSceneNode, btRigidBody* pRigidBody)
	: BaseObject(0, BaseObject::OBJ_SANDBOX_OBJ), 
	m_pSceneNode(pSceneNode), m_pRigidBody(pRigidBody), m_pEntity(nullptr)
{
	if (m_pRigidBody)
		m_pRigidBody->setUserPointer(this);
}

SandboxObject::~SandboxObject()
{
}

void SandboxObject::Initialize()
{

}

void SandboxObject::setMass(const Ogre::Real mass)
{
	btVector3 localInertia(0, 0, 0);
	m_pRigidBody->getCollisionShape()->calculateLocalInertia(mass, localInertia);
	m_pRigidBody->setMassProps(mass, localInertia);
	m_pRigidBody->updateInertiaTensor();
	m_pRigidBody->activate(true);
}

void SandboxObject::setPosition(const Ogre::Vector3& position)
{
	btVector3 btPosition(position.x, position.y, position.z);
	btTransform transform = m_pRigidBody->getWorldTransform();
	transform.setOrigin(btPosition);

	m_pRigidBody->setWorldTransform(transform);
	m_pRigidBody->activate(true);

	this->updateWorldTransform();
}

void SandboxObject::setRotation(const Ogre::Vector3& rotation)
{
	Ogre::Quaternion qRotation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
	this->setOrientation(qRotation);
}

void SandboxObject::setOrientation(const Ogre::Quaternion& quaternion)
{
	btQuaternion btRotation(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	btTransform transform = m_pRigidBody->getWorldTransform();
	transform.setRotation(btRotation);

	m_pRigidBody->setWorldTransform(transform);
	m_pRigidBody->activate(true);

	this->updateWorldTransform();
}

void SandboxObject::Update(int deltaMsec)
{
	this->updateWorldTransform();
}

void SandboxObject::updateWorldTransform()
{
	const btVector3& rigidBodyPos = m_pRigidBody->getWorldTransform().getOrigin();
	Ogre::Vector3 position(rigidBodyPos.m_floats[0], rigidBodyPos.m_floats[1], rigidBodyPos.m_floats[2]);
	m_pSceneNode->setPosition(position);

	const btQuaternion& rigidBodyRotation = m_pRigidBody->getWorldTransform().getRotation();
	Ogre::Quaternion rotation(rigidBodyRotation.w(), rigidBodyRotation.x(),
								rigidBodyRotation.y(), rigidBodyRotation.z());
	m_pSceneNode->setOrientation(rotation);
}