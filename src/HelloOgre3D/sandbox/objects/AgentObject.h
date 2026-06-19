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
class AgentPath;
class OpenSteerAdapter;
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
	AgentObject* getBody() { return this; }

	// Legacy Lua compatibility forwards. New C++ code should prefer typed
	// component accessors such as FindComponent<T>() / GetLocomotion().
	AgentAnim* GetAnimation(const char* animationName);
	AgentAnimStateMachine* GetObjectASM() const;
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

	void SetTarget(const Ogre::Vector3& targetPos);
	void SetTargetRadius(Ogre::Real radius);

	Ogre::Vector3 GetTarget() const;
	Ogre::Vector3 GetVelocity() const;

	void SetMass(Ogre::Real mass);
	void SetHeight(Ogre::Real height);
	void SetRadius(Ogre::Real radius);
	void SetSpeed(Ogre::Real speed);
	void SetHealth(Ogre::Real health);

	Ogre::Real GetMass() const;
	Ogre::Real GetSpeed() const;
	Ogre::Real GetHeight() const;
	Ogre::Real GetRadius() const;
	Ogre::Real GetHealth() const;

	void SetMaxForce(Ogre::Real maxForce);
	void SetMaxSpeed(Ogre::Real maxSpeed);

	Ogre::Real GetMaxForce() const;
	Ogre::Real GetMaxSpeed() const;

	Ogre::Real GetTargetRadius() const;
	Ogre::Vector3 PredictFuturePosition(Ogre::Real predictionTime) const;

	Ogre::Vector3 ForceToPosition(const Ogre::Vector3& position);
	Ogre::Vector3 ForceToFollowPath(Ogre::Real predictionTime);
	Ogre::Vector3 ForceToStayOnPath(Ogre::Real predictionTime);
	Ogre::Vector3 ForceToWander(Ogre::Real deltaMilliSeconds);
	Ogre::Vector3 ForceToTargetSpeed(Ogre::Real targetSpeed);

	Ogre::Vector3 ForceToAvoidAgents(Ogre::Real predictionTime);
	Ogre::Vector3 ForceToAvoidObjects(Ogre::Real predictionTime);

	Ogre::Vector3 ForceToCombine(const std::vector<AgentObject*>& agents, Ogre::Real distance, Ogre::Real angle);
	Ogre::Vector3 ForceToSeparate(const std::vector<AgentObject*>& agents, Ogre::Real distance, Ogre::Real angle);

	void ApplyForce(const Ogre::Vector3& force);

	void SetPath(const std::vector<Ogre::Vector3>& points, bool cyclic);
	const std::vector<Ogre::Vector3>& GetPath();
	bool HasPath() const;

	Ogre::Real GetDistanceAlongPath(const Ogre::Vector3& position) const;
	Ogre::Vector3 GetNearestPointOnPath(const Ogre::Vector3& position) const;
	Ogre::Vector3 GetPointOnPath(const Ogre::Real distance) const;

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
	void SetPath(const AgentPath& agentPath);
	AgentLocomotion* GetLocomotion();
	const AgentLocomotion* GetLocomotion() const;
	LuaScriptComponent* GetLuaScript();
	const LuaScriptComponent* GetLuaScript() const;
	OpenSteerAdapter* GetAdapter() const;
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
