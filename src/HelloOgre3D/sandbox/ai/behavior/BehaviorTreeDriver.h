#ifndef __BEHAVIOR_TREE_DRIVER_H__
#define __BEHAVIOR_TREE_DRIVER_H__

#include <string>
#include <vector>

#include "ai/common/IDecisionDriver.h"
#include "ai/common/Blackboard.h"
#include "ai/behavior/BehaviorTrace.h"
#include "script/LuaClassNameTraits.h"

class AgentObject;
class BehaviorTree;
class BehaviorNode;
class BehaviorSequence;
class BehaviorSelector;
class BehaviorParallel;
class BehaviorRandomSelector;
class BehaviorWait;
class BehaviorInverter;
class BehaviorForceSuccess;
class BehaviorForceFailure;
class LuaBehaviorAction;
class LuaCondition;

// BT flavour of IDecisionDriver。Mirrors DecisionTreeDriver 的设计：
//   - 每个 AgentObject 持有一个 driver
//   - driver 使用 AIController 注入的 Blackboard（同一 agent 的 driver 间稳定共享）
//   - 提供 NewXxx 工厂创建并持有所有节点的所有权（绕开 tolua 不暴露 .new() 的问题，
//     避免 tolua 双重所有权问题）
//   - driver 析构时 delete 所有 owned 节点 + 树
class BehaviorTreeDriver : public IDecisionDriver //tolua_exports
{ //tolua_exports
public:
	explicit BehaviorTreeDriver(AgentObject* owner, Blackboard* blackboard = nullptr);
	virtual ~BehaviorTreeDriver();

	//tolua_begin
	AgentObject* GetAgentOwner() const { return m_owner; }
	Blackboard*    GetBlackboard() { return m_blackboard; }

	// 节点工厂 —— driver 保留所有权。
	BehaviorTree*       NewTree();
	BehaviorSequence*   NewSequence(float reevaluateMs = -1.0f);
	BehaviorSelector*   NewSelector(float reevaluateMs = -1.0f);
	BehaviorParallel*   NewParallel(int successPolicy = 2, int failurePolicy = 1);
	BehaviorRandomSelector* NewRandomSelector();
	LuaBehaviorAction*  NewLuaAction(const std::string& name);
	LuaCondition*       NewCondition();
	BehaviorNode*       NewWait(float waitMs);
	BehaviorNode*       NewInverter(BehaviorNode* child);
	BehaviorNode*       NewForceSuccess(BehaviorNode* child);
	BehaviorNode*       NewForceFailure(BehaviorNode* child);

	void          SetTree(BehaviorTree* tree);
	BehaviorTree* GetTree() const { return m_tree; }

	void SetDebugTraceEnabled(bool enabled);
	bool IsDebugTraceEnabled() const { return m_debugTraceEnabled; }
	void SetDebugTracePrintEnabled(bool enabled);
	bool IsDebugTracePrintEnabled() const { return m_debugTracePrintEnabled; }
	void SetDebugTraceSampleInterval(int interval);
	int GetDebugTraceSampleInterval() const { return m_debugTraceSampleInterval; }
	void SetRuntimeTickIntervalMs(float intervalMs);
	float GetRuntimeTickIntervalMs() const { return m_runtimeTickIntervalMs; }
	void SetRuntimeTickStaggerEnabled(bool enabled);
	bool IsRuntimeTickStaggerEnabled() const { return m_runtimeTickStaggerEnabled; }
	void SetRuntimeDistanceLod(float nearDistance, float farDistance, float maxIntervalMultiplier);
	float GetRuntimeDistanceLodNearDistance() const { return m_runtimeDistanceLodNearDistance; }
	float GetRuntimeDistanceLodFarDistance() const { return m_runtimeDistanceLodFarDistance; }
	float GetRuntimeDistanceLodMaxIntervalMultiplier() const { return m_runtimeDistanceLodMaxIntervalMultiplier; }
	float GetRuntimeDistanceLodLastMultiplier() const { return m_runtimeLastDistanceLodMultiplier; }
	void SetRuntimeMaxTreeTicksPerFrame(int maxTicksPerFrame);
	int GetRuntimeMaxTreeTicksPerFrame() const;
	const std::string& GetLastDebugTrace() const { return m_lastDebugTrace; }
	int GetDebugTraceFrame() const { return m_debugTraceFrameIndex; }
	void SetNodeDebugName(BehaviorNode* node, const std::string& name);
	//tolua_end

	static void BeginRuntimeFrame();
	std::string BuildRuntimeDebugSummary() const;

	// IDecisionDriver impl
	virtual void Init() override;
	virtual void Tick(float deltaMs) override;

private:
	void WriteRuntimeStatsToBlackboard();
	void RefreshRuntimeCacheStats();
	void PrepareTreeBuildStorage();
	void RecycleActiveTreeStorage();
	void DeleteActiveTreeStorage();
	void DeleteNodePools();
	void DeleteRetiredLuaActions();
	void DeleteRetiredLuaConditions();
	int GetPooledNodeCount() const;
	float ComputeRuntimeTickPhaseMs() const;
	bool IsRuntimeDistanceLodEnabled() const;
	float ResolveRuntimeDistanceLodDistance() const;
	float ComputeRuntimeDistanceLodMultiplier();
	float ComputeRuntimeEffectiveTickIntervalMs();
	void ResetRuntimeTickAccumulator();
	bool ResolveRuntimeTick(float deltaMs, float& outDeltaMs);
	void ConsumeResolvedRuntimeTick();
	bool TryConsumeRuntimeFrameBudget();

	AgentObject*                 m_owner;
	Blackboard                   m_fallbackBlackboard;
	Blackboard*                  m_blackboard;
	BehaviorTree*                m_tree;            // 指向 m_ownedTrees 中的某棵
	std::vector<BehaviorNode*>   m_ownedNodes;      // 当前树 build 持有的 active 节点
	std::vector<BehaviorTree*>   m_ownedTrees;
	std::vector<BehaviorSequence*> m_sequencePool;
	std::vector<BehaviorSelector*> m_selectorPool;
	std::vector<BehaviorParallel*> m_parallelPool;
	std::vector<BehaviorRandomSelector*> m_randomSelectorPool;
	std::vector<LuaCondition*>    m_conditionPool;
	std::vector<BehaviorWait*>    m_waitPool;
	std::vector<BehaviorInverter*> m_inverterPool;
	std::vector<BehaviorForceSuccess*> m_forceSuccessPool;
	std::vector<BehaviorForceFailure*> m_forceFailurePool;
	std::vector<BehaviorTree*>    m_treePool;
	std::vector<LuaBehaviorAction*> m_retiredLuaActions;
	std::vector<LuaCondition*>    m_retiredLuaConditions;
	bool                         m_treeBuildInProgress;
	bool                         m_debugTraceEnabled;
	bool                         m_debugTracePrintEnabled;
	int                          m_debugTraceSampleInterval;
	int                          m_debugTraceTickCounter;
	int                          m_debugTraceFrameIndex;
	int                          m_totalTickCount;
	int                          m_runtimeTreeTickCount;
	int                          m_runtimeTickSkippedCount;
	int                          m_runtimeBudgetSkippedCount;
	float                        m_runtimeTickIntervalMs;
	float                        m_runtimeTickElapsedMs;
	bool                         m_runtimeTickStaggerEnabled;
	float                        m_runtimeDistanceLodNearDistance;
	float                        m_runtimeDistanceLodFarDistance;
	float                        m_runtimeDistanceLodMaxIntervalMultiplier;
	float                        m_runtimeLastDistanceLodDistance;
	float                        m_runtimeLastDistanceLodMultiplier;
	int                          m_runtimeDistanceLodSkippedCount;
	int                          m_traceSampleCount;
	int                          m_traceSkippedCount;
	int                          m_cacheHitCount;
	int                          m_cacheInvalidatedCount;
	int                          m_treeBuildCount;
	int                          m_treeStorageResetCount;
	int                          m_nodeAllocationCount;
	int                          m_nodeReuseCount;
	int                          m_treeAllocationCount;
	int                          m_treeReuseCount;
	int                          m_luaActionAllocationCount;
	std::string                  m_lastDebugTrace;
	BehaviorTraceFrame           m_traceFrame;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(BehaviorTreeDriver);

#endif  // __BEHAVIOR_TREE_DRIVER_H__
