#include "VehicleObject.h"
#include "btBulletDynamicsCommon.h"
#include "GameManager.h"
#include "ScriptLuaVM.h"
#include "BlockObject.h"
#include "AgentObject.h"
#include "play/PhysicsWorld.h"
#include "manager/SandboxMgr.h"
#include "manager/ObjectManager.h"

using namespace Ogre;

const float VehicleObject::DEFAULT_AGENT_MASS = 90.7f;	// kilograms (200 lbs)
const float VehicleObject::DEFAULT_AGENT_HEIGHT = 1.6f;	// meters (5.2 feet)
const float VehicleObject::DEFAULT_AGENT_RADIUS = 0.3f;	// meters (1.97 feet)
const float VehicleObject::DEFAULT_AGENT_SPEED = 0.0f;			// m/s (0 ft/s)
const float VehicleObject::DEFAULT_AGENT_HEALTH = 100.0f;			// default
const float VehicleObject::DEFAULT_AGENT_MAX_FORCE = 1000.0f;		// newtons (kg*m/s^2)
const float VehicleObject::DEFAULT_AGENT_MAX_SPEED = 7.0f;		// m/s (23.0 ft/s)
const float VehicleObject::DEFAULT_AGENT_TARGET_RADIUS = 0.5f;	// meters (1.64 feet)

VehicleObject::VehicleObject(btRigidBody* pRigidBody)
	: m_pRigidBody(pRigidBody), 
	m_mass(DEFAULT_AGENT_MASS), 
	m_height(DEFAULT_AGENT_HEIGHT), 
	m_radius(DEFAULT_AGENT_RADIUS),
	m_speed(DEFAULT_AGENT_SPEED),
	m_health(DEFAULT_AGENT_HEALTH),
	m_maxForce(DEFAULT_AGENT_MAX_FORCE),
	m_maxSpeed(DEFAULT_AGENT_MAX_SPEED),
	m_targetRadius(DEFAULT_AGENT_TARGET_RADIUS),
	m_pTargetAgent(nullptr), m_hasPath(false)
{
	if (m_pRigidBody)
	{
		this->SetMass(DEFAULT_AGENT_MASS);
		m_pRigidBody->setUserPointer(this);
	}
}

VehicleObject::~VehicleObject()
{
	m_pTargetAgent = nullptr;
	this->DeleteRighdBody();
}

void VehicleObject::DeleteRighdBody()
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

void VehicleObject::Initialize()
{
	
}

void VehicleObject::ResetRigidBody(btRigidBody* pRigidBody)
{
	this->DeleteRighdBody();

	m_pRigidBody = pRigidBody;
	m_pRigidBody->setUserPointer(this);

	PhysicsWorld* pPhysicsWorld = g_GameManager->getPhysicsWorld();
	pPhysicsWorld->addRigidBody(m_pRigidBody, this);
}

void VehicleObject::setPosition(const Ogre::Vector3& position)
{
	if (m_pRigidBody != nullptr)
	{
		btVector3 btPosition(position.x, position.y, position.z);
		btTransform transform = m_pRigidBody->getWorldTransform();
		transform.setOrigin(btPosition);

		m_pRigidBody->setWorldTransform(transform);
		m_pRigidBody->activate(true);
	}

	this->updateWorldTransform();
}

void VehicleObject::setRotation(const Ogre::Vector3& rotation)
{
	Ogre::Quaternion qRotation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
	this->setOrientation(qRotation);
}

void VehicleObject::setOrientation(const Ogre::Quaternion& quaternion)
{
	if (m_pRigidBody != nullptr)
	{
		btQuaternion btRotation(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
		btTransform transform = m_pRigidBody->getWorldTransform();
		transform.setRotation(btRotation);

		m_pRigidBody->setWorldTransform(transform);
		m_pRigidBody->activate(true);
	}

	this->updateWorldTransform();
}

Ogre::Vector3 VehicleObject::GetPosition() const
{
	if (m_pRigidBody != nullptr)
	{
		const btVector3& position = m_pRigidBody->getCenterOfMassPosition();
		return BtVector3ToVector3(position);
	}

	return Ogre::Vector3::ZERO;
}

Ogre::Quaternion VehicleObject::GetOrientation() const
{
	if (m_pRigidBody != nullptr)
	{
		const btQuaternion& orietation = m_pRigidBody->getOrientation();
		return BtQuaternionToQuaternion(orietation);
	}

	return Ogre::Quaternion::ZERO;
}

void VehicleObject::SetForward(const Ogre::Vector3& forward)
{
	Ogre::Vector3 up = Ogre::Vector3::UNIT_Y;

	Ogre::Vector3 zAxis = forward.normalisedCopy();
	Ogre::Vector3 xAxis = up.crossProduct(zAxis);
	Ogre::Vector3 yAxis = zAxis.crossProduct(xAxis);

	Ogre::Quaternion orientation(xAxis, yAxis, zAxis);

	if (m_pRigidBody != nullptr)
	{
		btTransform transform = m_pRigidBody->getWorldTransform();
		btQuaternion btRotation(orientation.x, orientation.y, orientation.z, orientation.w);
		transform.setRotation(btRotation);
		m_pRigidBody->setWorldTransform(transform);
		m_pRigidBody->activate(true);
	}

	this->updateWorldTransform();
}

void VehicleObject::SetVelocity(const Ogre::Vector3& velocity)
{
	if (m_pRigidBody != nullptr)
	{
		btVector3 btVelocity(velocity.x, velocity.y, velocity.z);
		m_pRigidBody->setLinearVelocity(btVelocity);
		m_pRigidBody->activate(true);
	}
	SetSpeed(Ogre::Vector3(velocity.x, 0, velocity.z).length());
}

void VehicleObject::SetTarget(const Ogre::Vector3& targetPos)
{
	m_targetPos = targetPos;
}

void VehicleObject::SetTargetRadius(Ogre::Real radius)
{
	m_targetRadius = std::max(Ogre::Real(0.0f), radius);
}

Ogre::Vector3 VehicleObject::GetUp() const
{
	if (m_pRigidBody != nullptr)
	{
		const btQuaternion& orietation = m_pRigidBody->getOrientation();
		return BtQuaternionToQuaternion(orietation).yAxis();
	}

	return Ogre::Vector3::UNIT_Y;
}

Ogre::Vector3 VehicleObject::GetLeft() const
{
	if (m_pRigidBody != nullptr)
	{
		const btQuaternion& orietation = m_pRigidBody->getOrientation();
		return BtQuaternionToQuaternion(orietation).xAxis();
	}

	return Ogre::Vector3::UNIT_X;
}

Ogre::Vector3 VehicleObject::GetForward() const
{
	if (m_pRigidBody != nullptr)
	{
		const btQuaternion& orietation = m_pRigidBody->getOrientation();
		return BtQuaternionToQuaternion(orietation).zAxis();
	}

	return Ogre::Vector3::UNIT_Z;
}

Ogre::Vector3 VehicleObject::GetTarget() const
{
	return m_targetPos;
}

Ogre::Vector3 VehicleObject::GetVelocity() const
{
	if (m_pRigidBody != nullptr)
	{
		btVector3 velocity = m_pRigidBody->getLinearVelocity();
		return BtVector3ToVector3(velocity);
	}
	
	return GetForward() * m_speed;
}

void VehicleObject::SetMass(const Ogre::Real mass)
{
	m_mass = std::max(Ogre::Real(0), mass);

	if (m_pRigidBody != nullptr) // SetRigidBodyMass
	{
		btVector3 localInertia(0, 0, 0);
		m_pRigidBody->getCollisionShape()->calculateLocalInertia(mass, localInertia);
		m_pRigidBody->setMassProps(mass, localInertia);
		m_pRigidBody->updateInertiaTensor();
		m_pRigidBody->activate(true);
	}
}

void VehicleObject::SetHeight(Ogre::Real height)
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

void VehicleObject::SetRadius(Ogre::Real radius)
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

void VehicleObject::SetSpeed(Ogre::Real speed)
{
	m_speed = speed;
}

void VehicleObject::SetHealth(Ogre::Real health)
{
	if (m_health == health) return;
	m_health = health;

	Event()->Emit("HEALTH_CHANGE", SandboxContext().Set_Number("health", health));
}

void VehicleObject::SetMaxForce(Ogre::Real maxForce)
{
	m_maxForce = std::max(Ogre::Real(0), maxForce);
}

void VehicleObject::SetMaxSpeed(Ogre::Real maxSpeed)
{
	m_maxSpeed = std::max(Ogre::Real(0), maxSpeed);
}

Ogre::Real VehicleObject::GetMass() const
{
	if (m_pRigidBody != nullptr)
	{
		btScalar invMass = m_pRigidBody->getInvMass();
		if (invMass == 0) return Ogre::Real(0.0f);
		
		return Ogre::Real(1.0f / invMass);
	}
	return m_mass;
}

Ogre::Real VehicleObject::GetSpeed() const
{
	if (m_pRigidBody != nullptr)
	{
		btVector3 velocity = m_pRigidBody->getLinearVelocity();
		return Ogre::Vector3(velocity.x(), 0, velocity.z()).length();
	}
	return m_speed;
}

Ogre::Vector3 VehicleObject::PredictFuturePosition(Ogre::Real predictionTime) const
{
	Ogre::Real upredictionTime = std::max(Ogre::Real(0), predictionTime);
	return GetPosition() + GetVelocity() * upredictionTime;
}

Ogre::Vector3 VehicleObject::ForceToPosition(const Ogre::Vector3& position)
{
	return Vec3ToVector3(steerForSeek(Vector3ToVec3(position)));
}

Ogre::Vector3 VehicleObject::ForceToFollowPath(Ogre::Real predictionTime)
{
	return ForceToFollowPath(m_path, predictionTime);
}

Ogre::Vector3 VehicleObject::ForceToFollowPath(AgentPath& path, Ogre::Real predictionTime)
{
	const static int FORWARD_DIRECTION = 1;
	const static float MIN_PREDICTION_TIME = 0.1f;

	if (path.GetNumberOfPathPoints() > 0)
	{
		float predictionTime1 = std::max(MIN_PREDICTION_TIME, predictionTime);
		return Vec3ToVector3(steerToFollowPath(FORWARD_DIRECTION, predictionTime1, path));
	}
	return Ogre::Vector3(0.0f);
}

Ogre::Vector3 VehicleObject::ForceToStayOnPath(Ogre::Real predictionTime)
{
	return ForceToStayOnPath(m_path, predictionTime);
}

Ogre::Vector3 VehicleObject::ForceToStayOnPath(AgentPath& path, Ogre::Real predictionTime)
{
	const static float MIN_PREDICTION_TIME = 0.1f;

	if (path.GetNumberOfPathPoints() > 0)
	{
		float predictionTime1 = std::max(MIN_PREDICTION_TIME, predictionTime);
		return Vec3ToVector3(steerToStayOnPath(predictionTime1, m_path));
	}
	return Ogre::Vector3(0.0f);
}

Ogre::Vector3 VehicleObject::ForceToWander(Ogre::Real deltaMilliSeconds)
{
	return Vec3ToVector3(steerForWander(deltaMilliSeconds));
}

Ogre::Vector3 VehicleObject::ForceToTargetSpeed(Ogre::Real targetSpeed)
{
	return Vec3ToVector3(steerForTargetSpeed(targetSpeed));
}

Ogre::Vector3 VehicleObject::ForceToCombine(const std::vector<AgentObject*>& agents, Ogre::Real distance, Ogre::Real angle)
{
	OpenSteer::AVGroup group;
	std::vector<AgentObject*>::const_iterator iter;
	for (iter = agents.begin(); iter != agents.end(); iter++)
	{
		group.push_back(*iter);
	}
	const float maxCosAngle = cosf(Ogre::Degree(angle).valueRadians());
	return Vec3ToVector3(steerForCohesion(distance, maxCosAngle, group));
}

Ogre::Vector3 VehicleObject::ForceToSeparate(const std::vector<AgentObject*>& agents, Ogre::Real distance, Ogre::Real angle)
{
	OpenSteer::AVGroup group;
	std::vector<AgentObject*>::const_iterator iter;
	for (iter = agents.begin(); iter != agents.end(); iter++)
	{
		group.push_back(*iter);
	}
	const float maxCosAngle = cosf(Ogre::Degree(angle).valueRadians());
	return Vec3ToVector3(steerForSeparation(distance, maxCosAngle, group));
}

Ogre::Vector3 VehicleObject::ForceToAvoidAgents(Ogre::Real predictionTime)
{
	std::vector<AgentObject*> newAgents;
	const std::vector<AgentObject*>& agents = g_ObjectManager->getAllAgents();
	
	std::vector<AgentObject*>::const_iterator iter;
	for (iter = agents.begin(); iter < agents.end(); iter++)
	{
		AgentObject* pAgent = *iter;
		if (pAgent->GetHealth() > 0)
		{
			newAgents.push_back(pAgent);
		}
	}
	return ForceToAvoidAgents(newAgents, predictionTime);
}

Ogre::Vector3 VehicleObject::ForceToAvoidAgents(const std::vector<AgentObject*>& agents, Ogre::Real predictionTime)
{
	const static float MIN_PREDICTION_TIME = 0.1f;

	OpenSteer::AVGroup group;
	std::vector<AgentObject*>::const_iterator iter;
	for (iter = agents.begin(); iter != agents.end(); iter++)
	{
		group.push_back(*iter);
	}
	float predictionTime1 = std::max(MIN_PREDICTION_TIME, predictionTime);
	return Vec3ToVector3(steerToAvoidNeighbors(predictionTime1, group));
}

Ogre::Vector3 VehicleObject::ForceToAvoidObjects(Ogre::Real predictionTime)
{
	const std::vector<BlockObject*>& allBlocks = g_ObjectManager->getAllBlocks();
	return ForceToAvoidObjects(allBlocks, predictionTime);
}

Ogre::Vector3 VehicleObject::ForceToAvoidObjects(const std::vector<BlockObject*>& objects, Ogre::Real predictionTime)
{
	const static float MIN_PREDICTION_TIME = 0.1f;
	float timeToCollision = std::max(MIN_PREDICTION_TIME, predictionTime);

	OpenSteer::Vec3 avoidForce = OpenSteer::Vec3::zero;
	std::vector<BlockObject*>::const_iterator iter;
	for (iter = objects.begin(); iter != objects.end(); iter++)
	{
		const BlockObject* pObject = *iter;
		if (pObject->GetMass() > 0)
		{
			avoidForce += pObject->steerToAvoid(*this, timeToCollision);
		}
	}
	return Vec3ToVector3(avoidForce);
}

void VehicleObject::ApplyForce(const Ogre::Vector3& force)
{
	if (m_pRigidBody != nullptr)
	{
		btVector3 centralForce(force.x, force.y, force.z);
		m_pRigidBody->applyCentralForce(centralForce);
		m_pRigidBody->activate(true);
	}
}

void VehicleObject::SetPath(const std::vector<Ogre::Vector3>& points, bool cyclic)
{
	SetPath(AgentPath(points, m_radius, cyclic));
}

void VehicleObject::SetPath(const AgentPath& agentPath)
{
	m_hasPath = true;
	m_path = agentPath;

	m_path.GetPathPoints(m_points);
}

void VehicleObject::update(int deltaMilisec)
{
	this->updateWorldTransform();
}

void VehicleObject::updateWorldTransform()
{

}


/**
* implementation of OpenSteer AbstractVehicle.
* @see opensteer/include/AbstractVehicle.h
*/
float VehicleObject::mass(void) const
{
	return static_cast<float>(GetMass());
}

float VehicleObject::setMass(float mass)
{
	SetMass(Ogre::Real(mass));
	return this->mass();
}

float VehicleObject::radius(void) const
{
	return static_cast<float>(GetRadius());
}

float VehicleObject::setRadius(float radius)
{
	SetRadius(Ogre::Real(radius));
	return this->radius();
}

OpenSteer::Vec3 VehicleObject::velocity(void) const
{
	return Vector3ToVec3(GetVelocity());
}

float VehicleObject::speed(void) const
{
	return static_cast<float>(GetSpeed());
}

float VehicleObject::setSpeed(float speed)
{
	SetSpeed(Ogre::Real(speed));
	return this->speed();
}


OpenSteer::Vec3 VehicleObject::predictFuturePosition(const float predictionTime) const
{
	return Vector3ToVec3(PredictFuturePosition(Ogre::Real(predictionTime)));
}

float VehicleObject::maxForce(void) const
{
	return static_cast<float>(GetMaxForce());
}

float VehicleObject::setMaxForce(float maxForce)
{
	SetMaxForce(Ogre::Real(maxForce));
	return this->maxForce();
}

float VehicleObject::maxSpeed(void) const
{
	return static_cast<float>(GetMaxSpeed());
}

float VehicleObject::setMaxSpeed(float maxSpeed)
{
	SetMaxSpeed(Ogre::Real(maxSpeed));
	return this->maxSpeed();
}

/**
* implementation of OpenSteer AbstractLocalSpace.
* @see opensteer/include/LocalSpace.h
*/
OpenSteer::Vec3 VehicleObject::side(void) const
{
	return Vector3ToVec3(GetLeft());
}

OpenSteer::Vec3 VehicleObject::setSide(OpenSteer::Vec3 s)
{
	assert(false);
	return OpenSteer::Vec3();
}

OpenSteer::Vec3 VehicleObject::up(void) const
{
	return Vector3ToVec3(GetUp());
}

OpenSteer::Vec3 VehicleObject::setUp(OpenSteer::Vec3 u)
{
	assert(false);
	return OpenSteer::Vec3();
}

OpenSteer::Vec3 VehicleObject::forward(void) const
{
	return Vector3ToVec3(GetForward());
}

OpenSteer::Vec3 VehicleObject::setForward(OpenSteer::Vec3 f)
{
	assert(false);
	return OpenSteer::Vec3();
}

OpenSteer::Vec3 VehicleObject::position(void) const
{
	return Vector3ToVec3(GetPosition());
}

OpenSteer::Vec3 VehicleObject::setPosition(OpenSteer::Vec3 pos)
{
	setPosition(Vec3ToVector3(pos));
	return this->position();
}

bool VehicleObject::rightHanded(void) const
{
	return true;
}

void VehicleObject::resetLocalSpace(void)
{
	assert(false);
}

void VehicleObject::setUnitSideFromForwardAndUp(void)
{
	assert(false);
}

OpenSteer::Vec3 VehicleObject::localizeDirection(const OpenSteer::Vec3& globalDirection) const
{
	float dirX = globalDirection.dot(Vector3ToVec3(GetLeft()));
	float dirY = globalDirection.dot(Vector3ToVec3(GetUp()));
	float dirZ = globalDirection.dot(Vector3ToVec3(GetForward()));
	return OpenSteer::Vec3(dirX, dirY, dirZ);
}

OpenSteer::Vec3 VehicleObject::localizePosition(const OpenSteer::Vec3& globalPosition) const
{
	const OpenSteer::Vec3& position = Vector3ToVec3(GetPosition());
	OpenSteer::Vec3 globalOffset = globalPosition - position;
	return localizeDirection(globalOffset);
}

OpenSteer::Vec3 VehicleObject::globalizePosition(const OpenSteer::Vec3& localPosition) const
{
	return OpenSteer::Vec3();
}

OpenSteer::Vec3 VehicleObject::globalizeDirection(const OpenSteer::Vec3& localDirection) const
{
	return OpenSteer::Vec3();
}

void VehicleObject::regenerateOrthonormalBasisUF(const OpenSteer::Vec3& newUnitForward)
{
	(void)newUnitForward;
}

void VehicleObject::regenerateOrthonormalBasis(const OpenSteer::Vec3& newForward)
{
	(void)newForward;
}

void VehicleObject::regenerateOrthonormalBasis(const OpenSteer::Vec3& newForward, const OpenSteer::Vec3& newUp)
{
	(void)newForward;
	(void)newUp;
}

OpenSteer::Vec3 VehicleObject::localRotateForwardToSide(const OpenSteer::Vec3& v) const
{
	return OpenSteer::Vec3();
}

OpenSteer::Vec3 VehicleObject::globalRotateForwardToSide(const OpenSteer::Vec3& globalForward) const
{
	return OpenSteer::Vec3();
}
