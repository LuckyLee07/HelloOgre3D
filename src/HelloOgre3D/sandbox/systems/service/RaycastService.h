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

	//tolua_begin
	int RayCastObjectId(const Ogre::Vector3& from, const Ogre::Vector3& to) const;
	//tolua_end

private:
	PhysicsWorld* m_physicsWorld = nullptr; // non-owning; injected by GameManager/ObjectManager wiring
}; //tolua_exports

#endif // __RAYCAST_SERVICE_H__
