#ifndef __BEHAVIOR_TREE_DRIVER_H__
#define __BEHAVIOR_TREE_DRIVER_H__

#include <string>
#include <vector>

#include "ai/common/IDecisionDriver.h"
#include "ai/common/Blackboard.h"
#include "script/LuaClassNameTraits.h"

class SoldierObject;
class BehaviorTree;
class BehaviorNode;
class BehaviorSequence;
class BehaviorSelector;
class LuaBehaviorAction;
class LuaCondition;

// BT flavour of IDecisionDriver。Mirrors DecisionTreeDriver 的设计：
//   - 每个 SoldierObject 持有一个 driver
//   - driver 持有自己的 Blackboard（agent 间不共享）
//   - 提供 NewXxx 工厂创建并持有所有节点的所有权（绕开 tolua 不暴露 .new() 的问题，
//     避免 tolua 双重所有权问题）
//   - driver 析构时 delete 所有 owned 节点 + 树
class BehaviorTreeDriver : public IDecisionDriver //tolua_exports
{ //tolua_exports
public:
	explicit BehaviorTreeDriver(SoldierObject* owner);
	virtual ~BehaviorTreeDriver();

	//tolua_begin
	SoldierObject* GetOwner() const { return m_owner; }
	Blackboard*    GetBlackboard() { return &m_blackboard; }

	// 节点工厂 —— driver 保留所有权。
	BehaviorTree*       NewTree();
	BehaviorSequence*   NewSequence();
	BehaviorSelector*   NewSelector();
	LuaBehaviorAction*  NewLuaAction(const std::string& name);
	LuaCondition*       NewCondition();

	void          SetTree(BehaviorTree* tree);
	BehaviorTree* GetTree() const { return m_tree; }
	//tolua_end

	// IDecisionDriver impl
	virtual void Init() override;
	virtual void Tick(float deltaMs) override;

private:
	SoldierObject*               m_owner;
	Blackboard                   m_blackboard;
	BehaviorTree*                m_tree;            // 指向 m_ownedTrees 中的某棵
	std::vector<BehaviorNode*>   m_ownedNodes;      // sequences + selectors + actions + conditions
	std::vector<BehaviorTree*>   m_ownedTrees;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(BehaviorTreeDriver);

#endif  // __BEHAVIOR_TREE_DRIVER_H__
