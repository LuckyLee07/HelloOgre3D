#ifndef __BEHAVIOR_COMPOSITE_H__
#define __BEHAVIOR_COMPOSITE_H__

#include <vector>

#include "BehaviorNode.h"

// Composite 共用基类：维护有序子列表 + 当前 RUNNING 子节点游标 m_runningIdx。
//
// 设计说明：BT 是"非反应式"的简化模型 —— 一旦某个子节点返回 RUNNING，
// 父 Composite 下一帧会从该子节点恢复继续 tick，而不重新从头评估。
// 想让 BT "再考虑一下"，由叶 Action 主动周期性返回 SUCCESS（参考 MoveAction 的 segment 切片做法）。
//
// Lua 端用 driver:NewSequence() / driver:NewSelector() 拿到指针，
// 再用 :AddChild() 装配；driver 持有所有节点的所有权。
class BehaviorComposite : public BehaviorNode //tolua_exports
{ //tolua_exports
public:
	BehaviorComposite();
	virtual ~BehaviorComposite();

	//tolua_begin
	void AddChild(BehaviorNode* child);
	int  GetChildCount() const { return (int)m_children.size(); }
	//tolua_end

	// Reset：通知当前 RUNNING 子节点收尾，并清空游标。
	virtual void Reset() override;

protected:
	std::vector<BehaviorNode*> m_children;  // 非拥有；driver 负责生命周期
	int m_runningIdx;                       // -1 表示从头开始；否则下一帧从该 child 恢复
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(BehaviorComposite);


// Sequence：按顺序 tick 子节点。
//   - 第一个返回 FAILURE 的子节点：本节点立即 FAILURE，剩余子节点不再评估
//   - 子节点 SUCCESS：继续下一个子节点
//   - 子节点 RUNNING：本节点 RUNNING，记下游标
//   - 全部 SUCCESS：本节点 SUCCESS
class BehaviorSequence : public BehaviorComposite //tolua_exports
{ //tolua_exports
public:
	virtual Status Tick(float deltaMs) override;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(BehaviorSequence);


// Selector：按顺序 tick 子节点（fallback 语义）。
//   - 第一个返回 SUCCESS 的子节点：本节点立即 SUCCESS
//   - 子节点 FAILURE：继续下一个
//   - 子节点 RUNNING：本节点 RUNNING，记下游标
//   - 全部 FAILURE：本节点 FAILURE
class BehaviorSelector : public BehaviorComposite //tolua_exports
{ //tolua_exports
public:
	virtual Status Tick(float deltaMs) override;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(BehaviorSelector);

#endif  // __BEHAVIOR_COMPOSITE_H__
