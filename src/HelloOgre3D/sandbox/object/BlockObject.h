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
	friend class VehicleObject;
public:
	BlockObject(const Ogre::String& meshFile, btRigidBody* pRigidBody = nullptr);
	BlockObject(const Ogre::MeshPtr& meshPtr, btRigidBody* pRigidBody = nullptr);
	BlockObject(Ogre::SceneNode* pSceneNode, btRigidBody* pRigidBody);

	virtual ~BlockObject();

	//void Initialize(BaseObject *owner = nullptr);
	void DeleteRighdBody();

	btRigidBody* getRigidBody() { return m_pRigidBody; }

	virtual void update(int deltaMsec) override;
	virtual void updateWorldTransform();

	//tolua_begin
	void setPosition(const Ogre::Vector3& position) override;
	void setRotation(const Ogre::Vector3& position) override;
	void setOrientation(const Ogre::Quaternion& quaternion) override;

	Ogre::Real GetMass() const;
	void SetMass(const Ogre::Real mass);

	Ogre::Real GetRadius() const;
	Ogre::Vector3 GetPosition() const;

	void applyImpulse(const Ogre::Vector3& impulse);
	void applyAngularImpulse(const Ogre::Vector3& aImpulse);
	//tolua_end

	void setBulletCollideImpact(const Collision& collision); //设置子弹碰撞后的效果
	virtual void onCollideWith(BaseObject* pCollideObj, const Collision& collision);

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
