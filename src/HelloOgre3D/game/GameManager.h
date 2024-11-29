#ifndef __GAME_MANAGER_H__  
#define __GAME_MANAGER_H__

#include <map>
#include <vector>
#include "OgrePrerequisites.h"
#include "OISMouse.h"
#include "OISKeyboard.h"
#include "SandboxDef.h"
#include "object/AgentObject.h"
#include "object/UIComponent.h"
#include "object/EntityObject.h"
#include "object/SandboxObject.h"
#include "play/PhysicsWorld.h"
#include "manager/SandboxMgr.h"


#define UI_LAYER_COUNT	16

namespace Gorilla
{
	class Layer;
	class Screen;
	class MarkupText;
}

class ScriptLuaVM;
class GameManager //tolua_exports
{ //tolua_exports
public:
	GameManager();
	~GameManager();

	static GameManager* GetInstance();

	void Initialize(Ogre::SceneManager* sceneManager);
	void Update(int deltaMilliseconds);

	void InitLuaEnv();
	void InitUIConfig();
	
public:
	//tolua_begin
	Ogre::Real getScreenWidth();
	Ogre::Real getScreenHeight();
	unsigned int getObjectCount();

	const std::vector<EntityObject*>& getAllEntitys() { return m_pEntitys; }
	const std::vector<SandboxObject*>& getAllObjects() { return m_pObjects; }
	const std::vector<AgentObject*>& getAllAgents() { return m_pAgents; }

	std::vector<AgentObject*> getSpecifyAgents(AGENT_OBJ_TYPE agentType);
	//tolua_end

	PhysicsWorld* getPhysicsWorld() { return m_pPhysicsWorld; }

	UIComponent* createUIComponent(unsigned int index);
	void addEntityObject(EntityObject* pEntityObject);
	void addSandboxObject(SandboxObject* pSandboxObject);
	void addAgentObject(AgentObject* pAgentObject);

	void setMarkupColor(unsigned int index, const Ogre::ColourValue& color);
	
public:
	void HandleWindowResized(unsigned int width, unsigned int height);
	void HandleWindowClosed();

	void HandleKeyPress(OIS::KeyCode keycode, unsigned int key);
	void HandleKeyRelease(OIS::KeyCode keycode, unsigned int key);

	void HandleMouseMove(int width, int height);
	void HandleMousePress(int width, int height, OIS::MouseButtonID buttonId);
	void HandleMouseRelease(int width, int height, OIS::MouseButtonID buttonId);

private:
	Gorilla::Layer* getUILayer(unsigned int index = 0);

	unsigned int getNextObjectId() { return ++m_objectIndex; }

private:
	ScriptLuaVM* m_pScriptVM;
	SandboxMgr* m_pSandboxMgr;
	PhysicsWorld* m_pPhysicsWorld;
	
	unsigned int m_objectIndex;
	std::vector<AgentObject*> m_pAgents;
	std::vector<EntityObject*> m_pEntitys;
	std::vector<SandboxObject*> m_pObjects;
	std::vector<UIComponent*> m_pUIComps;
	
	Gorilla::Screen* m_pUIScene;
	Gorilla::MarkupText* m_pMarkupText;
	Gorilla::Layer* m_pUILayers[UI_LAYER_COUNT];

}; //tolua_exports

extern GameManager* g_GameManager;
GameManager* GetGameManager();

#endif; // __GAME_MANAGER_H__