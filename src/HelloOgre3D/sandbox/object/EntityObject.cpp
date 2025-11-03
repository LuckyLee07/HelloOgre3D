#include "EntityObject.h"
#include "OgreSceneNode.h"
#include "OgreEntity.h"
#include "OgreSceneManager.h"
#include "GameDefine.h"
#include "GameFunction.h"
#include "manager/ClientManager.h"
#include "manager/SandboxMgr.h"
#include "animation/AgentAnim.h"
#include "animation/AgentAnimStateMachine.h"
#include "game/GameManager.h"

#include "compents/RenderComponent.h"

using namespace Ogre;

EntityObject::EntityObject(const Ogre::String& meshFile)
	: m_owner(nullptr), m_pAnimateStateMachine(nullptr)
{
	m_renderComp = new RenderComponent(meshFile);
}

EntityObject::EntityObject(const Ogre::MeshPtr& meshPtr)
	: m_owner(nullptr), m_pAnimateStateMachine(nullptr)
{
	m_renderComp = new RenderComponent(meshPtr);
}

EntityObject::EntityObject(Ogre::SceneNode* pSceneNode)
	: m_owner(nullptr), m_pAnimateStateMachine(nullptr)
{
	m_renderComp = new RenderComponent(pSceneNode);
}

EntityObject::~EntityObject()
{
	auto iter = m_animations.begin();
	for (; iter != m_animations.end(); iter++)
	{
		delete iter->second;
	}
	m_animations.clear();
	SAFE_DELETE(m_pAnimateStateMachine);

	SAFE_DELETE(m_renderComp);

	m_owner = nullptr;
}

void EntityObject::InitWithOwner(BaseObject *owner, bool canFireEvent)
{
	m_owner = owner != nullptr ? owner : this;
	m_pAnimateStateMachine = new AgentAnimStateMachine(m_owner);
	m_pAnimateStateMachine->SetCanFireEvent(canFireEvent);
}

void EntityObject::update(int deltaInMillis)
{
	long long currTimeInMillis = GetGameManager()->getTimeInMillis();
	if (m_pAnimateStateMachine != nullptr)
		m_pAnimateStateMachine->Update(deltaInMillis, currTimeInMillis);
}

void EntityObject::setPosition(const Ogre::Vector3& position)
{
	m_renderComp->SetPosition(position);
}

void EntityObject::setRotation(const Ogre::Vector3& rotation)
{
	m_renderComp->SetRotation(rotation);
}

void EntityObject::setOrientation(const Ogre::Quaternion& quaternion)
{
	m_renderComp->SetOrientation(quaternion);
}

Ogre::Vector3 EntityObject::GetPosition() const
{
	return m_renderComp->GetPosition();
}

Ogre::Quaternion EntityObject::GetOrientation() const
{
	return m_renderComp->GetOrientation();
}

void EntityObject::SetDerivedPosition(const Ogre::Vector3& position)
{
	
	m_renderComp->SetDerivedPosition(position);
}

void EntityObject::SetDerivedRotation(const Ogre::Vector3& rotation)
{
	
	m_renderComp->SetDerivedRotation(rotation);
}

void EntityObject::SetDerivedOrientation(const Ogre::Quaternion& quaternion)
{
	m_renderComp->SetDerivedOrientation(quaternion);
}

Ogre::Vector3 EntityObject::GetDerivedPosition() const
{
	return m_renderComp->GetDerivedPosition();
}

Ogre::Quaternion EntityObject::GetDerivedOrientation() const
{
	return m_renderComp->GetDerivedOrientation();
}

Ogre::Vector3 EntityObject::getOriginPos() const
{
	return m_renderComp->GetOriginPos();
}

void EntityObject::setOriginPos(const Ogre::Vector3& position)
{
	m_renderComp->SetOriginPos(position);
}

void EntityObject::setMaterial(const Ogre::String& materialName)
{
	m_renderComp->SetMaterial(materialName);
}

void EntityObject::AttachToBone(const Ogre::String& boneName, EntityObject* entityObj, const Ogre::Vector3& positionOffset, const Ogre::Vector3& rotationOffset)
{
	m_renderComp->AttachToBone(boneName, entityObj->getEntity(), positionOffset, rotationOffset);
}

Ogre::Entity* EntityObject::getEntity()
{
	return m_renderComp->GetEntity();
}

Ogre::SceneNode* EntityObject::getSceneNode()
{
	return m_renderComp->GetSceneNode();
}

AgentAnim* EntityObject::GetAnimation(const char* animationName)
{
	auto iter = m_animations.find(animationName);
	if (iter != m_animations.end())
	{
		return iter->second;
	}

	auto pAnimState = m_pEntity->getAnimationState(animationName);
	if (pAnimState != nullptr)
	{
		m_animations[animationName] = new AgentAnim(pAnimState);
	}
	return m_animations[animationName];
}

AgentAnimStateMachine* EntityObject::GetObjectASM()
{
	return m_pAnimateStateMachine;
}
