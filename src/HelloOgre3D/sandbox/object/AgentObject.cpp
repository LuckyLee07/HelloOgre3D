#include "AgentObject.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"
#include "btBulletDynamicsCommon.h"
#include "GameManager.h"
#include "BlockObject.h"
#include "EntityObject.h"
#include "play/PhysicsWorld.h"
#include "manager/SandboxMgr.h"
#include "manager/ObjectManager.h"
#include "animation/AgentAnimStateMachine.h"

using namespace Ogre;

static std::string g_EmptyStr = "";

AgentObject::AgentObject(EntityObject* pAgentBody, btRigidBody* pRigidBody/* = nullptr*/)
	: VehicleObject(pRigidBody), m_pAgentBody(pAgentBody), m_agentType(AGENT_OBJ_NONE)
{
	m_objType = OBJ_TYPE_AGENT;

	SetForward(Ogre::Vector3::UNIT_Z);

	this->CreateEventDispatcher();
}

AgentObject::~AgentObject()
{
	this->RemoveEventDispatcher();

	SAFE_DELETE(m_pAgentBody);
}

void AgentObject::CreateEventDispatcher()
{
	Event()->CreateDispatcher("HEALTH_CHANGE");
	Event()->Subscribe("HEALTH_CHANGE", [&](const SandboxContext& context) -> void {
		double health = context.Get_Number("health");
		if (health <= 0.0) this->OnDeath(3.5f);
	});
}

void AgentObject::RemoveEventDispatcher()
{
	Event()->RemoveDispatcher("HEALTH_CHANGE");
}

void AgentObject::Initialize()
{
	m_pAgentBody->InitWithOwner(this);
	this->callFunction("Agent_Initialize", "u[AgentObject]", this);
}

void AgentObject::initBody(const Ogre::String& meshFile)
{
	if (m_pAgentBody != nullptr)
	{
		delete m_pAgentBody;
	}
	m_pAgentBody = new EntityObject(meshFile);
	m_pAgentBody->InitWithOwner(this);
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

void AgentObject::update(int deltaMilisec)
{
	static int totalMilisec = 0;
	totalMilisec += deltaMilisec;
	if (true || totalMilisec > 1000)
	{
		totalMilisec = 0;
		this->callFunction("Agent_Update", "u[AgentObject]i", this, deltaMilisec);
	}

	m_pAgentBody->update(deltaMilisec);

	this->updateWorldTransform();
}

void AgentObject::updateWorldTransform()
{
	const btRigidBody* pRigidBody = getRigidBody();
	if (pRigidBody == nullptr) return;
	
	const btVector3& rigidBodyPos = pRigidBody->getWorldTransform().getOrigin();
	Ogre::Vector3 position(rigidBodyPos.m_floats[0], rigidBodyPos.m_floats[1], rigidBodyPos.m_floats[2]);
	m_pAgentBody->SetDerivedPosition(position + m_pAgentBody->getOriginPos());

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

	this->setNeedClear(int(lastSec * 20));

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

void AgentObject::setPosition(const Ogre::Vector3& position)
{
	VehicleObject::setPosition(position);
	if (m_pAgentBody) 
		m_pAgentBody->setPosition(position);
}

void AgentObject::setOrientation(const Ogre::Quaternion& quaternion)
{
	VehicleObject::setOrientation(quaternion);
	if (m_pAgentBody)
		m_pAgentBody->setOrientation(quaternion);
}

#define USE_CPP_FSM 1
bool AgentObject::GetUseCppFSM()
{
#ifdef USE_CPP_FSM
	return true;
#endif // USE_CPP_FSM
	return false;
}