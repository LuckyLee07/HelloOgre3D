#ifndef __OGRE_WRAPPER_H__  
#define __OGRE_WRAPPER_H__

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
	Ogre::SceneNode* CreatePlane(int length, int width);
	//tolua_end

private:
	Ogre::SceneManager* m_pSceneManager;
	Ogre::SceneNode* m_pRootSceneNode;

}; //tolua_exports

extern OgreWrapper* g_OgreWrapper;

#endif;