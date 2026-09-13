#ifndef __RENDER_COMPONENT_H__
#define __RENDER_COMPONENT_H__

#include "component/IComponent.h"
#include "OgreVector3.h"
#include "OgreQuaternion.h"
#include <string>
#include <unordered_map>
#include <vector>

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
	virtual int getUpdateOrder() const override;
	virtual void update(int deltaInMillis) override;
	void SyncFromOwnerTransform();
	void CaptureSimulationTransform();
	void RenderInterpolated(float alpha);
	void FreezeInterpolation() { m_previousPosition = m_currentPosition; m_previousOrientation = m_currentOrientation; }
	void AttachToBone(const Ogre::String& boneName, Ogre::Entity* entityObj, const Ogre::Vector3& positionOffset, const Ogre::Vector3& rotationOffset);
	bool AttachOwnedMeshToBone(const Ogre::String& meshFile, const Ogre::String& boneName, const Ogre::Vector3& positionOffset, const Ogre::Vector3& rotationOffset);
	void CopyOwnedBoneAttachmentsTo(RenderComponent& target) const;
	void SyncOwnedBoneAttachments();

private:
	struct OwnedBoneAttachment
	{
		Ogre::String meshFile;
		Ogre::String boneName;
		Ogre::Vector3 positionOffset;
		Ogre::Vector3 rotationOffset;
		RenderComponent* render;
	};
	std::vector<OwnedBoneAttachment> m_ownedBoneAttachments;
	Ogre::SceneNode* m_pSceneNode = nullptr;
	Ogre::Entity* m_pEntity = nullptr;
	Ogre::Vector3 m_visualOffset = Ogre::Vector3::ZERO;
	Ogre::Vector3 m_previousPosition = Ogre::Vector3::ZERO;
	Ogre::Vector3 m_currentPosition = Ogre::Vector3::ZERO;
	Ogre::Quaternion m_previousOrientation = Ogre::Quaternion::IDENTITY;
	Ogre::Quaternion m_currentOrientation = Ogre::Quaternion::IDENTITY;
	bool m_hasSimulationPose = false;
};

#endif // __RENDER_COMPONENT_H__
