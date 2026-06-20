#include "SoldierObject.h"
#include "core/SandboxServices.h"
#include "systems/manager/ObjectManager.h"
#include "OgreMath.h"
#include "animation/AgentAnimStateMachine.h"
#include "ai/common/AICommand.h"
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
#include "components/agent/AgentAttrib.h"
#include "components/ai/AIController.h"
#include "components/combat/WeaponComponent.h"
#include "components/ComponentKeys.h"
#include "components/render/RenderComponent.h"
#include "LogSystem.h"
#include "profiling/Profile.h"
#include <algorithm>
#include <vector>

using namespace Ogre;

namespace
{
	bool IsCrouchAnimState(int stateId)
	{
		return stateId >= CROUCH_SSTATE_DEAD && stateId <= CROUCH_SSTATE_FORWARD;
	}
}

SoldierObject::SoldierObject(RenderComponent* renderComp, btRigidBody* pRigidBody/* = nullptr*/)
	: AgentObject(renderComp, pRigidBody), m_inputInfo(nullptr)
{
	this->SetObjType(OBJ_TYPE_SOLDIER);
	this->SetLuaScriptClassName(LuaClassNameTraits<SoldierObject>::value);
}

SoldierObject::~SoldierObject()
{
	RemoveComponent(ComponentKeys::Anim);
	SAFE_DELETE(m_inputInfo);
}

void SoldierObject::Init()
{
	AgentObject::Init();

	const SandboxServices* services = GetSandboxServices();
	InputManager* inputMgr = services != nullptr ? services->input : nullptr;
	if (inputMgr != nullptr)
	{
		m_inputInfo = new PlayerInput(inputMgr);
	}
}

AIController* SoldierObject::GetAIController() const
{
	return const_cast<AIController*>(FindComponent<AIController>());
}

AIController* SoldierObject::GetAI() const
{
	return GetAIController();
}

AnimComponent* SoldierObject::GetAnimComponent() const
{
	return const_cast<AnimComponent*>(FindComponent<AnimComponent>());
}

void SoldierObject::ApplyCommand(const AICommand& command)
{
	H3D_PROFILE_SCOPE("AICommand::Apply");
	switch (command.kind)
	{
	case AICommand::COMMAND_IDLE:
		ApplyIdleCommand();
		break;
	case AICommand::COMMAND_MOVE:
		ApplyMoveCommand();
		break;
	case AICommand::COMMAND_ATTACK:
		ApplyAttackCommand();
		break;
	case AICommand::COMMAND_RELOAD:
		ApplyReloadCommand();
		break;
	case AICommand::COMMAND_DIE:
		ApplyDeathCommand();
		break;
	case AICommand::COMMAND_FIRE_WEAPON:
		ApplyFireWeaponCommand();
		break;
	case AICommand::COMMAND_REQUEST_STATE:
		ApplyRequestStateCommand(command.stateId, command.forceUpdate);
		break;
	case AICommand::COMMAND_MOVE_TO:
		ApplyMoveToCommand(command.targetPosition);
		break;
	case AICommand::COMMAND_STOP:
		ApplyStopCommand();
		break;
	case AICommand::COMMAND_USE_SKILL:
	case AICommand::COMMAND_INTERACT:
		ReportUnsupportedCommand(command);
		break;
	case AICommand::COMMAND_NONE:
		break;
	default:
		ReportUnsupportedCommand(command);
		break;
	}
}

void SoldierObject::initWeapon(const Ogre::String& meshFile)
{
	WeaponComponent* weaponComp = getWeapon();
	if (weaponComp != nullptr)
	{
		weaponComp->Init(meshFile);
	}
}

WeaponComponent* SoldierObject::getWeapon()
{
	return FindComponent<WeaponComponent>();
}

void SoldierObject::SetRenderVisible(bool visible)
{
	AgentObject::SetRenderVisible(visible);
	WeaponComponent* weaponComp = getWeapon();
	if (weaponComp != nullptr)
	{
		weaponComp->SetRenderVisible(visible);
	}
}

int SoldierObject::getStanceType() const
{
	const AgentAttrib* attrib = FindComponent<AgentAttrib>();
	return attrib != nullptr ? attrib->GetStanceType() : SOLDIER_STAND;
}

void SoldierObject::Update(int deltaMilisec)
{
	H3D_PROFILE_SCOPE("SoldierObject::Update");
	// Keep body transform in sync with physics before animation/bone evaluation.
	this->updateWorldTransform();

	BaseObject::Update(deltaMilisec);
	TryApplyPendingStance();
}

void SoldierObject::TickAi(int deltaMilisec)
{
	AIController* ai = GetAIController();
	if (ai != nullptr)
		ai->TickAI(deltaMilisec);
}

void SoldierObject::SetAiTickInUpdateEnabled(bool enabled)
{
	AIController* ai = GetAIController();
	if (ai != nullptr)
		ai->SetTickInOwnerUpdateEnabled(enabled);
}

void SoldierObject::SyncWeaponToHandBone()
{
	WeaponComponent* weaponComp = getWeapon();
	if (weaponComp != nullptr)
		weaponComp->SyncToHandBone();
}

void SoldierObject::ShootBullet()
{
	ApplyCommand(AICommand::FireWeapon());
}

void SoldierObject::DoShootBullet(const Ogre::Vector3& position, const Ogre::Quaternion& orientation)
{
	WeaponComponent* weaponComp = getWeapon();
	if (weaponComp != nullptr)
		weaponComp->DoShootBullet(position, orientation);
}

void SoldierObject::SetMaxHealth(Ogre::Real maxHealth)
{
	AgentAttrib* attrib = FindComponent<AgentAttrib>();
	if (attrib != nullptr)
	{
		attrib->SetMaxHealth(maxHealth);
	}
}

Ogre::Real SoldierObject::GetMaxHealth() const
{
	const AgentAttrib* attrib = FindComponent<AgentAttrib>();
	return attrib != nullptr ? attrib->GetMaxHealth() : std::max<Ogre::Real>(GetHealth(), 1.0f);
}

void SoldierObject::SetAmmo(int ammo)
{
	WeaponComponent* weaponComp = getWeapon();
	if (weaponComp != nullptr)
	{
		weaponComp->SetAmmo(ammo);
	}
}

int SoldierObject::GetAmmo() const
{
	const WeaponComponent* weaponComp = FindComponent<WeaponComponent>();
	return weaponComp != nullptr ? weaponComp->GetAmmo() : 0;
}

void SoldierObject::SetMaxAmmo(int maxAmmo)
{
	WeaponComponent* weaponComp = getWeapon();
	if (weaponComp != nullptr)
	{
		weaponComp->SetMaxAmmo(maxAmmo);
	}
}

int SoldierObject::GetMaxAmmo() const
{
	const WeaponComponent* weaponComp = FindComponent<WeaponComponent>();
	return weaponComp != nullptr ? weaponComp->GetMaxAmmo() : 0;
}

bool SoldierObject::HasAmmo() const
{
	const WeaponComponent* weaponComp = FindComponent<WeaponComponent>();
	return weaponComp != nullptr && weaponComp->HasAmmo();
}

void SoldierObject::ConsumeAmmo(int amount)
{
	WeaponComponent* weaponComp = getWeapon();
	if (weaponComp != nullptr)
	{
		weaponComp->ConsumeAmmo(amount);
	}
}

void SoldierObject::RestoreAmmo()
{
	WeaponComponent* weaponComp = getWeapon();
	if (weaponComp != nullptr)
	{
		weaponComp->RestoreAmmo();
	}
}

AgentObject* SoldierObject::GetEnemy() const
{
	AIController* ai = GetAIController();
	return ai != nullptr ? ai->GetEnemy() : nullptr;
}

Ogre::Vector3 SoldierObject::GetBonePosition(const Ogre::String& boneName) const
{
	if (m_renderComp != nullptr)
	{
		Ogre::SceneNode* node = m_renderComp->GetSceneNode();
		Ogre::Vector3 position;
		if (node != nullptr && SceneFactory::GetBonePosition(*node, boneName, position))
			return position;
	}

	return GetPosition() + Ogre::Vector3(0.0f, GetHeight() * 0.5f, 0.0f);
}

Ogre::Vector3 SoldierObject::GetBoneForward(const Ogre::String& boneName) const
{
	if (m_renderComp != nullptr)
	{
		Ogre::SceneNode* node = m_renderComp->GetSceneNode();
		Ogre::Quaternion orientation;
		if (node != nullptr && SceneFactory::GetBoneOrientation(*node, boneName, orientation))
		{
			const Ogre::Vector3 rotation = QuaternionToRotationDegrees(orientation);
			const Ogre::Quaternion legacyRotation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
			Ogre::Vector3 forward = legacyRotation * Ogre::Vector3(0.0f, 0.0f, -1.0f);
			if (!forward.isNaN() && !forward.isZeroLength())
			{
				forward.normalise();
				return forward;
			}
		}
	}

	Ogre::Vector3 forward = GetForward();
	if (forward.isNaN() || forward.isZeroLength())
		return Ogre::Vector3::UNIT_Z;
	forward.normalise();
	return forward;
}

bool SoldierObject::HasEnemy(const Ogre::String& navMeshName)
{
	AIController* ai = GetAIController();
	return ai != nullptr && ai->HasEnemy(navMeshName);
}

bool SoldierObject::CanShootEnemy(const Ogre::String& navMeshName, float shootDistance)
{
	AIController* ai = GetAIController();
	return ai != nullptr && ai->CanShootEnemy(navMeshName, shootDistance);
}
bool SoldierObject::HasMovePosition(float reachDistance) const
{
	AIController* ai = GetAIController();
	return ai != nullptr && ai->HasMovePosition(reachDistance);
}

void SoldierObject::SetMovePosition(const Ogre::Vector3& movePos)
{
	AIController* ai = GetAIController();
	if (ai != nullptr)
		ai->SetMovePosition(movePos);
}

void SoldierObject::ClearMovePosition()
{
	AIController* ai = GetAIController();
	if (ai != nullptr)
		ai->ClearMovePosition();
}

bool SoldierObject::IsTargetReached(float threshold) const
{
	AIController* ai = GetAIController();
	return ai != nullptr && ai->IsTargetReached(threshold);
}
void SoldierObject::changeStanceType(int stanceType)
{
	AgentAttrib* attrib = FindComponent<AgentAttrib>();
	if (attrib == nullptr)
		return;

	AgentAnimStateMachine* pAsm = GetObjectASM();
	if (pAsm == nullptr) return;

	SOLDIER_STATE currState = (SOLDIER_STATE)pAsm->GetCurrStateID();
	SOLDIER_STATE requestState = (SOLDIER_STATE)ConvertAnimID(currState, stanceType);
	if (currState == requestState)
		return;

	if (stanceType == SOLDIER_STAND)
	{
		attrib->SetPendingStanceType(SOLDIER_STAND);
	}
	else if (stanceType == SOLDIER_CROUCH)
	{
		attrib->SetPendingStanceType(SOLDIER_CROUCH);
	}
	else
	{
		return;
	}

	TryApplyPendingStance();
}

void SoldierObject::ApplyStanceParams(int stanceType)
{
	AgentAttrib* attrib = FindComponent<AgentAttrib>();
	float soldier_height = 0.0f;
	float soldier_speed = 0.0f;
	if (stanceType == SOLDIER_STAND)
	{
		if (attrib != nullptr)
			attrib->SetStanceType(SOLDIER_STAND);
		soldier_height = SOLDIER_STAND_HEIGHT;
		soldier_speed = SOLDIER_STAND_SPEED;
	}
	else if (stanceType == SOLDIER_CROUCH)
	{
		if (attrib != nullptr)
			attrib->SetStanceType(SOLDIER_CROUCH);
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
	AgentAttrib* attrib = FindComponent<AgentAttrib>();
	if (attrib == nullptr)
	{
		return;
	}

	const int pendingStanceType = attrib->GetPendingStanceType();
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
			attrib->ClearPendingStanceType();
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
	ApplyCommand(AICommand::RequestState(soldierState, forceUpdate));
}

void SoldierObject::ApplyRequestStateCommand(int soldierState, bool forceUpdate)
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
	AnimComponent* animComp = GetAnimComponent();
	return animComp != nullptr && animComp->HasNextAnim();
}

bool SoldierObject::IsAnimReadyForMove()
{
	AnimComponent* animComp = GetAnimComponent();
	return animComp != nullptr && animComp->IsAnimReadyForMove();
}

bool SoldierObject::IsAnimReadyForShoot()
{
	AnimComponent* animComp = GetAnimComponent();
	return animComp != nullptr && animComp->IsAnimReadyForShoot();
}

SoldierAnimController* SoldierObject::GetAnimController() const
{
	AnimComponent* animComp = GetAnimComponent();
	return animComp != nullptr ? animComp->GetSoldierController() : nullptr;
}

AgentStateController* SoldierObject::GetFsmController() const
{
	AIController* ai = GetAIController();
	return ai != nullptr ? ai->GetFsmController() : nullptr;
}

BaseObject* SoldierObject::GetAnimOwnerObject()
{
	return this;
}

AgentAnimStateMachine* SoldierObject::GetBodyAnimStateMachine() const
{
	return GetObjectASM();
}

AgentAnimStateMachine* SoldierObject::GetWeaponAnimStateMachine() const
{
	const WeaponComponent* weaponComp = FindComponent<WeaponComponent>();
	return weaponComp != nullptr ? weaponComp->GetObjectASM() : nullptr;
}

int SoldierObject::GetAnimStanceType() const
{
	return getStanceType();
}

bool SoldierObject::IsCppAnimControllerEnabled() const
{
	return const_cast<SoldierObject*>(this)->GetUseCppFSM();
}

void SoldierObject::ExecuteAnimShoot()
{
	ShootBullet();
}

AgentStateController* SoldierObject::GetAnimFsmController() const
{
	return GetFsmController();
}

void SoldierObject::EnterIdleAnim()
{
	ApplyCommand(AICommand::Idle());
}

void SoldierObject::ApplyIdleCommand()
{
	AnimComponent* animComp = GetAnimComponent();
	IAnimController* controller = animComp != nullptr ? animComp->GetController() : nullptr;
	if (!controller) return;
	controller->ClearAllActions();
	controller->SetLocomotionIntent(SoldierLocomotionIntent::Idle);
}

void SoldierObject::EnterMoveAnim()
{
	ApplyCommand(AICommand::Move());
}

void SoldierObject::ApplyMoveCommand()
{
	AnimComponent* animComp = GetAnimComponent();
	IAnimController* controller = animComp != nullptr ? animComp->GetController() : nullptr;
	if (!controller) return;
	controller->ClearAllActions();
	controller->SetLocomotionIntent(SoldierLocomotionIntent::Move);
}

void SoldierObject::EnterShootAnim()
{
	ApplyCommand(AICommand::Attack());
}

void SoldierObject::ApplyAttackCommand()
{
	AnimComponent* animComp = GetAnimComponent();
	IAnimController* controller = animComp != nullptr ? animComp->GetController() : nullptr;
	if (!controller) return;
	controller->SetLocomotionIntent(SoldierLocomotionIntent::Idle);
	controller->RequestAction(SoldierActionIntent::Shoot, true);
}

void SoldierObject::EnterReloadAnim()
{
	ApplyCommand(AICommand::Reload());
}

void SoldierObject::ApplyReloadCommand()
{
	AnimComponent* animComp = GetAnimComponent();
	IAnimController* controller = animComp != nullptr ? animComp->GetController() : nullptr;
	if (!controller) return;
	controller->SetLocomotionIntent(SoldierLocomotionIntent::Idle);
	controller->RequestAction(SoldierActionIntent::Reload, true);
}

void SoldierObject::EnterDeathAnim()
{
	ApplyCommand(AICommand::Die());
}

void SoldierObject::ApplyDeathCommand()
{
	AnimComponent* animComp = GetAnimComponent();
	IAnimController* controller = animComp != nullptr ? animComp->GetController() : nullptr;
	if (!controller) return;
	controller->RequestAction(SoldierActionIntent::Death, true);
}

void SoldierObject::ApplyFireWeaponCommand()
{
	WeaponComponent* weaponComp = getWeapon();
	if (weaponComp != nullptr)
		weaponComp->ShootBullet();
}

void SoldierObject::ApplyMoveToCommand(const Ogre::Vector3& targetPosition)
{
	AIController* ai = GetAIController();
	if (ai != nullptr)
	{
		ai->SetMovePosition(targetPosition);
		Blackboard* blackboard = ai->GetBlackboard();
		if (blackboard != nullptr)
			blackboard->SetVec3("movePos", targetPosition);
	}

	bool plannedPath = false;
	AgentStateController* fsm = ai != nullptr ? ai->GetFsmController() : nullptr;
	if (fsm != nullptr)
		plannedPath = fsm->PlanPathTo(targetPosition, false);
	if (!plannedPath)
		SetTarget(targetPosition);

	ApplyMoveCommand();
}

void SoldierObject::ApplyStopCommand()
{
	AIController* ai = GetAIController();
	if (ai != nullptr)
	{
		ai->ClearMovePosition();
		Blackboard* blackboard = ai->GetBlackboard();
		if (blackboard != nullptr)
			blackboard->Remove("movePos");
	}

	std::vector<Ogre::Vector3> emptyPath;
	SetPath(emptyPath, false);
	SetVelocity(Ogre::Vector3::ZERO);
	SetTarget(GetPosition());
	ApplyIdleCommand();
}

void SoldierObject::ReportUnsupportedCommand(const AICommand& command) const
{
	CCLOG_INFO("SoldierObject::ApplyCommand unsupported command: %s", AICommand::KindToString(command.kind));
}
