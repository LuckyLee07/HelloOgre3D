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

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

using namespace Ogre;

AgentObject::AgentObject(EntityObject* pAgentBody, btRigidBody* pRigidBody/* = nullptr*/)
	: VehicleObject(pRigidBody), m_pAgentBody(pAgentBody)
{
	m_pAgentWeapon = nullptr;
	m_objType = OBJ_TYPE_AGENT;
	m_pScriptVM = GetScriptLuaVM();

	SetForward(Ogre::Vector3::UNIT_Z);

	this->CreateEventDispatcher();
}

AgentObject::~AgentObject()
{
	this->RemoveEventDispatcher();

	SAFE_DELETE(m_pAgentBody);
	SAFE_DELETE(m_pAgentWeapon);
}

void AgentObject::CreateEventDispatcher()
{
	Event()->CreateDispatcher("FSM_STATE_CHANGE");
	Event()->Subscribe("FSM_STATE_CHANGE", [&](const SandboxContext& context) -> void {
		std::string stateId = context.Get_String("StateId");
		if (stateId.empty()) return;
	
		if (stateId == "fire" || stateId == "crouch_fire")
		{
			this->ShootBullet(); // 射击
		}
	});
}

void AgentObject::RemoveEventDispatcher()
{
	Event()->RemoveDispatcher("FSM_STATE_CHANGE");
}

void AgentObject::Initialize()
{
	m_pAgentBody->InitWithOwner(this);

	m_pScriptVM->callModuleFunc(m_luaRef, "Agent_Initialize", "u[AgentObject]", this);
}

void AgentObject::initAgentBody(const Ogre::String& meshFile)
{
	if (m_pAgentBody != nullptr)
	{
		delete m_pAgentBody;
	}
	m_pAgentBody = new EntityObject(meshFile);
	m_pAgentBody->InitWithOwner(this);
}

void AgentObject::initAgentWeapon(const Ogre::String& meshFile)
{
	if (m_pAgentWeapon != nullptr)
	{
		delete m_pAgentWeapon;
	}
	m_pAgentWeapon = new EntityObject(meshFile);
	m_pAgentWeapon->InitWithOwner(this);

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

void AgentObject::ShootBullet()
{
	Ogre::SceneNode* pSoldier = m_pAgentBody->getSceneNode();

	Ogre::Vector3 position;
	SandboxMgr::GetBonePosition(*pSoldier, "b_muzzle", position);
	Ogre::Quaternion orientation;
	SandboxMgr::GetBoneOrientation(*pSoldier, "b_muzzle", orientation);

	Ogre::Vector3 rotation = QuaternionToRotationDegrees(orientation);
	this->DoShootBullet(position, rotation);
}

void AgentObject::setPluginEnv(lua_State* L)
{
	if (!lua_istable(L, 2)) {
		CCLUA_ERROR("BindLuaEnv Error: param is not a table");
		return;
	}

	lua_pushvalue(L, 2); //self->为1 此处把table参数压入

	// 为防止被垃圾回收 在 Lua 注册表中创建一个引用
	m_luaRef = luaL_ref(L, LUA_REGISTRYINDEX);
}

void AgentObject::BindLuaPluginByFile(const std::string& fileName)
{
	m_pScriptVM->callModuleFunc("GLuaPluginMgr", "BindByLuaFile", "u[AgentObject]s>", this, fileName.c_str());
}

void AgentObject::DoShootBullet(const Ogre::Vector3& position, const Ogre::Vector3& rotation)
{
	Ogre::Quaternion qRotation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
	Vector3 forward = qRotation * Vector3(1, 0, 0);
	Vector3 up = qRotation * Vector3(0, 1, 0);
	Vector3 left = qRotation * Vector3(0, 0, -1);

	BlockObject* bullet = g_SandboxMgr->CreateBullet(0.3f, 0.01f);
	bullet->SetMass(0.1f);
	bullet->setPosition(position + forward * 0.2f);
	Ogre::Quaternion axisRot = Ogre::Quaternion(left, -forward, up);
	bullet->setOrientation(axisRot);

	Ogre::SceneNode* bulletParticle = SandboxMgr::CreateParticle(bullet->getSceneNode(), "Bullet");
	bulletParticle->setOrientation(QuaternionFromRotationDegrees(-90, 0, 0));

	bullet->applyImpulse(forward * 750);
}

void AgentObject::HandleKeyEvent(OIS::KeyCode keycode, unsigned int key)
{
	m_pScriptVM->callModuleFunc(m_luaRef, "Agent_EventHandle", "u[AgentObject]i", this, keycode);
}