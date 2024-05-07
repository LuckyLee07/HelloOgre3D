#ifndef __SANDBOX_MGR_H__  
#define __SANDBOX_MGR_H__

#include "OgreMesh.h"
#include "OgreVector3.h"
#include "OgreQuaternion.h"
#include "OgreString.h"
#include "SandboxObject.h"
#include "UIComponent.h"
#include "AgentObject.h"

#include "LinearMath/btVector3.h"
#include "LinearMath/btScalar.h"
#include "LinearMath/btQuaternion.h"

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

	Ogre::Real GetScreenWidth();
	Ogre::Real GetScreenHeight();

	Ogre::Vector3 GetCameraUp();
	Ogre::Vector3 GetCameraLeft();
	Ogre::Vector3 GetCameraForward();
	Ogre::Vector3 GetCameraPosition();
	Ogre::Vector3 GetCameraRotation();
	Ogre::Quaternion GetCameraOrientation();

	long long GetRenderTime();
	long long GetSimulateTime();
	long long GetTotalSimulateTime();

	unsigned int GetObjectCount();
	//tolua_end

public: //static methods
	static Ogre::SceneNode* CreateChildSceneNode();

	static Ogre::Vector3 BtVector3ToVector3(const btVector3& vector);
	static Ogre::Quaternion BtQuaternionToQuaternion(const btQuaternion& orientation);

	static Ogre::SceneNode* CreateNodePlane(Ogre::Real length, Ogre::Real width);
	static btRigidBody* CreateRigidBodyPlane(const btVector3& normal, const btScalar originOffset);

	static btRigidBody* CreateRigidBodyBox(Ogre::Mesh* meshPtr, const btScalar mass = 0.0f);
	static btConvexHullShape* CreateSimplifiedConvexHull(Ogre::Mesh* meshPtr);

	static Ogre::SceneNode* CreateNodeCapsule(Ogre::Real height, Ogre::Real radius);
	static btRigidBody* CreateRigidBodyCapsule(Ogre::Real height, Ogre::Real radius);

	static void GetMeshInfo(const Ogre::Mesh* mesh, size_t& vertex_count, Ogre::Vector3*& vertices, size_t& index_count, unsigned long*& indices);
	
public:
	//tolua_begin
	void SetSkyBox(const Ogre::String materialName, const Ogre::Vector3& rotation);

	void SetAmbientLight(const Ogre::Vector3& colourValue);

	Ogre::Light* CreateDirectionalLight(const Ogre::Vector3& rotation);

	void setMaterial(Ogre::SceneNode* pNode, const Ogre::String& materialName);
	void setMaterial(SandboxObject* pObject, const Ogre::String& materialName);
	
	void SetMarkupColor(unsigned int index, const Ogre::ColourValue& color);
	
	SandboxObject* CreatePlane(float length, float width);
	SandboxObject* CreateSandboxObject(const Ogre::String& meshfilePath);

	UIComponent* CreateUIComponent(unsigned int index = 1);

	AgentObject* CreateAgent(AGENT_OBJ_TYPE agentType);
	
	//tolua_end

private:
	Ogre::SceneNode* m_pRootSceneNode;
	Ogre::SceneManager* m_pSceneManager;

}; //tolua_exports

#endif; // __SANDBOX_MGR_H__