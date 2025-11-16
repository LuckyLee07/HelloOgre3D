#include "PhysicsComponent.h"
#include "object/VehicleObject.h"
#include "GameManager.h"
#include "play/PhysicsWorld.h"
#include "btBulletDynamicsCommon.h"
#include "SandboxMacros.h"
#include "GameFunction.h"
#include "manager/SandboxMgr.h"

PhysicsComponent::PhysicsComponent(VehicleObject* owner, btRigidBody* body)
	: m_owner(owner), m_body(body)
{
	if (m_body) m_body->setUserPointer(m_owner);
}

PhysicsComponent::~PhysicsComponent()
{
	DeleteRigidBody();
}

void PhysicsComponent::DeleteRigidBody()
{
	if (m_body != nullptr)
	{
		RemoveFromWorld();

		btMotionState* motion = m_body->getMotionState();
		if (motion != nullptr) SAFE_DELETE(motion);

		btCollisionShape* shape = m_body->getCollisionShape();
		if (shape != nullptr) SAFE_DELETE(shape);

		SAFE_DELETE(m_body);
	}
}

void PhysicsComponent::AddToWorld()
{
	if (!m_body) return;

	PhysicsWorld* pPhysicsWorld = g_GameManager->getPhysicsWorld();
	if (pPhysicsWorld)
		pPhysicsWorld->addRigidBody(m_body, m_owner);
}

void PhysicsComponent::RemoveFromWorld()
{
	if (!m_body) return;

	PhysicsWorld* pPhysicsWorld = g_GameManager->getPhysicsWorld();
	if (pPhysicsWorld)
		pPhysicsWorld->removeRigidBody(m_body);
}

// RigidBody ops
void PhysicsComponent::ResetRigidBody(btRigidBody* newBody)
{
	const Ogre::Vector3 position = GetPosition();
	const Ogre::Quaternion rot = GetOrientation();

	this->DeleteRigidBody();

	m_body = newBody;
	if (!m_body) return;
	m_body->setUserPointer(m_owner);

	// 还原质量（需在写回 transform 前设置惯量）
	this->SetMass(m_owner->GetMass());
	
	this->SetPosition(position);
	this->SetOrientation(rot);

	// 重新加入物理世界
	this->AddToWorld();
}

// Transforms/velocity/force
void PhysicsComponent::SetPosition(const Ogre::Vector3& position)
{
	if (!m_body) return;
	
	btTransform transform = m_body->getWorldTransform();
	transform.setOrigin(btVector3(position.x, position.y, position.z));

	m_body->setWorldTransform(transform);
	m_body->activate(true);
}

void PhysicsComponent::SetOrientation(const Ogre::Quaternion& quaternion)
{
	if (!m_body) return;

	btTransform transform = m_body->getWorldTransform();
	transform.setRotation(btQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w));

	m_body->setWorldTransform(transform);
	m_body->activate(true);
}

void PhysicsComponent::SetRotation(const Ogre::Vector3& rotation)
{
	Ogre::Quaternion qRotation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
	this->SetOrientation(qRotation);
}

Ogre::Vector3 PhysicsComponent::GetPosition() const
{
	if (!m_body) return Ogre::Vector3::ZERO;

	const btVector3& position = m_body->getCenterOfMassPosition();
	return BtVector3ToVector3(position);
}

Ogre::Quaternion PhysicsComponent::GetOrientation() const
{
	if (!m_body) return Ogre::Quaternion::ZERO;

	const btQuaternion& orietation = m_body->getOrientation();
	return BtQuaternionToQuaternion(orietation);
}

void PhysicsComponent::SetForward(const Ogre::Vector3& forward)
{
	if (!m_body) return;

	Ogre::Vector3 up = Ogre::Vector3::UNIT_Y;

	Ogre::Vector3 zAxis = forward.normalisedCopy();
	Ogre::Vector3 xAxis = up.crossProduct(zAxis);
	Ogre::Vector3 yAxis = zAxis.crossProduct(xAxis);

	Ogre::Quaternion orientation(xAxis, yAxis, zAxis);

	btTransform transform = m_body->getWorldTransform();
	transform.setRotation(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w));
	m_body->setWorldTransform(transform);
	m_body->activate(true);
}

void PhysicsComponent::SetVelocity(const Ogre::Vector3& velocity)
{
	if (!m_body) return;

	btVector3 btVelocity(velocity.x, velocity.y, velocity.z);
	m_body->setLinearVelocity(btVelocity);
	m_body->activate(true);
}

Ogre::Vector3 PhysicsComponent::GetVelocity() const
{
	if (!m_body) return Ogre::Vector3::ZERO;

	btVector3 velocity = m_body->getLinearVelocity();
	return BtVector3ToVector3(velocity);
}

Ogre::Vector3 PhysicsComponent::GetUp() const
{
	if (!m_body) return Ogre::Vector3::UNIT_Y;
	
	const btQuaternion& orietation = m_body->getOrientation();
	return BtQuaternionToQuaternion(orietation).yAxis();
}

Ogre::Vector3 PhysicsComponent::GetLeft() const
{
	if (!m_body) return Ogre::Vector3::UNIT_X;

	const btQuaternion& orietation = m_body->getOrientation();
	return BtQuaternionToQuaternion(orietation).xAxis();
}

Ogre::Vector3 PhysicsComponent::GetForward() const
{
	if (!m_body) return Ogre::Vector3::UNIT_Z;

	const btQuaternion& orietation = m_body->getOrientation();
	return BtQuaternionToQuaternion(orietation).zAxis();
}

void PhysicsComponent::ApplyForce(const Ogre::Vector3& force)
{
	if (!m_body) return;

	btVector3 centralForce(force.x, force.y, force.z);
	m_body->applyCentralForce(centralForce);
	m_body->activate(true);
}

// Parameters
void PhysicsComponent::SetMass(Ogre::Real mass)
{
	if (!m_body) return;

	btVector3 localInertia(0, 0, 0);
	m_body->getCollisionShape()->calculateLocalInertia(mass, localInertia);
	m_body->setMassProps(mass, localInertia);
	m_body->updateInertiaTensor();
	m_body->activate(true);
}

Ogre::Real PhysicsComponent::GetMass() const
{
	if (!m_body) return Ogre::Real(0.0f);

	btScalar invMass = m_body->getInvMass();
	if (invMass == 0) return Ogre::Real(0.0f);

	return Ogre::Real(1.0f / invMass);
}

Ogre::Real PhysicsComponent::GetHorizontalSpeed() const
{
	if (!m_body) return Ogre::Real(0.0f);

	btVector3 velocity = m_body->getLinearVelocity();
	return Ogre::Vector3(velocity.x(), 0, velocity.z()).length();
}

void PhysicsComponent::RebuildCapsule(Ogre::Real height, Ogre::Real radius)
{
	btRigidBody* capsule = SandboxMgr::CreateRigidBodyCapsule(height, radius);
	capsule->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
	this->ResetRigidBody(capsule);
}

