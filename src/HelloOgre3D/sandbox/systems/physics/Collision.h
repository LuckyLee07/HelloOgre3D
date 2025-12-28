#ifndef __COLLISION_H__
#define __COLLISION_H__

#include "OgreVector3.h"
#include "LinearMath/btVector3.h"

class btRigidBody;

struct Collision
{
	const btRigidBody* objectA_;
	const btRigidBody* objectB_;

	Ogre::Vector3 pointA_;
	Ogre::Vector3 pointB_;
	Ogre::Vector3 normalOnB_;

	Collision(const btRigidBody* objectA, 
		const btRigidBody* objectB,
		const btVector3& pointA, 
		const btVector3& pointB, 
		const btVector3& normalOnB)
		: objectA_(objectA), objectB_(objectB),
		pointA_(pointA.m_floats[0], pointA.m_floats[1], pointA.m_floats[2]),
		pointB_(pointB.m_floats[0], pointB.m_floats[1], pointB.m_floats[2]),
		normalOnB_(normalOnB.m_floats[0], normalOnB.m_floats[1], normalOnB.m_floats[2])
	{
	}
};

#endif  // __COLLISION_H__
