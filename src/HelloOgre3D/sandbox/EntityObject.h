#ifndef __Entity_OBJECT__
#define __Entity_OBJECT__

#include "BaseObject.h"
#include "OgreString.h"
#include "OgreMath.h"

namespace Ogre {
	class SceneNode;
	class Entity;
	class AnimationState;
}

class EntityObject : public BaseObject //tolua_exports
{ //tolua_exports
public:
	EntityObject() : BaseObject() {}
	EntityObject(const Ogre::String& meshFile);

	virtual ~EntityObject();

	void update(int deltaMsec) override;

	//tolua_begin
	virtual void Initialize();
	
	virtual void setPosition(const Ogre::Vector3& position);
	virtual void setRotation(const Ogre::Vector3& position);
	virtual void setOrientation(const Ogre::Quaternion& quaternion);

	Ogre::Vector3 GetPosition() const;
	
	void AttachToBone(const Ogre::String& boneName, EntityObject* entityObj, const Ogre::Vector3& positionOffset, const Ogre::Vector3& rotationOffset);

	Ogre::AnimationState* getAnimation(const Ogre::String& animationName);
	//tolua_end

	Ogre::Entity* getEntity() { return m_pEntity; }
	Ogre::Entity* getDetachEntity();
	Ogre::SceneNode* getSceneNode() { return m_pSceneNode; }

protected:
	Ogre::SceneNode* m_pSceneNode;
	Ogre::Entity* m_pEntity;

}; //tolua_exports

#endif  // __Entity_OBJECT__
