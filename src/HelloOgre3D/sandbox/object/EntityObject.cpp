#include "EntityObject.h"
#include "OgreSceneNode.h"
#include "OgreEntity.h"
#include "OgreSceneManager.h"
#include "SandboxDef.h"
#include "manager/ClientManager.h"
#include "manager/SandboxMgr.h"
#include "animation/Animation.h"
#include "animation/AnimationStateMachine.h"

using namespace Ogre;


EntityObject::EntityObject() : m_pSceneNode(nullptr), m_pEntity(nullptr)
{
	m_asm = new Fancy::AnimationStateMachine();
}

EntityObject::EntityObject(const Ogre::String& meshFile)
	: BaseObject(0, BaseObject::OBJ_SANDBOX_OBJ)
{
	SceneNode* pRootScene = GetClientMgr()->getRootSceneNode();

	m_pSceneNode = pRootScene->createChildSceneNode();
	m_pEntity = m_pSceneNode->getCreator()->createEntity(meshFile);
	m_pSceneNode->attachObject(m_pEntity);

	m_asm = new Fancy::AnimationStateMachine();
}

EntityObject::~EntityObject()
{
	m_pEntity = nullptr;
	m_pSceneNode = nullptr;

	delete m_asm; //先清掉asm再清anim

	auto iter = m_animations.begin();
	for (; iter != m_animations.end(); iter++)
	{
		delete iter->second;
	}
	m_animations.clear();
}

void EntityObject::Initialize()
{

}

void EntityObject::update(int deltaMsec)
{
	m_asm->Update(deltaMsec);
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

Fancy::Animation* EntityObject::GetAnimation(const Ogre::String& animationName)
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

Fancy::AnimationStateMachine* EntityObject::GetAnimStateMachine()
{
	return m_asm;
}
