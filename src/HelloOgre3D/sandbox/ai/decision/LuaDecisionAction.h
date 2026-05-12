#ifndef __LUA_DECISION_ACTION_H__
#define __LUA_DECISION_ACTION_H__

#include <string>

#include "DecisionAction.h"
#include "object/LuaEnvObject.h"
#include "script/LuaClassNameTraits.h"

class SoldierObject;

// Lua-backed Action leaf — body of Initialize/Update/CleanUp lives in a Lua file.
//
// Mirrors AgentLuaState's pattern:
//   - inherits DecisionAction (C++ lifecycle) + LuaEnvObject (Lua env per instance)
//   - BindToScript(filepath) loads the Lua file and binds it to this object's env
//   - the three virtual hooks forward to Lua functions: OnInitialize / OnUpdate / OnCleanUp
//
// The action holds a weak reference to the owning SoldierObject so Lua callbacks
// can reach the agent without the Lua side having to stash a pointer explicitly.
class LuaDecisionAction : public DecisionAction //tolua_exports
	, public LuaEnvObject
{ //tolua_exports
public:
	LuaDecisionAction(const std::string& name, SoldierObject* owner);
	virtual ~LuaDecisionAction();

	//tolua_begin
	bool BindToScript(const std::string& filepath);
	SoldierObject* GetOwner() const { return m_owner; }
	//tolua_end

protected:
	virtual void   OnInitialize() override;
	virtual Status OnUpdate(float deltaMs) override;
	virtual void   OnCleanUp() override;

private:
	SoldierObject* m_owner;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(LuaDecisionAction);

#endif  // __LUA_DECISION_ACTION_H__
