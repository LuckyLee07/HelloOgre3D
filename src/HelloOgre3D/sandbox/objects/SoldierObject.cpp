#include "SoldierObject.h"
#include "RenderableObject.h"
#include "GameManager.h"
#include "systems/manager/SandboxMgr.h"
#include "OgreMath.h"
#include "animation/AgentAnimStateMachine.h"
#include "ai/fsm/AgentStateController.h"
#include "systems/input/PlayerInput.h"
#include "ai/fsm/states/AgentState.h"
#include "GameFunction.h"
#include "OgreSceneNode.h"
#include "BlockObject.h"
#include "systems/service/SceneFactory.h"
#include "animation/AgentAnimState.h"

using namespace Ogre;

namespace
{
	bool IsCrouchAnimState(int stateId)
	{
		return stateId >= CROUCH_SSTATE_DEAD && stateId <= CROUCH_SSTATE_FORWARD;
	}
}

SoldierObject::SoldierObject(RenderableObject* pAgentBody, btRigidBody* pRigidBody/* = nullptr*/)
	: AgentObject(pAgentBody, pRigidBody), m_pWeapon(nullptr), m_stanceType(SOLDIER_STAND), m_stateController(nullptr)
{
	this->SetObjType(OBJ_TYPE_SOLDIER);

	this->CreateEventDispatcher(); // 构造函数里使用虚函数会导致未定义

	if (GetUseCppFSM()) // 使用C++或者lua的FSM
	{
		m_stateController = new AgentStateController(this);
		m_stateController->Init();
	}
}

SoldierObject::~SoldierObject()
{
	SAFE_DELETE(m_pWeapon);
	SAFE_DELETE(m_inputInfo);
	SAFE_DELETE(m_stateController);
}

void SoldierObject::CreateEventDispatcher()
{
	Event()->CreateDispatcher("ASM_STATE_CHANGE");
	Event()->Subscribe("ASM_STATE_CHANGE", [&](const SandboxContext& context) -> void {
		int stateId = (int)context.Get_Number("StateId");
		if (stateId == SSTATE_FIRE || stateId == CROUCH_SSTATE_FIRE)
		{
			this->ShootBullet(); // 射击
		}
		if (!m_stateController) return;
		
		// 将事件传递到State那
		AgentState* pState = m_stateController->GetCurrState();
		SandboxContext context1;
		context1.Set_Number("StateId", stateId);
		pState->Event()->Emit("FSM_STATE_CHANGE", context);
	});
}

void SoldierObject::RemoveEventDispatcher()
{
	Event()->RemoveDispatcher("ASM_STATE_CHANGE");
}

void SoldierObject::Init()
{
	AgentObject::Init();

	auto inputMgr = GetGameManager()->getInputManager();
	m_inputInfo = new PlayerInput(inputMgr);
}

void SoldierObject::initWeapon(const Ogre::String& meshFile)
{
	if (m_pWeapon != nullptr)
	{
		SAFE_DELETE(m_pWeapon);
	}
	m_pWeapon = new RenderableObject(meshFile);
	m_pWeapon->InitAsmWithOwner(this, false);

	m_weaponHandOffsetPos = Ogre::Vector3(0.04f, 0.05f, -0.01f);
	m_weaponHandOffsetOrientation = QuaternionFromRotationDegrees(98.0f, 97.0f, 0.0f);
	SyncWeaponToHandBone();
}

void SoldierObject::Update(int deltaMilisec)
{
	// Keep body transform in sync with physics before animation/bone evaluation.
	this->updateWorldTransform();

	static int totalMilisec = 0;
	totalMilisec += deltaMilisec;

	bool forceUpdate = true;
	if (forceUpdate || totalMilisec > 1000)
	{
		totalMilisec = 0;
		this->callFunction("Agent_Update", "u[SoldierObject]i", this, deltaMilisec);
	}

	if (m_stateController)
		m_stateController->Update((float)deltaMilisec);

	m_pAgentBody->Update(deltaMilisec);
	SyncWeaponToHandBone();
	if (m_pWeapon)
		m_pWeapon->Update(deltaMilisec);

	TryApplyPendingStance();
}

void SoldierObject::SyncWeaponToHandBone()
{
	if (m_pWeapon == nullptr || m_pAgentBody == nullptr)
	{
		return;
	}

	Ogre::SceneNode* soldierNode = m_pAgentBody->GetSceneNode();
	if (soldierNode == nullptr)
	{
		return;
	}

	Ogre::Vector3 handPosition;
	Ogre::Quaternion handOrientation;
	if (!SceneFactory::GetBonePosition(*soldierNode, "b_RightHand", handPosition))
	{
		return;
	}
	if (!SceneFactory::GetBoneOrientation(*soldierNode, "b_RightHand", handOrientation))
	{
		return;
	}

	m_pWeapon->SetPosition(handPosition + (handOrientation * m_weaponHandOffsetPos));
	m_pWeapon->SetOrientation(handOrientation * m_weaponHandOffsetOrientation);
}

void SoldierObject::ShootBullet()
{
	Ogre::SceneNode* pSoldier = m_pAgentBody->GetSceneNode();

	Ogre::Vector3 position;
	SceneFactory::GetBonePosition(*pSoldier, "b_muzzle", position);
	Ogre::Quaternion orientation;
	SceneFactory::GetBoneOrientation(*pSoldier, "b_muzzle", orientation);

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
	
	Ogre::SceneNode* bulletParticle = SceneFactory::CreateParticle(bullet->GetSceneNode(), "Bullet");
	bulletParticle->setOrientation(QuaternionFromRotationDegrees(-90, 0, 0));
	bullet->addParticleNode(bulletParticle);

	bullet->applyImpulse(forward * 750);
}

void SoldierObject::changeStanceType(int stanceType)
{
	AgentAnimStateMachine* pAsm = getBody()->GetObjectASM();
	if (pAsm == nullptr) return;

	SOLDIER_STATE currState = (SOLDIER_STATE)pAsm->GetCurrStateID();
	SOLDIER_STATE requestState = (SOLDIER_STATE)ConvertAnimID(currState, stanceType);
	if (currState == requestState)
		return;

	if (stanceType == SOLDIER_STAND)
	{
		m_pendingStanceType = SOLDIER_STAND;
	}
	else if (stanceType == SOLDIER_CROUCH)
	{
		m_pendingStanceType = SOLDIER_CROUCH;
	}
	else
	{
		return;
	}

	TryApplyPendingStance();
}

void SoldierObject::ApplyStanceParams(int stanceType)
{
	float soldier_height = 0.0f;
	float soldier_speed = 0.0f;
	if (stanceType == SOLDIER_STAND)
	{
		m_stanceType = SOLDIER_STAND;
		soldier_height = SOLDIER_STAND_HEIGHT;
		soldier_speed = SOLDIER_STAND_SPEED;
	}
	else if (stanceType == SOLDIER_CROUCH)
	{
		m_stanceType = SOLDIER_CROUCH;
		soldier_height = SOLDIER_CROUCH_HEIGHT;
		soldier_speed = SOLDIER_CROUCH_SPEED;
	}
	else
	{
		return;
	}

	if (soldier_height <= 0.0f || soldier_speed <= 0.0f) return;

	m_pAgentBody->SetOriginPos(Ogre::Vector3(0, -soldier_height / 2, 0));

	Ogre::Real newPosY = (this->GetHeight() - soldier_height) / 2;
	this->setPosition((GetPosition() - Ogre::Vector3(0, newPosY, 0)));

	this->SetHeight(soldier_height);
	this->SetMaxSpeed(soldier_speed);
	this->updateWorldTransform();
}

void SoldierObject::TryApplyPendingStance()
{
	if (m_pendingStanceType < 0 || m_onPlayDeathAnim)
	{
		return;
	}

	AgentAnimStateMachine* pAsm = getBody()->GetObjectASM();
	if (pAsm == nullptr)
	{
		return;
	}

	SOLDIER_STATE currState = (SOLDIER_STATE)pAsm->GetCurrStateID();
	if (currState < 0)
	{
		return;
	}

	bool isCrouchState = IsCrouchAnimState(currState);
	if (!pAsm->HasNextState())
	{
		if ((m_pendingStanceType == SOLDIER_CROUCH && isCrouchState) ||
			(m_pendingStanceType == SOLDIER_STAND && !isCrouchState))
		{
			ApplyStanceParams(m_pendingStanceType);
			m_pendingStanceType = -1;
			return;
		}

		SOLDIER_STATE requestState = (SOLDIER_STATE)ConvertAnimID(currState, m_pendingStanceType);
		if (requestState != currState)
		{
			pAsm->RequestState(requestState);
		}
		else if (m_pendingStanceType == SOLDIER_CROUCH)
		{
			pAsm->RequestState("crouch_idle_aim");
		}
		else
		{
			pAsm->RequestState("idle_aim");
		}
	}
}
void SoldierObject::RequestState(int soldierState, bool forceUpdate /*= false*/)
{
	//播放死亡动画时不再接受新的状态
	if (m_onPlayDeathAnim) return;

	AgentAnimStateMachine* pAsm = getBody()->GetObjectASM();
	if (pAsm == nullptr) return;

	SOLDIER_STATE currState = (SOLDIER_STATE)pAsm->GetCurrStateID();
	
	SOLDIER_STATE requestState = (SOLDIER_STATE)ConvertAnimID(soldierState, getStanceType());
	if (!forceUpdate && currState == requestState) return;

	pAsm->RequestState(requestState);
}

bool SoldierObject::HasNextAnim()
{
	AgentAnimStateMachine* pAsm = getBody()->GetObjectASM();
	if (pAsm == nullptr) return false;

	return pAsm->HasNextState();
}

bool SoldierObject::IsAnimReadyForMove()
{
	AgentAnimStateMachine* pAsm = getBody()->GetObjectASM();
	if (pAsm == nullptr) return false;

	SOLDIER_STATE animStateId = SOLDIER_STATE(pAsm->GetCurrStateID());

	if (animStateId == SSTATE_RUN_FORWARD || animStateId == CROUCH_SSTATE_FORWARD)
	{
		return true;
	}
	return false;
}

bool SoldierObject::IsAnimReadyForShoot()
{
	AgentAnimStateMachine* pAsm = getBody()->GetObjectASM();
	if (pAsm == nullptr) return false;

	SOLDIER_STATE animStateId = SOLDIER_STATE(pAsm->GetCurrStateID());

	if (animStateId == SSTATE_FIRE || animStateId == CROUCH_SSTATE_FIRE)
	{
		return true;
	}
	return false;
}
