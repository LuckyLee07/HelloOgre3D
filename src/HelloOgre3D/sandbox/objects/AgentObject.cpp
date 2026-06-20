#include "AgentObject.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"
#include "btBulletDynamicsCommon.h"
#include "BlockObject.h"
#include "ai/common/AICommand.h"
#include "ai/tactics/TacticalService.h"
#include "core/SandboxServices.h"
#include "systems/service/AgentConfigService.h"
#include "systems/physics/PhysicsWorld.h"
#include "systems/manager/ObjectManager.h"
#include "animation/AgentAnimStateMachine.h"
#include "components/agent/AgentAttrib.h"
#include "components/agent/AgentLocomotion.h"
#include "components/anim/AnimComponent.h"
#include "components/ComponentKeys.h"
#include "components/physics/PhysicsComponent.h"
#include "components/render/RenderComponent.h"
#include "components/script/LuaScriptComponent.h"
#include "event/SandboxEventPayload.h"
#include "systems/physics/Collision.h"
#include "LogSystem.h"
#include <algorithm>

using namespace Ogre;

static std::string g_EmptyStr = "";
static const Ogre::Real DEFAULT_AGENT_HEALTH = 100.0f;


AgentObject::AgentObject(RenderComponent* renderComp, btRigidBody* pRigidBody/* = nullptr*/)
	: m_renderComp(renderComp), m_agentType(AGENT_OBJ_NONE)
{
	m_objType = OBJ_TYPE_AGENT;

	AgentLocomotion* locomotion = new AgentLocomotion();
	if (!AddComponent(ComponentKeys::Locomotion, locomotion))
	{
		SAFE_DELETE(locomotion);
	}

	PhysicsComponent* physics = new PhysicsComponent(pRigidBody);
	if (!AddComponent(ComponentKeys::Physics, physics))
	{
		SAFE_DELETE(physics);
	}

	LuaScriptComponent* script = new LuaScriptComponent();
	if (AddComponent(ComponentKeys::Script, script))
	{
		script->SetLocalEnvOwner(this, LuaClassNameTraits<AgentObject>::value);
	}
	else
	{
		SAFE_DELETE(script);
	}

	this->SetMass(AgentLocomotion::DEFAULT_AGENT_MASS);

	if (m_renderComp != nullptr)
	{
		if (!AddComponent(ComponentKeys::Render, m_renderComp))
		{
			SAFE_DELETE(m_renderComp);
		}
	}

	SetForward(Ogre::Vector3::UNIT_Z);
}

AgentObject::~AgentObject()
{
	RemoveComponent(ComponentKeys::Attrib);
	m_renderComp = nullptr;
}

void AgentObject::Init()
{
	this->callFunction("Agent_Initialize", "u[AgentObject]", this);
}

void AgentObject::initBody(const Ogre::String& meshFile)
{
	if (m_renderComp != nullptr)
	{
		RemoveComponent(ComponentKeys::Render);
		m_renderComp = nullptr;
	}
	RenderComponent* renderComp = new RenderComponent(meshFile);
	if (AddComponent(ComponentKeys::Render, renderComp))
	{
		m_renderComp = renderComp;
		AnimComponent* anim = FindComponent<AnimComponent>();
		if (anim != nullptr)
		{
			anim->InitBodyAnimations(m_renderComp->GetEntity(), true);
		}
	}
	else
	{
		SAFE_DELETE(renderComp);
	}
}

AgentAnim* AgentObject::GetAnimation(const char* animationName)
{
	AnimComponent* anim = FindComponent<AnimComponent>();
	return anim != nullptr ? anim->GetBodyAnimation(animationName) : nullptr;
}

AgentAnimStateMachine* AgentObject::GetObjectASM() const
{
	const AnimComponent* anim = FindComponent<AnimComponent>();
	return anim != nullptr ? anim->GetBodyAsm() : nullptr;
}

void AgentObject::SetRenderVisible(bool visible)
{
	if (m_renderComp != nullptr)
	{
		m_renderComp->SetVisible(visible);
	}
}

void AgentObject::ApplyCommand(const AICommand& command)
{
	switch (command.kind)
	{
	case AICommand::COMMAND_NONE:
		break;
	case AICommand::COMMAND_MOVE_TO:
		if (AgentLocomotion* locomotion = FindComponent<AgentLocomotion>())
			locomotion->SetTarget(command.targetPosition);
		break;
	case AICommand::COMMAND_STOP:
		SetVelocity(Ogre::Vector3::ZERO);
		if (AgentLocomotion* locomotion = FindComponent<AgentLocomotion>())
			locomotion->SetTarget(GetPosition());
		break;
	default:
		CCLOG_INFO("AgentObject::ApplyCommand unsupported command: %s", AICommand::KindToString(command.kind));
		break;
	}
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
	else if (m_renderComp != nullptr)
	{
		return m_renderComp->GetDerivedPosition();
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
	else if (m_renderComp != nullptr)
	{
		return m_renderComp->GetDerivedOrientation();
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
	else if (m_renderComp != nullptr)
	{
		return m_renderComp->GetOrientation().yAxis();
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
	else if (m_renderComp != nullptr)
	{
		return m_renderComp->GetOrientation().xAxis();
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
	else if (m_renderComp != nullptr)
	{
		return m_renderComp->GetOrientation().zAxis();
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
	AgentLocomotion* locomotion = FindComponent<AgentLocomotion>();
	if (locomotion != nullptr)
		locomotion->SetMass(massValue);

	PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr)
		physicsComp->SetMass(massValue);
}

void AgentObject::SetHeight(Ogre::Real height)
{
	const Ogre::Real heightValue = std::max(Ogre::Real(0), height);
	AgentLocomotion* locomotion = FindComponent<AgentLocomotion>();
	if (locomotion != nullptr)
		locomotion->SetHeight(heightValue);

	PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr)
	{
		const Ogre::Real radius = locomotion != nullptr ? locomotion->GetRadius() : AgentLocomotion::DEFAULT_AGENT_RADIUS;
		physicsComp->RebuildCapsule(heightValue, radius);
	}
}

void AgentObject::SetRadius(Ogre::Real radius)
{
	const Ogre::Real radiusValue = std::max(Ogre::Real(0), radius);
	AgentLocomotion* locomotion = FindComponent<AgentLocomotion>();
	if (locomotion != nullptr)
		locomotion->SetRadius(radiusValue);

	PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr)
	{
		const Ogre::Real height = locomotion != nullptr ? locomotion->GetHeight() : AgentLocomotion::DEFAULT_AGENT_HEIGHT;
		physicsComp->RebuildCapsule(height, radiusValue);
	}
}

void AgentObject::SetSpeed(Ogre::Real speed)
{
	AgentLocomotion* locomotion = FindComponent<AgentLocomotion>();
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
		if (AddComponent(ComponentKeys::Attrib, newAttrib))
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

	if (currentHealth > 0.0f && GetHealth() <= 0.0f)
	{
		const SandboxServices* services = GetSandboxServices();
		ObjectManager* objectManager = services != nullptr ? services->objects : nullptr;
		TacticalService* tactics = objectManager != nullptr ? objectManager->GetTacticalService() : nullptr;
		if (tactics != nullptr)
		{
			tactics->publishTacticalEvent(
				SandboxEventTypes::DeadFriendlySighted(),
				static_cast<int>(GetObjId()),
				static_cast<int>(GetObjId()),
				GetTeamId(),
				GetTeamId(),
				GetPosition(),
				0,
				"global",
				false);
		}
	}
}

Ogre::Real AgentObject::GetSpeed() const
{
	const PhysicsComponent* physicsComp = FindComponent<PhysicsComponent>();
	if (physicsComp != nullptr && physicsComp->GetRigidBody() != nullptr)
	{
		const Ogre::Vector3 velocity = physicsComp->GetVelocity();
		return Ogre::Vector3(velocity.x, 0, velocity.z).length();
	}

	const AgentLocomotion* locomotion = FindComponent<AgentLocomotion>();
	return locomotion != nullptr ? locomotion->GetSpeed() : AgentLocomotion::DEFAULT_AGENT_SPEED;
}

Ogre::Real AgentObject::GetHealth() const
{
	const AgentAttrib* attrib = FindComponent<AgentAttrib>();
	return attrib != nullptr ? attrib->GetHealth() : DEFAULT_AGENT_HEALTH;
}

Ogre::Vector3 AgentObject::PredictFuturePosition(Ogre::Real predictionTime) const
{
	Ogre::Real upredictionTime = std::max(Ogre::Real(0), predictionTime);
	return GetPosition() + GetVelocity() * upredictionTime;
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

	if (m_renderComp != nullptr)
	{
		m_renderComp->Update(deltaMilisec);
	}
}

void AgentObject::updateWorldTransform()
{
	if (m_renderComp != nullptr)
	{
		m_renderComp->SyncFromOwnerTransform();
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
	auto pAsm = GetObjectASM();
	if (pAsm) return pAsm->GetCurrStateID();
	return -1;
}

std::string AgentObject::GetCurStateName()
{
	auto pAsm = GetObjectASM();
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
	const SandboxServices* services = GetSandboxServices();
	BlockObject::SpawnBulletImpact(collision, services);
	ObjectManager* objectManager = services != nullptr ? services->objects : nullptr;
	TacticalService* tactics = objectManager != nullptr ? objectManager->GetTacticalService() : nullptr;
	if (tactics != nullptr)
	{
		BaseObject* bulletOwner = pBullet->GetOwner();
		tactics->publishTacticalEvent(
			SandboxEventTypes::BulletImpact(),
			bulletOwner != nullptr ? static_cast<int>(bulletOwner->GetObjId()) : -1,
			static_cast<int>(GetObjId()),
			bulletOwner != nullptr ? bulletOwner->GetTeamId() : -1,
			GetTeamId(),
			collision.pointA_,
			0,
			"global",
			false);
	}
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
	if (m_renderComp && getRigidBody() == nullptr)
		m_renderComp->SetPosition(position);
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
	if (m_renderComp && getRigidBody() == nullptr)
		m_renderComp->SetOrientation(quaternion);
}

LuaScriptComponent* AgentObject::GetLuaScript()
{
	return FindComponent<LuaScriptComponent>();
}

const LuaScriptComponent* AgentObject::GetLuaScript() const
{
	return FindComponent<LuaScriptComponent>();
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
	const SandboxServices* services = GetSandboxServices();
	AgentConfigService* agentConfig = services != nullptr ? services->agentConfig : nullptr;
	return agentConfig != nullptr ? agentConfig->GetUseCppFsmFlag() : true;
}
