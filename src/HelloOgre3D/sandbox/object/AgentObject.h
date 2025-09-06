#ifndef __AGENT_OBJECT__
#define __AGENT_OBJECT__

#include "OgreMath.h"
#include "OgreVector3.h"
#include "SandboxDef.h"
#include "VehicleObject.h"
#include "OISKeyboard.h"
#include "LuaEnvObject.h"
#include "input/IPlayerInput.h"

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

	virtual void setPosition(const Ogre::Vector3& position);
	virtual void setOrientation(const Ogre::Quaternion& quaternion);

	virtual bool IsMoving();
	virtual bool IsFalling();
	virtual bool OnGround();

	bool GetUseCppFSM();
	int GetCurStateId();
	std::string GetCurStateName();

	virtual IPlayerInput* GetInput() { return nullptr; }
	virtual void RequestState(int soldierState, bool forceUpdate = false) {}

	virtual bool IsHasNextAnim() { return false; }
	virtual bool IsAnimReadyForMove() { return false; }
	virtual bool IsAnimReadyForShoot() { return false; }

	//tolua_end
	
	virtual void OnDeath(float lastSec);
	virtual void SlowMoving(float rate = 1.0f);
	
	void updateWorldTransform();
	void HandleKeyEvent(OIS::KeyCode keycode, unsigned int key);

protected:
	void CreateEventDispatcher();
	void RemoveEventDispatcher();
	
	EntityObject* m_pAgentBody;
	bool m_onPlayDeathAnim = false;

private:
	AGENT_OBJ_TYPE m_agentType;

}; //tolua_exports

REGISTER_LUA_CLASS_NAME(AgentObject);

#endif  // __AGENT_OBJECT__
