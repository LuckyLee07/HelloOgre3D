#include "WeaponComponent.h"

#include <algorithm>

#include "GameFunction.h"
#include "SandboxMacros.h"
#include "OgreSceneNode.h"
#include "core/SandboxServices.h"
#include "objects/BlockObject.h"
#include "objects/SoldierObject.h"
#include "objects/animation/AgentAnimStateMachine.h"
#include "objects/animation/SoldierAnimProfile.h"
#include "components/anim/AnimComponent.h"
#include "components/render/RenderComponent.h"
#include "systems/manager/SandboxMgr.h"
#include "systems/manager/ObjectManager.h"
#include "systems/service/SceneFactory.h"
#include "event/SandboxEventPayload.h"

namespace
{
	SandboxMgr* ResolveSandboxMgr(const WeaponComponent* component)
	{
		const SandboxServices* services = component != nullptr ? component->GetSandboxServices() : nullptr;
		if (services != nullptr && services->sandbox != nullptr)
			return services->sandbox;
		return g_SandboxMgr;
	}

	ObjectManager* ResolveObjectManager(const WeaponComponent* component)
	{
		const SandboxServices* services = component != nullptr ? component->GetSandboxServices() : nullptr;
		if (services != nullptr && services->objects != nullptr)
			return services->objects;
		return g_ObjectManager;
	}

	RenderComponent* FindOwnerRender(const WeaponComponent* component)
	{
		BaseObject* owner = component != nullptr ? component->getOwner() : nullptr;
		return owner != nullptr ? owner->FindComponent<RenderComponent>() : nullptr;
	}

	AnimComponent* FindOwnerAnim(const WeaponComponent* component)
	{
		BaseObject* owner = component != nullptr ? component->getOwner() : nullptr;
		return owner != nullptr ? owner->FindComponent<AnimComponent>() : nullptr;
	}
}

WeaponComponent::WeaponComponent(BaseObject* owner)
	: m_weaponRender(nullptr)
	, m_ammo(10)
	, m_maxAmmo(10)
	, m_handOffsetPos(Ogre::Vector3::ZERO)
	, m_handOffsetOrientation(Ogre::Quaternion::IDENTITY)
{
	(void)owner;
}

WeaponComponent::~WeaponComponent()
{
	SAFE_DELETE(m_weaponRender);
}

void WeaponComponent::onAttach(BaseObject* owner)
{
	IComponent::onAttach(owner);
}

void WeaponComponent::onDetach()
{
	IComponent::onDetach();
}

SoldierObject* WeaponComponent::GetSoldierOwner() const
{
	return dynamic_cast<SoldierObject*>(getOwner());
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
}

void WeaponComponent::Init(const Ogre::String& meshFile)
{
	SAFE_DELETE(m_weaponRender);

	m_weaponRender = new RenderComponent(meshFile);
	AnimComponent* anim = FindOwnerAnim(this);
	if (anim != nullptr)
	{
		anim->InitWeaponAnimations(m_weaponRender->GetEntity(), false);
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

	m_weaponRender->SetPosition(handPosition + (handOrientation * m_handOffsetPos));
	m_weaponRender->SetOrientation(handOrientation * m_handOffsetOrientation);
}

void WeaponComponent::ShootBullet()
{
	RenderComponent* ownerRender = FindOwnerRender(this);
	if (ownerRender == nullptr)
	{
		return;
	}

	Ogre::SceneNode* soldierNode = ownerRender->GetSceneNode();
	if (soldierNode == nullptr)
	{
		return;
	}

	Ogre::Vector3 position = soldierNode->_getDerivedPosition();
	Ogre::Quaternion orientation = soldierNode->_getDerivedOrientation();
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

	DoShootBullet(position, orientation);
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
	SandboxMgr* sandbox = ResolveSandboxMgr(this);
	SoldierObject* owner = GetSoldierOwner();
	if (owner == nullptr || sandbox == nullptr)
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
		forward = owner->GetForward();
	}
	forward.normalise();
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

	BlockObject* bullet = sandbox->CreateBullet(0.3f, 0.01f);
	if (bullet == nullptr)
	{
		return;
	}

	bullet->SetOwner(owner);
	bullet->SetMass(0.1f);
	bullet->setPosition(position + forward * 0.2f);
	Ogre::Quaternion axisRot = Ogre::Quaternion(left, -forward, up);
	bullet->setOrientation(axisRot);

	Ogre::SceneNode* bulletParticle = SceneFactory::CreateParticle(bullet->GetSceneNode(), "Bullet");
	if (bulletParticle != nullptr)
	{
		bulletParticle->setOrientation(QuaternionFromRotationDegrees(-90, 0, 0));
		bullet->addParticleNode(bulletParticle);
	}

	ObjectManager* objectManager = ResolveObjectManager(this);
	if (objectManager != nullptr)
	{
		objectManager->publishTacticalEvent(
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

	bullet->applyImpulse(forward * 750);
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
