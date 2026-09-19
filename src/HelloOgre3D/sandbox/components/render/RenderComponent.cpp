#include "RenderComponent.h"
#include "OgreSceneNode.h"
#include "OgreEntity.h"
#include "OgreSkeletonInstance.h"
#include "OgreSceneManager.h"
#include "GameFunction.h"
#include "OgreManualObject.h"
#include "object/BaseObject.h"
#include "components/physics/PhysicsComponent.h"
#include "components/ai/AIController.h"
#include "objects/AgentObject.h"
#include "objects/animation/AgentAnim.h"
#include "objects/animation/AgentAnimStateMachine.h"
#include "systems/service/SceneFactory.h"
#include <cmath>
#include <memory>

using namespace Ogre;

RenderComponent::RenderComponent(const Ogre::String& meshFile)
	: m_visualOffset(Ogre::Vector3::ZERO)
{
	m_pSceneNode = SceneFactory::CreateChildSceneNode();

	m_pEntity = m_pSceneNode->getCreator()->createEntity(meshFile);
	m_pSceneNode->attachObject(m_pEntity);
}

RenderComponent::RenderComponent(const Ogre::MeshPtr& meshPtr)
	: m_visualOffset(Ogre::Vector3::ZERO)
{
	m_pSceneNode = SceneFactory::CreateChildSceneNode();

	m_pEntity = m_pSceneNode->getCreator()->createEntity(meshPtr);
	m_pSceneNode->attachObject(m_pEntity);
}

RenderComponent::RenderComponent(Ogre::SceneNode* pSceneNode)
	: m_visualOffset(Ogre::Vector3::ZERO)
{
	m_pSceneNode = pSceneNode;
	unsigned short attachNum = m_pSceneNode->numAttachedObjects();
	if (attachNum > 0)
	{
		auto pObject = m_pSceneNode->getAttachedObject(0);
		m_pEntity = dynamic_cast<Ogre::Entity*>(pObject);
	}
}

RenderComponent::~RenderComponent()
{
	SceneManager* pSceneMananger = m_pSceneNode != nullptr ? m_pSceneNode->getCreator() : nullptr;
	for (const OwnedBoneAttachment& attachment : m_ownedBoneAttachments)
	{
		delete attachment.render;
	}
	m_ownedBoneAttachments.clear();
	if (m_pEntity != nullptr)
	{
		if (pSceneMananger != nullptr)
		{
			pSceneMananger->destroyEntity(m_pEntity);
		}
		m_pEntity = nullptr;
	}
	if (m_pSceneNode != nullptr)
	{
		m_pSceneNode->removeAndDestroyAllChildren();
		m_pSceneNode->getCreator()->destroySceneNode(m_pSceneNode);
		m_pSceneNode = nullptr;
	}
}

void RenderComponent::SetMaterial(const Ogre::String& materialName)
{
	if (m_pSceneNode == nullptr) return;

	Ogre::SceneNode::ObjectIterator it = m_pSceneNode->getAttachedObjectIterator();

	while (it.hasMoreElements())
	{
		const Ogre::String movableType =
			it.current()->second->getMovableType();

		if (movableType == Ogre::EntityFactory::FACTORY_TYPE_NAME)
		{
			Ogre::Entity* const entity =
				static_cast<Ogre::Entity*>(it.current()->second);
			entity->setMaterialName(materialName);
		}
		else if (movableType ==
			Ogre::ManualObjectFactory::FACTORY_TYPE_NAME)
		{
			Ogre::ManualObject* const entity =
				static_cast<Ogre::ManualObject*>(it.current()->second);
			unsigned int sections = entity->getNumSections();

			for (unsigned int id = 0; id < sections; ++id)
			{
				entity->setMaterialName(id, materialName);
			}
		}

		it.getNext();
	}
}

void RenderComponent::SetVisible(bool visible)
{
	if (m_pSceneNode != nullptr)
	{
		m_pSceneNode->setVisible(visible, true);
	}
	if (m_pEntity != nullptr)
	{
		m_pEntity->setVisible(visible);
	}
	for (const OwnedBoneAttachment& attachment : m_ownedBoneAttachments)
	{
		attachment.render->SetVisible(visible);
	}
}

void RenderComponent::SetPosition(const Ogre::Vector3& position)
{
	m_pSceneNode->setPosition(position);
}

void RenderComponent::SetRotation(const Ogre::Vector3& rotation)
{
	Ogre::Quaternion qRotation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
	m_pSceneNode->setOrientation(qRotation);
}

void RenderComponent::SetOrientation(const Ogre::Quaternion& quaternion)
{
	m_pSceneNode->setOrientation(quaternion);
}

Ogre::Vector3 RenderComponent::GetPosition() const
{
	return m_pSceneNode->getPosition();
}

Ogre::Quaternion RenderComponent::GetOrientation() const
{
	return m_pSceneNode->getOrientation();
}

void RenderComponent::SetDerivedPosition(const Ogre::Vector3& position)
{
	m_pSceneNode->_setDerivedPosition(position);
}

void RenderComponent::SetDerivedRotation(const Ogre::Vector3& rotation)
{
	Ogre::Quaternion qRotation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
	m_pSceneNode->_setDerivedOrientation(qRotation);
}

void RenderComponent::SetDerivedOrientation(const Ogre::Quaternion& quaternion)
{
	m_pSceneNode->_setDerivedOrientation(quaternion);
}

Ogre::Vector3 RenderComponent::GetDerivedPosition() const
{
	return m_pSceneNode->_getDerivedPosition();
}

Ogre::Quaternion RenderComponent::GetDerivedOrientation() const
{
	return m_pSceneNode->_getDerivedOrientation();
}

Ogre::Entity* RenderComponent::GetDetachEntity()
{
	m_pSceneNode->detachObject(m_pEntity);
	//m_pSceneNode->getCreator()->destroySceneNode(m_pSceneNode);
	//m_pSceneNode = nullptr;

	return m_pEntity;
}

void RenderComponent::AttachToBone(const Ogre::String& boneName, Ogre::Entity* entity, const Ogre::Vector3& positionOffset, const Ogre::Vector3& rotationOffset)
{
	Ogre::Quaternion orientationOffset = QuaternionFromRotationDegrees(rotationOffset.x, rotationOffset.y, rotationOffset.z);
	m_pEntity->attachObjectToBone(boneName, entity, orientationOffset, positionOffset);
}

bool RenderComponent::AttachOwnedMeshToBone(const Ogre::String& meshFile, const Ogre::String& boneName, const Ogre::Vector3& positionOffset, const Ogre::Vector3& rotationOffset, bool followBoneOrientation)
{
	if (m_pEntity == nullptr || !m_pEntity->hasSkeleton() || !m_pEntity->getSkeleton()->hasBone(boneName))
		return false;

	std::unique_ptr<RenderComponent> attachment(new RenderComponent(meshFile));
	attachment->SetVisible(m_pEntity->isVisible());
	m_ownedBoneAttachments.push_back({ meshFile, boneName, positionOffset, rotationOffset, followBoneOrientation, attachment.get() });
	attachment.release();
	SyncOwnedBoneAttachments();
	return true;
}

void RenderComponent::CopyOwnedBoneAttachmentsTo(RenderComponent& target) const
{
	for (const OwnedBoneAttachment& attachment : m_ownedBoneAttachments)
	{
		target.AttachOwnedMeshToBone(attachment.meshFile, attachment.boneName, attachment.positionOffset, attachment.rotationOffset, attachment.followBoneOrientation);
	}
}

void RenderComponent::SyncOwnedBoneAttachments()
{
	if (m_ownedBoneAttachments.empty() || m_pEntity == nullptr || m_pSceneNode == nullptr)
		return;

	// Mirror the weapon's pose query: do not consume Ogre's render-frame skinning
	// cache before the locomotion layer has applied its displayed bone pose.
	Ogre::SkeletonInstance* skeleton = m_pEntity->getSkeleton();
	skeleton->setAnimationState(*m_pEntity->getAllAnimationStates());
	skeleton->_updateTransforms();
	for (const OwnedBoneAttachment& attachment : m_ownedBoneAttachments)
	{
		Ogre::Vector3 bonePosition;
		if (!SceneFactory::GetBonePosition(*m_pSceneNode, attachment.boneName, bonePosition))
			continue;
		// Body equipment can stay aligned to physical facing, while articulated
		// parts such as a weapon magazine need the bone's animated orientation.
		Ogre::Quaternion attachmentOrientation = m_pSceneNode->_getDerivedOrientation();
		if (attachment.followBoneOrientation && !SceneFactory::GetBoneOrientation(*m_pSceneNode, attachment.boneName, attachmentOrientation))
			continue;
		attachment.render->SetPosition(bonePosition + attachmentOrientation * attachment.positionOffset);
		const Ogre::Vector3& rotation = attachment.rotationOffset;
		attachment.render->SetOrientation(attachmentOrientation * QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z));
	}
}

void RenderComponent::Update(int deltaInMillis)
{
	update(deltaInMillis);
}

int RenderComponent::getUpdateOrder() const
{
	return ComponentUpdateOrder::Render;
}

void RenderComponent::update(int deltaInMillis)
{
	(void)deltaInMillis;
	SyncFromOwnerTransform();
}

void RenderComponent::SyncFromOwnerTransform()
{
	if (m_pSceneNode == nullptr)
	{
		return;
	}

	BaseObject* owner = getOwner();
	if (owner == nullptr)
	{
		return;
	}

	const PhysicsComponent* physics = owner->GetPhysicsComponent();
	if (physics == nullptr || physics->GetRigidBody() == nullptr)
	{
		return;
	}

	m_pSceneNode->_setDerivedPosition(physics->GetPosition() + m_visualOffset);
	m_pSceneNode->_setDerivedOrientation(physics->GetOrientation());
}

void RenderComponent::CaptureSimulationTransform()
{
	BaseObject* owner = getOwner();
	const PhysicsComponent* physics = owner != nullptr ? owner->GetPhysicsComponent() : nullptr;
	if (physics == nullptr || physics->GetRigidBody() == nullptr) { m_hasSimulationPose = false; return; }
	// Capture offset with the physical pose: a capsule resize moves both together.
	const Ogre::Vector3 position = physics->GetPosition() + m_visualOffset;
	const Ogre::Quaternion orientation = physics->GetOrientation();
	if (!m_hasSimulationPose || (position - m_currentPosition).squaredLength() > 4.0f)
	{
		m_currentPosition = position;
		m_currentOrientation = orientation;
	}
	m_previousPosition = m_currentPosition;
	m_previousOrientation = m_currentOrientation;
	m_currentPosition = position;
	m_currentOrientation = orientation;
	m_hasSimulationPose = true;
}

void RenderComponent::RenderInterpolated(float alpha)
{
	if (!m_hasSimulationPose || m_pSceneNode == nullptr) return;
	alpha = Ogre::Math::Clamp(alpha, 0.0f, 1.0f);
	// Display only. PhysicsComponent remains the position/orientation source for gameplay.
	Ogre::Vector3 position = m_previousPosition + (m_currentPosition - m_previousPosition) * alpha;
	Ogre::Quaternion orientation = Ogre::Quaternion::Slerp(alpha, m_previousOrientation, m_currentOrientation, true);
	BaseObject* owner = getOwner();
	AIController* ai = owner != nullptr ? owner->GetAIComponent() : nullptr;
	Blackboard* blackboard = ai != nullptr ? ai->GetBlackboard() : nullptr;
	if (blackboard != nullptr && blackboard->GetBool("crossfire.enabled"))
	{
		AgentObject* agent = dynamic_cast<AgentObject*>(owner);
		if (agent != nullptr && agent->GetHealth() <= 0.0f)
		{
			orientation = orientation * Ogre::Quaternion(Ogre::Degree(12.0f), Ogre::Vector3::UNIT_Z);
		}
		else if (agent != nullptr)
		{
			const int clockMs = blackboard->GetInt("crossfire.clockMs");
			if (!blackboard->GetBool("crossfire.sentinel"))
			{
				// The sample clock freezes while planning. Never advance cosmetic motion from wall time.
				position.y += 0.025f * std::sin(static_cast<float>(clockMs) * 0.0035f + owner->GetObjId() * 0.7f);
				Ogre::Vector3 velocity = orientation.Inverse() * agent->GetVelocity();
				velocity.y = 0.0f;
				const float speed = velocity.length();
				if (speed > 3.2f) velocity *= 3.2f / speed;
				orientation = orientation * Ogre::Quaternion(Ogre::Degree(velocity.z * (3.4f / 3.2f)), Ogre::Vector3::UNIT_X)
					* Ogre::Quaternion(Ogre::Degree(-velocity.x * (3.4f / 3.2f)), Ogre::Vector3::UNIT_Z);
			}
			if (blackboard->Has("crossfire.lastShotMs"))
			{
				const int shotAgeMs = clockMs - blackboard->GetInt("crossfire.lastShotMs");
				if (shotAgeMs >= 0 && shotAgeMs < 130)
				{
					const float recovery = 1.0f - static_cast<float>(shotAgeMs) / 130.0f;
					position -= agent->GetForward() * (0.055f * recovery * recovery);
				}
			}
		}
	}
	m_pSceneNode->_setDerivedPosition(position);
	m_pSceneNode->_setDerivedOrientation(orientation);
}
