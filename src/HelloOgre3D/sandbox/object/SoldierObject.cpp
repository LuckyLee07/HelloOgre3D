#include "SoldierObject.h"
#include "EntityObject.h"
#include "manager/SandboxMgr.h"
#include "ScriptLuaVM.h"

using namespace Ogre;

SoldierObject::SoldierObject(EntityObject* pAgentBody, btRigidBody* pRigidBody/* = nullptr*/)
	: AgentObject(pAgentBody, pRigidBody), m_pWeapon(nullptr)
{
	this->setObjType(OBJ_TYPE_SOLDIER);
}

SoldierObject::~SoldierObject()
{
	SAFE_DELETE(m_pWeapon);
}

void SoldierObject::CreateEventDispatcher()
{
	Event()->CreateDispatcher("FSM_STATE_CHANGE");
	Event()->Subscribe("FSM_STATE_CHANGE", [&](const SandboxContext& context) -> void {
		std::string stateId = context.Get_String("StateId");
		if (stateId.empty()) return;
	
		if (stateId == "fire" || stateId == "crouch_fire")
		{
			this->ShootBullet(); // Éä»÷
		}
	});
}

void SoldierObject::RemoveEventDispatcher()
{
	Event()->RemoveDispatcher("FSM_STATE_CHANGE");
}

void SoldierObject::Initialize()
{
	this->CreateEventDispatcher();

	m_pAgentBody->InitWithOwner(this);
	m_pScriptVM->callModuleFunc(m_luaRef, "Agent_Initialize", "u[SoldierObject]", this);
}

void SoldierObject::initWeapon(const Ogre::String& meshFile)
{
	if (m_pWeapon != nullptr)
	{
		SAFE_DELETE(m_pWeapon);
	}
	m_pWeapon = new EntityObject(meshFile);
	m_pWeapon->InitWithOwner(this);

	Ogre::Vector3 positionOffset(0.04f, 0.05f, -0.01f);
	Ogre::Vector3 rotationOffset(98.0f, 97.0f, 0.0f);
	m_pAgentBody->AttachToBone("b_RightHand", m_pWeapon, positionOffset, rotationOffset);
}

void SoldierObject::update(int deltaMilisec)
{
	static int totalMilisec = 0;
	totalMilisec += deltaMilisec;
	if (true || totalMilisec > 1000)
	{
		totalMilisec = 0;
		m_pScriptVM->callModuleFunc(m_luaRef, "Agent_Update", "u[SoldierObject]i", this, deltaMilisec);
	}

	m_pAgentBody->update(deltaMilisec);
	if (m_pWeapon)
		m_pWeapon->update(deltaMilisec);

	this->updateWorldTransform();
}

void SoldierObject::ShootBullet()
{
	Ogre::SceneNode* pSoldier = m_pAgentBody->getSceneNode();

	Ogre::Vector3 position;
	SandboxMgr::GetBonePosition(*pSoldier, "b_muzzle", position);
	Ogre::Quaternion orientation;
	SandboxMgr::GetBoneOrientation(*pSoldier, "b_muzzle", orientation);

	Ogre::Vector3 rotation = QuaternionToRotationDegrees(orientation);
	this->DoShootBullet(position, rotation);
}

void SoldierObject::DoShootBullet(const Ogre::Vector3& position, const Ogre::Vector3& rotation)
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
	bullet->addParticleNode(bulletParticle);

	bullet->applyImpulse(forward * 750);
}

void SoldierObject::handleEventByLua(OIS::KeyCode keycode)
{
	m_pScriptVM->callModuleFunc(m_luaRef, "Agent_EventHandle", "u[SoldierObject]i", this, keycode);
}