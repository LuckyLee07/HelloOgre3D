#ifndef __BEHAVIOR_COMPOSITE_H__
#define __BEHAVIOR_COMPOSITE_H__

#include <vector>

#include "BehaviorNode.h"

// Composite 共用基类：维护有序子列表 + 当前 RUNNING 子节点游标 m_runningIdx。
//
// 默认语义仍是"非反应式"：一旦某个子节点 RUNNING，父 Composite 下一帧从该
// 子节点恢复。配置 reevaluateMs 后，父节点会按间隔从第一个 child 重新检查，
// 用来支持数据驱动触发器、黑板条件变化和上层中断。
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
	void ConfigureReevaluation(float reevaluateMs);
	bool ShouldReevaluate(float deltaMs);
	void ResetRunningChild();
	void ResetRunningChildIfChanged(int nextRunningIdx);

	std::vector<BehaviorNode*> m_children;  // 非拥有；driver 负责生命周期
	int m_runningIdx;                       // -1 表示从头开始；否则下一帧从该 child 恢复
	float m_reevaluateMs;                   // < 0 表示关闭；0 表示每帧重评估
	float m_reevaluateElapsedMs;
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
	explicit BehaviorSequence(float reevaluateMs = -1.0f);

	virtual Status Tick(float deltaMs) override;
	virtual const char* GetDebugType() const override { return "Sequence"; }
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
	explicit BehaviorSelector(float reevaluateMs = -1.0f);

	virtual Status Tick(float deltaMs) override;
	virtual const char* GetDebugType() const override { return "Selector"; }
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(BehaviorSelector);


// Parallel：同一帧 tick 所有未终结子节点。
//   - successPolicy: POLICY_ALL 时全部 SUCCESS 才 SUCCESS；POLICY_ONE 时任一 SUCCESS 即 SUCCESS
//   - failurePolicy: POLICY_ONE 时任一 FAILURE 即 FAILURE；POLICY_ALL 时全部 FAILURE 才 FAILURE
// 默认值是常见的"全成功 / 一失败"语义。
class BehaviorParallel : public BehaviorComposite //tolua_exports
{ //tolua_exports
public:
	enum Policy
	{
		POLICY_ONE = 1,
		POLICY_ALL = 2,
	};

	BehaviorParallel(int successPolicy = POLICY_ALL, int failurePolicy = POLICY_ONE);

	virtual Status Tick(float deltaMs) override;
	virtual void Reset() override;
	virtual const char* GetDebugType() const override { return "Parallel"; }

private:
	std::vector<Status> m_childStatus;
	int m_successPolicy;
	int m_failurePolicy;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(BehaviorParallel);


// RandomSelector：每次从头评估时随机打乱子节点顺序，其余语义同 Selector。
class BehaviorRandomSelector : public BehaviorComposite //tolua_exports
{ //tolua_exports
public:
	virtual Status Tick(float deltaMs) override;
	virtual void Reset() override;
	virtual const char* GetDebugType() const override { return "Random"; }

private:
	void BuildOrder();

	std::vector<int> m_order;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(BehaviorRandomSelector);

#endif  // __BEHAVIOR_COMPOSITE_H__
