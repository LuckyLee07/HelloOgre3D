#include "RenderComponent.h"
#include "OgreSceneNode.h"
#include "OgreEntity.h"
#include "OgreSceneManager.h"
#include "GameFunction.h"
#include "OgreManualObject.h"
#include "object/BaseObject.h"
#include "components/physics/PhysicsComponent.h"
#include "objects/animation/AgentAnim.h"
#include "objects/animation/AgentAnimStateMachine.h"
#include "systems/service/SceneFactory.h"

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
	if (m_pEntity != nullptr)
	{
		SceneManager* pSceneMananger = m_pSceneNode != nullptr ? m_pSceneNode->getCreator() : nullptr;
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
	const Ogre::Vector3 position = physics->GetPosition();
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
	m_pSceneNode->_setDerivedPosition(m_previousPosition + (m_currentPosition - m_previousPosition) * alpha + m_visualOffset);
	m_pSceneNode->_setDerivedOrientation(Ogre::Quaternion::Slerp(alpha, m_previousOrientation, m_currentOrientation, true));
}
