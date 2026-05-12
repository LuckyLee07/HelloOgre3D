#ifndef __DECISION_NODE_H__
#define __DECISION_NODE_H__

#include "script/LuaClassNameTraits.h"

class DecisionAction;

// Base for anything that can appear inside a decision tree: branches or actions.
// Resolve() walks down the tree (branches recurse via their evaluator; actions return self).
// It returns the leaf Action that should execute, or nullptr if nothing applies.
//
// Exposed to tolua as an opaque handle so Lua can pass branches/actions through
// uniformly (e.g. AddChild). No constructor is exposed.
class DecisionNode //tolua_exports
{ //tolua_exports
public:
	virtual ~DecisionNode() {}
	virtual DecisionAction* Resolve() = 0;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(DecisionNode);

#endif  // __DECISION_NODE_H__
