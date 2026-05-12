#ifndef __DECISION_BRANCH_H__
#define __DECISION_BRANCH_H__

#include <vector>
#include "DecisionNode.h"
#include "script/LuaClassNameTraits.h"

class DecisionAction;

// Internal node of a decision tree.
//
// Holds an ordered list of children (each either a Branch or an Action) and a Lua
// function reference (held via luaL_ref in the Lua registry) that returns a 1-based
// child index when invoked. Resolve() invokes the evaluator, picks the child, and
// recurses — matching chapter_6 DecisionBranch.lua semantics exactly, but with the
// traversal engine living in C++.
//
// Ownership: the branch does NOT delete its children in its destructor. Lua keeps
// all created nodes alive in a local table in the agent's plugin env (see
// DecisionSoldierAgent.lua). Avoids tolua double-ownership headaches.
class DecisionBranch : public DecisionNode //tolua_exports
{ //tolua_exports
public:
	DecisionBranch();
	virtual ~DecisionBranch();

	//tolua_begin
	void AddChild(DecisionNode* child);
	int  GetChildCount() const { return (int)m_children.size(); }
	//tolua_end

	// The Lua-facing SetEvaluator(function) is implemented as a manual tolua wrapper
	// in ManualToLua.cpp (it must capture a Lua function via luaL_ref).
	// This C++ entry stores the registry ref.
	void SetEvaluatorRef(int luaRef);
	int  GetEvaluatorRef() const { return m_evalLuaRef; }

	// DecisionNode impl
	virtual DecisionAction* Resolve() override;

private:
	std::vector<DecisionNode*> m_children;  // non-owning; Lua holds lifetime
	int                        m_evalLuaRef;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(DecisionBranch);

#endif  // __DECISION_BRANCH_H__
