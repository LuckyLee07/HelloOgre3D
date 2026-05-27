#include "WeaponComponent.h"

#include <algorithm>

#include "GameFunction.h"
#include "SandboxMacros.h"
#include "OgreSceneNode.h"
#include "objects/BlockObject.h"
#include "objects/RenderableObject.h"
#include "objects/SoldierObject.h"
#include "objects/animation/AgentAnimStateMachine.h"
#include "objects/animation/SoldierAnimProfile.h"
#include "systems/manager/SandboxMgr.h"
#include "systems/service/SceneFactory.h"

WeaponComponent::WeaponComponent(SoldierObject* owner)
	: m_owner(owner)
	, m_weaponBody(nullptr)
	, m_ammo(10)
	, m_maxAmmo(10)
	, m_handOffsetPos(Ogre::Vector3::ZERO)
	, m_handOffsetOrientation(Ogre::Quaternion::IDENTITY)
{
}

WeaponComponent::~WeaponComponent()
{
	SAFE_DELETE(m_weaponBody);
}

void WeaponComponent::onAttach(BaseObject* owner)
{
	IComponent::onAttach(owner);
	if (m_owner == nullptr)
	{
		m_owner = dynamic_cast<SoldierObject*>(getOwner());
	}
}

void WeaponComponent::onDetach()
{
	m_owner = nullptr;
	IComponent::onDetach();
}

void WeaponComponent::update(int deltaMs)
{
	SyncToHandBone();
	if (m_weaponBody != nullptr)
	{
		m_weaponBody->Update(deltaMs);
	}
}

void WeaponComponent::Init(const Ogre::String& meshFile)
{
	SAFE_DELETE(m_weaponBody);

	m_weaponBody = new RenderableObject(meshFile);
	m_weaponBody->InitAsmWithOwner(m_owner, false);
	if (m_weaponBody->GetObjectASM())
	{
		m_weaponBody->GetObjectASM()->SetStateIdResolver(&SoldierAnimProfile::GetStateIdByName);
	}

	m_handOffsetPos = Ogre::Vector3(0.04f, 0.05f, -0.01f);
	m_handOffsetOrientation = QuaternionFromRotationDegrees(98.0f, 97.0f, 0.0f);
	SyncToHandBone();
}

void WeaponComponent::SyncToHandBone()
{
	if (m_owner == nullptr || m_weaponBody == nullptr || m_owner->getBody() == nullptr)
	{
		return;
	}

	Ogre::SceneNode* soldierNode = m_owner->getBody()->GetSceneNode();
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

	m_weaponBody->SetPosition(handPosition + (handOrientation * m_handOffsetPos));
	m_weaponBody->SetOrientation(handOrientation * m_handOffsetOrientation);
}

void WeaponComponent::ShootBullet()
{
	if (m_owner == nullptr || m_owner->getBody() == nullptr)
	{
		return;
	}

	Ogre::SceneNode* soldierNode = m_owner->getBody()->GetSceneNode();
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

	if ((!hasPosition || !hasOrientation) && m_weaponBody != nullptr)
	{
		Ogre::SceneNode* weaponNode = m_weaponBody->GetSceneNode();
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

void WeaponComponent::DoShootBullet(const Ogre::Vector3& position, const Ogre::Quaternion& orientation)
{
	if (m_owner == nullptr || g_SandboxMgr == nullptr)
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
		forward = m_owner->GetForward();
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

	BlockObject* bullet = g_SandboxMgr->CreateBullet(0.3f, 0.01f);
	if (bullet == nullptr)
	{
		return;
	}

	bullet->SetOwner(m_owner);
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
