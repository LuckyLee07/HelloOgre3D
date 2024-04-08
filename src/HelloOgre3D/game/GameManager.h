#ifndef __OGRE_WRAPPER_H__  
#define __OGRE_WRAPPER_H__

#include "OgreString.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"

class GameManager //tolua_exports
{ //tolua_exports
public:
	GameManager(Ogre::SceneManager* sceneManager);
	~GameManager();
	
	static GameManager* GetInstance();

public:
	//tolua_begin
	Ogre::Camera* GetCamera();

	Ogre::SceneManager* GetSceneManager();
	
	void SetSkyBox(const Ogre::String materialName, const Ogre::Vector3& rotation);

	void SetAmbientLight(const Ogre::Vector3& colourValue);

	Ogre::SceneNode* CreatePlane(float length, float width);

	Ogre::Light* CreateDirectionalLight(const Ogre::Vector3& rotation);

	void setMaterial(Ogre::SceneNode* pNode, const Ogre::String& materialName);

	//tolua_end

private:
	Ogre::SceneManager* m_pSceneManager;
	Ogre::SceneNode* m_pRootSceneNode;

}; //tolua_exports

extern GameManager* g_GameManager;

#endif;