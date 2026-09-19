#include "WeaponComponent.h"

#include <algorithm>
#include <cstdlib>
#include <limits>

#include "GameFunction.h"
#include "SandboxMacros.h"
#include "OgreParticleSystem.h"
#include "OgreEntity.h"
#include "OgreSkeletonInstance.h"
#include "OgreLogManager.h"
#include "OgreSceneNode.h"
#include "OgreStringConverter.h"
#include "ai/tactics/TacticalService.h"
#include "components/ai/AIController.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "core/SandboxServices.h"
#include "core/object/BaseObject.h"
#include "objects/BlockObject.h"
#include "objects/animation/AgentAnimStateMachine.h"
#include "objects/animation/SoldierAnimProfile.h"
#include "components/anim/AnimComponent.h"
#include "components/physics/PhysicsComponent.h"
#include "components/render/RenderComponent.h"
#include "systems/manager/ObjectManager.h"
#include "systems/service/ObjectFactory.h"
#include "systems/service/SceneFactory.h"
#include "event/SandboxEventPayload.h"

namespace
{
	void EvaluateAttachmentPose(Ogre::Entity* entity)
	{
		if (entity == nullptr || !entity->hasSkeleton()) return;
		// Bone queries need the current action pose, but must not consume Ogre's
		// once-per-render-frame skinning cache before the lower-body layer runs.
		Ogre::SkeletonInstance* skeleton = entity->getSkeleton();
		skeleton->setAnimationState(*entity->getAllAnimationStates());
		skeleton->_updateTransforms();
	}

	ObjectFactory* ResolveObjectFactory(const WeaponComponent* component)
	{
		const SandboxServices* services = component != nullptr ? component->GetSandboxServices() : nullptr;
		if (services != nullptr && services->objectFactory != nullptr)
			return services->objectFactory;
		return nullptr;
	}

	ObjectManager* ResolveObjectManager(const WeaponComponent* component)
	{
		const SandboxServices* services = component != nullptr ? component->GetSandboxServices() : nullptr;
		if (services != nullptr && services->objects != nullptr)
			return services->objects;
		return nullptr;
	}

	RenderComponent* FindOwnerRender(const WeaponComponent* component)
	{
		BaseObject* owner = component != nullptr ? component->getOwner() : nullptr;
		return owner != nullptr ? owner->FindComponent<RenderComponent>() : nullptr;
	}

	AnimComponent* FindOwnerAnim(const WeaponComponent* component)
	{
		BaseObject* owner = component != nullptr ? component->getOwner() : nullptr;
		return owner != nullptr ? owner->GetAnimComponent() : nullptr;
	}
}

CrossfireProjectileComponent::CrossfireProjectileComponent(unsigned int source, unsigned int team, float shotDamage, const Ogre::Vector3& shotDirection)
	: sourceId(source)
	, sourceTeam(team)
	, damage(shotDamage)
	, direction(shotDirection)
	, m_consumed(false)
	, m_remainingMs(4000)
{
}

bool CrossfireProjectileComponent::Consume()
{
	if (m_consumed) return false;
	m_consumed = true;
	if (getOwner() != nullptr) getOwner()->SetNeedClear(0, true);
	return true;
}

void CrossfireProjectileComponent::update(int deltaMs)
{
	m_remainingMs -= std::max(0, deltaMs);
	if (m_remainingMs <= 0) Consume();
}

WeaponComponent::WeaponComponent(BaseObject* owner)
	: m_weaponRender(nullptr)
	, m_weaponShellRender(nullptr)
	, m_ammo(10)
	, m_maxAmmo(10)
	, m_handOffsetPos(Ogre::Vector3::ZERO)
	, m_handOffsetOrientation(Ogre::Quaternion::IDENTITY)
	, m_commanderRifleShellEnabled(false)
{
	(void)owner;
}

WeaponComponent::~WeaponComponent()
{
	SAFE_DELETE(m_weaponShellRender);
	SAFE_DELETE(m_weaponRender);
}

void WeaponComponent::onAttach(BaseObject* owner)
{
	IComponent::onAttach(owner);
}

void WeaponComponent::onDetach()
{
	AnimComponent* anim = FindOwnerAnim(this);
	if (anim != nullptr) anim->ResetWeaponPresentation();
	IComponent::onDetach();
}

int WeaponComponent::getUpdateOrder() const
{
	return ComponentUpdateOrder::Weapon;
}

void WeaponComponent::update(int deltaMs)
{
	SyncToHandBone();
	if (m_weaponRender != nullptr)
	{
		m_weaponRender->Update(deltaMs);
	}
	AnimComponent* anim = FindOwnerAnim(this);
	if (anim != nullptr)
	{
		anim->UpdateWeaponAnimations(deltaMs);
	}
	if (m_weaponRender != nullptr)
	{
		m_weaponRender->SyncOwnedBoneAttachments();
	}
}

void WeaponComponent::Init(const Ogre::String& meshFile)
{
	AnimComponent* anim = FindOwnerAnim(this);
	if (anim != nullptr) anim->ResetWeaponPresentation();
	SAFE_DELETE(m_weaponShellRender);
	SAFE_DELETE(m_weaponRender);

	m_weaponRender = new RenderComponent(meshFile);
	if (m_commanderRifleShellEnabled)
	{
		m_weaponShellRender = new RenderComponent("models/sandbox19/commander_rifle_shell.mesh");
	}
	if (anim != nullptr)
	{
		anim->InitWeaponAnimations(m_weaponRender->GetEntity(), false);
	}
	if (m_commanderRifleShellEnabled)
	{
		m_weaponRender->AttachOwnedMeshToBone("models/sandbox19/commander_rifle_magazine.mesh", "b_Clip",
			Ogre::Vector3::ZERO, Ogre::Vector3::ZERO, true);
	}

	m_handOffsetPos = Ogre::Vector3(0.04f, 0.05f, -0.01f);
	m_handOffsetOrientation = QuaternionFromRotationDegrees(98.0f, 97.0f, 0.0f);
	SyncToHandBone();
}

void WeaponComponent::SetRenderVisible(bool visible)
{
	if (m_weaponRender != nullptr)
	{
		m_weaponRender->SetVisible(visible);
	}
	if (m_weaponShellRender != nullptr)
	{
		m_weaponShellRender->SetVisible(visible);
	}
}

void WeaponComponent::SyncToHandBone()
{
	RenderComponent* ownerRender = FindOwnerRender(this);
	if (m_weaponRender == nullptr || ownerRender == nullptr)
	{
		return;
	}

	Ogre::SceneNode* soldierNode = ownerRender->GetSceneNode();
	if (soldierNode == nullptr)
	{
		return;
	}

	// Bone queries can precede Ogre render traversal (e.g. a fire notify).
	// Evaluate the current animation before resolving the visual attachment.
	EvaluateAttachmentPose(ownerRender->GetEntity());
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

	const Ogre::Vector3 weaponPosition = handPosition + (handOrientation * m_handOffsetPos);
	const Ogre::Quaternion weaponOrientation = handOrientation * m_handOffsetOrientation;
	m_weaponRender->SetPosition(weaponPosition);
	m_weaponRender->SetOrientation(weaponOrientation);
	if (m_weaponShellRender != nullptr)
	{
		m_weaponShellRender->SetPosition(weaponPosition);
		m_weaponShellRender->SetOrientation(weaponOrientation);
	}
}

Ogre::Vector3 WeaponComponent::GetMuzzlePosition()
{
	Ogre::Vector3 position;
	Ogre::Quaternion orientation;
	if (ResolveMuzzleTransform(position, orientation)) return position;
	const float invalid = std::numeric_limits<float>::quiet_NaN();
	return Ogre::Vector3(invalid, invalid, invalid);
}

void WeaponComponent::ShootBullet()
{
	Ogre::Vector3 position;
	Ogre::Quaternion orientation;
	if (ResolveMuzzleTransform(position, orientation))
		DoShootBullet(position, orientation);
}

void WeaponComponent::ShootBulletTowards(const Ogre::Vector3& direction)
{
	Ogre::Vector3 position;
	Ogre::Quaternion orientation;
	if (!ResolveMuzzleTransform(position, orientation))
		return;

	Ogre::Vector3 aimDirection = direction;
	if (aimDirection.isNaN() || aimDirection.isZeroLength())
		return;
	aimDirection.normalise();
	orientation = Ogre::Vector3::UNIT_X.getRotationTo(aimDirection, Ogre::Vector3::UNIT_Y);
	DoShootBullet(position, orientation);
}

void WeaponComponent::ShootBulletAt(const Ogre::Vector3& worldTarget)
{
	Ogre::Vector3 position;
	Ogre::Quaternion orientation;
	if (worldTarget.isNaN() || !ResolveMuzzleTransform(position, orientation))
		return;
	Ogre::Vector3 direction = worldTarget - position;
	if (direction.isNaN() || direction.isZeroLength())
		return;
	direction.normalise();
	orientation = Ogre::Vector3::UNIT_X.getRotationTo(direction, Ogre::Vector3::UNIT_Y);
	DoShootBullet(position, orientation);
}

bool WeaponComponent::ResolveMuzzleTransform(Ogre::Vector3& position, Ogre::Quaternion& orientation)
{
	BaseObject* owner = getOwner();
	AIController* ai = owner != nullptr ? owner->GetAIComponent() : nullptr;
	Blackboard* blackboard = ai != nullptr ? ai->GetBlackboard() : nullptr;
	if (blackboard != nullptr && blackboard->GetBool("crossfire.enabled"))
	{
		RenderComponent* render = FindOwnerRender(this);
		PhysicsComponent* physics = owner->GetPhysicsComponent();
		if (render == nullptr || physics == nullptr || physics->GetRigidBody() == nullptr)
			return false;
		const Ogre::Vector3 muzzle = blackboard->Has("crossfire.muzzle")
			? blackboard->GetVec3("crossfire.muzzle") : Ogre::Vector3(0.0f, -0.02f, 0.90f);
		const Ogre::Quaternion bodyOrientation = physics->GetOrientation();
		position = physics->GetPosition() + render->GetVisualOffset() + bodyOrientation * muzzle;
		// Legacy weapons fire along local +X; these mesh profiles face +Z.
		orientation = Ogre::Vector3::UNIT_X.getRotationTo(bodyOrientation * Ogre::Vector3::UNIT_Z, Ogre::Vector3::UNIT_Y);
		return !position.isNaN();
	}

	SyncToHandBone();
	if (m_weaponRender != nullptr) EvaluateAttachmentPose(m_weaponRender->GetEntity());
	RenderComponent* ownerRender = FindOwnerRender(this);
	if (ownerRender == nullptr)
	{
		return false;
	}

	Ogre::SceneNode* soldierNode = ownerRender->GetSceneNode();
	if (soldierNode == nullptr)
	{
		return false;
	}

	position = soldierNode->_getDerivedPosition();
	orientation = soldierNode->_getDerivedOrientation();
	bool hasPosition = false;
	bool hasOrientation = false;

	hasPosition = SceneFactory::GetBonePosition(*soldierNode, "b_muzzle", position);
	hasOrientation = SceneFactory::GetBoneOrientation(*soldierNode, "b_muzzle", orientation);

	if ((!hasPosition || !hasOrientation) && m_weaponRender != nullptr)
	{
		Ogre::SceneNode* weaponNode = m_weaponRender->GetSceneNode();
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

	if (!hasPosition)
	{
		hasPosition = SceneFactory::GetBonePosition(*soldierNode, "b_RightHand", position);
	}
	if (!hasOrientation)
	{
		hasOrientation = SceneFactory::GetBoneOrientation(*soldierNode, "b_RightHand", orientation);
	}

	return true;
}

AgentAnim* WeaponComponent::GetAnimation(const char* animationName)
{
	AnimComponent* anim = FindOwnerAnim(this);
	return anim != nullptr ? anim->GetWeaponAnimation(animationName) : nullptr;
}

AgentAnimStateMachine* WeaponComponent::GetObjectASM() const
{
	AnimComponent* anim = FindOwnerAnim(this);
	return anim != nullptr ? anim->GetWeaponAsm() : nullptr;
}

void WeaponComponent::DoShootBullet(const Ogre::Vector3& position, const Ogre::Quaternion& orientation)
{
	ObjectFactory* objectFactory = ResolveObjectFactory(this);
	BaseObject* owner = getOwner();
	if (owner == nullptr || objectFactory == nullptr)
	{
		return;
	}

	Ogre::Quaternion qRotation = orientation;
	qRotation.normalise();

	Ogre::Vector3 forward = qRotation * Ogre::Vector3::UNIT_X;
	Ogre::Vector3 up = qRotation * Ogre::Vector3::UNIT_Y;
	Ogre::Vector3 left = qRotation * (-Ogre::Vector3::UNIT_Z);

	if (forward.isNaN() || forward.isZeroLength())
	{
		RenderComponent* ownerRender = FindOwnerRender(this);
		forward = ownerRender != nullptr ? ownerRender->GetDerivedOrientation() * Ogre::Vector3::UNIT_X : Ogre::Vector3::UNIT_X;
	}
	forward.normalise();
	if (std::getenv("HELLO_FX_TRACE") != nullptr)
		Ogre::LogManager::getSingleton().logMessage("[ShotTrace] owner=" + Ogre::StringConverter::toString(owner->GetObjId())
			+ " muzzle=" + Ogre::StringConverter::toString(position)
			+ " direction=" + Ogre::StringConverter::toString(forward));
	if (up.isNaN() || up.isZeroLength())
	{
		up = Ogre::Vector3::UNIT_Y;
	}
	up.normalise();
	left = up.crossProduct(forward);
	if (left.isNaN() || left.isZeroLength())
	{
		left = Ogre::Vector3::UNIT_X;
	}
	left.normalise();

	BlockObject* bullet = objectFactory->CreateBullet(WeaponProjectileGeometry::Height, WeaponProjectileGeometry::Radius);
	if (bullet == nullptr)
	{
		return;
	}

	AIController* ai = owner->GetAIComponent();
	Blackboard* blackboard = ai != nullptr ? ai->GetBlackboard() : nullptr;
	const bool crossfire = blackboard != nullptr && blackboard->GetBool("crossfire.enabled");
	if (crossfire)
	{
		const float defaultDamage = blackboard->GetBool("crossfire.sentinel") ? 12.0f : 10.0f;
		CrossfireProjectileComponent* projectile = new CrossfireProjectileComponent(owner->GetObjId(), owner->GetTeamId(),
			std::max(0.0f, blackboard->GetFloat("crossfire.damage", defaultDamage)), forward);
		if (!bullet->AddComponent("crossfire.projectile", projectile))
		{
			delete projectile;
			bullet->SetNeedClear(0, true);
			return;
		}
		bullet->SetTeamId(owner->GetTeamId());
		bullet->SetOwner(nullptr);
	}
	else
	{
		bullet->SetOwner(owner);
	}
	bullet->SetMass(0.1f);
	bullet->setPosition(position + forward * WeaponProjectileGeometry::SpawnOffset);
	Ogre::Quaternion axisRot = Ogre::Quaternion(left, -forward, up);
	bullet->setOrientation(axisRot);

	Ogre::SceneNode* bulletParticle = SceneFactory::CreateParticle(bullet->GetSceneNode(), "Bullet");
	if (bulletParticle != nullptr)
	{
		bulletParticle->setOrientation(QuaternionFromRotationDegrees(-90, 0, 0));
		bullet->addParticleNode(bulletParticle);
	}

	ObjectManager* objectManager = ResolveObjectManager(this);
	Ogre::SceneNode* muzzleFlash = objectManager != nullptr ? SceneFactory::CreateParticle("MuzzleFlash") : nullptr;
	if (muzzleFlash != nullptr)
	{
		muzzleFlash->setPosition(position + forward * 0.16f);
		muzzleFlash->setOrientation(qRotation);
		if (muzzleFlash->numAttachedObjects() > 0)
			static_cast<Ogre::ParticleSystem*>(muzzleFlash->getAttachedObject(0))->setEmitting(true);
		objectManager->markNodeRemInSeconds(muzzleFlash, 0.5f);
	}

	// The Bullet particle follows the physical projectile and ends on impact.
	// A separate fixed-length debug streak could cross nearby hit surfaces.

	TacticalService* tactics = objectManager != nullptr ? objectManager->GetTacticalService() : nullptr;
	if (tactics != nullptr)
	{
		tactics->publishTacticalEvent(
			SandboxEventTypes::BulletShot(),
			static_cast<int>(owner->GetObjId()),
			-1,
			owner->GetTeamId(),
			-1,
			position,
			0,
			"global",
			false);
	}

	// Use an explicit gameplay velocity. The previous 750 N.s impulse on a
	// 0.1 kg body produced roughly 7500 m/s, crossing the whole sample between
	// 30 Hz physics steps and making both the tracer and collisions unreliable.
	PhysicsComponent* bulletPhysics = bullet->GetPhysicsComponent();
	if (bulletPhysics != nullptr)
	{
		if (crossfire && bulletPhysics->GetRigidBody() != nullptr)
		{
			bulletPhysics->GetRigidBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
			bulletPhysics->GetRigidBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
		}
		bulletPhysics->SetVelocity(forward * 48.0f);
	}

	const char* replayPath = std::getenv("HELLO_INPUT_REPLAY");
	if (replayPath != nullptr && replayPath[0] != '\0')
	{
		Ogre::LogManager::getSingleton().logMessage("[WeaponShot] owner="
			+ Ogre::StringConverter::toString(owner->GetObjId())
			+ " team=" + Ogre::StringConverter::toString(owner->GetTeamId())
			+ " position=" + Ogre::StringConverter::toString(position)
			+ " forward=" + Ogre::StringConverter::toString(forward)
			+ " speed=48");
	}
}

void WeaponComponent::SetAmmo(int ammo)
{
	m_ammo = std::min(std::max(0, ammo), m_maxAmmo);
}

void WeaponComponent::SetMaxAmmo(int maxAmmo)
{
	m_maxAmmo = std::max(1, maxAmmo);
	if (m_ammo > m_maxAmmo)
	{
		m_ammo = m_maxAmmo;
	}
}

void WeaponComponent::ConsumeAmmo(int amount)
{
	if (amount <= 0)
	{
		return;
	}

	SetAmmo(m_ammo - amount);
}

void WeaponComponent::RestoreAmmo()
{
	m_ammo = m_maxAmmo;
}
