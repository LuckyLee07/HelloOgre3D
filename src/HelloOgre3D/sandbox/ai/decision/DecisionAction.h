#ifndef __DECISION_ACTION_H__
#define __DECISION_ACTION_H__

#include <string>
#include "DecisionNode.h"
#include "script/LuaClassNameTraits.h"

// Leaf node: an executable Action with a three-phase lifecycle (mirrors chapter_6 Action.lua).
//
// State machine (owned in C++, subclasses only supply the per-phase body):
//   UNINITIALIZED --Initialize()--> RUNNING
//   RUNNING       --Update()-----> RUNNING or TERMINATED
//   TERMINATED    --CleanUp()----> UNINITIALIZED
//
// DecisionTree calls Initialize() once when the action is selected, then Update() every
// frame while it's running, then CleanUp() when OnUpdate returns TERMINATED. The tree
// then re-resolves from the root to pick the next action.
class DecisionAction : public DecisionNode //tolua_exports
{ //tolua_exports
public:
	enum Status
	{
		STATUS_UNINITIALIZED = 0,
		STATUS_RUNNING = 1,
		STATUS_TERMINATED = 2,
	};

	DecisionAction(const std::string& name);
	virtual ~DecisionAction();

	void     Initialize();
	Status   Update(float deltaMs);
	void     CleanUp();

	//tolua_begin
	const std::string& GetName() const { return m_name; }
	int                GetStatus() const { return (int)m_status; }
	//tolua_end

	// DecisionNode: leaf resolves to itself.
	virtual DecisionAction* Resolve() override { return this; }

protected:
	// Subclass hooks. Default no-op lets pure-C++ actions opt-in.
	virtual void   OnInitialize() {}
	virtual Status OnUpdate(float /*deltaMs*/) { return STATUS_TERMINATED; }
	virtual void   OnCleanUp() {}

private:
	std::string m_name;
	Status      m_status;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(DecisionAction);

#endif  // __DECISION_ACTION_H__
