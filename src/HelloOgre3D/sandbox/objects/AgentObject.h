#ifndef __AGENT_OBJECT__
#define __AGENT_OBJECT__

#include <vector>

#include "OgreMath.h"
#include "OgreVector3.h"
#include "OISKeyboard.h"
#include "object/BaseObject.h"
#include "GameDefine.h"
#include "systems/input/IPlayerInput.h"

namespace Ogre {
	class SceneNode;
}

class btRigidBody;
struct lua_State;
class ScriptLuaVM;
class BlockObject;
class AgentAnim;
class AgentAnimStateMachine;
class AgentLocomotion;
class AnimComponent;
class LuaScriptComponent;
class RenderComponent;
struct Collision;
struct AICommand;

class AgentObject : public BaseObject //tolua_exports
{ //tolua_exports
public:
	AgentObject(RenderComponent* renderComp, btRigidBody* pRigidBody = nullptr);
	virtual ~AgentObject();

	virtual void Init() override;
	virtual void Update(int deltaMs);

	//tolua_begin
	void initBody(const Ogre::String& meshFile);
	//tolua_end
	AgentObject* getBody() { return this; }

	// C++ compatibility forwards. Lua should prefer component accessors through
	// AgentComponentAccess (GetBodyAnimation/GetBodyAsm).
	AgentAnim* GetAnimation(const char* animationName);
	AgentAnimStateMachine* GetObjectASM() const;
	//tolua_begin
	virtual void SetRenderVisible(bool visible);

	AGENT_OBJ_TYPE getAgentType() { return m_agentType; }
	void setAgentType(AGENT_OBJ_TYPE agentType) { m_agentType = agentType; }
	
	Ogre::Vector3 GetPosition() const;
	Ogre::Quaternion GetOrientation() const;

	virtual Ogre::Vector3 GetUp() const;
	virtual Ogre::Vector3 GetLeft() const;
	virtual Ogre::Vector3 GetForward() const;

	virtual void setPosition(const Ogre::Vector3& position);
	virtual void setRotation(const Ogre::Vector3& rotation);
	virtual void setOrientation(const Ogre::Quaternion& quaternion);

	void SetForward(const Ogre::Vector3& forward);
	void SetVelocity(const Ogre::Vector3& velocity);
	//tolua_end

	//tolua_begin
	Ogre::Vector3 GetVelocity() const;
	//tolua_end

	//tolua_begin
	void SetMass(Ogre::Real mass);
	void SetHeight(Ogre::Real height);
	void SetRadius(Ogre::Real radius);
	//tolua_end
	void SetSpeed(Ogre::Real speed);
	//tolua_begin
	void SetHealth(Ogre::Real health);

	Ogre::Real GetSpeed() const;
	//tolua_end
	//tolua_begin
	Ogre::Real GetHealth() const;
	Ogre::Vector3 PredictFuturePosition(Ogre::Real predictionTime) const;
	//tolua_end

	//tolua_begin
	virtual bool IsMoving();
	virtual bool IsFalling();
	virtual bool OnGround();

	bool GetUseCppFSM();
	int GetCurStateId();
	std::string GetCurStateName();

	virtual IPlayerInput* GetInput() { return nullptr; }
	virtual void RequestState(int soldierState, bool forceUpdate = false) {}

	virtual bool HasNextAnim() { return false; }
	virtual bool IsAnimReadyForMove() { return false; }
	virtual bool IsAnimReadyForShoot() { return false; }

	//tolua_end

	virtual void ApplyCommand(const AICommand& command);
	
	virtual void OnDeath(float lastSec);
	virtual void SlowMoving(float rate = 1.0f);
	virtual void CollideWithObject(BaseObject* pCollideObj, const Collision& collision) override;
	
	btRigidBody* getRigidBody() const;
	void ResetRigidBody(btRigidBody* pRigidBody);
	bool setPluginEnv(lua_State* L);
	bool callFunction(const char* funcname, const char* format, ...);
	void SetLuaScriptClassName(const char* className);
	void updateWorldTransform();
	void HandleKeyEvent(OIS::KeyCode keycode, unsigned int key);
	LuaScriptComponent* GetLuaScript();
	const LuaScriptComponent* GetLuaScript() const;
	// Legacy render component accessor kept for older C++ call sites; prefer
	// FindComponent<RenderComponent>() when touching nearby code.
	RenderComponent* GetRenderComponent() const { return m_renderComp; }

protected:
	RenderComponent* m_renderComp; // non-owning; owned by BaseObject component map
	bool m_onPlayDeathAnim = false;

private:
	AGENT_OBJ_TYPE m_agentType;

}; //tolua_exports

REGISTER_LUA_CLASS_NAME(AgentObject);

#endif  // __AGENT_OBJECT__
