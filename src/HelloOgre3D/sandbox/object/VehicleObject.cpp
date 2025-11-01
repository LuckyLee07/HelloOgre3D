#include "VehicleObject.h"
#include "btBulletDynamicsCommon.h"
#include "GameManager.h"
#include "ScriptLuaVM.h"
#include "BlockObject.h"
#include "AgentObject.h"
#include "GameFunction.h"
#include "play/PhysicsWorld.h"
#include "manager/SandboxMgr.h"
#include "manager/ObjectManager.h"
#include "compents/AgentLocomotion.h"

using namespace Ogre;

const float VehicleObject::DEFAULT_AGENT_MASS = 90.7f;	// kilograms (200 lbs)
const float VehicleObject::DEFAULT_AGENT_HEIGHT = 1.6f;	// meters (5.2 feet)
const float VehicleObject::DEFAULT_AGENT_RADIUS = 0.3f;	// meters (1.97 feet)
const float VehicleObject::DEFAULT_AGENT_SPEED = 0.0f;			// m/s (0 ft/s)
const float VehicleObject::DEFAULT_AGENT_HEALTH = 100.0f;			// default

VehicleObject::VehicleObject(btRigidBody* pRigidBody)
	: m_pRigidBody(pRigidBody), 
	m_mass(DEFAULT_AGENT_MASS), 
	m_height(DEFAULT_AGENT_HEIGHT), 
	m_radius(DEFAULT_AGENT_RADIUS),
	m_speed(DEFAULT_AGENT_SPEED),
	m_health(DEFAULT_AGENT_HEALTH)
{
	if (m_pRigidBody)
	{
		this->SetMass(DEFAULT_AGENT_MASS);
		m_pRigidBody->setUserPointer(this);
	}
	m_locomotion = new AgentLocomotion(this);
}

VehicleObject::~VehicleObject()
{
	this->DeleteRighdBody();
	SAFE_DELETE(m_locomotion);
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
	const Ogre::Vector3 position = GetPosition();
	const Ogre::Quaternion rot = GetOrientation();

	this->DeleteRighdBody();

	m_pRigidBody = pRigidBody;
	this->SetMass(DEFAULT_AGENT_MASS);
	m_pRigidBody->setUserPointer(this);

	//this->SetMass(this->GetMass());
	this->setPosition(position);
	this->setOrientation(rot);

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
	if (m_locomotion)
		m_locomotion->SetTarget(targetPos);
}

void VehicleObject::SetTargetRadius(Ogre::Real radius)
{
	if (m_locomotion)
		m_locomotion->SetTargetRadius(radius);
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
	return m_locomotion ? m_locomotion->GetTarget() : Ogre::Vector3::ZERO;
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
	if (m_locomotion)
		m_locomotion->SetMaxForce(maxForce);
}

void VehicleObject::SetMaxSpeed(Ogre::Real maxSpeed)
{
	if (m_locomotion)
		m_locomotion->SetMaxSpeed(maxSpeed);
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

Ogre::Real VehicleObject::GetMaxForce() const
{ 
	return m_locomotion ? m_locomotion->GetMaxForce() : 0.0f;
}

Ogre::Real VehicleObject::GetMaxSpeed() const
{ 
	return m_locomotion ? m_locomotion->GetMaxSpeed() : 0.0f; 
}

Ogre::Real VehicleObject::GetTargetRadius() const
{ 
	return m_locomotion ? m_locomotion->GetTargetRadius() : 0.0f;
}

Ogre::Vector3 VehicleObject::PredictFuturePosition(Ogre::Real predictionTime) const
{
	Ogre::Real upredictionTime = std::max(Ogre::Real(0), predictionTime);
	return GetPosition() + GetVelocity() * upredictionTime;
}

Ogre::Vector3 VehicleObject::ForceToPosition(const Ogre::Vector3& position)
{
	return m_locomotion ? m_locomotion->ForceToPosition(position) : Ogre::Vector3::ZERO;
}

Ogre::Vector3 VehicleObject::ForceToFollowPath(Ogre::Real predictionTime)
{
	return m_locomotion ? m_locomotion->ForceToFollowPath(predictionTime) : Ogre::Vector3::ZERO;
}

Ogre::Vector3 VehicleObject::ForceToStayOnPath(Ogre::Real predictionTime)
{
	return m_locomotion ? m_locomotion->ForceToStayOnPath(predictionTime) : Ogre::Vector3::ZERO;
}

Ogre::Vector3 VehicleObject::ForceToWander(Ogre::Real deltaMilliSeconds)
{
	return m_locomotion ? m_locomotion->ForceToWander(deltaMilliSeconds) : Ogre::Vector3::ZERO;
}

Ogre::Vector3 VehicleObject::ForceToTargetSpeed(Ogre::Real targetSpeed)
{
	return m_locomotion ? m_locomotion->ForceToTargetSpeed(targetSpeed) : Ogre::Vector3::ZERO;
}

Ogre::Vector3 VehicleObject::ForceToCombine(const std::vector<AgentObject*>& agents, Ogre::Real distance, Ogre::Real angle)
{
	return m_locomotion ? m_locomotion->ForceToCombine(agents, distance, angle) : Ogre::Vector3::ZERO;
}

Ogre::Vector3 VehicleObject::ForceToSeparate(const std::vector<AgentObject*>& agents, Ogre::Real distance, Ogre::Real angle)
{
	return m_locomotion ? m_locomotion->ForceToSeparate(agents, distance, angle) : Ogre::Vector3::ZERO;
}

Ogre::Vector3 VehicleObject::ForceToAvoidAgents(Ogre::Real predictionTime)
{
	return m_locomotion ? m_locomotion->ForceToAvoidAgents(predictionTime) : Ogre::Vector3::ZERO;
}

Ogre::Vector3 VehicleObject::ForceToAvoidObjects(Ogre::Real predictionTime)
{
	return m_locomotion ? m_locomotion->ForceToAvoidObjects(predictionTime) : Ogre::Vector3::ZERO;
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
	if (m_locomotion) m_locomotion->SetPath(points, cyclic);
}

const std::vector<Ogre::Vector3>& VehicleObject::GetPath()
{
	static const std::vector<Ogre::Vector3> kEmpty;
	return m_locomotion ? m_locomotion->GetPath() : kEmpty;
}

bool VehicleObject::HasPath() const
{
	return m_locomotion ? m_locomotion->HasPath() : false;
}

Ogre::Real VehicleObject::GetDistanceAlongPath(const Ogre::Vector3& position) const
{
	return m_locomotion ? m_locomotion->GetDistanceAlongPath(position) : 0.0f;
}

Ogre::Vector3 VehicleObject::GetNearestPointOnPath(const Ogre::Vector3& position) const
{
	return m_locomotion ? m_locomotion->GetNearestPointOnPath(position) : Ogre::Vector3::ZERO;
}

Ogre::Vector3 VehicleObject::GetPointOnPath(const Ogre::Real distance) const
{
	return m_locomotion ? m_locomotion->GetPointOnPath(distance) : Ogre::Vector3::ZERO;
}

void VehicleObject::SetPath(const AgentPath& agentPath)
{
	// 不再保存成员 m_path/m_hasPath，仅把点同步给 Locomotion
	std::vector<Ogre::Vector3> points;
	agentPath.GetPathPoints(points);

	// AgentPath 目前没有 IsCyclic 访问器，这里先按需要设置是否循环
	const bool cyclic = false; // TODO: 若后续 AgentPath 增加 IsCyclic() 则改为读取真实值
	if (m_locomotion) m_locomotion->SetPath(points, cyclic);
}

OpenSteerAdapter* VehicleObject::GetAdapter() const 
{
	return m_locomotion ? m_locomotion->GetAdapter() : nullptr;
}

void VehicleObject::update(int deltaMilisec)
{
	this->updateWorldTransform();
}

void VehicleObject::updateWorldTransform()
{

}
