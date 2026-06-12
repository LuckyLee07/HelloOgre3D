#ifndef __RENDER_COMPONENT_H__
#define __RENDER_COMPONENT_H__

#include "component/IComponent.h"
#include "OgreVector3.h"
#include <string>
#include <unordered_map>

namespace Ogre {
	class Entity;
	class SceneNode;
}

class AgentAnim;
class BaseObject;

class RenderComponent : public IComponent
{
public:
	RenderComponent(const Ogre::String& meshFile);
	RenderComponent(const Ogre::MeshPtr& meshPtr);
	RenderComponent(Ogre::SceneNode* pSceneNode);
	virtual ~RenderComponent();
	
	Ogre::Entity* GetEntity() const { return m_pEntity; }
	Ogre::Entity* GetDetachEntity();
	Ogre::SceneNode* GetSceneNode() const { return m_pSceneNode; }

	void SetVisualOffset(const Ogre::Vector3& offset) { m_visualOffset = offset; }
	const Ogre::Vector3& GetVisualOffset() const { return m_visualOffset; };
	void SetOriginPos(const Ogre::Vector3& position) { SetVisualOffset(position); }
	const Ogre::Vector3& GetOriginPos() const { return GetVisualOffset(); };
	
	void SetMaterial(const Ogre::String& materialName);
	void SetVisible(bool visible);

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
	virtual void update(int deltaInMillis) override;
	void SyncFromOwnerTransform();
	void AttachToBone(const Ogre::String& boneName, Ogre::Entity* entityObj, const Ogre::Vector3& positionOffset, const Ogre::Vector3& rotationOffset);

private:
	Ogre::SceneNode* m_pSceneNode = nullptr;
	Ogre::Entity* m_pEntity = nullptr;
	Ogre::Vector3 m_visualOffset = Ogre::Vector3::ZERO;
};

#endif // __RENDER_COMPONENT_H__
