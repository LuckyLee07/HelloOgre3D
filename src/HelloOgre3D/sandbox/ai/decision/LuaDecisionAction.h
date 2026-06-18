#ifndef __LUA_DECISION_ACTION_H__
#define __LUA_DECISION_ACTION_H__

#include <string>

#include "DecisionAction.h"
#include "object/LuaEnvObject.h"
#include "script/LuaClassNameTraits.h"

class AgentObject;
class Blackboard;
class SoldierObject;

// Lua-backed Action leaf — body of Initialize/Update/CleanUp lives in a Lua file.
//
// Mirrors AgentLuaState's pattern:
//   - inherits DecisionAction (C++ lifecycle) + LuaEnvObject (Lua env per instance)
//   - BindToScript(filepath) loads the Lua file and binds it to this object's env
//   - the three virtual hooks forward to Lua functions: OnInitialize / OnUpdate / OnCleanUp
//
// The action holds a weak reference to the owning AgentObject so Lua callbacks
// can reach the agent without the Lua side having to stash a pointer explicitly.
class LuaDecisionAction : public DecisionAction //tolua_exports
	, public LuaEnvObject
{ //tolua_exports
public:
	LuaDecisionAction(const std::string& name, AgentObject* owner, Blackboard* blackboard = nullptr);
	LuaDecisionAction(const std::string& name, SoldierObject* owner);
	virtual ~LuaDecisionAction();

	//tolua_begin
	bool BindToScript(const std::string& filepath);
	SoldierObject* GetOwner() const;
	//tolua_end

protected:
	virtual void   OnInitialize() override;
	virtual Status OnUpdate(float deltaMs) override;
	virtual void   OnCleanUp() override;

private:
	Blackboard* ResolveBlackboard() const;

	AgentObject* m_owner;
	Blackboard* m_blackboard;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(LuaDecisionAction);

#endif  // __LUA_DECISION_ACTION_H__
