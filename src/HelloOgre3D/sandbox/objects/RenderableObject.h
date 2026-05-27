#ifndef __RENDERABLE_OBJECT__
#define __RENDERABLE_OBJECT__

#include "object/BaseObject.h"
#include "OgreString.h"
#include "OgreMath.h"
#include "OgreVector3.h"
#include <string>
#include <unordered_map>

namespace Ogre {
	class Entity;
	class SceneNode;
}
class AgentAnim;
class RenderComponent;
class AgentAnimStateMachine;

class RenderableObject //tolua_exports
{ //tolua_exports
public:
	RenderableObject(const Ogre::String& meshFile);
	RenderableObject(const Ogre::MeshPtr& meshPtr);
	RenderableObject(Ogre::SceneNode* pSceneNode);
	virtual ~RenderableObject();

	virtual void Update(int deltaMsec);

	//BaseObject* getOwner() { return m_owner; }
	void InitAsmWithOwner(BaseObject* owner, bool canFireEvent = true);
	bool AttachRenderComponent(BaseObject* owner, const std::string& key = "render");
	RenderComponent* GetRenderComponent() const { return m_renderComp; }
	
	Ogre::Vector3 GetOriginPos() const;
	void SetOriginPos(const Ogre::Vector3& position);
	Ogre::Vector3 GetVisualOffset() const;
	void SetVisualOffset(const Ogre::Vector3& offset);
	void SetMaterial(const Ogre::String& materialName);
	
	virtual void SetPosition(const Ogre::Vector3& position);
	virtual void SetRotation(const Ogre::Vector3& position);
	virtual void SetOrientation(const Ogre::Quaternion& quaternion);

	virtual Ogre::Vector3 GetPosition() const;
	virtual Ogre::Quaternion GetOrientation() const;

	void AttachToBone(const Ogre::String& boneName, Ogre::Entity* pEntity, const Ogre::Vector3& positionOffset, const Ogre::Vector3& rotationOffset);

	//tolua_begin
	AgentAnim* GetAnimation(const char* animationName);
	AgentAnimStateMachine* GetObjectASM();
	//tolua_end

	void SetDerivedPosition(const Ogre::Vector3& position);
	void SetDerivedRotation(const Ogre::Vector3& position);
	void SetDerivedOrientation(const Ogre::Quaternion& quaternion);
	void SyncWorldTransform();

	Ogre::Vector3 GetDerivedPosition() const;
	Ogre::Quaternion GetDerivedOrientation() const;

	Ogre::Entity* GetEntity();
	Ogre::Entity* GetDetachEntity();
	Ogre::SceneNode* GetSceneNode();

protected:
	RenderComponent* m_renderComp = nullptr;
	bool m_ownsRenderComp = true;

	std::unordered_map<std::string, AgentAnim*> m_animations;
	AgentAnimStateMachine* m_pAnimateStateMachine = nullptr;
	
}; //tolua_exports

#endif  // __RENDERABLE_OBJECT__
