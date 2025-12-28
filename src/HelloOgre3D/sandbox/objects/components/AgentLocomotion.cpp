#include "AgentLocomotion.h"
#include "ai/steer/OpenSteerAdapter.h"
#include "SandboxMacros.h"
#include "GameFunction.h"
#include "objects/VehicleObject.h"
#include "objects/AgentObject.h"
#include "objects/BlockObject.h"
#include "ai/steer/AgentPath.h"
#include "systems/manager/ObjectManager.h"

const float AgentLocomotion::DEFAULT_AGENT_MAX_FORCE = 1000.0f;		// newtons (kg*m/s^2)
const float AgentLocomotion::DEFAULT_AGENT_MAX_SPEED = 7.0f;		// m/s (23.0 ft/s)
const float AgentLocomotion::DEFAULT_AGENT_TARGET_RADIUS = 0.5f;	// meters (1.64 feet)

AgentLocomotion::AgentLocomotion() 
	: m_owner(nullptr),
	m_maxForce(DEFAULT_AGENT_MAX_FORCE),
	m_maxSpeed(DEFAULT_AGENT_MAX_SPEED),
	m_targetRadius(DEFAULT_AGENT_TARGET_RADIUS)
{
	m_adapter = new OpenSteerAdapter(this);
}

AgentLocomotion::~AgentLocomotion()
{
	SAFE_DELETE(m_adapter);
}

void AgentLocomotion::onAttach(GameObject* owner)
{
	IComponent::onAttach(owner);

	auto* pObject = getOwner();
	assert(pObject != nullptr);
	m_owner = dynamic_cast<VehicleObject*>(pObject);
}

void AgentLocomotion::onDetach()
{
	m_owner = nullptr;
	IComponent::onDetach();
}

// --- 参数/状态：暂转发 ---
void AgentLocomotion::SetPosition(const Ogre::Vector3& position)
{
	m_owner->setPosition(position);
}

Ogre::Vector3 AgentLocomotion::GetPosition() const
{
	return m_owner->GetPosition();
}

void AgentLocomotion::SetMass(Ogre::Real mass)
{
	m_owner->SetMass(mass);
}

void AgentLocomotion::SetHeight(Ogre::Real height)
{
	m_owner->SetHeight(height);
}

void AgentLocomotion::SetRadius(Ogre::Real radius)
{
	m_owner->SetRadius(radius);
}

void AgentLocomotion::SetSpeed(Ogre::Real speed)
{
	m_owner->SetSpeed(speed);
}

void AgentLocomotion::SetHealth(Ogre::Real health)
{
	m_owner->SetHealth(health);
}

// 内聚：MaxForce/MaxSpeed
void AgentLocomotion::SetMaxForce(Ogre::Real maxForce)
{
	m_maxForce = std::max(Ogre::Real(0), maxForce);
}

void AgentLocomotion::SetMaxSpeed(Ogre::Real maxSpeed)
{
	m_maxSpeed = std::max(Ogre::Real(0), maxSpeed);
}

Ogre::Real AgentLocomotion::GetMass() const
{
	return m_owner->GetMass();
}

Ogre::Real AgentLocomotion::GetSpeed() const
{
	return m_owner->GetSpeed();
}

Ogre::Real AgentLocomotion::GetHeight() const
{
	return m_owner->GetHeight();
}

Ogre::Real AgentLocomotion::GetRadius() const
{
	return m_owner->GetRadius();
}

Ogre::Real AgentLocomotion::GetHealth() const
{
	return m_owner->GetHealth();
}

Ogre::Real AgentLocomotion::GetMaxForce() const
{
	return m_maxForce;
}

Ogre::Real AgentLocomotion::GetMaxSpeed() const
{
	return m_maxSpeed;
}

// --- 姿态/速度：转发 ---
void AgentLocomotion::SetForward(const Ogre::Vector3& forward)
{
	m_owner->SetForward(forward);
}

void AgentLocomotion::SetVelocity(const Ogre::Vector3& velocity)
{
	m_owner->SetVelocity(velocity);
}

void AgentLocomotion::SetTarget(const Ogre::Vector3& targetPos)
{
	m_targetPos = targetPos;
}

void AgentLocomotion::SetTargetRadius(Ogre::Real radius)
{
	m_targetRadius = std::max(Ogre::Real(0.0f), radius);
}

Ogre::Real AgentLocomotion::GetTargetRadius() const
{
	return m_targetRadius;
}

Ogre::Vector3 AgentLocomotion::GetTarget() const
{
	return m_targetPos;
}

Ogre::Vector3 AgentLocomotion::GetVelocity() const
{
	return m_owner->GetVelocity();
}

Ogre::Vector3 AgentLocomotion::GetUp() const
{
	return m_owner->GetUp();
}

Ogre::Vector3 AgentLocomotion::GetLeft() const
{
	return m_owner->GetLeft();
}

Ogre::Vector3 AgentLocomotion::GetForward() const
{
	return m_owner->GetForward();
}

void AgentLocomotion::SetPath(const std::vector<Ogre::Vector3>& points, bool cyclic)
{
	m_points = points;
	m_pathCyclic = cyclic;
}

// 距离/采样（仍转发；可后续迁移）
Ogre::Real AgentLocomotion::GetDistanceAlongPath(const Ogre::Vector3& position) const
{
	if (!m_points.empty())
	{
		AgentPath local(m_points, m_owner->GetRadius(), m_pathCyclic);
		return local.GetDistanceAlongPath(position);
	}
	return 0.0f;
}

Ogre::Vector3 AgentLocomotion::GetNearestPointOnPath(const Ogre::Vector3& position) const
{
	if (!m_points.empty())
	{
		AgentPath local(m_points, m_owner->GetRadius(), m_pathCyclic);
		return local.GetNearestPointOnPath(position);
	}
	return Ogre::Vector3::ZERO;
}

Ogre::Vector3 AgentLocomotion::GetPointOnPath(const Ogre::Real distance) const
{
	if (!m_points.empty())
	{
		AgentPath local(m_points, m_owner->GetRadius(), m_pathCyclic);
		return local.GetPointOnPath(distance);
	}
	return Ogre::Vector3::ZERO;
}

// Steering forces
Ogre::Vector3 AgentLocomotion::ForceToPosition(const Ogre::Vector3& position)
{
	if (!m_adapter) return Ogre::Vector3::ZERO;
	const OpenSteer::Vec3 steer = m_adapter->steerForSeek(Vector3ToVec3(position));
	return Vec3ToVector3(steer);
}

Ogre::Vector3 AgentLocomotion::ForceToFollowPath(Ogre::Real predictionTime)
{
	if (!m_adapter) return Ogre::Vector3::ZERO;
	// 用当前路径点构建 Pathway 并调用 OpenSteer 行为
	AgentPath local(m_points, m_owner->GetRadius(), m_pathCyclic);
	const int FORWARD = 1; // 沿路径前进
	const float pt = std::max(0.1f, static_cast<float>(predictionTime)); // 秒

	const OpenSteer::Vec3 steer = m_adapter->steerToFollowPath(FORWARD, pt, local);

	return Vec3ToVector3(steer);
}

Ogre::Vector3 AgentLocomotion::ForceToStayOnPath(Ogre::Real predictionTime)
{
	if (!m_adapter) return Ogre::Vector3::ZERO;

	const float pt = std::max(0.1f, static_cast<float>(predictionTime)); // 秒

	AgentPath local(m_points, m_owner->GetRadius(), m_pathCyclic);
	const OpenSteer::Vec3 steer = m_adapter->steerToStayOnPath(pt, local);

	return Vec3ToVector3(steer);
}

Ogre::Vector3 AgentLocomotion::ForceToWander(Ogre::Real deltaMilliSeconds)
{
	if (!m_adapter) return Ogre::Vector3::ZERO;
	const float dt = static_cast<float>(deltaMilliSeconds);// / 1000.0f;
	const OpenSteer::Vec3 steer = m_adapter->steerForWander(dt);
	return Vec3ToVector3(steer);
}

Ogre::Vector3 AgentLocomotion::ForceToTargetSpeed(Ogre::Real targetSpeed)
{
	if (!m_adapter) return Ogre::Vector3::ZERO;
	const OpenSteer::Vec3 steer = m_adapter->steerForTargetSpeed(targetSpeed);
	return Vec3ToVector3(steer);
}

// 规避之力 智能体或其他障碍物
Ogre::Vector3 AgentLocomotion::ForceToAvoidAgents(Ogre::Real predictionTime)
{
	if (!m_adapter) return Ogre::Vector3::ZERO;

	// 收集有效 Agent(排除自己、生命 > 0)
	const auto& agents = g_ObjectManager->getAllAgents();

	OpenSteer::AVGroup group;
	group.reserve(agents.size());
	for (auto* a : agents)
	{
		if (!a || a == m_owner) continue; // 排除自己
		if (a->GetHealth() <= 0) continue;
		auto* veh = a->GetAdapter();
		if (veh) group.push_back(veh);
	}

	const float pt = std::max(0.1f, static_cast<float>(predictionTime));
	const OpenSteer::Vec3 steer = m_adapter->steerToAvoidNeighbors(pt, group);
	return Vec3ToVector3(steer);
}

Ogre::Vector3 AgentLocomotion::ForceToAvoidObjects(Ogre::Real predictionTime)
{
	if (!m_adapter) return Ogre::Vector3::ZERO;

	float timeToCollision = std::max(0.1f, static_cast<float>(predictionTime));

	const auto& blocks = g_ObjectManager->getAllBlocks();
	OpenSteer::Vec3 totalAvoidForce = OpenSteer::Vec3::zero;
	for (auto* b : blocks)
	{
		if (!b || b->GetMass() <= 0) continue;
		totalAvoidForce += b->steerToAvoid(*m_adapter, timeToCollision);
	}
	return Vec3ToVector3(totalAvoidForce);
}

Ogre::Vector3 AgentLocomotion::ForceToCombine(const std::vector<AgentObject*>& agents, Ogre::Real distance, Ogre::Real angle)
{
	if (!m_adapter) return Ogre::Vector3::ZERO;

	OpenSteer::AVGroup group;
	group.reserve(agents.size());
	for (auto* a : agents)
	{
		if (a == nullptr || a == m_owner) // 排除自己
			continue;
		auto* veh = a->GetAdapter();
		if (veh) group.push_back(veh);
	}

	const float maxCos = cosf(Ogre::Degree(angle).valueRadians());
	const OpenSteer::Vec3 steer = m_adapter->steerForCohesion(static_cast<float>(distance), maxCos, group);
	return Vec3ToVector3(steer);
}

Ogre::Vector3 AgentLocomotion::ForceToSeparate(const std::vector<AgentObject*>& agents, Ogre::Real distance, Ogre::Real angle)
{
	if (!m_adapter) return Ogre::Vector3::ZERO;

	OpenSteer::AVGroup group;
	group.reserve(agents.size());
	for (auto* a : agents)
	{
		if (a == nullptr || a == m_owner) // 排除自己
			continue;
		auto* veh = a->GetAdapter();
		if (!veh) continue;
		group.push_back(veh);
	}

	const float maxCos = cosf(Ogre::Degree(angle).valueRadians());
	const OpenSteer::Vec3 steer = m_adapter->steerForSeparation(static_cast<float>(distance), maxCos, group);
	return Vec3ToVector3(steer);
}

void AgentLocomotion::ApplyForce(const Ogre::Vector3& force)
{
	return m_owner->ApplyForce(force);
}