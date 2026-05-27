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
#include "components/agent/AgentLocomotion.h"
#include "systems/physics/Collision.h"
#include "BlockObject.h"
#include <algorithm>

using namespace Ogre;

static std::string g_EmptyStr = "";


AgentObject::AgentObject(RenderableObject* pAgentBody, btRigidBody* pRigidBody/* = nullptr*/)
	: VehicleObject(pRigidBody), m_pAgentBody(pAgentBody), m_agentType(AGENT_OBJ_NONE)
{
	m_objType = OBJ_TYPE_AGENT;
	if (m_pAgentBody != nullptr)
	{
		m_pAgentBody->AttachRenderComponent(this);
	}

	SetForward(Ogre::Vector3::UNIT_Z);
	if (GetLocomotion() != nullptr)
	{
		GetLocomotion()->SetOwner(this);
	}
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

Ogre::Vector3 AgentObject::GetPosition() const
{
	if (getRigidBody() != nullptr)
	{
		return VehicleObject::GetPosition();
	}
	else if (m_pAgentBody != nullptr)
	{
		return m_pAgentBody->GetDerivedPosition();
	}

	return Ogre::Vector3::ZERO;
}

Ogre::Quaternion AgentObject::GetOrientation() const
{
	if (getRigidBody() != nullptr)
	{
		return VehicleObject::GetOrientation();
	}
	else if (m_pAgentBody != nullptr)
	{
		return m_pAgentBody->GetDerivedOrientation();
	}

	return Ogre::Quaternion::ZERO;
}

Ogre::Vector3 AgentObject::GetUp() const
{
	if (getRigidBody() != nullptr)
	{
		return VehicleObject::GetUp();
	}
	else if (m_pAgentBody != nullptr)
	{
		return m_pAgentBody->GetOrientation().yAxis();
	}

	return Ogre::Vector3::UNIT_Y;
}

Ogre::Vector3 AgentObject::GetLeft() const
{
	if (getRigidBody() != nullptr)
	{
		return VehicleObject::GetLeft();
	}
	else if (m_pAgentBody != nullptr)
	{
		return m_pAgentBody->GetOrientation().xAxis();
	}

	return Ogre::Vector3::UNIT_X;
}

Ogre::Vector3 AgentObject::GetForward() const
{
	if (getRigidBody() != nullptr)
	{
		return VehicleObject::GetForward();
	}
	else if (m_pAgentBody != nullptr)
	{
		return m_pAgentBody->GetOrientation().zAxis();
	}

	return Ogre::Vector3::UNIT_Z;
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
	const btRigidBody* pRigidBody = getRigidBody();
	if (pRigidBody == nullptr) return;
	
	const btVector3& rigidBodyPos = pRigidBody->getWorldTransform().getOrigin();
	Ogre::Vector3 position(rigidBodyPos.m_floats[0], rigidBodyPos.m_floats[1], rigidBodyPos.m_floats[2]);
	m_pAgentBody->SetDerivedPosition(position + m_pAgentBody->GetOriginPos());

	const btQuaternion& rigidBodyRotation = pRigidBody->getWorldTransform().getRotation();
	Ogre::Quaternion rotation(rigidBodyRotation.w(), rigidBodyRotation.x(),
								rigidBodyRotation.y(), rigidBodyRotation.z());
	m_pAgentBody->SetDerivedOrientation(rotation);
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
	VehicleObject::setPosition(position);
	// Physics-driven agents must keep render transform sourced from rigid body.
	// Otherwise we can momentarily overwrite origin-offset corrections (e.g. crouch/stand).
	if (m_pAgentBody && getRigidBody() == nullptr)
		m_pAgentBody->SetPosition(position);
}

void AgentObject::setOrientation(const Ogre::Quaternion& quaternion)
{
	VehicleObject::setOrientation(quaternion);
	if (m_pAgentBody && getRigidBody() == nullptr)
		m_pAgentBody->SetOrientation(quaternion);
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
