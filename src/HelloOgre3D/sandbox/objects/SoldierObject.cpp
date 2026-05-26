#include "SoldierObject.h"
#include "RenderableObject.h"
#include "GameManager.h"
#include "systems/manager/SandboxMgr.h"
#include "systems/manager/ObjectManager.h"
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
#include "animation/SoldierAnimController.h"
#include "animation/SoldierAnimProfile.h"
#include "ai/decision/DecisionTreeDriver.h"
#include "ai/behavior/BehaviorTreeDriver.h"
#include "components/agent/AgentAttrib.h"
#include "components/ai/AIController.h"
#include "components/combat/WeaponComponent.h"
#include "profiling/Profile.h"
#include <algorithm>

using namespace Ogre;

namespace
{
	bool IsCrouchAnimState(int stateId)
	{
		return stateId >= CROUCH_SSTATE_DEAD && stateId <= CROUCH_SSTATE_FORWARD;
	}
}

SoldierObject::SoldierObject(RenderableObject* pAgentBody, btRigidBody* pRigidBody/* = nullptr*/)
	: AgentObject(pAgentBody, pRigidBody), m_pWeapon(nullptr), m_stanceType(SOLDIER_STAND), m_maxHealth(100.0f), m_ammo(10), m_maxAmmo(10), m_attrib(nullptr), m_weaponComp(nullptr), m_ai(nullptr), m_inputInfo(nullptr), m_animController(nullptr)
{
	this->SetObjType(OBJ_TYPE_SOLDIER);

	m_maxHealth = std::max<Ogre::Real>(GetHealth(), 1.0f);
	m_maxAmmo = std::max(1, m_maxAmmo);
	m_ammo = std::min(std::max(0, m_ammo), m_maxAmmo);

	AgentAttrib* attrib = new AgentAttrib(GetHealth(), m_maxHealth, m_stanceType, m_pendingStanceType);
	if (AddComponent("attrib", attrib))
	{
		m_attrib = attrib;
	}
	else
	{
		SAFE_DELETE(attrib);
	}

	WeaponComponent* weapon = new WeaponComponent(this);
	weapon->SetMaxAmmo(m_maxAmmo);
	weapon->SetAmmo(m_ammo);
	if (AddComponent("weapon", weapon))
	{
		m_weaponComp = weapon;
	}
	else
	{
		SAFE_DELETE(weapon);
	}

	this->CreateEventDispatcher(); // 构造函数里使用虚函数会导致未定义

	// 将 soldier 专属的 state name -> id 映射注入 body ASM；以前这张表由通用 AgentAnimState 持有，
	// 现在收敛到 SoldierAnimProfile，ASM 侧仅通过 resolver 访问。
	if (getBody() && getBody()->GetObjectASM())
	{
		getBody()->GetObjectASM()->SetStateIdResolver(&SoldierAnimProfile::GetStateIdByName);
	}

	AIController* ai = new AIController(this);
	if (AddComponent("ai", ai))
	{
		m_ai = ai;
	}
	else
	{
		SAFE_DELETE(ai);
	}

	m_animController = new SoldierAnimController(*this);
}

SoldierObject::~SoldierObject()
{
	this->RemoveEventDispatcher();
	SAFE_DELETE(m_inputInfo);
	SAFE_DELETE(m_animController);
}

void SoldierObject::CreateEventDispatcher()
{
	Event()->CreateDispatcher("ASM_STATE_CHANGE");
	Event()->CreateDispatcher("ASM_NOTIFY");
	m_asmStateChangeEventToken = Event()->Subscribe("ASM_STATE_CHANGE", [&](const SandboxContext& context) -> void {
		int stateId = (int)context.Get_Number("StateId");
		if (!GetUseCppFSM() && (stateId == SSTATE_FIRE || stateId == CROUCH_SSTATE_FIRE))
		{
			this->ShootBullet();
		}
		if (m_animController)
		{
			m_animController->OnBodyStateChanged(stateId);
		}
		AgentStateController* fsm = GetFsmController();
		if (!fsm) return;

		AgentState* pState = fsm->GetCurrState();
		if (!pState) return;
		pState->Event()->Emit("FSM_STATE_CHANGE", context);
	});
	m_asmNotifyEventToken = Event()->Subscribe("ASM_NOTIFY", [&](const SandboxContext& context) -> void {
		const std::string eventName = context.Get_String("EventName");
		const int stateId = (int)context.Get_Number("StateId");
		const float normalizedTime = (float)context.Get_Number("NormalizedTime");
		if (GetUseCppFSM() && eventName == "shoot_fire")
		{
			this->ShootBullet();
		}
		if (m_animController)
		{
			m_animController->OnBodyNotify(eventName, stateId, normalizedTime);
		}
	});
}

void SoldierObject::RemoveEventDispatcher()
{
	Event()->Unsubscribe("ASM_STATE_CHANGE", m_asmStateChangeEventToken);
	Event()->Unsubscribe("ASM_NOTIFY", m_asmNotifyEventToken);
	m_asmStateChangeEventToken = 0;
	m_asmNotifyEventToken = 0;
	Event()->RemoveDispatcher("ASM_STATE_CHANGE");
	Event()->RemoveDispatcher("ASM_NOTIFY");
}

void SoldierObject::Init()
{
	AgentObject::Init();

	auto inputMgr = GetGameManager()->getInputManager();
	m_inputInfo = new PlayerInput(inputMgr);
}

void SoldierObject::initWeapon(const Ogre::String& meshFile)
{
	if (m_weaponComp != nullptr)
	{
		m_weaponComp->Init(meshFile);
		m_pWeapon = m_weaponComp->GetWeapon();
		m_weaponHandOffsetPos = m_weaponComp->GetHandOffsetPos();
		m_weaponHandOffsetOrientation = m_weaponComp->GetHandOffsetOrientation();
	}
}

RenderableObject* SoldierObject::getWeapon()
{
	return m_weaponComp != nullptr ? m_weaponComp->GetWeapon() : m_pWeapon;
}

int SoldierObject::getStanceType() const
{
	return m_attrib != nullptr ? m_attrib->GetStanceType() : m_stanceType;
}

void SoldierObject::Update(int deltaMilisec)
{
	H3D_PROFILE_SCOPE("SoldierObject::Update");
	// Keep body transform in sync with physics before animation/bone evaluation.
	this->updateWorldTransform();

	if (m_ai != nullptr && m_ai->IsTickInOwnerUpdateEnabled())
		TickAi(deltaMilisec);

	if (m_animController && GetUseCppFSM())
	{
		H3D_PROFILE_SCOPE("SoldierAnimController::Update");
		m_animController->Update((float)deltaMilisec);
	}

	{
		H3D_PROFILE_SCOPE("AgentBody::Update");
		m_pAgentBody->Update(deltaMilisec);
	}
	if (m_weaponComp)
	{
		H3D_PROFILE_SCOPE("Weapon::Update");
		m_weaponComp->update(deltaMilisec);
	}

	TryApplyPendingStance();
}

void SoldierObject::TickAi(int deltaMilisec)
{
	if (m_ai != nullptr)
		m_ai->TickAI(deltaMilisec);
}

void SoldierObject::SetAiTickInUpdateEnabled(bool enabled)
{
	if (m_ai != nullptr)
		m_ai->SetTickInOwnerUpdateEnabled(enabled);
}

void SoldierObject::SyncWeaponToHandBone()
{
	if (m_weaponComp != nullptr)
		m_weaponComp->SyncToHandBone();
}

void SoldierObject::ShootBullet()
{
	if (m_weaponComp != nullptr)
		m_weaponComp->ShootBullet();
}

void SoldierObject::DoShootBullet(const Ogre::Vector3& position, const Ogre::Quaternion& orientation)
{
	if (m_weaponComp != nullptr)
		m_weaponComp->DoShootBullet(position, orientation);
}

void SoldierObject::SetMaxHealth(Ogre::Real maxHealth)
{
	if (m_attrib != nullptr)
	{
		m_attrib->SetMaxHealth(maxHealth);
		m_maxHealth = m_attrib->GetMaxHealth();
	}
	else
	{
		m_maxHealth = std::max<Ogre::Real>(maxHealth, 1.0f);
	}
}

Ogre::Real SoldierObject::GetMaxHealth() const
{
	return m_attrib != nullptr ? m_attrib->GetMaxHealth() : m_maxHealth;
}

void SoldierObject::SetAmmo(int ammo)
{
	if (m_weaponComp != nullptr)
	{
		m_weaponComp->SetAmmo(ammo);
		m_ammo = m_weaponComp->GetAmmo();
	}
	else
	{
		m_ammo = std::min(std::max(0, ammo), m_maxAmmo);
	}
}

int SoldierObject::GetAmmo() const
{
	return m_weaponComp != nullptr ? m_weaponComp->GetAmmo() : m_ammo;
}

void SoldierObject::SetMaxAmmo(int maxAmmo)
{
	if (m_weaponComp != nullptr)
	{
		m_weaponComp->SetMaxAmmo(maxAmmo);
		m_maxAmmo = m_weaponComp->GetMaxAmmo();
		m_ammo = m_weaponComp->GetAmmo();
	}
	else
	{
		m_maxAmmo = std::max(1, maxAmmo);
		if (m_ammo > m_maxAmmo)
		{
			m_ammo = m_maxAmmo;
		}
	}
}

int SoldierObject::GetMaxAmmo() const
{
	return m_weaponComp != nullptr ? m_weaponComp->GetMaxAmmo() : m_maxAmmo;
}

bool SoldierObject::HasAmmo() const
{
	return m_weaponComp != nullptr ? m_weaponComp->HasAmmo() : m_ammo > 0;
}

void SoldierObject::ConsumeAmmo(int amount)
{
	if (m_weaponComp != nullptr)
	{
		m_weaponComp->ConsumeAmmo(amount);
		m_ammo = m_weaponComp->GetAmmo();
		return;
	}

	if (amount <= 0)
	{
		return;
	}
	SetAmmo(m_ammo - amount);
}

void SoldierObject::RestoreAmmo()
{
	if (m_weaponComp != nullptr)
	{
		m_weaponComp->RestoreAmmo();
		m_ammo = m_weaponComp->GetAmmo();
		m_maxAmmo = m_weaponComp->GetMaxAmmo();
		return;
	}

	m_ammo = m_maxAmmo;
}

AgentObject* SoldierObject::GetEnemy() const
{
	return m_ai != nullptr ? m_ai->GetEnemy() : nullptr;
}

bool SoldierObject::HasEnemy(const Ogre::String& navMeshName)
{
	return m_ai != nullptr && m_ai->HasEnemy(navMeshName);
}

bool SoldierObject::CanShootEnemy(const Ogre::String& navMeshName, float shootDistance)
{
	return m_ai != nullptr && m_ai->CanShootEnemy(navMeshName, shootDistance);
}
bool SoldierObject::HasMovePosition(float reachDistance) const
{
	return m_ai != nullptr && m_ai->HasMovePosition(reachDistance);
}

void SoldierObject::SetMovePosition(const Ogre::Vector3& movePos)
{
	if (m_ai != nullptr)
		m_ai->SetMovePosition(movePos);
}

void SoldierObject::ClearMovePosition()
{
	if (m_ai != nullptr)
		m_ai->ClearMovePosition();
}

bool SoldierObject::IsTargetReached(float threshold) const
{
	return m_ai != nullptr && m_ai->IsTargetReached(threshold);
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
	if (m_attrib != nullptr)
	{
		m_attrib->SetPendingStanceType(m_pendingStanceType);
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
		if (m_attrib != nullptr)
			m_attrib->SetStanceType(SOLDIER_STAND);
		soldier_height = SOLDIER_STAND_HEIGHT;
		soldier_speed = SOLDIER_STAND_SPEED;
	}
	else if (stanceType == SOLDIER_CROUCH)
	{
		m_stanceType = SOLDIER_CROUCH;
		if (m_attrib != nullptr)
			m_attrib->SetStanceType(SOLDIER_CROUCH);
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
	const int pendingStanceType = m_attrib != nullptr ? m_attrib->GetPendingStanceType() : m_pendingStanceType;
	if (pendingStanceType < 0 || m_onPlayDeathAnim)
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
		if ((pendingStanceType == SOLDIER_CROUCH && isCrouchState) ||
			(pendingStanceType == SOLDIER_STAND && !isCrouchState))
		{
			ApplyStanceParams(pendingStanceType);
			m_pendingStanceType = -1;
			if (m_attrib != nullptr)
				m_attrib->ClearPendingStanceType();
			return;
		}

		SOLDIER_STATE requestState = (SOLDIER_STATE)ConvertAnimID(currState, pendingStanceType);
		if (requestState != currState)
		{
			pAsm->RequestState(requestState);
		}
		else if (pendingStanceType == SOLDIER_CROUCH)
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
	//闁圭虎鍘介弬浣割潰鐠佹娊顎楅柛鏂诲妿閺侀箖寮張鐢电憹闁告劕绉电敮鎾矗濡や焦鐓€闁汇劌瀚慨鎼佸箑?
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

	const std::string moveStateName = SoldierAnimProfile::GetStateNameById(ConvertAnimID(SSTATE_RUN_FORWARD, getStanceType()));
	return pAsm->IsCurrentState(moveStateName) || pAsm->IsNextState(moveStateName);
}

bool SoldierObject::IsAnimReadyForShoot()
{
	AgentAnimStateMachine* pAsm = getBody()->GetObjectASM();
	if (pAsm == nullptr) return false;

	const std::string shootStateName = SoldierAnimProfile::GetStateNameById(ConvertAnimID(SSTATE_FIRE, getStanceType()));
	return pAsm->IsCurrentState(shootStateName) || pAsm->IsNextState(shootStateName);
}

void SoldierObject::UseDecisionTreeDriver()
{
	// Swap out whatever driver is currently installed (typically the FSM
	// controller created in the ctor) — DT-driven soldiers author behavior in Lua.
	if (m_ai != nullptr)
		m_ai->UseDecisionTreeDriver();
}

DecisionTreeDriver* SoldierObject::GetDecisionTreeDriver() const
{
	return m_ai != nullptr ? m_ai->GetDecisionTreeDriver() : nullptr;
}

void SoldierObject::UseBehaviorTreeDriver()
{
	if (m_ai != nullptr)
		m_ai->UseBehaviorTreeDriver();
}

BehaviorTreeDriver* SoldierObject::GetBehaviorTreeDriver() const
{
	return m_ai != nullptr ? m_ai->GetBehaviorTreeDriver() : nullptr;
}

AgentStateController* SoldierObject::GetFsmController() const
{
	return m_ai != nullptr ? m_ai->GetFsmController() : nullptr;
}

void SoldierObject::EnterIdleAnim()
{
	if (!m_animController) return;
	m_animController->ClearAllActions();
	m_animController->SetLocomotionIntent(SoldierLocomotionIntent::Idle);
}

void SoldierObject::EnterMoveAnim()
{
	if (!m_animController) return;
	m_animController->ClearAllActions();
	m_animController->SetLocomotionIntent(SoldierLocomotionIntent::Move);
}

void SoldierObject::EnterShootAnim()
{
	if (!m_animController) return;
	m_animController->SetLocomotionIntent(SoldierLocomotionIntent::Idle);
	m_animController->RequestAction(SoldierActionIntent::Shoot, true);
}

void SoldierObject::EnterReloadAnim()
{
	if (!m_animController) return;
	m_animController->SetLocomotionIntent(SoldierLocomotionIntent::Idle);
	m_animController->RequestAction(SoldierActionIntent::Reload, true);
}

void SoldierObject::EnterDeathAnim()
{
	if (!m_animController) return;
	m_animController->RequestAction(SoldierActionIntent::Death, true);
}
