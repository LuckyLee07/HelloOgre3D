#include "BlockObject.h"
#include "OgreSceneNode.h"
#include "OgreEntity.h"
#include "OgreSceneManager.h"
#include "btBulletDynamicsCommon.h"
#include "SandboxDef.h"
#include "manager/SandboxMgr.h"
#include "manager/ClientManager.h"
#include "play/PhysicsWorld.h"
#include "game/GameManager.h"

using namespace Ogre;

BlockObject::BlockObject(const Ogre::String& meshFile, btRigidBody* pRigidBody)
	: EntityObject(meshFile), m_pRigidBody(pRigidBody)
{
	setObjType(BaseObject::OBJ_TYPE_BLOCK);
	Ogre::Mesh* meshPtr = m_pEntity->getMesh().getPointer();
	if (pRigidBody == nullptr)
	{
		m_pRigidBody = SandboxMgr::CreateRigidBodyBox(meshPtr, 1.0f);
		m_pRigidBody->setUserPointer(this);
	}
}

BlockObject::BlockObject(const Ogre::MeshPtr& meshPtr, btRigidBody* pRigidBody)
	: EntityObject(meshPtr), m_pRigidBody(pRigidBody)
{
	if (pRigidBody == nullptr)
	{
		m_pRigidBody = SandboxMgr::CreateRigidBodyBox(meshPtr.get(), 1.0f);
		m_pRigidBody->setUserPointer(this);
	}
}

BlockObject::BlockObject(Ogre::SceneNode* pSceneNode, btRigidBody* pRigidBody)
	: EntityObject(pSceneNode), m_pRigidBody(pRigidBody)
{
	m_pEntity = nullptr;
	m_pSceneNode = pSceneNode;
	
	m_pRigidBody->setUserPointer(this);
}

BlockObject::~BlockObject()
{
	this->DeleteRighdBody();
}

void BlockObject::Initialize()
{

}

void BlockObject::DeleteRighdBody()
{
	if (m_pRigidBody != nullptr)
	{
		PhysicsWorld* pPhysicsWorld = g_GameManager->getPhysicsWorld();
		pPhysicsWorld->removeRigidBody(m_pRigidBody);

		delete m_pRigidBody->getMotionState();
		delete m_pRigidBody->getCollisionShape();
		SAFE_DELETE(m_pRigidBody);
	}
}

void BlockObject::setMass(const Ogre::Real mass)
{
	btVector3 localInertia(0, 0, 0);
	m_pRigidBody->getCollisionShape()->calculateLocalInertia(mass, localInertia);
	m_pRigidBody->setMassProps(mass, localInertia);
	m_pRigidBody->updateInertiaTensor();
	m_pRigidBody->activate(true);
}

Ogre::Real BlockObject::getMass() const
{
	btScalar inverseMass = m_pRigidBody->getInvMass();

	if (inverseMass <= 0)
		return 0.0f;

	return 1.0f / inverseMass;
}

void BlockObject::setPosition(const Ogre::Vector3& position)
{
	btVector3 btPosition(position.x, position.y, position.z);
	btTransform transform = m_pRigidBody->getWorldTransform();
	transform.setOrigin(btPosition);

	m_pRigidBody->setWorldTransform(transform);
	m_pRigidBody->activate(true);

	this->updateWorldTransform();
}

Ogre::Vector3 BlockObject::GetPosition() const
{
	const btVector3& position = m_pRigidBody->getCenterOfMassPosition();
	return BtVector3ToVector3(position);
}

Ogre::Real BlockObject::GetRadius() const
{
	btVector3 aabbMin;
	btVector3 aabbMax;

	m_pRigidBody->getAabb(aabbMin, aabbMax);
	return aabbMax.distance(aabbMin) / 2.0f;
}

void BlockObject::setRotation(const Ogre::Vector3& rotation)
{
	Ogre::Quaternion qRotation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
	this->setOrientation(qRotation);
}

void BlockObject::setOrientation(const Ogre::Quaternion& quaternion)
{
	btQuaternion btRotation(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	btTransform transform = m_pRigidBody->getWorldTransform();
	transform.setRotation(btRotation);

	m_pRigidBody->setWorldTransform(transform);
	m_pRigidBody->activate(true);

	this->updateWorldTransform();
}

void BlockObject::applyImpulse(const Ogre::Vector3& impulse)
{
	btVector3 torque(impulse.x, impulse.y, impulse.z);
	m_pRigidBody->applyCentralForce(torque);
	m_pRigidBody->activate(true);
}

void BlockObject::applyAngularImpulse(const Ogre::Vector3& aImpulse)
{
	btVector3 torqueImpulse(aImpulse.x, aImpulse.y, aImpulse.z);
	m_pRigidBody->applyTorque(torqueImpulse);
	m_pRigidBody->activate(true);
}

void BlockObject::update(int deltaMsec)
{
	this->updateWorldTransform();
}

void BlockObject::updateWorldTransform()
{
	const btVector3& rigidBodyPos = m_pRigidBody->getWorldTransform().getOrigin();
	m_pSceneNode->setPosition(BtVector3ToVector3(rigidBodyPos));

	const btQuaternion& rigidBodyRotation = m_pRigidBody->getWorldTransform().getRotation();
	m_pSceneNode->setOrientation(BtQuaternionToQuaternion(rigidBodyRotation));
}

OpenSteer::Vec3 BlockObject::getPosition() const
{
	return Vector3ToVec3(GetPosition());
}

float BlockObject::getRadius() const
{
	return GetRadius();
}

OpenSteer::Vec3 BlockObject::steerToAvoid(const OpenSteer::AbstractVehicle& vehicle,
	const float minTimeToCollision) const
{
	// minimum distance to obstacle before avoidance is required
	const float minDistanceToCollision = minTimeToCollision * vehicle.speed();
	const float minDistanceToCenter = minDistanceToCollision + getRadius();

	// contact distance: sum of radii of obstacle and vehicle
	const float totalRadius = getRadius() + vehicle.radius();

	// obstacle center relative to vehicle position
	const OpenSteer::Vec3 localOffset = getPosition() - vehicle.position();

	// distance along vehicle's forward axis to obstacle's center
	const float forwardComponent = localOffset.dot(vehicle.forward());
	const OpenSteer::Vec3 forwardOffset = forwardComponent * vehicle.forward();

	// offset from forward axis to obstacle's center
	const OpenSteer::Vec3 offForwardOffset = localOffset - forwardOffset;

	// test to see if sphere overlaps with obstacle-free corridor
	const bool inCylinder = offForwardOffset.length() < totalRadius;
	const bool nearby = forwardComponent < minDistanceToCenter;
	const bool inFront = forwardComponent > 0;

	// if all three conditions are met, steer away from sphere center
	if (inCylinder && nearby && inFront)
	{
		return offForwardOffset * -1;
	}
	else
	{
		return OpenSteer::Vec3::zero;
	}
}