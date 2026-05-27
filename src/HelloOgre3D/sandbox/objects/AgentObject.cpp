#include "AgentObject.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"
#include "btBulletDynamicsCommon.h"
#include "GameManager.h"
#include "BlockObject.h"
#include "RenderableObject.h"
#include "systems/physics/PhysicsWorld.h"
#include "systems/manager/SandboxMgr.h"
#include "systems/manager/ObjectManager.h"
#include "animation/AgentAnimStateMachine.h"
#include "components/agent/AgentAttrib.h"
#include "components/agent/AgentLocomotion.h"
#include "components/physics/PhysicsComponent.h"
#include "components/script/LuaScriptComponent.h"
#include "event/SandboxEventPayload.h"
#include "objects/steer/AgentPath.h"
#include "systems/physics/Collision.h"
#include "BlockObject.h"
#include <algorithm>

using namespace Ogre;

static std::string g_EmptyStr = "";
static const Ogre::Real DEFAULT_AGENT_HEALTH = 100.0f;


AgentObject::AgentObject(RenderableObject* pAgentBody, btRigidBody* pRigidBody/* = nullptr*/)
	: m_pAgentBody(pAgentBody), m_agentType(AGENT_OBJ_NONE)
{
	m_objType = OBJ_TYPE_AGENT;

	AgentLocomotion* locomotion = new AgentLocomotion();
	if (!AddComponent("locomotion", locomotion))
	{
		SAFE_DELETE(locomotion);
	}

	PhysicsComponent* physics = new PhysicsComponent(pRigidBody);
	if (!AddComponent("physics", physics))
	{
		SAFE_DELETE(physics);
	}

	LuaScriptComponent* script = new LuaScriptComponent();
	if (AddComponent("script", script))
	{
		script->SetLocalEnvOwner(this, LuaClassNameTraits<AgentObject>::value);
	}
	else
	{
		SAFE_DELETE(script);
	}

	this->SetMass(AgentLocomotion::DEFAULT_AGENT_MASS);

	if (m_pAgentBody != nullptr)
	{
		m_pAgentBody->AttachRenderComponent(this);
	}

	SetForward(Ogre::Vector3::UNIT_Z);
}

AgentObject::~AgentObject()
{
	RemoveComponent("attrib");

	SAFE_DELETE(m_pAgentBody);
}

void AgentObject::Init()
{
	m_pAgentBody->InitAsmWithOwner(this, true);
	this->callFunction("Agent_Initialize", "u[AgentObject]", this);
}

void AgentObject::initBody(const Ogre::String& meshFile)
{
	if (m_pAgentBody != nullptr)
	{
		RemoveComponent("render");
		delete m_pAgentBody;
	}
	m_pAgentBody = new RenderableObject(meshFile);
	m_pAgentBody->AttachRenderComponent(this);
	m_pAgentBody->InitAsmWithOwner(this, true);
}

btRigidBody* AgentObject::getRigidBody() const
{
	const PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	return physicsComp != nullptr ? physicsComp->GetRigidBody() : nullptr;
}

void AgentObject::ResetRigidBody(btRigidBody* pRigidBody)
{
	PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr)
		physicsComp->ResetRigidBody(pRigidBody);
}

bool AgentObject::setPluginEnv(lua_State* L)
{
	LuaScriptComponent* script = GetLuaScript();
	return script != nullptr ? script->setPluginEnv(L) : false;
}

bool AgentObject::callFunction(const char* funcname, const char* format, ...)
{
	LuaScriptComponent* script = GetLuaScript();
	if (script == nullptr)
	{
		return false;
	}

	va_list vl;
	va_start(vl, format);
	bool result = script->callFunctionV(funcname, format, vl);
	va_end(vl);
	return result;
}

void AgentObject::SetLuaScriptClassName(const char* className)
{
	LuaScriptComponent* script = GetLuaScript();
	if (script != nullptr)
	{
		script->SetLocalEnvOwner(this, className);
	}
}

Ogre::Vector3 AgentObject::GetPosition() const
{
	const PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr && physicsComp->GetRigidBody() != nullptr)
	{
		return physicsComp->GetPosition();
	}
	else if (m_pAgentBody != nullptr)
	{
		return m_pAgentBody->GetDerivedPosition();
	}

	return Ogre::Vector3::ZERO;
}

Ogre::Quaternion AgentObject::GetOrientation() const
{
	const PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr && physicsComp->GetRigidBody() != nullptr)
	{
		return physicsComp->GetOrientation();
	}
	else if (m_pAgentBody != nullptr)
	{
		return m_pAgentBody->GetDerivedOrientation();
	}

	return Ogre::Quaternion::ZERO;
}

Ogre::Vector3 AgentObject::GetUp() const
{
	const PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr && physicsComp->GetRigidBody() != nullptr)
	{
		return physicsComp->GetUp();
	}
	else if (m_pAgentBody != nullptr)
	{
		return m_pAgentBody->GetOrientation().yAxis();
	}

	return Ogre::Vector3::UNIT_Y;
}

Ogre::Vector3 AgentObject::GetLeft() const
{
	const PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr && physicsComp->GetRigidBody() != nullptr)
	{
		return physicsComp->GetLeft();
	}
	else if (m_pAgentBody != nullptr)
	{
		return m_pAgentBody->GetOrientation().xAxis();
	}

	return Ogre::Vector3::UNIT_X;
}

Ogre::Vector3 AgentObject::GetForward() const
{
	const PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr && physicsComp->GetRigidBody() != nullptr)
	{
		return physicsComp->GetForward();
	}
	else if (m_pAgentBody != nullptr)
	{
		return m_pAgentBody->GetOrientation().zAxis();
	}

	return Ogre::Vector3::UNIT_Z;
}

void AgentObject::SetForward(const Ogre::Vector3& forward)
{
	PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr)
		physicsComp->SetForward(forward);

	this->updateWorldTransform();
}

void AgentObject::SetVelocity(const Ogre::Vector3& velocity)
{
	PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr)
		physicsComp->SetVelocity(velocity);

	SetSpeed(Ogre::Vector3(velocity.x, 0, velocity.z).length());
}

void AgentObject::SetTarget(const Ogre::Vector3& targetPos)
{
	AgentLocomotion* locomotion = GetLocomotion();
	if (locomotion != nullptr)
		locomotion->SetTarget(targetPos);
}

void AgentObject::SetTargetRadius(Ogre::Real radius)
{
	AgentLocomotion* locomotion = GetLocomotion();
	if (locomotion != nullptr)
		locomotion->SetTargetRadius(radius);
}

Ogre::Vector3 AgentObject::GetTarget() const
{
	const AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->GetTarget() : Ogre::Vector3::ZERO;
}

Ogre::Vector3 AgentObject::GetVelocity() const
{
	const PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr)
		return physicsComp->GetVelocity();

	return GetForward() * GetSpeed();
}

void AgentObject::SetMass(const Ogre::Real mass)
{
	const Ogre::Real massValue = std::max(Ogre::Real(0), mass);
	AgentLocomotion* locomotion = GetLocomotion();
	if (locomotion != nullptr)
		locomotion->SetMass(massValue);

	PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr)
		physicsComp->SetMass(massValue);
}

void AgentObject::SetHeight(Ogre::Real height)
{
	const Ogre::Real heightValue = std::max(Ogre::Real(0), height);
	AgentLocomotion* locomotion = GetLocomotion();
	if (locomotion != nullptr)
		locomotion->SetHeight(heightValue);

	PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr)
	{
		Ogre::Real radius = this->GetRadius();
		physicsComp->RebuildCapsule(heightValue, radius);
	}
}

void AgentObject::SetRadius(Ogre::Real radius)
{
	const Ogre::Real radiusValue = std::max(Ogre::Real(0), radius);
	AgentLocomotion* locomotion = GetLocomotion();
	if (locomotion != nullptr)
		locomotion->SetRadius(radiusValue);

	PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr)
	{
		Ogre::Real height = this->GetHeight();
		physicsComp->RebuildCapsule(height, radiusValue);
	}
}

void AgentObject::SetSpeed(Ogre::Real speed)
{
	AgentLocomotion* locomotion = GetLocomotion();
	if (locomotion != nullptr)
		locomotion->SetSpeed(speed);
}

void AgentObject::SetHealth(Ogre::Real health)
{
	AgentAttrib* attrib = FindComponent<AgentAttrib>();
	const Ogre::Real currentHealth = GetHealth();
	if (currentHealth == health || currentHealth <= 0.0f)
		return;

	if (attrib == nullptr)
	{
		AgentAttrib* newAttrib = new AgentAttrib(DEFAULT_AGENT_HEALTH, DEFAULT_AGENT_HEALTH, SOLDIER_STAND, -1);
		if (AddComponent("attrib", newAttrib))
		{
			attrib = newAttrib;
		}
		else
		{
			SAFE_DELETE(newAttrib);
			return;
		}
	}
	attrib->SetHealth(health);

	SandboxContext context = SandboxEventPayload::Make(SandboxEventTypes::HealthChanged(), SandboxEventScope::Local, this);
	SandboxEventPayload::SetPosition(context, GetPosition());
	context.Set_Number("health", GetHealth());
	Event()->Emit("HEALTH_CHANGE", context);
}

Ogre::Real AgentObject::GetMass() const
{
	const AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->GetMass() : AgentLocomotion::DEFAULT_AGENT_MASS;
}

Ogre::Real AgentObject::GetSpeed() const
{
	const AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->GetSpeed() : AgentLocomotion::DEFAULT_AGENT_SPEED;
}

Ogre::Real AgentObject::GetHeight() const
{
	const AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->GetHeight() : AgentLocomotion::DEFAULT_AGENT_HEIGHT;
}

Ogre::Real AgentObject::GetRadius() const
{
	const AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->GetRadius() : AgentLocomotion::DEFAULT_AGENT_RADIUS;
}

Ogre::Real AgentObject::GetHealth() const
{
	const AgentAttrib* attrib = FindComponent<AgentAttrib>();
	return attrib != nullptr ? attrib->GetHealth() : DEFAULT_AGENT_HEALTH;
}

void AgentObject::SetMaxForce(Ogre::Real maxForce)
{
	AgentLocomotion* locomotion = GetLocomotion();
	if (locomotion != nullptr)
		locomotion->SetMaxForce(maxForce);
}

void AgentObject::SetMaxSpeed(Ogre::Real maxSpeed)
{
	AgentLocomotion* locomotion = GetLocomotion();
	if (locomotion != nullptr)
		locomotion->SetMaxSpeed(maxSpeed);
}

Ogre::Real AgentObject::GetMaxForce() const
{
	const AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->GetMaxForce() : 0.0f;
}

Ogre::Real AgentObject::GetMaxSpeed() const
{
	const AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->GetMaxSpeed() : 0.0f;
}

Ogre::Real AgentObject::GetTargetRadius() const
{
	const AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->GetTargetRadius() : 0.0f;
}

Ogre::Vector3 AgentObject::PredictFuturePosition(Ogre::Real predictionTime) const
{
	Ogre::Real upredictionTime = std::max(Ogre::Real(0), predictionTime);
	return GetPosition() + GetVelocity() * upredictionTime;
}

Ogre::Vector3 AgentObject::ForceToPosition(const Ogre::Vector3& position)
{
	AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->ForceToPosition(position) : Ogre::Vector3::ZERO;
}

Ogre::Vector3 AgentObject::ForceToFollowPath(Ogre::Real predictionTime)
{
	AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->ForceToFollowPath(predictionTime) : Ogre::Vector3::ZERO;
}

Ogre::Vector3 AgentObject::ForceToStayOnPath(Ogre::Real predictionTime)
{
	AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->ForceToStayOnPath(predictionTime) : Ogre::Vector3::ZERO;
}

Ogre::Vector3 AgentObject::ForceToWander(Ogre::Real deltaMilliSeconds)
{
	AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->ForceToWander(deltaMilliSeconds) : Ogre::Vector3::ZERO;
}

Ogre::Vector3 AgentObject::ForceToTargetSpeed(Ogre::Real targetSpeed)
{
	AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->ForceToTargetSpeed(targetSpeed) : Ogre::Vector3::ZERO;
}

Ogre::Vector3 AgentObject::ForceToAvoidAgents(Ogre::Real predictionTime)
{
	AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->ForceToAvoidAgents(predictionTime) : Ogre::Vector3::ZERO;
}

Ogre::Vector3 AgentObject::ForceToAvoidObjects(Ogre::Real predictionTime)
{
	AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->ForceToAvoidObjects(predictionTime) : Ogre::Vector3::ZERO;
}

Ogre::Vector3 AgentObject::ForceToCombine(const std::vector<AgentObject*>& agents, Ogre::Real distance, Ogre::Real angle)
{
	AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->ForceToCombine(agents, distance, angle) : Ogre::Vector3::ZERO;
}

Ogre::Vector3 AgentObject::ForceToSeparate(const std::vector<AgentObject*>& agents, Ogre::Real distance, Ogre::Real angle)
{
	AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->ForceToSeparate(agents, distance, angle) : Ogre::Vector3::ZERO;
}

void AgentObject::ApplyForce(const Ogre::Vector3& force)
{
	PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr)
		physicsComp->ApplyForce(force);
}

void AgentObject::SetPath(const std::vector<Ogre::Vector3>& points, bool cyclic)
{
	AgentLocomotion* locomotion = GetLocomotion();
	if (locomotion != nullptr)
		locomotion->SetPath(points, cyclic);
}

const std::vector<Ogre::Vector3>& AgentObject::GetPath()
{
	static const std::vector<Ogre::Vector3> kEmpty;
	AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->GetPath() : kEmpty;
}

bool AgentObject::HasPath() const
{
	const AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->HasPath() : false;
}

Ogre::Real AgentObject::GetDistanceAlongPath(const Ogre::Vector3& position) const
{
	const AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->GetDistanceAlongPath(position) : 0.0f;
}

Ogre::Vector3 AgentObject::GetNearestPointOnPath(const Ogre::Vector3& position) const
{
	const AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->GetNearestPointOnPath(position) : Ogre::Vector3::ZERO;
}

Ogre::Vector3 AgentObject::GetPointOnPath(const Ogre::Real distance) const
{
	const AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->GetPointOnPath(distance) : Ogre::Vector3::ZERO;
}

void AgentObject::Update(int deltaMilisec)
{
	// Sync physics transform before stepping animations so bone attachments
	// use the latest world transform in the current frame.
	this->updateWorldTransform();

	static int totalMilisec = 0;
	totalMilisec += deltaMilisec;
	if (true || totalMilisec > 1000)
	{
		totalMilisec = 0;
		this->callFunction("Agent_Update", "u[AgentObject]i", this, deltaMilisec);
	}

	m_pAgentBody->Update(deltaMilisec);
}

void AgentObject::updateWorldTransform()
{
	if (m_pAgentBody != nullptr)
	{
		m_pAgentBody->SyncWorldTransform();
	}
}

void AgentObject::HandleKeyEvent(OIS::KeyCode keycode, unsigned int key)
{
	callFunction("Agent_EventHandle", "u[AgentObject]i", this, keycode);
}

bool AgentObject::IsMoving()
{
	Ogre::Real velocity = GetVelocity().squaredLength();
	return velocity > 2.0f;
}

bool AgentObject::IsFalling()
{
	return GetVelocity().y < (-9.8f * 0.5f);
}

bool AgentObject::OnGround()
{
	return GetVelocity().y > (-9.8f * 0.1f);
}

int AgentObject::GetCurStateId()
{
	auto pAsm = m_pAgentBody->GetObjectASM();
	if (pAsm) return pAsm->GetCurrStateID();
	return -1;
}

std::string AgentObject::GetCurStateName()
{
	auto pAsm = m_pAgentBody->GetObjectASM();
	if (pAsm) return pAsm->GetCurrStateName();
	
	return g_EmptyStr;
}

void AgentObject::OnDeath(float lastSec)
{
	if (m_onPlayDeathAnim) return;

	this->SetNeedClear(int(lastSec * 20));

	m_onPlayDeathAnim = true;
}

void AgentObject::SlowMoving(float rate /*= 1.0f*/)
{
	Ogre::Vector3 horizontalVelocity = this->GetVelocity();
	Ogre::Real yMovement = horizontalVelocity.y;

	horizontalVelocity.y = 0;
	horizontalVelocity = horizontalVelocity * 0.91f * (1 / rate);
	horizontalVelocity.y = yMovement;

	this->SetVelocity(horizontalVelocity);
}

void AgentObject::CollideWithObject(BaseObject* pCollideObj, const Collision& collision)
{
	(void)collision;
	if (pCollideObj == nullptr || pCollideObj->GetObjType() != OBJ_TYPE_BULLET)
	{
		return;
	}

	BlockObject* pBullet = dynamic_cast<BlockObject*>(pCollideObj);
	if (pBullet == nullptr)
	{
		return;
	}

	if (pBullet->GetOwner() == this || GetHealth() <= 0.0f)
	{
		return;
	}

	pBullet->SetNeedClear();
	BlockObject::SpawnBulletImpact(collision);
	const Ogre::Real damage = 5.0f;
	this->SetHealth(std::max<Ogre::Real>(0.0f, GetHealth() - damage));
}

void AgentObject::setPosition(const Ogre::Vector3& position)
{
	PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr)
		physicsComp->SetPosition(position);

	this->updateWorldTransform();
	// Physics-driven agents must keep render transform sourced from rigid body.
	// Otherwise we can momentarily overwrite origin-offset corrections (e.g. crouch/stand).
	if (m_pAgentBody && getRigidBody() == nullptr)
		m_pAgentBody->SetPosition(position);
}

void AgentObject::setRotation(const Ogre::Vector3& rotation)
{
	PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr)
		physicsComp->SetRotation(rotation);

	this->updateWorldTransform();
}

void AgentObject::setOrientation(const Ogre::Quaternion& quaternion)
{
	PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr)
		physicsComp->SetOrientation(quaternion);

	this->updateWorldTransform();
	if (m_pAgentBody && getRigidBody() == nullptr)
		m_pAgentBody->SetOrientation(quaternion);
}

void AgentObject::SetPath(const AgentPath& agentPath)
{
	std::vector<Ogre::Vector3> points;
	agentPath.GetPathPoints(points);

	const bool cyclic = false;
	AgentLocomotion* locomotion = GetLocomotion();
	if (locomotion != nullptr)
		locomotion->SetPath(points, cyclic);
}

AgentLocomotion* AgentObject::GetLocomotion()
{
	return FindComponent<AgentLocomotion>();
}

const AgentLocomotion* AgentObject::GetLocomotion() const
{
	return FindComponent<AgentLocomotion>();
}

LuaScriptComponent* AgentObject::GetLuaScript()
{
	return FindComponent<LuaScriptComponent>();
}

const LuaScriptComponent* AgentObject::GetLuaScript() const
{
	return FindComponent<LuaScriptComponent>();
}

OpenSteerAdapter* AgentObject::GetAdapter() const
{
	const AgentLocomotion* locomotion = GetLocomotion();
	return locomotion != nullptr ? locomotion->GetAdapter() : nullptr;
}

#define USE_CPP_FSM 1
bool AgentObject::GetUseCppFSM()
{
	/*
#ifdef USE_CPP_FSM
	return true;
#else
	return false;
#endif // USE_CPP_FSM
	*/
	return g_SandboxMgr->GetUseCppFsmFlag();
}
