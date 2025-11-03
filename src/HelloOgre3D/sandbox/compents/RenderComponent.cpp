#include "RenderComponent.h"
#include "manager/ClientManager.h"
#include "OgreSceneNode.h"
#include "OgreEntity.h"
#include "OgreSceneManager.h"
#include "GameFunction.h"
#include "OgreManualObject.h"

using namespace Ogre;

RenderComponent::RenderComponent(const Ogre::String& meshFile)
	: m_originPos(Ogre::Vector3::ZERO)
{
	SceneNode* pRootScene = GetClientMgr()->getRootSceneNode();
	m_pSceneNode = pRootScene->createChildSceneNode();

	m_pEntity = m_pSceneNode->getCreator()->createEntity(meshFile);
	m_pSceneNode->attachObject(m_pEntity);
}

RenderComponent::RenderComponent(const Ogre::MeshPtr& meshPtr)
	: m_originPos(Ogre::Vector3::ZERO)
{
	SceneNode* pRootScene = GetClientMgr()->getRootSceneNode();
	m_pSceneNode = pRootScene->createChildSceneNode();

	m_pEntity = m_pSceneNode->getCreator()->createEntity(meshPtr);
	m_pSceneNode->attachObject(m_pEntity);
}

RenderComponent::RenderComponent(Ogre::SceneNode* pSceneNode)
	: m_originPos(Ogre::Vector3::ZERO)
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
		SceneManager* pSceneMananger = GetClientMgr()->getSceneManager();
		pSceneMananger->destroyEntity(m_pEntity);
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

void RenderComponent::SetPosition(const Ogre::Vector3& position)
{
	if (m_pSceneNode)
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
	return m_pSceneNode ? m_pSceneNode->getPosition() : Ogre::Vector3::ZERO;
}

Ogre::Quaternion RenderComponent::GetOrientation() const
{
	return m_pSceneNode ? m_pSceneNode->getOrientation() : Ogre::Quaternion::ZERO;
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

void RenderComponent::AttachToBone(const Ogre::String& boneName, Ogre::Entity* entity, const Ogre::Vector3& positionOffset, const Ogre::Vector3& rotationOffset)
{
	Ogre::Quaternion orientationOffset = QuaternionFromRotationDegrees(rotationOffset.x, rotationOffset.y, rotationOffset.z);
	m_pEntity->attachObjectToBone(boneName, entity, orientationOffset, positionOffset);
}