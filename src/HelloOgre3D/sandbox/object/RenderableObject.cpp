#include "RenderableObject.h"
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

RenderableObject::RenderableObject(const Ogre::String& meshFile)
{
	m_renderComp = new RenderComponent(meshFile);
}

RenderableObject::RenderableObject(const Ogre::MeshPtr& meshPtr)
{
	m_renderComp = new RenderComponent(meshPtr);
}

RenderableObject::RenderableObject(Ogre::SceneNode* pSceneNode)
{
	m_renderComp = new RenderComponent(pSceneNode);
}

RenderableObject::~RenderableObject()
{
	SAFE_DELETE(m_renderComp);
}

void RenderableObject::InitAsmWithOwner(BaseObject *owner, bool canFireEvent)
{
	m_renderComp->CreateAnimationFSM(owner, canFireEvent);
}

void RenderableObject::Update(int deltaInMillis)
{
	m_renderComp->Update(deltaInMillis);
}

void RenderableObject::SetPosition(const Ogre::Vector3& position)
{
	m_renderComp->SetPosition(position);
}

void RenderableObject::SetRotation(const Ogre::Vector3& rotation)
{
	m_renderComp->SetRotation(rotation);
}

void RenderableObject::SetOrientation(const Ogre::Quaternion& quaternion)
{
	m_renderComp->SetOrientation(quaternion);
}

Ogre::Vector3 RenderableObject::GetPosition() const
{
	return m_renderComp->GetPosition();
}

Ogre::Quaternion RenderableObject::GetOrientation() const
{
	return m_renderComp->GetOrientation();
}

void RenderableObject::SetDerivedPosition(const Ogre::Vector3& position)
{
	
	m_renderComp->SetDerivedPosition(position);
}

void RenderableObject::SetDerivedRotation(const Ogre::Vector3& rotation)
{
	
	m_renderComp->SetDerivedRotation(rotation);
}

void RenderableObject::SetDerivedOrientation(const Ogre::Quaternion& quaternion)
{
	m_renderComp->SetDerivedOrientation(quaternion);
}

Ogre::Vector3 RenderableObject::GetDerivedPosition() const
{
	return m_renderComp->GetDerivedPosition();
}

Ogre::Quaternion RenderableObject::GetDerivedOrientation() const
{
	return m_renderComp->GetDerivedOrientation();
}

Ogre::Vector3 RenderableObject::GetOriginPos() const
{
	return m_renderComp->GetOriginPos();
}

void RenderableObject::SetOriginPos(const Ogre::Vector3& position)
{
	m_renderComp->SetOriginPos(position);
}

void RenderableObject::SetMaterial(const Ogre::String& materialName)
{
	m_renderComp->SetMaterial(materialName);
}

void RenderableObject::AttachToBone(const Ogre::String& boneName, Ogre::Entity* pEntity, const Ogre::Vector3& positionOffset, const Ogre::Vector3& rotationOffset)
{
	m_renderComp->AttachToBone(boneName, pEntity, positionOffset, rotationOffset);
}

Ogre::Entity* RenderableObject::GetEntity()
{
	return m_renderComp->GetEntity();
}

Ogre::Entity* RenderableObject::GetDetachEntity()
{
	return m_renderComp->GetDetachEntity();
}

Ogre::SceneNode* RenderableObject::GetSceneNode()
{
	return m_renderComp->GetSceneNode();
}

AgentAnim* RenderableObject::GetAnimation(const char* animationName)
{
	return m_renderComp->GetAnimation(animationName);
}

AgentAnimStateMachine* RenderableObject::GetObjectASM()
{
	return m_renderComp->GetAnimationFSM();
}
