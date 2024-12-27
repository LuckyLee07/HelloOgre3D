#ifndef __Entity_OBJECT__
#define __Entity_OBJECT__

#include "BaseObject.h"
#include "OgreString.h"
#include "OgreMath.h"
#include <unordered_map>

namespace Ogre {
	class Entity;
	class SceneNode;
}
namespace Fancy{
	class Animation;
	class AnimationStateMachine;
};
class EntityObject : public BaseObject //tolua_exports
{ //tolua_exports
public:
	EntityObject();
	EntityObject(const Ogre::String& meshFile);

	virtual ~EntityObject();

	void update(int deltaMsec) override;

	//tolua_begin
	virtual void Initialize();
	
	virtual void setPosition(const Ogre::Vector3& position);
	virtual void setRotation(const Ogre::Vector3& position);
	virtual void setOrientation(const Ogre::Quaternion& quaternion);

	Ogre::Vector3 GetPosition() const;
	
	void setMaterial(const Ogre::String& materialName);

	void AttachToBone(const Ogre::String& boneName, EntityObject* entityObj, const Ogre::Vector3& positionOffset, const Ogre::Vector3& rotationOffset);

	Fancy::Animation* GetAnimation(const char* animationName);
	Fancy::AnimationStateMachine* GetObjectASM();
	//tolua_end

	Ogre::Entity* getEntity() { return m_pEntity; }
	Ogre::Entity* getDetachEntity();
	Ogre::SceneNode* getSceneNode() { return m_pSceneNode; }

protected:
	Ogre::SceneNode* m_pSceneNode;
	Ogre::Entity* m_pEntity;

	Fancy::AnimationStateMachine* m_pAnimateStateMachine;
	std::unordered_map<std::string, Fancy::Animation*> m_animations;
}; //tolua_exports

#endif  // __Entity_OBJECT__
