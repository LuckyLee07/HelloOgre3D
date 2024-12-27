#ifndef __BLOCK_OBJECT__
#define __BLOCK_OBJECT__

#include "EntityObject.h"
#include "OgreString.h"
#include "OgreMath.h"
#include "opensteer/include/Obstacle.h"

namespace Ogre {
	class SceneNode;
	class Entity;
}

class btRigidBody;

class BlockObject : public EntityObject //tolua_exports
	, private OpenSteer::SphericalObstacle
{ //tolua_exports
	friend class AgentObject;
public:
	BlockObject(const Ogre::String& meshFile, btRigidBody* pRigidBody = nullptr);
	BlockObject(const Ogre::MeshPtr& meshPtr, btRigidBody* pRigidBody = nullptr);
	BlockObject(Ogre::SceneNode* pSceneNode, btRigidBody* pRigidBody);

	virtual ~BlockObject();

	void Initialize() override;

	void update(int deltaMsec) override;

	//tolua_begin
	void setPosition(const Ogre::Vector3& position);
	void setRotation(const Ogre::Vector3& position);
	void setOrientation(const Ogre::Quaternion& quaternion);

	Ogre::Real GetRadius() const;
	Ogre::Vector3 GetPosition() const;

	void applyImpulse(const Ogre::Vector3& impulse);
	void applyAngularImpulse(const Ogre::Vector3& aImpulse);

	Ogre::Real getMass() const;
	void setMass(const Ogre::Real mass);
	//tolua_end

	void updateWorldTransform();

	btRigidBody* getRigidBody() { return m_pRigidBody; }
	
private:
	OpenSteer::Vec3 getPosition() const;

	// Overloading the SphericalObstacle's radius implementation.
	virtual float getRadius() const;

	virtual OpenSteer::Vec3 steerToAvoid(const OpenSteer::AbstractVehicle& vehicle,
		const float minTimeToCollision) const;

private:
	btRigidBody* m_pRigidBody;

}; //tolua_exports

#endif  // __BLOCK_OBJECT__
