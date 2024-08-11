#include "EntityObject.h"
#include "OgreSceneNode.h"
#include "OgreEntity.h"
#include "OgreSceneManager.h"
#include "ClientManager.h"
#include "SandboxMgr.h"
#include "SandboxDef.h"

using namespace Ogre;

EntityObject::EntityObject(const Ogre::String& meshFile)
	: BaseObject(0, BaseObject::OBJ_SANDBOX_OBJ)
{
	SceneNode* pRootScene = GetClientMgr()->getRootSceneNode();

	m_pSceneNode = pRootScene->createChildSceneNode();
	m_pEntity = m_pSceneNode->getCreator()->createEntity(meshFile);
	m_pSceneNode->attachObject(m_pEntity);
}

EntityObject::~EntityObject()
{
	m_pEntity = nullptr;
	m_pSceneNode = nullptr;
}

void EntityObject::Initialize()
{

}

void EntityObject::update(int deltaMsec)
{

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

Ogre::Vector3 EntityObject::GetPosition() const
{
	return Ogre::Vector3::ZERO;
}

void EntityObject::AttachToBone(const Ogre::String& boneName, EntityObject* entityObj, const Ogre::Vector3& positionOffset, const Ogre::Vector3& rotationOffset)
{
	Ogre::Quaternion& orientationOffset = QuaternionFromRotationDegrees(rotationOffset.x, rotationOffset.y, rotationOffset.z);
	m_pEntity->attachObjectToBone(boneName, entityObj->getDetachEntity(), orientationOffset, positionOffset);
}

Ogre::Entity* EntityObject::getDetachEntity()
{
	m_pSceneNode->detachObject(m_pEntity);
	m_pSceneNode->getCreator()->destroySceneNode(m_pSceneNode);
	return m_pEntity;
}

Ogre::AnimationState* EntityObject::getAnimation(const Ogre::String& animationName)
{
	return m_pEntity->getAnimationState(animationName);
}
