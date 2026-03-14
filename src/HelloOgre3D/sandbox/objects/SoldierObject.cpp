#include "SoldierObject.h"
#include "RenderableObject.h"
#include "GameManager.h"
#include "systems/manager/SandboxMgr.h"
#include "OgreMath.h"
#include "animation/AgentAnimStateMachine.h"
#include "ai/fsm/AgentStateController.h"
#include "ai/command/SoldierCommandController.h"
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

	bool TryGetAnimStateByFsmStateName(const std::string& fsmStateName, SOLDIER_STATE& outAnimState)
	{
		if (fsmStateName == "IdleState")
		{
			outAnimState = SSTATE_IDLE_AIM;
			return true;
		}
		if (fsmStateName == "MoveState")
		{
			outAnimState = SSTATE_RUN_FORWARD;
			return true;
		}
		if (fsmStateName == "ShootState")
		{
			outAnimState = SSTATE_FIRE;
			return true;
		}
		if (fsmStateName == "DeathState")
		{
			outAnimState = SSTATE_DEAD;
			return true;
		}
		return false;
	}
}

SoldierObject::SoldierObject(RenderableObject* pAgentBody, btRigidBody* pRigidBody/* = nullptr*/)
	: AgentObject(pAgentBody, pRigidBody), m_pWeapon(nullptr), m_stanceType(SOLDIER_STAND), m_stateController(nullptr), m_commandController(nullptr)
{
	this->SetObjType(OBJ_TYPE_SOLDIER);

	this->CreateEventDispatcher(); // 构造函数里使用虚函数会导致未定义

	if (GetUseCppFSM()) // 使用C++或者lua的FSM
	{
		m_stateController = new AgentStateController(this);
		m_stateController->Init();
	}

	m_commandController = new SoldierCommandController(*this);
}

SoldierObject::~SoldierObject()
{
	SAFE_DELETE(m_pWeapon);
	SAFE_DELETE(m_inputInfo);
	SAFE_DELETE(m_stateController);
	SAFE_DELETE(m_commandController);
}

void SoldierObject::CreateEventDispatcher()
{
	Event()->CreateDispatcher("ASM_STATE_CHANGE");
	Event()->CreateDispatcher("ASM_STATE_ENTER");
	Event()->CreateDispatcher("ASM_STATE_LOOP");
	Event()->Subscribe("ASM_STATE_CHANGE", [&](const SandboxContext& context) -> void {
		int stateId = (int)context.Get_Number("StateId");
		int eventType = (int)context.Get_Number("EventType");
		if (m_commandController)
		{
			m_commandController->HandleAsmStateChange(stateId, eventType);
		}

		bool isEnterOrLoop = (eventType == AgentAnimStateMachine::ASM_EVENT_ENTER ||
			eventType == AgentAnimStateMachine::ASM_EVENT_LOOP ||
			eventType == 0);
		if (isEnterOrLoop && (stateId == SSTATE_FIRE || stateId == CROUCH_SSTATE_FIRE))
		{
			this->ShootBullet(); // 射击
		}
		if (!m_stateController) return;
		if (eventType != AgentAnimStateMachine::ASM_EVENT_ENTER && eventType != 0) return;
		
		// 将事件传递到State那
		AgentState* pState = m_stateController->GetCurrState();
		if (!pState) return;

		SandboxContext context1;
		context1.Set_Number("StateId", stateId);
		context1.Set_Number("EventType", eventType);
		pState->Event()->Emit("FSM_STATE_CHANGE", context1);
	});
}

void SoldierObject::RemoveEventDispatcher()
{
	Event()->RemoveDispatcher("ASM_STATE_CHANGE");
	Event()->RemoveDispatcher("ASM_STATE_ENTER");
	Event()->RemoveDispatcher("ASM_STATE_LOOP");
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

	if (m_commandController)
	{
		m_commandController->Update(deltaMilisec);
	}

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
	if (m_pAgentBody == nullptr)
	{
		return;
	}

	Ogre::SceneNode* soldierNode = m_pAgentBody->GetSceneNode();
	if (soldierNode == nullptr)
	{
		return;
	}

	Ogre::Vector3 position = soldierNode->_getDerivedPosition();
	Ogre::Quaternion orientation = soldierNode->_getDerivedOrientation();
	bool hasPosition = false;
	bool hasOrientation = false;

	// Preferred path: fire from muzzle bone on soldier mesh.
	hasPosition = SceneFactory::GetBonePosition(*soldierNode, "b_muzzle", position);
	hasOrientation = SceneFactory::GetBoneOrientation(*soldierNode, "b_muzzle", orientation);

	// Fallback: some assets keep muzzle data on weapon mesh.
	if ((!hasPosition || !hasOrientation) && m_pWeapon != nullptr)
	{
		Ogre::SceneNode* weaponNode = m_pWeapon->GetSceneNode();
		if (weaponNode != nullptr)
		{
			if (!hasPosition)
			{
				hasPosition = SceneFactory::GetBonePosition(*weaponNode, "b_muzzle", position);
				if (!hasPosition)
				{
					position = weaponNode->_getDerivedPosition();
					hasPosition = true;
				}
			}
			if (!hasOrientation)
			{
				hasOrientation = SceneFactory::GetBoneOrientation(*weaponNode, "b_muzzle", orientation);
				if (!hasOrientation)
				{
					orientation = weaponNode->_getDerivedOrientation();
					hasOrientation = true;
				}
			}
		}
	}

	// Last fallback: hand bone usually exists even if muzzle does not.
	if (!hasPosition)
	{
		hasPosition = SceneFactory::GetBonePosition(*soldierNode, "b_RightHand", position);
	}
	if (!hasOrientation)
	{
		hasOrientation = SceneFactory::GetBoneOrientation(*soldierNode, "b_RightHand", orientation);
	}

	this->DoShootBullet(position, orientation);
}

void SoldierObject::DoShootBullet(const Ogre::Vector3& position, const Ogre::Quaternion& orientation)
{
	Ogre::Quaternion qRotation = orientation;
	qRotation.normalise();

	Vector3 forward = qRotation * Vector3::UNIT_X;
	Vector3 up = qRotation * Vector3::UNIT_Y;
	Vector3 left = qRotation * (-Vector3::UNIT_Z);

	if (forward.isNaN() || forward.isZeroLength())
	{
		forward = GetForward();
	}
	forward.normalise();
	if (up.isNaN() || up.isZeroLength())
	{
		up = Vector3::UNIT_Y;
	}
	up.normalise();
	left = up.crossProduct(forward);
	if (left.isNaN() || left.isZeroLength())
	{
		left = Vector3::UNIT_X;
	}
	left.normalise();

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

bool SoldierObject::RequestAnimByFsmState(const std::string& fsmStateName, bool forceUpdate /*= false*/)
{
	SOLDIER_STATE requestState = SSTATE_MAXCOUNT;
	if (!TryGetAnimStateByFsmStateName(fsmStateName, requestState))
	{
		return false;
	}

	RequestState((int)requestState, forceUpdate);
	return true;
}

bool SoldierObject::IsAnimReadyByFsmState(const std::string& fsmStateName)
{
	AgentAnimStateMachine* pAsm = getBody()->GetObjectASM();
	if (pAsm == nullptr) return false;

	SOLDIER_STATE animStateId = SOLDIER_STATE(pAsm->GetCurrStateID());
	if (fsmStateName == "MoveState")
	{
		return animStateId == SSTATE_RUN_FORWARD || animStateId == CROUCH_SSTATE_FORWARD;
	}
	if (fsmStateName == "ShootState")
	{
		return animStateId == SSTATE_FIRE || animStateId == CROUCH_SSTATE_FIRE;
	}
	if (fsmStateName == "IdleState")
	{
		return animStateId == SSTATE_IDLE_AIM || animStateId == CROUCH_SSTATE_IDLE_AIM;
	}
	if (fsmStateName == "DeathState")
	{
		return animStateId == SSTATE_DEAD || animStateId == CROUCH_SSTATE_DEAD;
	}
	return true;
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
	if (forceUpdate)
	{
		pAsm->RequestStatePreferLatest(requestState);
		return;
	}
	if (currState == requestState) return;
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
	return IsAnimReadyByFsmState("MoveState");
}

bool SoldierObject::IsAnimReadyForShoot()
{
	return IsAnimReadyByFsmState("ShootState");
}

bool SoldierObject::QueueCommand(AgentCommandType commandType, int priority /*= 0*/, int arg /*= 0*/, const std::string& source /*= ""*/)
{
	return m_commandController ? m_commandController->QueueCommand(commandType, priority, arg, source) : false;
}

bool SoldierObject::ImmediateCommand(AgentCommandType commandType, int priority /*= 100*/, int arg /*= 0*/, const std::string& source /*= ""*/)
{
	return m_commandController ? m_commandController->ImmediateCommand(commandType, priority, arg, source) : false;
}

void SoldierObject::ClearCommands()
{
	if (m_commandController)
	{
		m_commandController->ClearCommands();
	}
}

bool SoldierObject::HasPendingCommands() const
{
	return m_commandController ? m_commandController->HasPendingCommands() : false;
}

bool SoldierObject::HasCurrentCommand() const
{
	return m_commandController ? m_commandController->HasCurrentCommand() : false;
}

int SoldierObject::GetPendingCommandCount() const
{
	return m_commandController ? m_commandController->GetPendingCommandCount() : 0;
}

AgentCommandType SoldierObject::GetCurrentCommandType() const
{
	return m_commandController ? m_commandController->GetCurrentCommandType() : AGENT_COMMAND_NONE;
}

AgentCommandType SoldierObject::GetPreviousCommandType() const
{
	return m_commandController ? m_commandController->GetPreviousCommandType() : AGENT_COMMAND_NONE;
}
