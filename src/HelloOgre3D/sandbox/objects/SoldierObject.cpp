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
#include <algorithm>
#include <limits>
#include <vector>

using namespace Ogre;

namespace
{
	bool IsCrouchAnimState(int stateId)
	{
		return stateId >= CROUCH_SSTATE_DEAD && stateId <= CROUCH_SSTATE_FORWARD;
	}
}

SoldierObject::SoldierObject(RenderableObject* pAgentBody, btRigidBody* pRigidBody/* = nullptr*/)
	: AgentObject(pAgentBody, pRigidBody), m_pWeapon(nullptr), m_stanceType(SOLDIER_STAND), m_maxHealth(100.0f), m_ammo(10), m_maxAmmo(10), m_enemy(nullptr), m_hasMovePos(false), m_movePos(Ogre::Vector3::ZERO), m_inputInfo(nullptr), m_stateController(nullptr), m_animController(nullptr)
{
	this->SetObjType(OBJ_TYPE_SOLDIER);

	m_maxHealth = std::max<Ogre::Real>(GetHealth(), 1.0f);
	m_maxAmmo = std::max(1, m_maxAmmo);
	m_ammo = std::min(std::max(0, m_ammo), m_maxAmmo);

	this->CreateEventDispatcher(); // 构造函数里使用虚函数会导致未定义

	if (GetUseCppFSM()) // 使用C++或者lua的FSM
	{
		m_stateController = new AgentStateController(this);
		m_stateController->Init();
	}

	m_animController = new SoldierAnimController(*this);
}

SoldierObject::~SoldierObject()
{
	SAFE_DELETE(m_pWeapon);
	SAFE_DELETE(m_inputInfo);
	SAFE_DELETE(m_stateController);
	SAFE_DELETE(m_animController);
}

void SoldierObject::CreateEventDispatcher()
{
	Event()->CreateDispatcher("ASM_STATE_CHANGE");
	Event()->CreateDispatcher("ASM_NOTIFY");
	Event()->Subscribe("ASM_STATE_CHANGE", [&](const SandboxContext& context) -> void {
		int stateId = (int)context.Get_Number("StateId");
		if (!GetUseCppFSM() && (stateId == SSTATE_FIRE || stateId == CROUCH_SSTATE_FIRE))
		{
			this->ShootBullet();
		}
		if (m_animController)
		{
			m_animController->OnBodyStateChanged(stateId);
		}
		if (!m_stateController) return;
		
		AgentState* pState = m_stateController->GetCurrState();
		if (!pState) return;
		pState->Event()->Emit("FSM_STATE_CHANGE", context);
	});
	Event()->Subscribe("ASM_NOTIFY", [&](const SandboxContext& context) -> void {
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
	if (m_animController)
		m_animController->Update((float)deltaMilisec);

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
	bullet->SetOwner(this);
	bullet->SetMass(0.1f);
	bullet->setPosition(position + forward * 0.2f);
	Ogre::Quaternion axisRot = Ogre::Quaternion(left, -forward, up);
	bullet->setOrientation(axisRot);
	
	Ogre::SceneNode* bulletParticle = SceneFactory::CreateParticle(bullet->GetSceneNode(), "Bullet");
	bulletParticle->setOrientation(QuaternionFromRotationDegrees(-90, 0, 0));
	bullet->addParticleNode(bulletParticle);

	bullet->applyImpulse(forward * 750);
}

void SoldierObject::SetMaxHealth(Ogre::Real maxHealth)
{
	m_maxHealth = std::max<Ogre::Real>(maxHealth, 1.0f);
}

void SoldierObject::SetAmmo(int ammo)
{
	m_ammo = std::min(std::max(0, ammo), m_maxAmmo);
}

void SoldierObject::SetMaxAmmo(int maxAmmo)
{
	m_maxAmmo = std::max(1, maxAmmo);
	if (m_ammo > m_maxAmmo)
	{
		m_ammo = m_maxAmmo;
	}
}

void SoldierObject::ConsumeAmmo(int amount)
{
	if (amount <= 0)
	{
		return;
	}

	SetAmmo(m_ammo - amount);
}

void SoldierObject::RestoreAmmo()
{
	m_ammo = m_maxAmmo;
}

bool SoldierObject::IsEnemyValid(AgentObject* enemy, const Ogre::String& navMeshName, bool requirePath) const
{
	if (!enemy || enemy == this)
	{
		return false;
	}

	if (enemy->GetHealth() <= 0.0f)
	{
		return false;
	}

	unsigned int enemyTeamId = enemy->GetTeamId();
	if (enemyTeamId == GetTeamId())
	{
		return false;
	}

	if (!requirePath || !g_SandboxMgr)
	{
		return true;
	}

	std::vector<Ogre::Vector3> path;
	return g_SandboxMgr->FindPath(navMeshName, GetPosition(), enemy->GetPosition(), path) && !path.empty();
}

AgentObject* SoldierObject::FindNearestEnemy(const Ogre::String& navMeshName)
{
	if (!g_ObjectManager)
	{
		return nullptr;
	}

	const std::vector<AgentObject*>& agents = g_ObjectManager->getAllAgents();

	AgentObject* nearestEnemy = nullptr;
	float nearestDistance = std::numeric_limits<float>::max();

	for (AgentObject* candidate : agents)
	{
		if (!IsEnemyValid(candidate, navMeshName, true))
		{
			continue;
		}

		const float distSquared = GetPosition().squaredDistance(candidate->GetPosition());
		if (distSquared >= nearestDistance)
		{
			continue;
		}

		nearestEnemy = candidate;
		nearestDistance = distSquared;
	}

	return nearestEnemy;
}

void SoldierObject::SetEnemy(AgentObject* enemy)
{
	m_enemy = enemy;
	m_enemyId = enemy ? static_cast<int>(enemy->GetObjId()) : -1;
}

AgentObject* SoldierObject::GetEnemy() const
{
	if (m_enemy == nullptr || m_enemyId < 0 || !g_ObjectManager)
	{
		return m_enemy;
	}

	BaseObject* object = g_ObjectManager->getObjectById(m_enemyId);
	return object == m_enemy ? dynamic_cast<AgentObject*>(object) : nullptr;
}

bool SoldierObject::HasEnemy(const Ogre::String& navMeshName)
{
	AgentObject* enemy = GetEnemy();
	if (IsEnemyValid(enemy, navMeshName, true))
	{
		return true;
	}

	SetEnemy(FindNearestEnemy(navMeshName));
	return GetEnemy() != nullptr;
}

bool SoldierObject::CanShootEnemy(const Ogre::String& navMeshName, float shootDistance)
{
	AgentObject* enemy = GetEnemy();
	if (!IsEnemyValid(enemy, navMeshName, false))
	{
		return false;
	}

	const float distance = std::max(0.0f, shootDistance);
	const float shootSquared = distance * distance;
	const float distSquared = GetPosition().squaredDistance(enemy->GetPosition());
	return distSquared < shootSquared;
}
bool SoldierObject::HasMovePosition(float reachDistance) const
{
	const float distance = std::max(0.0f, reachDistance);
	const float reachSquared = distance * distance;

	if (m_hasMovePos)
	{
		return GetPosition().squaredDistance(m_movePos) > reachSquared;
	}

	if (!HasPath())
	{
		return false;
	}

	const Ogre::Vector3 target = GetTarget();
	return GetPosition().squaredDistance(target) > reachSquared;
}

void SoldierObject::SetMovePosition(const Ogre::Vector3& movePos)
{
	m_movePos = movePos;
	m_hasMovePos = true;
}

void SoldierObject::ClearMovePosition()
{
	m_hasMovePos = false;
	m_movePos = Ogre::Vector3::ZERO;
}

bool SoldierObject::IsTargetReached(float threshold) const
{
	const float distance = std::max(0.0f, threshold);
	const float thresholdSquared = distance * distance;

	if (m_hasMovePos)
	{
		return GetPosition().squaredDistance(m_movePos) < thresholdSquared;
	}

	if (!HasPath())
	{
		return false;
	}

	const Ogre::Vector3 target = GetTarget();
	return GetPosition().squaredDistance(target) < thresholdSquared;
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

	const std::string moveStateName = AgentAnimState::GetNameByID(ConvertAnimID(SSTATE_RUN_FORWARD, getStanceType()));
	return pAsm->IsCurrentState(moveStateName) || pAsm->IsNextState(moveStateName);
}

bool SoldierObject::IsAnimReadyForShoot()
{
	AgentAnimStateMachine* pAsm = getBody()->GetObjectASM();
	if (pAsm == nullptr) return false;

	const std::string shootStateName = AgentAnimState::GetNameByID(ConvertAnimID(SSTATE_FIRE, getStanceType()));
	return pAsm->IsCurrentState(shootStateName) || pAsm->IsNextState(shootStateName);
}
