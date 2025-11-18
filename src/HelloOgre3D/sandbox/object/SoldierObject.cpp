#include "SoldierObject.h"
#include "RenderableObject.h"
#include "manager/SandboxMgr.h"
#include "OgreMath.h"
#include "animation/AgentAnimStateMachine.h"
#include "state/AgentStateController.h"
#include "manager/ClientManager.h"
#include "input/PlayerInput.h"
#include "state/AgentState.h"
#include "GameFunction.h"
#include "OgreSceneNode.h"
#include "BlockObject.h"
#include "service/SceneFactory.h"

using namespace Ogre;

SoldierObject::SoldierObject(RenderableObject* pAgentBody, btRigidBody* pRigidBody/* = nullptr*/)
	: AgentObject(pAgentBody, pRigidBody), m_pWeapon(nullptr), m_stanceType(SOLDIER_STAND), m_stateController(nullptr)
{
	this->setObjType(OBJ_TYPE_SOLDIER);

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
		int stateId = context.Get_Number("StateId");
		if (stateId == SSTATE_FIRE || stateId == CROUCH_SSTATE_FIRE)
		{
			this->ShootBullet(); // 射击
		}
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

void SoldierObject::Initialize()
{
	AgentObject::Initialize();

	auto inputMgr = GetClientMgr()->getInputManager();
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

	Ogre::Vector3 positionOffset(0.04f, 0.05f, -0.01f);
	Ogre::Vector3 rotationOffset(98.0f, 97.0f, 0.0f);
	m_pAgentBody->AttachToBone("b_RightHand", m_pWeapon->GetDetachEntity(), positionOffset, rotationOffset);
}

void SoldierObject::update(int deltaMilisec)
{
	static int totalMilisec = 0;
	totalMilisec += deltaMilisec;
	if (true || totalMilisec > 1000)
	{
		totalMilisec = 0;
		this->callFunction("Agent_Update", "u[SoldierObject]i", this, deltaMilisec);
	}

	if (m_stateController)
		m_stateController->Update(deltaMilisec);

	m_pAgentBody->Update(deltaMilisec);
	if (m_pWeapon)
		m_pWeapon->Update(deltaMilisec);

	this->updateWorldTransform();
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

	if (soldier_height <= 0.0f || soldier_speed <= 0.0f) return;

	m_pAgentBody->SetOriginPos(Ogre::Vector3(0, -soldier_height / 2, 0));

	Ogre::Real newPosY = (this->GetHeight() - soldier_height) / 2;
	this->setPosition((GetPosition() - Ogre::Vector3(0, newPosY, 0)));

	this->SetHeight(soldier_height);
	this->SetMaxSpeed(soldier_speed);

	AgentAnimStateMachine* pAsm = getBody()->GetObjectASM();
	if (pAsm == nullptr) return;

	int currStateId = pAsm->GetCurrStateID();
	if (currStateId > 0) RequestState(currStateId, true);
}

void SoldierObject::RequestState(int soldierState, bool forceUpdate /*= false*/)
{
	if (m_onPlayDeathAnim) return; //播放死亡动画时不再接受新的状态
	
	SOLDIER_STATE requestState = (SOLDIER_STATE)ConvertAnimID(soldierState, getStanceType());

	AgentAnimStateMachine* pAsm = getBody()->GetObjectASM();
	if (pAsm == nullptr) return;

	if (!forceUpdate && pAsm->GetCurrStateID() == requestState) return;

	pAsm->RequestState(requestState);
}

bool SoldierObject::IsHasNextAnim()
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