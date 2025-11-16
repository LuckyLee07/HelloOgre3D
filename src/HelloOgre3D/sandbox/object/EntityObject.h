#ifndef __Entity_OBJECT__
#define __Entity_OBJECT__

#include "object/BaseObject.h"
#include "OgreString.h"
#include "OgreMath.h"
#include "OgreVector3.h"
#include <unordered_map>

namespace Ogre {
	class Entity;
	class SceneNode;
}
class AgentAnim;
class AgentAnimStateMachine;
class RenderComponent;

class EntityObject : public BaseObject //tolua_exports
{ //tolua_exports
public:
	EntityObject(const Ogre::String& meshFile);
	EntityObject(const Ogre::MeshPtr& meshPtr);
	EntityObject(Ogre::SceneNode* pSceneNode);
	virtual ~EntityObject();

	void update(int deltaMsec) override;

	//BaseObject* getOwner() { return m_owner; }
	void InitAsmWithOwner(BaseObject* owner, bool canFireEvent = true);
	
	//tolua_begin
	Ogre::Vector3 getOriginPos() const;
	void setOriginPos(const Ogre::Vector3& position);
	void setMaterial(const Ogre::String& materialName);
	
	virtual void setPosition(const Ogre::Vector3& position);
	virtual void setRotation(const Ogre::Vector3& position);
	virtual void setOrientation(const Ogre::Quaternion& quaternion);

	virtual Ogre::Vector3 GetPosition() const;
	virtual Ogre::Quaternion GetOrientation() const;

	void AttachToBone(const Ogre::String& boneName, Ogre::Entity* pEntity, const Ogre::Vector3& positionOffset, const Ogre::Vector3& rotationOffset);

	AgentAnim* GetAnimation(const char* animationName);
	AgentAnimStateMachine* GetObjectASM();
	//tolua_end

	void SetDerivedPosition(const Ogre::Vector3& position);
	void SetDerivedRotation(const Ogre::Vector3& position);
	void SetDerivedOrientation(const Ogre::Quaternion& quaternion);

	Ogre::Vector3 GetDerivedPosition() const;
	Ogre::Quaternion GetDerivedOrientation() const;

	Ogre::Entity* getEntity();
	Ogre::Entity* getDetachEntity();
	Ogre::SceneNode* getSceneNode();

protected:
	RenderComponent* m_renderComp = nullptr;
}; //tolua_exports

#endif  // __Entity_OBJECT__
