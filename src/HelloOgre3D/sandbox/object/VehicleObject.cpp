#include "VehicleObject.h"
#include "GameManager.h"
#include "ScriptLuaVM.h"
#include "BlockObject.h"
#include "AgentObject.h"
#include "GameFunction.h"
#include "play/PhysicsWorld.h"
#include "manager/SandboxMgr.h"
#include "manager/ObjectManager.h"
#include "components/AgentLocomotion.h"
#include "components/PhysicsComponent.h"
#include "object/GameObject.h"

using namespace Ogre;

const float VehicleObject::DEFAULT_AGENT_MASS = 90.7f;	// kilograms (200 lbs)
const float VehicleObject::DEFAULT_AGENT_HEIGHT = 1.6f;	// meters (5.2 feet)
const float VehicleObject::DEFAULT_AGENT_RADIUS = 0.3f;	// meters (1.97 feet)
const float VehicleObject::DEFAULT_AGENT_SPEED = 0.0f;			// m/s (0 ft/s)
const float VehicleObject::DEFAULT_AGENT_HEALTH = 100.0f;			// default

VehicleObject::VehicleObject(btRigidBody* pRigidBody)
	: m_mass(DEFAULT_AGENT_MASS), 
	m_height(DEFAULT_AGENT_HEIGHT), 
	m_radius(DEFAULT_AGENT_RADIUS),
	m_speed(DEFAULT_AGENT_SPEED),
	m_health(DEFAULT_AGENT_HEALTH)
{
	m_locomotion = new AgentLocomotion();
	m_pGameObjet->addComponent("locomotion", m_locomotion);

	m_physicsComp = new PhysicsComponent(pRigidBody);
	m_pGameObjet->addComponent("physics", m_physicsComp);

	this->SetMass(DEFAULT_AGENT_MASS);
}

VehicleObject::~VehicleObject()
{
	//SAFE_DELETE(m_locomotion);
	//SAFE_DELETE(m_physicsComp);
}

void VehicleObject::Init()
{
}

btRigidBody* VehicleObject::getRigidBody() const
{
	if (m_physicsComp)
		return m_physicsComp->GetRigidBody();

	return nullptr;
}

void VehicleObject::ResetRigidBody(btRigidBody* pRigidBody)
{
	if (m_physicsComp)
		m_physicsComp->ResetRigidBody(pRigidBody);
}

void VehicleObject::setPosition(const Ogre::Vector3& position)
{
	if (m_physicsComp)
		m_physicsComp->SetPosition(position);

	this->updateWorldTransform();
}

void VehicleObject::setOrientation(const Ogre::Quaternion& quaternion)
{
	if (m_physicsComp)
		m_physicsComp->SetOrientation(quaternion);

	this->updateWorldTransform();
}

void VehicleObject::setRotation(const Ogre::Vector3& rotation)
{
	if (m_physicsComp)
		m_physicsComp->SetRotation(rotation);

	this->updateWorldTransform();
}

Ogre::Vector3 VehicleObject::GetPosition() const
{
	if (m_physicsComp)
		return m_physicsComp->GetPosition();

	return Ogre::Vector3::ZERO;
}

Ogre::Quaternion VehicleObject::GetOrientation() const
{
	if (m_physicsComp)
		return m_physicsComp->GetOrientation();

	return Ogre::Quaternion::ZERO;
}

void VehicleObject::SetForward(const Ogre::Vector3& forward)
{
	if (m_physicsComp)
		m_physicsComp->SetForward(forward);

	this->updateWorldTransform();
}

void VehicleObject::SetVelocity(const Ogre::Vector3& velocity)
{
	if (m_physicsComp)
		m_physicsComp->SetVelocity(velocity);
	
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
	if (m_physicsComp)
		return m_physicsComp->GetUp();

	return Ogre::Vector3::UNIT_Y;
}

Ogre::Vector3 VehicleObject::GetLeft() const
{
	if (m_physicsComp)
		return m_physicsComp->GetLeft();

	return Ogre::Vector3::UNIT_X;
}

Ogre::Vector3 VehicleObject::GetForward() const
{
	if (m_physicsComp)
		return m_physicsComp->GetForward();

	return Ogre::Vector3::UNIT_Z;
}

Ogre::Vector3 VehicleObject::GetTarget() const
{
	return m_locomotion ? m_locomotion->GetTarget() : Ogre::Vector3::ZERO;
}

Ogre::Vector3 VehicleObject::GetVelocity() const
{
	if (m_physicsComp)
		return m_physicsComp->GetVelocity();
	
	return GetForward() * m_speed;
}

void VehicleObject::SetMass(const Ogre::Real mass)
{
	m_mass = std::max(Ogre::Real(0), mass);

	if (m_physicsComp)
		m_physicsComp->SetMass(mass);
}

void VehicleObject::SetHeight(Ogre::Real height)
{
	m_height = std::max(Ogre::Real(0), height);
	
	if (m_physicsComp)
	{
		Ogre::Real radius = this->GetRadius();
		m_physicsComp->RebuildCapsule(height, radius);
	}
}

void VehicleObject::SetRadius(Ogre::Real radius)
{
	m_radius = std::max(Ogre::Real(0), radius);

	if (m_physicsComp)
	{
		Ogre::Real height = this->GetHeight();
		m_physicsComp->RebuildCapsule(height, radius);
	}
}

void VehicleObject::SetSpeed(Ogre::Real speed)
{
	m_speed = speed;
}

void VehicleObject::SetHealth(Ogre::Real health)
{
	if (m_health == health || m_health <= 0.0f) 
		return;

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
	return m_mass;
}

Ogre::Real VehicleObject::GetSpeed() const
{
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
	if (m_physicsComp)
		m_physicsComp->ApplyForce(force);
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

void VehicleObject::Update(int deltaMs)
{
	this->updateWorldTransform();
}

void VehicleObject::updateWorldTransform()
{

}
