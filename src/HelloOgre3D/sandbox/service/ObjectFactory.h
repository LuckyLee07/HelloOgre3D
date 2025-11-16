#ifndef __OBJECT_FACTORY_H__
#define __OBJECT_FACTORY_H__

#include "OgreString.h"
#include "GameDefine.h"

class EntityObject;
class BlockObject;
class AgentObject;
class SoldierObject;
class UIComponent;

class ObjectFactory
{
public:
	static BlockObject* CreatePlane(float length, float width);
	static BlockObject* CreateBlockObject(const Ogre::String& meshfilePath);
	static BlockObject* CreateBlockBox(float width, float height, float length, float uTile, float vTile);
	static BlockObject* CreateBullet(Ogre::Real height, Ogre::Real radius);

	static EntityObject* CreateEntityObject(const Ogre::String& meshFilePath);

	static AgentObject* CreateAgent(AGENT_OBJ_TYPE agentType, const char* filepath = nullptr);
	static SoldierObject* CreateSoldier(const Ogre::String& meshFile, const char* filepath = nullptr);
};

#endif // __OBJECT_FACTORY_H__