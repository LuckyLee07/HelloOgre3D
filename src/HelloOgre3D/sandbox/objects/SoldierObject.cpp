#include "SoldierObject.h"
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
#include "components/anim/AnimComponent.h"
#include "components/anim/IAnimController.h"
#include "ai/decision/DecisionTreeDriver.h"
#include "ai/behavior/BehaviorTreeDriver.h"
#include "components/agent/AgentAttrib.h"
#include "components/ai/AIController.h"
#include "components/combat/WeaponComponent.h"
#include "components/render/RenderComponent.h"
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

SoldierObject::SoldierObject(RenderComponent* renderComp, btRigidBody* pRigidBody/* = nullptr*/)
	: AgentObject(renderComp, pRigidBody), m_attrib(nullptr), m_weaponComp(nullptr), m_ai(nullptr), m_animComp(nullptr), m_inputInfo(nullptr)
{
	this->SetObjType(OBJ_TYPE_SOLDIER);
	this->SetLuaScriptClassName(LuaClassNameTraits<SoldierObject>::value);

	AgentAttrib* attrib = new AgentAttrib(GetHealth(), std::max<Ogre::Real>(GetHealth(), 1.0f), SOLDIER_STAND, -1);
	if (AddComponent("attrib", attrib))
	{
		m_attrib = attrib;
	}
	else
	{
		SAFE_DELETE(attrib);
	}

	WeaponComponent* weapon = new WeaponComponent(this);
	if (AddComponent("weapon", weapon))
	{
		m_weaponComp = weapon;
	}
	else
	{
		SAFE_DELETE(weapon);
	}

	// 将 soldier 专属的 state name -> id 映射注入 body ASM；以前这张表由通用 AgentAnimState 持有，
	// 现在收敛到 SoldierAnimProfile，ASM 侧仅通过 resolver 访问。
	AIController* ai = new AIController(this);
	if (AddComponent("ai", ai))
	{
		m_ai = ai;
	}
	else
	{
		SAFE_DELETE(ai);
	}

	AnimComponent* anim = new AnimComponent(this);
	if (AddComponent("anim", anim))
	{
		m_animComp = anim;
		if (m_renderComp != nullptr)
		{
			m_animComp->InitBodyAnimations(m_renderComp->GetEntity(), true);
		}
	}
	else
	{
		SAFE_DELETE(anim);
	}
}

SoldierObject::~SoldierObject()
{
	RemoveComponent("anim");
	SAFE_DELETE(m_inputInfo);
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
	}
}

WeaponComponent* SoldierObject::getWeapon()
{
	return m_weaponComp;
}

int SoldierObject::getStanceType() const
{
	return m_attrib != nullptr ? m_attrib->GetStanceType() : SOLDIER_STAND;
}

void SoldierObject::Update(int deltaMilisec)
{
	H3D_PROFILE_SCOPE("SoldierObject::Update");
	// Keep body transform in sync with physics before animation/bone evaluation.
	this->updateWorldTransform();

	if (m_ai != nullptr && m_ai->IsTickInOwnerUpdateEnabled())
		TickAi(deltaMilisec);

	if (m_animComp != nullptr)
		m_animComp->UpdateController(deltaMilisec);

	{
		H3D_PROFILE_SCOPE("AgentBody::Update");
		if (m_renderComp != nullptr)
			m_renderComp->Update(deltaMilisec);
		if (m_animComp != nullptr)
			m_animComp->UpdateBodyAnimations(deltaMilisec);
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
	}
}

Ogre::Real SoldierObject::GetMaxHealth() const
{
	return m_attrib != nullptr ? m_attrib->GetMaxHealth() : std::max<Ogre::Real>(GetHealth(), 1.0f);
}

void SoldierObject::SetAmmo(int ammo)
{
	if (m_weaponComp != nullptr)
	{
		m_weaponComp->SetAmmo(ammo);
	}
}

int SoldierObject::GetAmmo() const
{
	return m_weaponComp != nullptr ? m_weaponComp->GetAmmo() : 0;
}

void SoldierObject::SetMaxAmmo(int maxAmmo)
{
	if (m_weaponComp != nullptr)
	{
		m_weaponComp->SetMaxAmmo(maxAmmo);
	}
}

int SoldierObject::GetMaxAmmo() const
{
	return m_weaponComp != nullptr ? m_weaponComp->GetMaxAmmo() : 0;
}

bool SoldierObject::HasAmmo() const
{
	return m_weaponComp != nullptr && m_weaponComp->HasAmmo();
}

void SoldierObject::ConsumeAmmo(int amount)
{
	if (m_weaponComp != nullptr)
	{
		m_weaponComp->ConsumeAmmo(amount);
	}
}

void SoldierObject::RestoreAmmo()
{
	if (m_weaponComp != nullptr)
	{
		m_weaponComp->RestoreAmmo();
	}
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
	if (m_attrib == nullptr)
		return;

	AgentAnimStateMachine* pAsm = GetObjectASM();
	if (pAsm == nullptr) return;

	SOLDIER_STATE currState = (SOLDIER_STATE)pAsm->GetCurrStateID();
	SOLDIER_STATE requestState = (SOLDIER_STATE)ConvertAnimID(currState, stanceType);
	if (currState == requestState)
		return;

	if (stanceType == SOLDIER_STAND)
	{
		m_attrib->SetPendingStanceType(SOLDIER_STAND);
	}
	else if (stanceType == SOLDIER_CROUCH)
	{
		m_attrib->SetPendingStanceType(SOLDIER_CROUCH);
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
		if (m_attrib != nullptr)
			m_attrib->SetStanceType(SOLDIER_STAND);
		soldier_height = SOLDIER_STAND_HEIGHT;
		soldier_speed = SOLDIER_STAND_SPEED;
	}
	else if (stanceType == SOLDIER_CROUCH)
	{
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

	if (m_renderComp != nullptr)
		m_renderComp->SetVisualOffset(Ogre::Vector3(0, -soldier_height / 2, 0));

	Ogre::Real newPosY = (this->GetHeight() - soldier_height) / 2;
	this->setPosition((GetPosition() - Ogre::Vector3(0, newPosY, 0)));

	this->SetHeight(soldier_height);
	this->SetMaxSpeed(soldier_speed);
	this->updateWorldTransform();
}

void SoldierObject::TryApplyPendingStance()
{
	if (m_attrib == nullptr)
	{
		return;
	}

	const int pendingStanceType = m_attrib->GetPendingStanceType();
	if (pendingStanceType < 0 || m_onPlayDeathAnim)
	{
		return;
	}

	AgentAnimStateMachine* pAsm = GetObjectASM();
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

	AgentAnimStateMachine* pAsm = GetObjectASM();
	if (pAsm == nullptr) return;

	SOLDIER_STATE currState = (SOLDIER_STATE)pAsm->GetCurrStateID();
	
	SOLDIER_STATE requestState = (SOLDIER_STATE)ConvertAnimID(soldierState, getStanceType());
	if (!forceUpdate && currState == requestState) return;

	pAsm->RequestState(requestState);
}
bool SoldierObject::HasNextAnim()
{
	return m_animComp != nullptr && m_animComp->HasNextAnim();
}

bool SoldierObject::IsAnimReadyForMove()
{
	return m_animComp != nullptr && m_animComp->IsAnimReadyForMove();
}

bool SoldierObject::IsAnimReadyForShoot()
{
	return m_animComp != nullptr && m_animComp->IsAnimReadyForShoot();
}

SoldierAnimController* SoldierObject::GetAnimController() const
{
	return m_animComp != nullptr ? m_animComp->GetSoldierController() : nullptr;
}

void SoldierObject::UseDecisionTreeDriver()
{
	// Swap out whatever driver is currently installed (typically the FSM
	// controller created in the ctor) — DT-driven soldiers author behavior in Lua.
	if (m_ai != nullptr)
		m_ai->SetDriverByType("dt");
}

DecisionTreeDriver* SoldierObject::GetDecisionTreeDriver() const
{
	return m_ai != nullptr ? m_ai->GetDecisionTreeDriver() : nullptr;
}

void SoldierObject::UseBehaviorTreeDriver()
{
	if (m_ai != nullptr)
		m_ai->SetDriverByType("bt");
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
	IAnimController* controller = m_animComp != nullptr ? m_animComp->GetController() : nullptr;
	if (!controller) return;
	controller->ClearAllActions();
	controller->SetLocomotionIntent(SoldierLocomotionIntent::Idle);
}

void SoldierObject::EnterMoveAnim()
{
	IAnimController* controller = m_animComp != nullptr ? m_animComp->GetController() : nullptr;
	if (!controller) return;
	controller->ClearAllActions();
	controller->SetLocomotionIntent(SoldierLocomotionIntent::Move);
}

void SoldierObject::EnterShootAnim()
{
	IAnimController* controller = m_animComp != nullptr ? m_animComp->GetController() : nullptr;
	if (!controller) return;
	controller->SetLocomotionIntent(SoldierLocomotionIntent::Idle);
	controller->RequestAction(SoldierActionIntent::Shoot, true);
}

void SoldierObject::EnterReloadAnim()
{
	IAnimController* controller = m_animComp != nullptr ? m_animComp->GetController() : nullptr;
	if (!controller) return;
	controller->SetLocomotionIntent(SoldierLocomotionIntent::Idle);
	controller->RequestAction(SoldierActionIntent::Reload, true);
}

void SoldierObject::EnterDeathAnim()
{
	IAnimController* controller = m_animComp != nullptr ? m_animComp->GetController() : nullptr;
	if (!controller) return;
	controller->RequestAction(SoldierActionIntent::Death, true);
}
