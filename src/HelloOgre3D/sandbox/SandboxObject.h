#ifndef __SANDBOX_OBJECT__
#define __SANDBOX_OBJECT__

#include "BaseObject.h"
#include "OgreString.h"
#include "OgreMath.h"

namespace Ogre {
	class SceneNode;
	class Entity;
}

class SandboxObject : public BaseObject //tolua_exports
{ //tolua_exports
public:
	//tolua_begin
	SandboxObject(unsigned int objectId, const Ogre::String& meshFile);
	virtual ~SandboxObject();

	void Initialize();

	void setMass(const Ogre::Real mass);

	void setPosition(const Ogre::Vector3& position);

	void setOrientation(const Ogre::Quaternion& quaternion);
	//tolua_end
private:
	Ogre::SceneNode* m_pSceneNode;
	Ogre::Entity* m_pEntity;
	
}; //tolua_exports

#endif  // __SANDBOX_OBJECT__
