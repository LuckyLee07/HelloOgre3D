#ifndef __LUA_CONDITION_H__
#define __LUA_CONDITION_H__

#include "BehaviorNode.h"
#include "script/LuaClassNameTraits.h"

// 无状态布尔条件叶。复用 DecisionBranch.SetEvaluator 的 luaL_ref 闭包模式：
//   driver:NewCondition():SetEvaluator(function() return agent:HasEnemy() end)
// Tick：调用闭包，true → SUCCESS，false → FAILURE。永不 RUNNING。
//
// 闭包可以有副作用（例如 HasEnemy 顺手把 enemy 写到 blackboard）。
class LuaCondition : public BehaviorNode //tolua_exports
{ //tolua_exports
public:
	LuaCondition();
	virtual ~LuaCondition();

	// SetEvaluator(function() return bool end) —— 在 ManualToLua.cpp 里手写 binding。
	void SetEvaluatorRef(int luaRef);
	int  GetEvaluatorRef() const { return m_evalLuaRef; }

	virtual Status Tick(float deltaMs) override;

private:
	int m_evalLuaRef;  // LUA_REGISTRYINDEX 上的闭包引用；LUA_NOREF 表示未设置
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(LuaCondition);

#endif  // __LUA_CONDITION_H__
