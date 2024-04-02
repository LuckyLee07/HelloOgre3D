#ifndef __OGRE_WRAPPER_H__  
#define __OGRE_WRAPPER_H__

#include "OgreString.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"

class OgreWrapper //tolua_exports
{ //tolua_exports
public:
	OgreWrapper(Ogre::SceneManager* sceneManager);
	~OgreWrapper();
	
	static OgreWrapper* GetInstance();

public:
	//tolua_begin
	Ogre::Camera* GetCamera();

	Ogre::SceneNode* CreatePlane(float length, float width);

	void CreateSkyBox(const Ogre::String materialName, Ogre::Vector3& rotation);

	//tolua_end

private:
	Ogre::SceneManager* m_pSceneManager;
	Ogre::SceneNode* m_pRootSceneNode;

}; //tolua_exports

extern OgreWrapper* g_OgreWrapper;

#endif;