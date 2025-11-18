#ifndef __BLOCK_OBJECT__
#define __BLOCK_OBJECT__

#include "object/BaseObject.h"
#include "OgreString.h"
#include "OgreMath.h"
#include "opensteer/include/Obstacle.h"

namespace Ogre {
	class SceneNode;
	class Entity;
}

class btRigidBody;
class RenderableObject;

class BlockObject : public BaseObject //tolua_exports
	, public OpenSteer::SphericalObstacle
{ //tolua_exports
	friend class VehicleObject;
public:
	BlockObject(const Ogre::String& meshFile, btRigidBody* pRigidBody = nullptr);
	BlockObject(const Ogre::MeshPtr& meshPtr, btRigidBody* pRigidBody = nullptr);
	BlockObject(Ogre::SceneNode* pSceneNode, btRigidBody* pRigidBody);

	virtual ~BlockObject();

	//void Initialize(BaseObject *owner = nullptr);
	void DeleteRighdBody();

	btRigidBody* getRigidBody() const override { return m_pRigidBody; }

	virtual void update(int deltaMsec) override;
	virtual void updateWorldTransform();

	//tolua_begin
	void setPosition(const Ogre::Vector3& position);
	void setRotation(const Ogre::Vector3& position);
	void setOrientation(const Ogre::Quaternion& quaternion);

	Ogre::Real GetMass() const;
	void SetMass(const Ogre::Real mass);

	Ogre::Real GetRadius() const;
	Ogre::Vector3 GetPosition() const;

	void applyImpulse(const Ogre::Vector3& impulse);
	void applyAngularImpulse(const Ogre::Vector3& aImpulse);
	//tolua_end

	void addParticleNode(Ogre::SceneNode* particleNode);
	void setBulletCollideImpact(const Collision& collision); //设置子弹碰撞后的效果

	virtual bool canCollide(); // 是否可以碰撞
	virtual void onCollideWith(BaseObject* pCollideObj, const Collision& collision);

	Ogre::Entity* GetEntity();
	Ogre::SceneNode* GetSceneNode();

public:
	OpenSteer::Vec3 getPosition() const;

	// Overloading the SphericalObstacle's radius implementation.
	virtual float getRadius() const;

	virtual OpenSteer::Vec3 steerToAvoid(const OpenSteer::AbstractVehicle& vehicle,
		const float minTimeToCollision) const;

private:
	btRigidBody* m_pRigidBody = nullptr;
	RenderableObject* m_pEntity = nullptr;
	std::vector<Ogre::SceneNode*> m_particleNodes;

}; //tolua_exports

#endif  // __BLOCK_OBJECT__
