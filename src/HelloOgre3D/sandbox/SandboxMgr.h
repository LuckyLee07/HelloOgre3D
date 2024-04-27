#ifndef __SANDBOX_MGR_H__  
#define __SANDBOX_MGR_H__

#include "OgreMesh.h"
#include "OgreVector3.h"
#include "OgreString.h"
#include "SandboxObject.h"

#include "LinearMath/btVector3.h"
#include "LinearMath/btScalar.h"

namespace Ogre {
	class Camera;
	class SceneNode;
	class SceneManager;
}
class btRigidBody;
class btConvexHullShape;

class SandboxMgr //tolua_exports
{ //tolua_exports
public:
	SandboxMgr(Ogre::SceneManager* sceneManager);
	~SandboxMgr();

	//tolua_begin
	Ogre::Camera* GetCamera();
	Ogre::SceneManager* GetSceneManager();
	//tolua_end

public:
	static Ogre::SceneNode* CreatePlaneNode(Ogre::Real length, Ogre::Real width);
	static btRigidBody* CreatePlane(const btVector3& normal, const btScalar originOffset);

	static btRigidBody* CreateRigidBody(Ogre::Mesh* meshPtr, const btScalar mass = 0.0f);
	static btConvexHullShape* CreateSimplifiedConvexHull(Ogre::Mesh* meshPtr);

	static void GetMeshInfo(const Ogre::Mesh* mesh, size_t& vertex_count, Ogre::Vector3*& vertices, size_t& index_count, unsigned long*& indices);
	
public:
	//tolua_begin
	void SetSkyBox(const Ogre::String materialName, const Ogre::Vector3& rotation);

	void SetAmbientLight(const Ogre::Vector3& colourValue);

	Ogre::Light* CreateDirectionalLight(const Ogre::Vector3& rotation);

	void setMaterial(Ogre::SceneNode* pNode, const Ogre::String& materialName);
	void setMaterial(SandboxObject* pObject, const Ogre::String& materialName);
	
	SandboxObject* CreatePlane(float length, float width);
	SandboxObject* CreateSandboxObject(const Ogre::String& meshfilePath);

	//tolua_end

private:
	Ogre::SceneNode* m_pRootSceneNode;
	Ogre::SceneManager* m_pSceneManager;

}; //tolua_exports

#endif; // __SANDBOX_MGR_H__