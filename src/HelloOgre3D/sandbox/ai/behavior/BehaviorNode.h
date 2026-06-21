#ifndef __BEHAVIOR_NODE_H__
#define __BEHAVIOR_NODE_H__

#include <string>

#include "script/LuaClassNameTraits.h"

// BT 节点统一状态。
//   RUNNING — 还在跑，下一帧继续
//   SUCCESS / FAILURE — 终结，由父节点决定怎么消化
class BehaviorNode //tolua_exports
{ //tolua_exports
public:
	BehaviorNode();

	enum Status
	{
		STATUS_INVALID = 0,
		STATUS_RUNNING = 1,
		STATUS_SUCCESS = 2,
		STATUS_FAILURE = 3,
	};

	virtual ~BehaviorNode() {}

	//tolua_begin
	void SetDebugName(const std::string& name);
	const std::string& GetDebugName() const { return m_debugName; }
	//tolua_end

	// 每帧由父节点调用；返回当前状态。
	virtual Status Tick(float deltaMs) = 0;

	// 父节点决定放弃当前子树时调用，子树用来清理 RUNNING 状态（OnCleanUp、累积器等）。
	// 默认空实现：纯条件叶不需要做任何事。
	virtual void Reset() {}
	void ClearDebugName();

	virtual const char* GetDebugType() const { return "Node"; }
	std::string GetDebugLabel() const;

protected:
	Status TraceStatus(Status status) const;

private:
	std::string m_debugName;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(BehaviorNode);

#endif  // __BEHAVIOR_NODE_H__
