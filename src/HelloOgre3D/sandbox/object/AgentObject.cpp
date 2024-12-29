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

using namespace Ogre;

AgentObject::AgentObject(EntityObject* pAgentBody, btRigidBody* pRigidBody/* = nullptr*/)
	: VehicleObject(pRigidBody), m_pAgentBody(pAgentBody)
{
	m_pAgentWeapon = nullptr;
	m_agentType = AGENT_OBJ_NONE;
}

AgentObject::~AgentObject()
{
	SAFE_DELETE(m_pAgentBody);
	SAFE_DELETE(m_pAgentWeapon);
}

void AgentObject::Initialize()
{
	m_pAgentBody->Initialize();
	m_pAgentBody->setOwner(this);
	
	GetScriptLuaVM()->callFunction("Agent_Initialize", "u[AgentObject]", this);
}

void AgentObject::initAgentBody(const Ogre::String& meshFile)
{
	if (m_pAgentBody != nullptr)
	{
		delete m_pAgentBody;
	}
	m_pAgentBody = new EntityObject(meshFile);
	m_pAgentBody->Initialize();
	m_pAgentBody->setOwner(this);
}

void AgentObject::initAgentWeapon(const Ogre::String& meshFile)
{
	if (m_pAgentWeapon != nullptr)
	{
		delete m_pAgentWeapon;
	}
	m_pAgentWeapon = new EntityObject(meshFile);
	m_pAgentWeapon->Initialize();
	m_pAgentWeapon->setOwner(this);

	Ogre::Vector3 positionOffset(0.04f, 0.05f, -0.01f);
	Ogre::Vector3 rotationOffset(98.0f, 97.0f, 0.0f);
	m_pAgentBody->AttachToBone("b_RightHand", m_pAgentWeapon, positionOffset, rotationOffset);
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
		return m_pAgentBody->getOrientation().yAxis();
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
		return m_pAgentBody->getOrientation().xAxis();
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
		return m_pAgentBody->getOrientation().zAxis();
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
		GetScriptLuaVM()->callFunction("Agent_Update", "u[AgentObject]i", this, deltaMilisec);
	}

	m_pAgentBody->update(deltaMilisec);
	if (m_pAgentWeapon)
		m_pAgentWeapon->update(deltaMilisec);

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
