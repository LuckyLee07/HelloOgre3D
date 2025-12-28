#ifndef __PHYSICS_FACTORY_H__
#define __PHYSICS_FACTORY_H__

#include "OgreMesh.h"
#include "OgreVector3.h"
#include "LinearMath/btVector3.h"
#include "LinearMath/btScalar.h"

class btRigidBody;
class btConvexHullShape;

class PhysicsFactory
{
public:
	static btRigidBody* CreateRigidBodyBox(Ogre::Mesh* meshPtr, const btScalar mass = 0.0f);
	static btRigidBody* CreateRigidBodyBox(Ogre::Real width, Ogre::Real height, Ogre::Real length);
	static btRigidBody* CreateRigidBodyPlane(const btVector3& normal, const btScalar originOffset);
	static btRigidBody* CreateRigidBodyCapsule(Ogre::Real height, Ogre::Real radius);
	static btConvexHullShape* CreateSimplifiedConvexHull(Ogre::Mesh* meshPtr);
};

#endif // __PHYSICS_FACTORY_H__