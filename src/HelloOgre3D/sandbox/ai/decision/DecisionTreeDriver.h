#ifndef __DECISION_TREE_DRIVER_H__
#define __DECISION_TREE_DRIVER_H__

#include <string>
#include <vector>

#include "ai/common/IDecisionDriver.h"
#include "ai/common/Blackboard.h"
#include "script/LuaClassNameTraits.h"

class AgentObject;
class DecisionTree;
class DecisionBranch;
class LuaDecisionAction;
class DecisionNode;

// Decision-tree flavour of IDecisionDriver. Each AgentObject that uses DT
// holds one of these; the active Blackboard is injected by AIController and
// remains stable across driver switches.
//
// Ownership: every node created via the New*() factory below is owned by the
// driver. The driver deletes them in its destructor. This matches the project
// idiom (Lua never calls C++ ctors directly; it asks an engine object for new
// instances) and avoids tolua double-ownership pitfalls.
class DecisionTreeDriver : public IDecisionDriver //tolua_exports
{ //tolua_exports
public:
	explicit DecisionTreeDriver(AgentObject* owner, Blackboard* blackboard = nullptr);
	virtual ~DecisionTreeDriver();

	//tolua_begin
	AgentObject* GetAgentOwner() const { return m_owner; }
	Blackboard*    GetBlackboard() { return m_blackboard; }

	// Factories — driver retains ownership of every returned pointer.
	DecisionTree*      NewTree();
	DecisionBranch*    NewBranch();
	LuaDecisionAction* NewLuaAction(const std::string& name);

	void           SetTree(DecisionTree* tree);
	DecisionTree*  GetTree() const { return m_tree; }
	//tolua_end

	// IDecisionDriver impl
	virtual void Init() override;
	virtual void Tick(float deltaMs) override;

private:
	AgentObject*                 m_owner;
	Blackboard                   m_fallbackBlackboard;
	Blackboard*                  m_blackboard;
	DecisionTree*                m_tree;            // points into m_ownedTrees
	std::vector<DecisionNode*>   m_ownedNodes;      // branches + actions
	std::vector<DecisionTree*>   m_ownedTrees;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(DecisionTreeDriver);

#endif  // __DECISION_TREE_DRIVER_H__
