#ifndef __GAME_MANAGER_H__  
#define __GAME_MANAGER_H__

#include <map>
#include "SandboxObject.h"
#include "PhysicsWorld.h"
#include "SandboxMgr.h"

namespace Ogre
{
	class SceneManager;
}

class GameManager //tolua_exports
{ //tolua_exports
public:
	GameManager();
	~GameManager();

	static GameManager* GetInstance();

	void Initialize(Ogre::SceneManager* sceneManager);
	void Update(int deltaMilliseconds);
	
	void InitLuaEnv();

	void AddSandboxObject(SandboxObject* pSandboxObject);

private:
	unsigned int getNextObjectId() { return ++m_objectIndex; }

private:
	SandboxMgr* m_pSandboxMgr;
	PhysicsWorld* m_pPhysicsWorld;
	
	unsigned int m_objectIndex;
	std::map<unsigned int, SandboxObject*> m_pObjects;

	
	//Gorilla::Screen* p;

}; //tolua_exports

extern GameManager* g_GameManager;
GameManager* GetGameManager();

#endif; // __GAME_MANAGER_H__