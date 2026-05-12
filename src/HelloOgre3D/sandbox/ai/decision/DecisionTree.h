#ifndef __DECISION_TREE_H__
#define __DECISION_TREE_H__

#include "script/LuaClassNameTraits.h"

class DecisionNode;
class DecisionAction;

// Engine-side decision tree: owns the "current action" cursor and drives the
// Resolve -> Initialize -> Update* -> CleanUp lifecycle on each Tick.
//
// Does NOT own the node graph (branches / actions); Lua keeps them alive in a
// local table for the owning agent's lifetime. See DecisionSoldierAgent.lua.
class DecisionTree //tolua_exports
{ //tolua_exports
public:
	DecisionTree();
	~DecisionTree();

	//tolua_begin
	void SetRoot(DecisionNode* root);

	// Main entry — called each frame by DecisionTreeDriver::Tick.
	// If no action is running, resolves from the root; otherwise continues the
	// currently-running action until it terminates, then re-resolves.
	void Tick(float deltaMs);
	//tolua_end

	DecisionNode*   GetRoot() const { return m_root; }
	DecisionAction* GetCurrentAction() const { return m_currentAction; }

private:
	DecisionNode*   m_root;
	DecisionAction* m_currentAction;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(DecisionTree);

#endif  // __DECISION_TREE_H__
