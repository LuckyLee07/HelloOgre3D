#ifndef __AGENT_OBJECT__
#define __AGENT_OBJECT__

#include "OgreMath.h"
#include "OgreVector3.h"
#include "SandboxDef.h"
#include "VehicleObject.h"
#include "OISKeyboard.h"
#include "LuaEnvObject.h"

namespace Ogre {
	class SceneNode;
}

class ScriptLuaVM;
class BlockObject;
class EntityObject;

class AgentObject : public VehicleObject //tolua_exports
	, public LuaEnvObject
{ //tolua_exports
public:
	AgentObject(EntityObject* pAgentBody, btRigidBody* pRigidBody = nullptr);
	virtual ~AgentObject();

	virtual void Initialize();
	virtual void update(int deltaMilisec);
	virtual void handleEventByLua(OIS::KeyCode keycode);

	//tolua_begin
	void initBody(const Ogre::String& meshFile);
	EntityObject* getBody() { return m_pAgentBody; }

	AGENT_OBJ_TYPE getAgentType() { return m_agentType; }
	void setAgentType(AGENT_OBJ_TYPE agentType) { m_agentType = agentType; }
	
	Ogre::Vector3 GetPosition() const override;
	Ogre::Quaternion GetOrientation() const override;

	virtual Ogre::Vector3 GetUp() const;
	virtual Ogre::Vector3 GetLeft() const;
	virtual Ogre::Vector3 GetForward() const;

	virtual bool IsMoving();
	virtual bool IsFalling();
	virtual bool OnGround();

	std::string GetCurStateName();
	//tolua_end
	
	virtual void OnDeath(float lastSec);
	
	void updateWorldTransform();
	void HandleKeyEvent(OIS::KeyCode keycode, unsigned int key);

protected:
	void CreateEventDispatcher();
	void RemoveEventDispatcher();
	
	ScriptLuaVM* m_pScriptVM;
	EntityObject* m_pAgentBody;
	bool m_onPlayDeathAnim = false;

private:
	AGENT_OBJ_TYPE m_agentType;
}; //tolua_exports

#endif  // __AGENT_OBJECT__
