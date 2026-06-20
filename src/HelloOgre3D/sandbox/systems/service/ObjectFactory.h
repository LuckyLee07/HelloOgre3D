#ifndef __OBJECT_FACTORY_H__
#define __OBJECT_FACTORY_H__

#include "OgreString.h"
#include "GameDefine.h"

class BlockObject;
class AgentObject;
class SoldierObject;
class ObjectManager;

class ObjectFactory //tolua_exports
{ //tolua_exports
public:
	ObjectFactory(ObjectManager* pMananger);
	~ObjectFactory() { m_objectManager = nullptr; }

	//tolua_begin
	BlockObject* CreatePlane(float length, float width);
	BlockObject* CreateBlockObject(const Ogre::String& meshfilePath);
	BlockObject* CreateBlockBox(float width, float height, float length, float uTile, float vTile);
	BlockObject* CreateBullet(Ogre::Real height, Ogre::Real radius);

	AgentObject* CreateAgent(AGENT_OBJ_TYPE agentType, const char* filepath = nullptr);
	AgentObject* CreateAgentWithProfile(AGENT_OBJ_TYPE agentType, const char* profileName, const char* filepath = nullptr);
	SoldierObject* CreateSoldier(const Ogre::String& meshFile, const char* filepath = nullptr);
	//tolua_end

	ObjectManager* GetObjectManager() const { return m_objectManager; }

private:
	ObjectManager* m_objectManager; // non-owning; injected by ObjectManager/GameManager wiring
}; //tolua_exports

#endif // __OBJECT_FACTORY_H__
