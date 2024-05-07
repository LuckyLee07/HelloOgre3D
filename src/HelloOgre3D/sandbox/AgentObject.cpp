#include "AgentObject.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"
#include "btBulletDynamicsCommon.h"
#include "SandboxMgr.h"
#include "SandboxDef.h"

using namespace Ogre;

const float AgentObject::DEFAULT_AGENT_MASS = 90.7f;	// kilograms (200 lbs)
const float AgentObject::DEFAULT_AGENT_HEIGHT = 1.6f;	// meters (5.2 feet)
const float AgentObject::DEFAULT_AGENT_RADIUS = 0.3f;	// meters (1.97 feet)

AgentObject::AgentObject(Ogre::SceneNode* pSceneNode, btRigidBody* pRigidBody)
	: BaseObject(0, BaseObject::OBJ_AGENT), m_pSceneNode(pSceneNode), m_pRigidBody(pRigidBody),
	m_mass(0.0f), m_height(DEFAULT_AGENT_HEIGHT), m_radius(DEFAULT_AGENT_RADIUS)
{
	if (m_pRigidBody)
	{
		this->setMass(DEFAULT_AGENT_MASS);
		m_pRigidBody->setUserPointer(this);
	}
	setForward(Ogre::Vector3::UNIT_Z);
}

AgentObject::~AgentObject()
{
}

void AgentObject::Initialize()
{
	setPosition(Ogre::Vector3::ZERO);

	setTarget(Ogre::Vector3(50.0f, 0.0f, 0.0f));
	setTargetRadius(1.5f);
}

void AgentObject::setMass(const Ogre::Real mass)
{
	m_mass = mass;

	btVector3 localInertia(0, 0, 0);
	m_pRigidBody->getCollisionShape()->calculateLocalInertia(mass, localInertia);
	m_pRigidBody->setMassProps(mass, localInertia);
	m_pRigidBody->updateInertiaTensor();
	m_pRigidBody->activate(true);
}

void AgentObject::setPosition(const Ogre::Vector3& position)
{
	btVector3 btPosition(position.x, position.y, position.z);
	btTransform transform = m_pRigidBody->getWorldTransform();
	transform.setOrigin(btPosition);

	m_pRigidBody->setWorldTransform(transform);
	m_pRigidBody->activate(true);
}

void AgentObject::setRotation(const Ogre::Vector3& rotation)
{
	Ogre::Quaternion qRotation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
	this->setOrientation(qRotation);
}

void AgentObject::setOrientation(const Ogre::Quaternion& quaternion)
{
	btQuaternion btRotation(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	btTransform transform = m_pRigidBody->getWorldTransform();
	transform.setRotation(btRotation);

	m_pRigidBody->setWorldTransform(transform);
	m_pRigidBody->activate(true);

	this->updateWorldTransform();
}

void AgentObject::setForward(const Ogre::Vector3& forward)
{
	Ogre::Vector3 up = Ogre::Vector3::UNIT_Y;

	Ogre::Vector3 zAxis = forward.normalisedCopy();
	Ogre::Vector3 xAxis = up.crossProduct(zAxis);
	Ogre::Vector3 yAxis = zAxis.crossProduct(xAxis);

	Ogre::Quaternion orientation(xAxis, yAxis, zAxis);

	btTransform transform = m_pRigidBody->getWorldTransform();
	btQuaternion btRotation(orientation.x, orientation.y, orientation.z, orientation.w);
	transform.setRotation(btRotation);
	m_pRigidBody->setWorldTransform(transform);
	m_pRigidBody->activate(true);

	m_pSceneNode->setOrientation(orientation);
}

void AgentObject::setTarget(const Ogre::Vector3& targetPos)
{
	m_targetPos = targetPos;
}

void AgentObject::setTargetRadius(Ogre::Real radius)
{
	m_targetRadius = std::max(Ogre::Real(0.0f), radius);
}

Ogre::Vector3 AgentObject::getPosition()
{
	if (m_pRigidBody != nullptr)
	{
		const btVector3& position = m_pRigidBody->getCenterOfMassPosition();
		return SandboxMgr::BtVector3ToVector3(position);
	}
	else if (m_pSceneNode != nullptr)
	{
		return m_pSceneNode->_getDerivedPosition();
	}

	return Ogre::Vector3::ZERO;
}

Ogre::Quaternion AgentObject::getOrientation()
{
	if (m_pRigidBody != nullptr)
	{
		const btQuaternion& orietation = m_pRigidBody->getOrientation();
		return SandboxMgr::BtQuaternionToQuaternion(orietation);
	}
	else if (m_pSceneNode != nullptr)
	{
		return m_pSceneNode->_getDerivedOrientation();
	}

	return Ogre::Quaternion::ZERO;
}

void AgentObject::update(int deltaMsec)
{
	this->updateWorldTransform();
}

void AgentObject::updateWorldTransform()
{
	const btVector3& rigidBodyPos = m_pRigidBody->getWorldTransform().getOrigin();
	Ogre::Vector3 position(rigidBodyPos.m_floats[0], rigidBodyPos.m_floats[1], rigidBodyPos.m_floats[2]);
	m_pSceneNode->setPosition(position);

	const btQuaternion& rigidBodyRotation = m_pRigidBody->getWorldTransform().getRotation();
	Ogre::Quaternion rotation(rigidBodyRotation.w(), rigidBodyRotation.x(),
								rigidBodyRotation.y(), rigidBodyRotation.z());
	m_pSceneNode->setOrientation(rotation);
}