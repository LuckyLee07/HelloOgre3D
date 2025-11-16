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

#include "components/RenderComponent.h"

using namespace Ogre;

EntityObject::EntityObject(const Ogre::String& meshFile)
{
	m_renderComp = new RenderComponent(meshFile);
}

EntityObject::EntityObject(const Ogre::MeshPtr& meshPtr)
{
	m_renderComp = new RenderComponent(meshPtr);
}

EntityObject::EntityObject(Ogre::SceneNode* pSceneNode)
{
	m_renderComp = new RenderComponent(pSceneNode);
}

EntityObject::~EntityObject()
{
	SAFE_DELETE(m_renderComp);
}

void EntityObject::InitAsmWithOwner(BaseObject *owner, bool canFireEvent)
{
	m_renderComp->CreateAnimationFSM(owner, canFireEvent);
}

void EntityObject::update(int deltaInMillis)
{
	m_renderComp->Update(deltaInMillis);
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

void EntityObject::AttachToBone(const Ogre::String& boneName, Ogre::Entity* pEntity, const Ogre::Vector3& positionOffset, const Ogre::Vector3& rotationOffset)
{
	m_renderComp->AttachToBone(boneName, pEntity, positionOffset, rotationOffset);
}

Ogre::Entity* EntityObject::getEntity()
{
	return m_renderComp->GetEntity();
}

Ogre::Entity* EntityObject::getDetachEntity()
{
	return m_renderComp->GetDetachEntity();
}

Ogre::SceneNode* EntityObject::getSceneNode()
{
	return m_renderComp->GetSceneNode();
}

AgentAnim* EntityObject::GetAnimation(const char* animationName)
{
	return m_renderComp->GetAnimation(animationName);
}

AgentAnimStateMachine* EntityObject::GetObjectASM()
{
	return m_renderComp->GetAnimationFSM();
}
