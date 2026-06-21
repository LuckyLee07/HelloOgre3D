#ifndef __LUA_CONDITION_H__
#define __LUA_CONDITION_H__

#include <string>
#include <vector>

#include "BehaviorNode.h"
#include "script/LuaClassNameTraits.h"

class Blackboard;

// 无状态布尔条件叶。复用 DecisionBranch.SetEvaluator 的 luaL_ref 闭包模式：
//   driver:NewCondition():SetEvaluator(function() return AgentComponents.HasEnemy(agent) end)
// Tick：调用闭包，true → SUCCESS，false → FAILURE。永不 RUNNING。
//
// 闭包可以有副作用（例如 HasEnemy 顺手把 enemy 写到 blackboard）。
class LuaCondition : public BehaviorNode //tolua_exports
{ //tolua_exports
public:
	explicit LuaCondition(Blackboard* blackboard = nullptr);
	virtual ~LuaCondition();

	// SetEvaluator(function() return bool end) —— 在 ManualToLua.cpp 里手写 binding。
	void SetEvaluatorRef(int luaRef);
	int  GetEvaluatorRef() const { return m_evalLuaRef; }

	//tolua_begin
	void SetResultCacheEnabled(bool enabled);
	bool IsResultCacheEnabled() const { return m_resultCacheEnabled; }
	void SetResultCacheTtlMs(float ttlMs);
	float GetResultCacheTtlMs() const { return m_resultCacheTtlMs; }
	void AddResultCacheDependencyKey(const std::string& key);
	void ClearResultCacheDependencyKeys();
	int GetResultCacheDependencyCount() const;
	int GetResultCacheHitCount() const { return m_resultCacheHitCount; }
	int GetResultCacheInvalidatedCount() const { return m_resultCacheInvalidatedCount; }
	//tolua_end

	virtual Status Tick(float deltaMs) override;
	void ResetForBuild(Blackboard* blackboard);
	virtual const char* GetDebugType() const override { return "Condition"; }

private:
	int ComputeDependencyRevision() const;
	void InvalidateResultCache();

	int m_evalLuaRef;  // LUA_REGISTRYINDEX 上的闭包引用；LUA_NOREF 表示未设置
	Blackboard* m_blackboard;
	bool m_resultCacheEnabled;
	bool m_hasCachedResult;
	float m_resultCacheTtlMs;
	float m_resultCacheAgeMs;
	int m_cachedRevision;
	Status m_cachedStatus;
	std::vector<std::string> m_resultCacheDependencyKeys;
	int m_resultCacheHitCount;
	int m_resultCacheInvalidatedCount;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(LuaCondition);

#endif  // __LUA_CONDITION_H__
