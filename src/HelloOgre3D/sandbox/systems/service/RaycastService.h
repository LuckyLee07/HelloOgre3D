#ifndef __RAYCAST_SERVICE_H__
#define __RAYCAST_SERVICE_H__

#include "OgreVector3.h"

class PhysicsWorld;

class RaycastService //tolua_exports
{ //tolua_exports
public:
	explicit RaycastService(PhysicsWorld* physicsWorld);
	~RaycastService();

	void SetPhysicsWorld(PhysicsWorld* physicsWorld);
	Ogre::Vector3 SweepCamera(const Ogre::Vector3& from, const Ogre::Vector3& to, float radius) const;

	//tolua_begin
	int RayCastObjectId(const Ogre::Vector3& from, const Ogre::Vector3& to) const;
	// 0 = clear, -1 = invalid/unknown, positive = first object's ID.
	// hitPoint is a world-space surface point, or to when clear. Inputs are snapshots.
	int TraceProjectile(const Ogre::Vector3& from, const Ogre::Vector3& to, int ignoreObjectId, Ogre::Vector3& hitPoint) const;
	int PickSurface(const Ogre::Vector3& from, const Ogre::Vector3& to, Ogre::Vector3& hitPoint) const;
	//tolua_end

private:
	PhysicsWorld* m_physicsWorld = nullptr; // non-owning; injected by GameManager/ObjectManager wiring
}; //tolua_exports

#endif // __RAYCAST_SERVICE_H__
