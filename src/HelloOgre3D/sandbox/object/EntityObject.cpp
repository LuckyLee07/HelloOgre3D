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

using namespace Ogre;


EntityObject::EntityObject() : m_pSceneNode(nullptr), m_pEntity(nullptr)
{

}

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

	auto asmsIter = m_asms.begin();
	for (; asmsIter != m_asms.end(); asmsIter++)
	{
		delete asmsIter->second;
	}
	m_asms.clear();
	
	auto animIter = m_animations.begin();
	for (; animIter != m_animations.end(); animIter++)
	{
		delete animIter->second;
	}
	m_animations.clear();
}

void EntityObject::Initialize()
{

}

void EntityObject::update(int deltaInMillis)
{
	long long currTimeInMillis = GetGameManager()->getTimeInMillis();
	for (auto iter = m_asms.begin(); iter != m_asms.end(); iter++)
	{
		iter->second->Update(deltaInMillis, currTimeInMillis);
	}
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

Fancy::AnimationStateMachine* EntityObject::GetMoveASM()
{
	if (m_asms.find("move") == m_asms.end())
	{
		m_asms["move"] = new Fancy::AnimationStateMachine();
	}
	return m_asms["move"];
}

Fancy::AnimationStateMachine* EntityObject::GetWeaponASM()
{
	if (m_asms.find("weapon") == m_asms.end())
	{
		m_asms["weapon"] = new Fancy::AnimationStateMachine();
	}
	return m_asms["weapon"];
}