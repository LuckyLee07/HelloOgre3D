#include "AgentObject.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"
#include "btBulletDynamicsCommon.h"
#include "SandboxMgr.h"
#include "SandboxDef.h"
#include "GameManager.h"
#include "ScriptLuaVM.h"
#include "opensteer/include/SteerLibrary.h"

using namespace Ogre;

const float AgentObject::DEFAULT_AGENT_MASS = 90.7f;	// kilograms (200 lbs)
const float AgentObject::DEFAULT_AGENT_HEIGHT = 1.6f;	// meters (5.2 feet)
const float AgentObject::DEFAULT_AGENT_RADIUS = 0.3f;	// meters (1.97 feet)
const float AgentObject::DEFAULT_AGENT_SPEED = 0.0f;			// m/s (0 ft/s)
const float AgentObject::DEFAULT_AGENT_HEALTH = 100.0f;			// default
const float AgentObject::DEFAULT_AGENT_MAX_FORCE = 1000.0f;		// newtons (kg*m/s^2)
const float AgentObject::DEFAULT_AGENT_MAX_SPEED = 7.0f;		// m/s (23.0 ft/s)
const float AgentObject::DEFAULT_AGENT_TARGET_RADIUS = 0.5f;	// meters (1.64 feet)

AgentObject::AgentObject(Ogre::SceneNode* pSceneNode, btRigidBody* pRigidBody)
	: BaseObject(0, BaseObject::OBJ_AGENT), 
	m_pSceneNode(pSceneNode), m_pRigidBody(pRigidBody),
	m_mass(DEFAULT_AGENT_MASS), 
	m_height(DEFAULT_AGENT_HEIGHT), 
	m_radius(DEFAULT_AGENT_RADIUS),
	m_speed(DEFAULT_AGENT_SPEED),
	m_health(DEFAULT_AGENT_HEALTH),
	m_maxForce(DEFAULT_AGENT_MAX_FORCE),
	m_maxSpeed(DEFAULT_AGENT_MAX_SPEED),
	m_targetRadius(DEFAULT_AGENT_TARGET_RADIUS)
{
	if (m_pRigidBody)
	{
		this->SetMass(DEFAULT_AGENT_MASS);
		m_pRigidBody->setUserPointer(this);
	}
	SetForward(Ogre::Vector3::UNIT_Z);
}

AgentObject::~AgentObject()
{
	m_pSceneNode = nullptr;

	delete m_pRigidBody->getMotionState();
	delete m_pRigidBody->getCollisionShape();
	delete m_pRigidBody;
	m_pRigidBody = nullptr;
}

void AgentObject::Initialize()
{
	SetPosition(Ogre::Vector3::ZERO);

	SetTarget(Ogre::Vector3(50.0f, 0.0f, 0.0f));
	SetTargetRadius(1.5f);
}

void AgentObject::ResetRigidBody(btRigidBody* pRigidBody)
{
	PhysicsWorld* pPhysicsWorld = g_GameManager->getPhysicsWorld();
	if (m_pRigidBody != nullptr)
	{
		pPhysicsWorld->removeRigidBody(m_pRigidBody);

		delete m_pRigidBody->getMotionState();
		delete m_pRigidBody->getCollisionShape();
		delete m_pRigidBody;
		m_pRigidBody = nullptr;
	}

	m_pRigidBody = pRigidBody;
	m_pRigidBody->setUserPointer(this);
	pPhysicsWorld->addRigidBody(m_pRigidBody);
}

void AgentObject::SetPosition(const Ogre::Vector3& position)
{
	btVector3 btPosition(position.x, position.y, position.z);
	btTransform transform = m_pRigidBody->getWorldTransform();
	transform.setOrigin(btPosition);

	m_pRigidBody->setWorldTransform(transform);
	m_pRigidBody->activate(true);
}

void AgentObject::SetRotation(const Ogre::Vector3& rotation)
{
	Ogre::Quaternion qRotation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
	this->SetOrientation(qRotation);
}

void AgentObject::SetOrientation(const Ogre::Quaternion& quaternion)
{
	btQuaternion btRotation(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	btTransform transform = m_pRigidBody->getWorldTransform();
	transform.setRotation(btRotation);

	m_pRigidBody->setWorldTransform(transform);
	m_pRigidBody->activate(true);

	this->updateWorldTransform();
}

void AgentObject::SetForward(const Ogre::Vector3& forward)
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

void AgentObject::SetVelocity(const Ogre::Vector3& velocity)
{
	if (m_pRigidBody != nullptr)
	{
		btVector3 btVelocity(velocity.x, velocity.y, velocity.z);
		m_pRigidBody->setLinearVelocity(btVelocity);
		m_pRigidBody->activate(true);
	}
	SetSpeed(Ogre::Vector3(velocity.x, 0, velocity.z).length());
}

void AgentObject::SetTarget(const Ogre::Vector3& targetPos)
{
	m_targetPos = targetPos;
}

void AgentObject::SetTargetRadius(Ogre::Real radius)
{
	m_targetRadius = std::max(Ogre::Real(0.0f), radius);
}

Ogre::Vector3 AgentObject::GetPosition() const
{
	if (m_pRigidBody != nullptr)
	{
		const btVector3& position = m_pRigidBody->getCenterOfMassPosition();
		return BtVector3ToVector3(position);
	}
	else if (m_pSceneNode != nullptr)
	{
		return m_pSceneNode->_getDerivedPosition();
	}

	return Ogre::Vector3::ZERO;
}

Ogre::Quaternion AgentObject::GetOrientation() const
{
	if (m_pRigidBody != nullptr)
	{
		const btQuaternion& orietation = m_pRigidBody->getOrientation();
		return BtQuaternionToQuaternion(orietation);
	}
	else if (m_pSceneNode != nullptr)
	{
		return m_pSceneNode->_getDerivedOrientation();
	}

	return Ogre::Quaternion::ZERO;
}

Ogre::Vector3 AgentObject::GetUp() const
{
	if (m_pRigidBody != nullptr)
	{
		const btQuaternion& orietation = m_pRigidBody->getOrientation();
		return BtQuaternionToQuaternion(orietation).yAxis();
	}
	else if (m_pSceneNode != nullptr)
	{
		return m_pSceneNode->getOrientation().yAxis();
	}

	return Ogre::Vector3::UNIT_Y;
}

Ogre::Vector3 AgentObject::GetLeft() const
{
	if (m_pRigidBody != nullptr)
	{
		const btQuaternion& orietation = m_pRigidBody->getOrientation();
		return BtQuaternionToQuaternion(orietation).xAxis();
	}
	else if (m_pSceneNode != nullptr)
	{
		return m_pSceneNode->getOrientation().xAxis();
	}

	return Ogre::Vector3::UNIT_X;
}

Ogre::Vector3 AgentObject::GetTarget() const
{
	return m_targetPos;
}

Ogre::Vector3 AgentObject::GetForward() const
{
	if (m_pRigidBody != nullptr)
	{
		const btQuaternion& orietation = m_pRigidBody->getOrientation();
		return BtQuaternionToQuaternion(orietation).zAxis();
	}
	else if (m_pSceneNode != nullptr)
	{
		return m_pSceneNode->getOrientation().zAxis();
	}

	return Ogre::Vector3::UNIT_Z;
}

Ogre::Vector3 AgentObject::GetVelocity() const
{
	if (m_pRigidBody != nullptr)
	{
		btVector3 velocity = m_pRigidBody->getLinearVelocity();
		return BtVector3ToVector3(velocity);
	}
	
	return GetForward() * m_speed;
}

void AgentObject::SetMass(const Ogre::Real objMass)
{
	m_mass = objMass;

	if (m_pRigidBody != nullptr) // SetRigidBodyMass
	{
		btVector3 localInertia(0, 0, 0);
		m_pRigidBody->getCollisionShape()->calculateLocalInertia(objMass, localInertia);
		m_pRigidBody->setMassProps(objMass, localInertia);
		m_pRigidBody->updateInertiaTensor();
		m_pRigidBody->activate(true);
	}
}

void AgentObject::SetHeight(Ogre::Real height)
{
	m_height = std::max(Ogre::Real(0), height);

	if (m_pRigidBody != nullptr)
	{
		Ogre::Real radius = this->GetRadius();
		btRigidBody* capsuleRigidBody = SandboxMgr::CreateRigidBodyCapsule(height, radius);
		capsuleRigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
		this->ResetRigidBody(capsuleRigidBody);
	}
}

void AgentObject::SetRadius(Ogre::Real radius)
{
	m_radius = std::max(Ogre::Real(0), radius);

	if (m_pRigidBody != nullptr)
	{
		Ogre::Real height = this->GetHeight();
		btRigidBody* capsuleRigidBody = SandboxMgr::CreateRigidBodyCapsule(height, radius);
		capsuleRigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
		this->ResetRigidBody(capsuleRigidBody);
	}
}

void AgentObject::SetSpeed(Ogre::Real speed)
{
	m_speed = speed;
}

void AgentObject::SetHealth(Ogre::Real health)
{
	m_health = health;
}

void AgentObject::SetMaxForce(Ogre::Real maxForce)
{
	m_maxForce = std::max(Ogre::Real(0), maxForce);
}

void AgentObject::SetMaxSpeed(Ogre::Real maxSpeed)
{
	m_maxSpeed = std::max(Ogre::Real(0), maxSpeed);
}

Ogre::Real AgentObject::GetMass() const
{
	if (m_pRigidBody != nullptr)
	{
		btScalar invMass = m_pRigidBody->getInvMass();
		if (invMass == 0) return Ogre::Real(0.0f);
		
		return Ogre::Real(1.0f / invMass);
	}

	return m_mass;
}

Ogre::Real AgentObject::GetSpeed() const
{
	if (m_pRigidBody != nullptr)
	{
		btVector3 velocity = m_pRigidBody->getLinearVelocity();
		return Ogre::Vector3(velocity.x(), 0, velocity.z()).length();
	}

	return m_speed;
}

Ogre::Vector3 AgentObject::PredictFuturePosition(Ogre::Real predictionTime) const
{
	Ogre::Real upredictionTime = std::max(Ogre::Real(0), predictionTime);
	return GetPosition() + GetVelocity() * upredictionTime;
}

Ogre::Vector3 AgentObject::ForceToPosition(const Ogre::Vector3& position)
{
	return Vec3ToVector3(steerForSeek(Vector3ToVec3(position)));
}

void AgentObject::ApplyForce(const Ogre::Vector3& force)
{
	if (m_pRigidBody != nullptr)
	{
		btVector3 centralForce(force.x, force.y, force.z);
		m_pRigidBody->applyCentralForce(centralForce);
		m_pRigidBody->activate(true);
	}
}


void AgentObject::update(int deltaMsec)
{
	static int totalMsec = 0;
	totalMsec += deltaMsec;
	if (true || totalMsec > 1000)
	{
		totalMsec = 0;
		GetScriptLuaVM()->callFunction("Agent_Update", "u[AgentObject]i", this, deltaMsec);
	}

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



/**
* implementation of OpenSteer AbstractVehicle.
* @see opensteer/include/AbstractVehicle.h
*/
float AgentObject::mass(void) const
{
	return static_cast<float>(GetMass());
}

float AgentObject::setMass(float mass)
{
	SetMass(Ogre::Real(mass));
	return this->mass();
}

float AgentObject::radius(void) const
{
	return static_cast<float>(GetRadius());
}

float AgentObject::setRadius(float radius)
{
	SetRadius(Ogre::Real(radius));
	return this->radius();
}

OpenSteer::Vec3 AgentObject::velocity(void) const
{
	return Vector3ToVec3(GetVelocity());
}

float AgentObject::speed(void) const
{
	return static_cast<float>(GetSpeed());
}

float AgentObject::setSpeed(float speed)
{
	SetSpeed(Ogre::Real(speed));
	return this->speed();
}


OpenSteer::Vec3 AgentObject::predictFuturePosition(const float predictionTime) const
{
	return Vector3ToVec3(PredictFuturePosition(Ogre::Real(predictionTime)));
}

float AgentObject::maxForce(void) const
{
	return static_cast<float>(GetMaxForce());
}

float AgentObject::setMaxForce(float maxForce)
{
	SetMaxForce(Ogre::Real(maxForce));
	return this->maxForce();
}

float AgentObject::maxSpeed(void) const
{
	return static_cast<float>(GetMaxSpeed());
}

float AgentObject::setMaxSpeed(float maxSpeed)
{
	SetMaxSpeed(Ogre::Real(maxSpeed));
	return this->maxSpeed();
}

/**
* implementation of OpenSteer AbstractLocalSpace.
* @see opensteer/include/LocalSpace.h
*/
OpenSteer::Vec3 AgentObject::side(void) const
{
	return Vector3ToVec3(GetLeft());
}

OpenSteer::Vec3 AgentObject::setSide(OpenSteer::Vec3 s)
{
	assert(false);
	return OpenSteer::Vec3();
}

OpenSteer::Vec3 AgentObject::up(void) const
{
	return Vector3ToVec3(GetUp());
}

OpenSteer::Vec3 AgentObject::setUp(OpenSteer::Vec3 u)
{
	assert(false);
	return OpenSteer::Vec3();
}

OpenSteer::Vec3 AgentObject::forward(void) const
{
	return Vector3ToVec3(GetForward());
}

OpenSteer::Vec3 AgentObject::setForward(OpenSteer::Vec3 f)
{
	assert(false);
	return OpenSteer::Vec3();
}

OpenSteer::Vec3 AgentObject::position(void) const
{
	return Vector3ToVec3(GetPosition());
}

OpenSteer::Vec3 AgentObject::setPosition(OpenSteer::Vec3 pos)
{
	SetPosition(Vec3ToVector3(pos));
	return this->position();
}

bool AgentObject::rightHanded(void) const
{
	return true;
}

void AgentObject::resetLocalSpace(void)
{
	assert(false);
}

void AgentObject::setUnitSideFromForwardAndUp(void)
{
	assert(false);
}

OpenSteer::Vec3 AgentObject::localizeDirection(const OpenSteer::Vec3& globalDirection) const
{
	float dirX = globalDirection.dot(Vector3ToVec3(GetLeft()));
	float dirY = globalDirection.dot(Vector3ToVec3(GetUp()));
	float dirZ = globalDirection.dot(Vector3ToVec3(GetForward()));
	return OpenSteer::Vec3(dirX, dirY, dirZ);
}

OpenSteer::Vec3 AgentObject::localizePosition(const OpenSteer::Vec3& globalPosition) const
{
	const OpenSteer::Vec3& position = Vector3ToVec3(GetPosition());
	OpenSteer::Vec3 globalOffset = globalPosition - position;
	return localizeDirection(globalOffset);
}

OpenSteer::Vec3 AgentObject::globalizePosition(const OpenSteer::Vec3& localPosition) const
{
	return OpenSteer::Vec3();
}

OpenSteer::Vec3 AgentObject::globalizeDirection(const OpenSteer::Vec3& localDirection) const
{
	return OpenSteer::Vec3();
}

void AgentObject::regenerateOrthonormalBasisUF(const OpenSteer::Vec3& newUnitForward)
{
	(void)newUnitForward;
}

void AgentObject::regenerateOrthonormalBasis(const OpenSteer::Vec3& newForward)
{
	(void)newForward;
}

void AgentObject::regenerateOrthonormalBasis(const OpenSteer::Vec3& newForward, const OpenSteer::Vec3& newUp)
{
	(void)newForward;
	(void)newUp;
}

OpenSteer::Vec3 AgentObject::localRotateForwardToSide(const OpenSteer::Vec3& v) const
{
	return OpenSteer::Vec3();
}

OpenSteer::Vec3 AgentObject::globalRotateForwardToSide(const OpenSteer::Vec3& globalForward) const
{
	return OpenSteer::Vec3();
}
