#ifndef __SOLDIER_FACTORY_H__
#define __SOLDIER_FACTORY_H__

#include "OgreString.h"

class ObjectManager;
class RenderComponent;
class SoldierObject;
class btRigidBody;

class SoldierFactory
{
public:
	static SoldierObject* CreateSoldier(ObjectManager* objectManager, const Ogre::String& meshFile, const char* filepath = nullptr);
	static SoldierObject* CreateSoldierWithProfile(ObjectManager* objectManager, const Ogre::String& meshFile, const char* profileName, const char* filepath = nullptr);

private:
	static RenderComponent* CreateSoldierRender(const Ogre::String& meshFile);
	static btRigidBody* CreateSoldierRigidBody();
};

#endif // __SOLDIER_FACTORY_H__
