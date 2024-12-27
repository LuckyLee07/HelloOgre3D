#include "EntityObject.h"
#include "OgreSceneNode.h"
#include "OgreEntity.h"
#include "OgreSceneManager.h"
#include "SandboxDef.h"
#include "manager/ClientManager.h"
#include "manager/SandboxMgr.h"
#include "animation/Animation.h"
#include "animation/AnimationStateMachine.h"
#include "game/GameManager.h"
#include "OgreManualObject.h"

using namespace Ogre;

EntityObject::EntityObject(const Ogre::String& meshFile)
{
	m_pAnimateStateMachine = nullptr;
	
	SceneNode* pRootScene = GetClientMgr()->getRootSceneNode();
	m_pSceneNode = pRootScene->createChildSceneNode();

	m_pEntity = m_pSceneNode->getCreator()->createEntity(meshFile);
	m_pSceneNode->attachObject(m_pEntity);
}

EntityObject::EntityObject(const Ogre::MeshPtr& meshPtr)
{
	m_pAnimateStateMachine = nullptr;

	SceneNode* pRootScene = GetClientMgr()->getRootSceneNode();
	m_pSceneNode = pRootScene->createChildSceneNode();

	m_pEntity = m_pSceneNode->getCreator()->createEntity(meshPtr);
	m_pSceneNode->attachObject(m_pEntity);
}

EntityObject::EntityObject(Ogre::SceneNode* pSceneNode)
{
	m_pAnimateStateMachine = nullptr;

	m_pSceneNode = pSceneNode;
	MovableObject* pObject = m_pSceneNode->getAttachedObject(0);
	m_pEntity = dynamic_cast<Ogre::Entity*>(pObject);
}

EntityObject::~EntityObject()
{
	SAFE_DELETE(m_pAnimateStateMachine);

	auto iter = m_animations.begin();
	for (; iter != m_animations.end(); iter++)
	{
		delete iter->second;
	}
	m_animations.clear();

	m_pSceneNode->getCreator()->destroyEntity(m_pEntity);
	m_pEntity = nullptr;

	SceneNode* pRootScene = GetClientMgr()->getRootSceneNode();
	pRootScene->removeChild(m_pSceneNode);
	m_pSceneNode = nullptr;
}

void EntityObject::Initialize()
{
	m_pAnimateStateMachine = new Fancy::AnimationStateMachine();
}

void EntityObject::update(int deltaInMillis)
{
	long long currTimeInMillis = GetGameManager()->getTimeInMillis();
	if (m_pAnimateStateMachine != nullptr)
		m_pAnimateStateMachine->Update(deltaInMillis, currTimeInMillis);
}

void EntityObject::setPosition(const Ogre::Vector3& position)
{
	m_pSceneNode->setPosition(position);
}

void EntityObject::setRotation(const Ogre::Vector3& rotation)
{
	Ogre::Quaternion qRotation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
	m_pSceneNode->setOrientation(qRotation);
}

void EntityObject::setOrientation(const Ogre::Quaternion& quaternion)
{
	m_pSceneNode->setOrientation(quaternion);
}

Ogre::Vector3 EntityObject::getPosition() const
{
	return m_pSceneNode->getPosition();
}

Ogre::Quaternion EntityObject::getOrientation() const
{
	return m_pSceneNode->getOrientation();
}

Ogre::Vector3 EntityObject::GetDerivedPosition() const
{
	return m_pSceneNode->_getDerivedPosition();
}

Ogre::Quaternion EntityObject::GetDerivedOrientation() const
{
	return m_pSceneNode->_getDerivedOrientation();
}

void EntityObject::setMaterial(const Ogre::String& materialName)
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

void EntityObject::AttachToBone(const Ogre::String& boneName, EntityObject* entityObj, const Ogre::Vector3& positionOffset, const Ogre::Vector3& rotationOffset)
{
	Ogre::Quaternion orientationOffset = QuaternionFromRotationDegrees(rotationOffset.x, rotationOffset.y, rotationOffset.z);
	m_pEntity->attachObjectToBone(boneName, entityObj->getDetachEntity(), orientationOffset, positionOffset);
}

Ogre::Entity* EntityObject::getDetachEntity()
{
	m_pSceneNode->detachObject(m_pEntity);
	m_pSceneNode->getCreator()->destroySceneNode(m_pSceneNode);
	return m_pEntity;
}

Fancy::Animation* EntityObject::GetAnimation(const char* animationName)
{
	auto iter = m_animations.find(animationName);
	if (iter != m_animations.end())
	{
		return iter->second;
	}

	auto pAnimState = m_pEntity->getAnimationState(animationName);
	if (pAnimState != nullptr)
	{
		m_animations[animationName] = new Fancy::Animation(pAnimState);
	}
	return m_animations[animationName];
}

Fancy::AnimationStateMachine* EntityObject::GetObjectASM()
{
	return m_pAnimateStateMachine;
}
