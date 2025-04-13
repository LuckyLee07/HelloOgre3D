#include "AgentObject.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"
#include "btBulletDynamicsCommon.h"
#include "GameManager.h"
#include "ScriptLuaVM.h"
#include "BlockObject.h"
#include "EntityObject.h"
#include "play/PhysicsWorld.h"
#include "manager/SandboxMgr.h"
#include "manager/ObjectManager.h"
#include "animation/AnimationStateMachine.h"

using namespace Ogre;

static std::string g_EmptyStr = "";

AgentObject::AgentObject(EntityObject* pAgentBody, btRigidBody* pRigidBody/* = nullptr*/)
	: VehicleObject(pRigidBody), m_pAgentBody(pAgentBody), m_agentType(AGENT_OBJ_NONE)
{
	m_objType = OBJ_TYPE_AGENT;
	m_pScriptVM = GetScriptLuaVM();

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
	Event()->CreateDispatcher("FSM_STATE_CHANGE");
	Event()->Subscribe("FSM_STATE_CHANGE", [&](const SandboxContext& context) -> void {
		std::string stateId = context.Get_String("StateId");
		if (stateId.empty()) return;

		if (stateId == "dead" || stateId == "crouch_dead")
		{
			this->OnDeath(3.5f);
		}
	});

	Event()->CreateDispatcher("HEALTH_CHANGE");
	Event()->Subscribe("HEALTH_CHANGE", [&](const SandboxContext& context) -> void {
		double health = context.Get_Number("health");
		if (health <= 0.0) this->OnDeath(3.5f);
	});
}

void AgentObject::RemoveEventDispatcher()
{
	Event()->RemoveDispatcher("HEALTH_CHANGE");
	Event()->RemoveDispatcher("FSM_STATE_CHANGE");
}

void AgentObject::Initialize()
{
	m_pAgentBody->InitWithOwner(this);
	m_pScriptVM->callModuleFunc(m_luaRef, "Agent_Initialize", "u[AgentObject]", this);
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
	if (m_pRigidBody != nullptr)
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
	if (m_pRigidBody != nullptr)
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
	if (m_pRigidBody != nullptr)
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
	if (m_pRigidBody != nullptr)
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
	if (m_pRigidBody != nullptr)
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
		m_pScriptVM->callModuleFunc(m_luaRef, "Agent_Update", "u[AgentObject]i", this, deltaMilisec);
	}

	m_pAgentBody->update(deltaMilisec);

	this->updateWorldTransform();
}

void AgentObject::updateWorldTransform()
{
	if (m_pRigidBody == nullptr) return;
	
	const btVector3& rigidBodyPos = m_pRigidBody->getWorldTransform().getOrigin();
	Ogre::Vector3 position(rigidBodyPos.m_floats[0], rigidBodyPos.m_floats[1], rigidBodyPos.m_floats[2]);
	m_pAgentBody->SetDerivedPosition(position + m_pAgentBody->getOriginPos());

	const btQuaternion& rigidBodyRotation = m_pRigidBody->getWorldTransform().getRotation();
	Ogre::Quaternion rotation(rigidBodyRotation.w(), rigidBodyRotation.x(),
								rigidBodyRotation.y(), rigidBodyRotation.z());
	m_pAgentBody->SetDerivedOrientation(rotation);
}

void AgentObject::handleEventByLua(OIS::KeyCode keycode)
{
	m_pScriptVM->callModuleFunc(m_luaRef, "Agent_EventHandle", "u[AgentObject]i", this, keycode);
}

void AgentObject::HandleKeyEvent(OIS::KeyCode keycode, unsigned int key)
{
	this->handleEventByLua(keycode);
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

std::string AgentObject::GetCurStateName()
{
	auto pAsm = m_pAgentBody->GetObjectASM();
	if (pAsm) return pAsm->GetCurrStateName();
	
	return g_EmptyStr;
}

void AgentObject::OnDeath(float lastSec)
{
	if (m_onPlayDeathAnim) return;

	this->setNeedClear(lastSec * 20);

	m_onPlayDeathAnim = true;
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
