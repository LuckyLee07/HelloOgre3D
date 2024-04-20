#ifndef __SANDBOX_OBJECT__
#define __SANDBOX_OBJECT__

#include "BaseObject.h"
#include "OgreString.h"
#include "OgreMath.h"

namespace Ogre {
	class SceneNode;
	class Entity;
}

class btRigidBody;

class SandboxObject : public BaseObject //tolua_exports
{ //tolua_exports
public:
	//tolua_begin
	SandboxObject(unsigned int objectId, const Ogre::String& meshFile);
	SandboxObject(unsigned int objectId, Ogre::SceneNode* pSceneNode, btRigidBody* pRigidBody);

	virtual ~SandboxObject();

	void Initialize();

	btRigidBody* getRigidBody() { return m_pRigidBody; }
	Ogre::SceneNode* getSceneNode() { return m_pSceneNode; }

	void setMass(const Ogre::Real mass);
	void setPosition(const Ogre::Vector3& position);
	void setRotation(const Ogre::Vector3& position);
	void setOrientation(const Ogre::Quaternion& quaternion);

	//tolua_end

	void Update(int deltaMsec);
	void updateWorldTransform();

private:
	Ogre::SceneNode* m_pSceneNode;
	Ogre::Entity* m_pEntity;
	btRigidBody* m_pRigidBody;
	
}; //tolua_exports

#endif  // __SANDBOX_OBJECT__
