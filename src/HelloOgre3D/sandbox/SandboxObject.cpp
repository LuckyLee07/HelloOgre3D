#include "SandboxObject.h"
#include "OgreSceneNode.h"
#include "OgreEntity.h"
#include "OgreSceneManager.h"
#include "ClientManager.h"

using namespace Ogre;

SandboxObject::SandboxObject(unsigned int objectId, const Ogre::String& meshFile)
	: BaseObject(objectId, BaseObject::OBJ_SANDBOX_OBJ)
{
	SceneNode* pRootScene = GetClientMgr()->getRootSceneNode();
	m_pSceneNode = pRootScene->createChildSceneNode();
	m_pEntity = m_pSceneNode->getCreator()->createEntity(meshFile);
	m_pSceneNode->attachObject(m_pEntity);
}

SandboxObject::~SandboxObject()
{
}

void SandboxObject::Initialize()
{

}


void SandboxObject::setMass(const Ogre::Real mass)
{

}

void SandboxObject::setPosition(const Ogre::Vector3& position)
{
	m_pSceneNode->setPosition(position);
}

void SandboxObject::setOrientation(const Ogre::Quaternion& quaternion)
{

}