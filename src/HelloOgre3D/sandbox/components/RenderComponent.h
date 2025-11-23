#ifndef __RENDER_COMPONENT_H__
#define __RENDER_COMPONENT_H__

#include "OgreVector3.h"
#include <string>
#include <unordered_map>

namespace Ogre {
	class Entity;
	class SceneNode;
}

class AgentAnim;
class BaseObject;

class RenderComponent
{
public:
	RenderComponent(const Ogre::String& meshFile);
	RenderComponent(const Ogre::MeshPtr& meshPtr);
	RenderComponent(Ogre::SceneNode* pSceneNode);
	~RenderComponent();
	
	Ogre::Entity* GetEntity() const { return m_pEntity; }
	Ogre::Entity* GetDetachEntity();
	Ogre::SceneNode* GetSceneNode() const { return m_pSceneNode; }

	void SetOriginPos(const Ogre::Vector3& position) { m_originPos = position; }
	const Ogre::Vector3& GetOriginPos() const { return m_originPos; };
	
	void SetMaterial(const Ogre::String& materialName);

	void SetPosition(const Ogre::Vector3& position);
	void SetRotation(const Ogre::Vector3& rotation);
	void SetOrientation(const Ogre::Quaternion& quaternion);

	Ogre::Vector3 GetPosition() const;
	Ogre::Quaternion GetOrientation() const;

	void SetDerivedPosition(const Ogre::Vector3& position);
	void SetDerivedRotation(const Ogre::Vector3& rotation);
	void SetDerivedOrientation(const Ogre::Quaternion& quaternion);

	Ogre::Vector3 GetDerivedPosition() const;
	Ogre::Quaternion GetDerivedOrientation() const;

	void Update(int deltaInMillis);
	void AttachToBone(const Ogre::String& boneName, Ogre::Entity* entityObj, const Ogre::Vector3& positionOffset, const Ogre::Vector3& rotationOffset);

private:
	Ogre::SceneNode* m_pSceneNode = nullptr;
	Ogre::Entity* m_pEntity = nullptr;
	Ogre::Vector3 m_originPos = Ogre::Vector3::ZERO;
};

#endif // __RENDER_COMPONENT_H__
