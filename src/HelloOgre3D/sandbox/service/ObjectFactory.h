#ifndef __OBJECT_FACTORY_H__
#define __OBJECT_FACTORY_H__

#include "OgreString.h"
#include "GameDefine.h"

class EntityObject;
class BlockObject;
class AgentObject;
class SoldierObject;
class UIComponent;
class ObjectManager;

class ObjectFactory
{
public:
	ObjectFactory(ObjectManager* pMananger);
	~ObjectFactory() { m_objectManager = nullptr; }

	BlockObject* CreatePlane(float length, float width);
	BlockObject* CreateBlockObject(const Ogre::String& meshfilePath);
	BlockObject* CreateBlockBox(float width, float height, float length, float uTile, float vTile);
	BlockObject* CreateBullet(Ogre::Real height, Ogre::Real radius);

	EntityObject* CreateEntityObject(const Ogre::String& meshFilePath);

	AgentObject* CreateAgent(AGENT_OBJ_TYPE agentType, const char* filepath = nullptr);
	SoldierObject* CreateSoldier(const Ogre::String& meshFile, const char* filepath = nullptr);

private:
	ObjectManager* m_objectManager;
};

#endif // __OBJECT_FACTORY_H__