#ifndef __Entity_OBJECT__
#define __Entity_OBJECT__

#include "BaseObject.h"
#include "OgreString.h"
#include "OgreMath.h"

namespace Ogre {
	class SceneNode;
	class Entity;
}

class btRigidBody;
class EntityObject : public BaseObject //tolua_exports
{ //tolua_exports
public:
	EntityObject() : BaseObject() {}
	EntityObject(const Ogre::String& meshFile);

	virtual ~EntityObject();

	//tolua_begin
	virtual void Initialize();
	
	virtual btRigidBody* getRigidBody() { return nullptr; }
	Ogre::SceneNode* getSceneNode() { return m_pSceneNode; }

	virtual void setPosition(const Ogre::Vector3& position);
	virtual void setRotation(const Ogre::Vector3& position);
	virtual void setOrientation(const Ogre::Quaternion& quaternion);

	Ogre::Vector3 GetPosition() const;

	//tolua_end

	void update(int deltaMsec) override;

protected:
	Ogre::SceneNode* m_pSceneNode;
	Ogre::Entity* m_pEntity;

}; //tolua_exports

#endif  // __Entity_OBJECT__
