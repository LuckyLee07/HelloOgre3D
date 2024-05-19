#ifndef __SANDBOX_OBJECT__
#define __SANDBOX_OBJECT__

#include "BaseObject.h"
#include "OgreString.h"
#include "OgreMath.h"
#include "opensteer/include/Obstacle.h"

namespace Ogre {
	class SceneNode;
	class Entity;
}

class btRigidBody;

class SandboxObject : public BaseObject //tolua_exports
	, private OpenSteer::SphericalObstacle
{ //tolua_exports
	friend class AgentObject;
public:
	SandboxObject(const Ogre::String& meshFile);
	SandboxObject(Ogre::SceneNode* pSceneNode, btRigidBody* pRigidBody);

	virtual ~SandboxObject();

	//tolua_begin
	void Initialize();

	btRigidBody* getRigidBody() { return m_pRigidBody; }
	Ogre::SceneNode* getSceneNode() { return m_pSceneNode; }

	void setPosition(const Ogre::Vector3& position);
	void setRotation(const Ogre::Vector3& position);
	void setOrientation(const Ogre::Quaternion& quaternion);

	Ogre::Vector3 GetPosition() const;
	Ogre::Real GetRadius() const;

	void applyImpulse(const Ogre::Vector3& impulse);
	void applyAngularImpulse(const Ogre::Vector3& aImpulse);

	Ogre::Real getMass() const;
	void setMass(const Ogre::Real mass);
	//tolua_end

	void update(int deltaMsec) override;
	void updateWorldTransform();
	
private:
	OpenSteer::Vec3 getPosition() const;

	// Overloading the SphericalObstacle's radius implementation.
	virtual float getRadius() const;

	virtual OpenSteer::Vec3 steerToAvoid(const OpenSteer::AbstractVehicle& vehicle,
		const float minTimeToCollision) const;

private:
	Ogre::SceneNode* m_pSceneNode;
	Ogre::Entity* m_pEntity;
	btRigidBody* m_pRigidBody;

}; //tolua_exports

#endif  // __SANDBOX_OBJECT__
