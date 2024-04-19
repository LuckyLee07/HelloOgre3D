#ifndef __SANDBOX_MGR_H__  
#define __SANDBOX_MGR_H__

#include "OgreMesh.h"
#include "OgreVector3.h"

class btRigidBody;
class btConvexHullShape;

class SandboxMgr
{
public:
	SandboxMgr() {}
	~SandboxMgr() {}

	static btRigidBody* CreateRigidBody(Ogre::Mesh* meshPtr);
	static btConvexHullShape* CreateSimplifiedConvexHull(Ogre::Mesh* meshPtr);

	static void GetMeshInfo(const Ogre::Mesh* mesh, size_t& vertex_count, Ogre::Vector3*& vertices, size_t& index_count, unsigned long*& indices);
	
};

#endif; // __SANDBOX_MGR_H__