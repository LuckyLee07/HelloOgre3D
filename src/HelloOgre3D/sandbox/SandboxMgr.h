#ifndef __SANDBOX_MGR_H__  
#define __SANDBOX_MGR_H__

#include "OgreMesh.h"
#include "OgreVector3.h"
#include "LinearMath/btVector3.h"
#include "LinearMath/btScalar.h"

class btRigidBody;
class btConvexHullShape;

class SandboxMgr
{
public:
	SandboxMgr() {}
	~SandboxMgr() {}

	static Ogre::SceneNode* CreatePlane(Ogre::Real length, Ogre::Real width);
	static btRigidBody* CreatePlane(const btVector3& normal, const btScalar originOffset);

	static btRigidBody* CreateRigidBody(Ogre::Mesh* meshPtr, const btScalar mass = 0.0f);
	static btConvexHullShape* CreateSimplifiedConvexHull(Ogre::Mesh* meshPtr);

	static void GetMeshInfo(const Ogre::Mesh* mesh, size_t& vertex_count, Ogre::Vector3*& vertices, size_t& index_count, unsigned long*& indices);
	
};

#endif; // __SANDBOX_MGR_H__