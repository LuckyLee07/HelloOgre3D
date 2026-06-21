#include "BehaviorTreeDriver.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include "BehaviorTree.h"
#include "BehaviorComposite.h"
#include "BehaviorDecorator.h"
#include "BehaviorUtility.h"
#include "LuaBehaviorAction.h"
#include "LuaCondition.h"
#include "objects/AgentObject.h"
#include "profiling/Profile.h"

namespace
{
	int ResolveTraceSampleInterval()
	{
		const char* value = std::getenv("HELLO_BT_TRACE_SAMPLE_INTERVAL");
		if (value == nullptr || value[0] == '\0')
			return 1;
		return std::max(1, std::atoi(value));
	}

	float ResolveRuntimeTickIntervalMs()
	{
		const char* value = std::getenv("HELLO_BT_TICK_INTERVAL_MS");
		if (value == nullptr || value[0] == '\0')
			return 0.0f;
		return std::max(0.0f, static_cast<float>(std::atof(value)));
	}

	bool ResolveRuntimeTickStaggerEnabled()
	{
		const char* value = std::getenv("HELLO_BT_TICK_STAGGER");
		if (value == nullptr || value[0] == '\0')
			return false;
		return std::strcmp(value, "1") == 0
			|| std::strcmp(value, "true") == 0
			|| std::strcmp(value, "TRUE") == 0
			|| std::strcmp(value, "True") == 0
			|| std::strcmp(value, "yes") == 0
			|| std::strcmp(value, "YES") == 0;
	}

	int ResolveRuntimeMaxTreeTicksPerFrame()
	{
		const char* value = std::getenv("HELLO_BT_MAX_TREE_TICKS_PER_FRAME");
		if (value == nullptr || value[0] == '\0')
			return 0;
		return std::max(0, std::atoi(value));
	}

	float ResolveRuntimeFloatEnv(const char* name, float defaultValue, float minValue)
	{
		const char* value = std::getenv(name);
		if (value == nullptr || value[0] == '\0')
			return defaultValue;
		return std::max(minValue, static_cast<float>(std::atof(value)));
	}

	float ResolveRuntimeDistanceLodNearDistance()
	{
		return ResolveRuntimeFloatEnv("HELLO_BT_DISTANCE_LOD_NEAR", 0.0f, 0.0f);
	}

	float ResolveRuntimeDistanceLodFarDistance()
	{
		return ResolveRuntimeFloatEnv("HELLO_BT_DISTANCE_LOD_FAR", 0.0f, 0.0f);
	}

	float ResolveRuntimeDistanceLodMaxIntervalMultiplier()
	{
		return ResolveRuntimeFloatEnv("HELLO_BT_DISTANCE_LOD_MAX_MULTIPLIER", 1.0f, 1.0f);
	}

	int& RuntimeMaxTreeTicksPerFrame()
	{
		static int value = ResolveRuntimeMaxTreeTicksPerFrame();
		return value;
	}

	int& RuntimeFrameTreeTickCount()
	{
		static int value = 0;
		return value;
	}

	int& RuntimeFrameBudgetSkippedCount()
	{
		static int value = 0;
		return value;
	}

	template<typename T>
	T* PopPool(std::vector<T*>& pool)
	{
		if (pool.empty())
			return nullptr;
		T* value = pool.back();
		pool.pop_back();
		return value;
	}

	template<typename T>
	void DeletePointerVector(std::vector<T*>& values)
	{
		for (T* value : values)
			delete value;
		values.clear();
	}

}

BehaviorTreeDriver::BehaviorTreeDriver(AgentObject* owner, Blackboard* blackboard)
	: m_owner(owner)
	, m_fallbackBlackboard(owner)
	, m_blackboard(blackboard != nullptr ? blackboard : &m_fallbackBlackboard)
	, m_tree(nullptr)
	, m_treeBuildInProgress(false)
	, m_debugTraceEnabled(false)
	, m_debugTracePrintEnabled(false)
	, m_debugTraceSampleInterval(ResolveTraceSampleInterval())
	, m_debugTraceTickCounter(0)
	, m_debugTraceFrameIndex(0)
	, m_totalTickCount(0)
	, m_runtimeTreeTickCount(0)
	, m_runtimeTickSkippedCount(0)
	, m_runtimeBudgetSkippedCount(0)
	, m_runtimeTickIntervalMs(ResolveRuntimeTickIntervalMs())
	, m_runtimeTickElapsedMs(0.0f)
	, m_runtimeTickStaggerEnabled(ResolveRuntimeTickStaggerEnabled())
	, m_runtimeDistanceLodNearDistance(ResolveRuntimeDistanceLodNearDistance())
	, m_runtimeDistanceLodFarDistance(ResolveRuntimeDistanceLodFarDistance())
	, m_runtimeDistanceLodMaxIntervalMultiplier(ResolveRuntimeDistanceLodMaxIntervalMultiplier())
	, m_runtimeLastDistanceLodDistance(-1.0f)
	, m_runtimeLastDistanceLodMultiplier(1.0f)
	, m_runtimeDistanceLodSkippedCount(0)
	, m_traceSampleCount(0)
	, m_traceSkippedCount(0)
	, m_cacheHitCount(0)
	, m_cacheInvalidatedCount(0)
	, m_treeBuildCount(0)
	, m_treeStorageResetCount(0)
	, m_nodeAllocationCount(0)
	, m_nodeReuseCount(0)
	, m_treeAllocationCount(0)
	, m_treeReuseCount(0)
	, m_luaActionAllocationCount(0)
{
	ResetRuntimeTickAccumulator();
}

BehaviorTreeDriver::~BehaviorTreeDriver()
{
	DeleteActiveTreeStorage();
	DeleteNodePools();
	DeleteRetiredLuaActions();
	DeleteRetiredLuaConditions();
}

BehaviorTree* BehaviorTreeDriver::NewTree()
{
	PrepareTreeBuildStorage();
	BehaviorTree* t = PopPool(m_treePool);
	if (t != nullptr)
	{
		++m_treeReuseCount;
		t->SetRoot(nullptr);
	}
	else
	{
		t = new BehaviorTree();
		++m_treeAllocationCount;
	}
	m_ownedTrees.push_back(t);
	return t;
}

BehaviorSequence* BehaviorTreeDriver::NewSequence(float reevaluateMs)
{
	PrepareTreeBuildStorage();
	BehaviorSequence* s = PopPool(m_sequencePool);
	if (s != nullptr)
	{
		++m_nodeReuseCount;
		s->ResetForBuild(reevaluateMs);
	}
	else
	{
		s = new BehaviorSequence(reevaluateMs);
		++m_nodeAllocationCount;
	}
	m_ownedNodes.push_back(s);
	return s;
}

BehaviorSelector* BehaviorTreeDriver::NewSelector(float reevaluateMs)
{
	PrepareTreeBuildStorage();
	BehaviorSelector* s = PopPool(m_selectorPool);
	if (s != nullptr)
	{
		++m_nodeReuseCount;
		s->ResetForBuild(reevaluateMs);
	}
	else
	{
		s = new BehaviorSelector(reevaluateMs);
		++m_nodeAllocationCount;
	}
	m_ownedNodes.push_back(s);
	return s;
}

BehaviorParallel* BehaviorTreeDriver::NewParallel(int successPolicy, int failurePolicy)
{
	PrepareTreeBuildStorage();
	BehaviorParallel* p = PopPool(m_parallelPool);
	if (p != nullptr)
	{
		++m_nodeReuseCount;
		p->ResetForBuild(successPolicy, failurePolicy);
	}
	else
	{
		p = new BehaviorParallel(successPolicy, failurePolicy);
		++m_nodeAllocationCount;
	}
	m_ownedNodes.push_back(p);
	return p;
}

BehaviorRandomSelector* BehaviorTreeDriver::NewRandomSelector()
{
	PrepareTreeBuildStorage();
	BehaviorRandomSelector* r = PopPool(m_randomSelectorPool);
	if (r != nullptr)
	{
		++m_nodeReuseCount;
		r->ResetForBuild();
	}
	else
	{
		r = new BehaviorRandomSelector();
		++m_nodeAllocationCount;
	}
	m_ownedNodes.push_back(r);
	return r;
}

LuaBehaviorAction* BehaviorTreeDriver::NewLuaAction(const std::string& name)
{
	PrepareTreeBuildStorage();
	LuaBehaviorAction* a = new LuaBehaviorAction(name, m_owner, m_blackboard);
	++m_nodeAllocationCount;
	++m_luaActionAllocationCount;
	m_ownedNodes.push_back(a);
	return a;
}

LuaCondition* BehaviorTreeDriver::NewCondition()
{
	PrepareTreeBuildStorage();
	LuaCondition* c = PopPool(m_conditionPool);
	if (c != nullptr)
	{
		++m_nodeReuseCount;
		c->ResetForBuild(m_blackboard);
	}
	else
	{
		c = new LuaCondition(m_blackboard);
		++m_nodeAllocationCount;
	}
	m_ownedNodes.push_back(c);
	return c;
}

BehaviorNode* BehaviorTreeDriver::NewWait(float waitMs)
{
	PrepareTreeBuildStorage();
	BehaviorWait* node = PopPool(m_waitPool);
	if (node != nullptr)
	{
		++m_nodeReuseCount;
		node->ResetForBuild(waitMs);
	}
	else
	{
		node = new BehaviorWait(waitMs);
		++m_nodeAllocationCount;
	}
	m_ownedNodes.push_back(node);
	return node;
}

BehaviorNode* BehaviorTreeDriver::NewInverter(BehaviorNode* child)
{
	PrepareTreeBuildStorage();
	BehaviorInverter* node = PopPool(m_inverterPool);
	if (node != nullptr)
	{
		++m_nodeReuseCount;
		node->ResetForBuild(child);
	}
	else
	{
		node = new BehaviorInverter(child);
		++m_nodeAllocationCount;
	}
	m_ownedNodes.push_back(node);
	return node;
}

BehaviorNode* BehaviorTreeDriver::NewForceSuccess(BehaviorNode* child)
{
	PrepareTreeBuildStorage();
	BehaviorForceSuccess* node = PopPool(m_forceSuccessPool);
	if (node != nullptr)
	{
		++m_nodeReuseCount;
		node->ResetForBuild(child);
	}
	else
	{
		node = new BehaviorForceSuccess(child);
		++m_nodeAllocationCount;
	}
	m_ownedNodes.push_back(node);
	return node;
}

BehaviorNode* BehaviorTreeDriver::NewForceFailure(BehaviorNode* child)
{
	PrepareTreeBuildStorage();
	BehaviorForceFailure* node = PopPool(m_forceFailurePool);
	if (node != nullptr)
	{
		++m_nodeReuseCount;
		node->ResetForBuild(child);
	}
	else
	{
		node = new BehaviorForceFailure(child);
		++m_nodeAllocationCount;
	}
	m_ownedNodes.push_back(node);
	return node;
}

void BehaviorTreeDriver::SetTree(BehaviorTree* tree)
{
	m_tree = tree;
	m_treeBuildInProgress = false;
	++m_treeBuildCount;
	ResetRuntimeTickAccumulator();
}

void BehaviorTreeDriver::SetDebugTraceEnabled(bool enabled)
{
	m_debugTraceEnabled = enabled;
	if (!enabled)
	{
		m_lastDebugTrace.clear();
		m_blackboard->Remove("__bt.trace");
		m_blackboard->Remove("__bt.traceFrame");
		m_blackboard->Remove("__bt.traceEventCount");
		m_blackboard->Remove("__bt.tickCount");
		m_blackboard->Remove("__bt.traceSampleCount");
		m_blackboard->Remove("__bt.traceSkippedCount");
		m_blackboard->Remove("__bt.traceSampleInterval");
		m_blackboard->Remove("__bt.treeTickCount");
		m_blackboard->Remove("__bt.tickSkippedCount");
		m_blackboard->Remove("__bt.budgetSkippedCount");
		m_blackboard->Remove("__bt.tickIntervalMs");
		m_blackboard->Remove("__bt.tickStaggerEnabled");
		m_blackboard->Remove("__bt.maxTreeTicksPerFrame");
		m_blackboard->Remove("__bt.frameTreeTickCount");
		m_blackboard->Remove("__bt.frameBudgetSkippedCount");
		m_blackboard->Remove("__bt.distanceLodNearDistance");
		m_blackboard->Remove("__bt.distanceLodFarDistance");
		m_blackboard->Remove("__bt.distanceLodMaxIntervalMultiplier");
		m_blackboard->Remove("__bt.distanceLodDistance");
		m_blackboard->Remove("__bt.distanceLodMultiplier");
		m_blackboard->Remove("__bt.effectiveTickIntervalMs");
		m_blackboard->Remove("__bt.distanceLodSkippedCount");
		m_blackboard->Remove("__bt.cacheHitCount");
		m_blackboard->Remove("__bt.cacheInvalidatedCount");
		m_blackboard->Remove("__bt.activeNodeCount");
		m_blackboard->Remove("__bt.pooledNodeCount");
		m_blackboard->Remove("__bt.treeBuildCount");
		m_blackboard->Remove("__bt.treeStorageResetCount");
		m_blackboard->Remove("__bt.nodeAllocationCount");
		m_blackboard->Remove("__bt.nodeReuseCount");
		m_blackboard->Remove("__bt.treeAllocationCount");
		m_blackboard->Remove("__bt.treeReuseCount");
		m_blackboard->Remove("__bt.luaActionAllocationCount");
		m_blackboard->Remove("__bt.retiredLuaActionCount");
		m_blackboard->Remove("__bt.retiredLuaConditionCount");
	}
}

void BehaviorTreeDriver::SetDebugTracePrintEnabled(bool enabled)
{
	m_debugTracePrintEnabled = enabled;
}

void BehaviorTreeDriver::SetDebugTraceSampleInterval(int interval)
{
	m_debugTraceSampleInterval = std::max(1, interval);
}

void BehaviorTreeDriver::SetRuntimeTickIntervalMs(float intervalMs)
{
	m_runtimeTickIntervalMs = std::max(0.0f, intervalMs);
	ResetRuntimeTickAccumulator();
}

void BehaviorTreeDriver::SetRuntimeTickStaggerEnabled(bool enabled)
{
	m_runtimeTickStaggerEnabled = enabled;
	ResetRuntimeTickAccumulator();
}

void BehaviorTreeDriver::SetRuntimeDistanceLod(float nearDistance, float farDistance, float maxIntervalMultiplier)
{
	m_runtimeDistanceLodNearDistance = std::max(0.0f, nearDistance);
	m_runtimeDistanceLodFarDistance = std::max(0.0f, farDistance);
	m_runtimeDistanceLodMaxIntervalMultiplier = std::max(1.0f, maxIntervalMultiplier);
	m_runtimeLastDistanceLodDistance = -1.0f;
	m_runtimeLastDistanceLodMultiplier = 1.0f;
	ResetRuntimeTickAccumulator();
}

void BehaviorTreeDriver::SetRuntimeMaxTreeTicksPerFrame(int maxTicksPerFrame)
{
	RuntimeMaxTreeTicksPerFrame() = std::max(0, maxTicksPerFrame);
}

int BehaviorTreeDriver::GetRuntimeMaxTreeTicksPerFrame() const
{
	return RuntimeMaxTreeTicksPerFrame();
}

void BehaviorTreeDriver::SetNodeDebugName(BehaviorNode* node, const std::string& name)
{
	if (node) node->SetDebugName(name);
}

void BehaviorTreeDriver::BeginRuntimeFrame()
{
	RuntimeFrameTreeTickCount() = 0;
	RuntimeFrameBudgetSkippedCount() = 0;
}

std::string BehaviorTreeDriver::BuildRuntimeDebugSummary() const
{
	std::ostringstream stream;
	stream << "[BTStats] ticks=" << m_totalTickCount
		<< " treeTicks=" << m_runtimeTreeTickCount
		<< " tickSkipped=" << m_runtimeTickSkippedCount
		<< " budgetSkipped=" << m_runtimeBudgetSkippedCount
		<< " tickIntervalMs=" << m_runtimeTickIntervalMs
		<< " tickStagger=" << (m_runtimeTickStaggerEnabled ? "true" : "false")
		<< " effectiveTickIntervalMs=" << (m_runtimeTickIntervalMs * m_runtimeLastDistanceLodMultiplier)
		<< " distanceLodNear=" << m_runtimeDistanceLodNearDistance
		<< " distanceLodFar=" << m_runtimeDistanceLodFarDistance
		<< " distanceLodMax=" << m_runtimeDistanceLodMaxIntervalMultiplier
		<< " distanceLodDistance=" << m_runtimeLastDistanceLodDistance
		<< " distanceLodMultiplier=" << m_runtimeLastDistanceLodMultiplier
		<< " distanceLodSkipped=" << m_runtimeDistanceLodSkippedCount
		<< " budgetMax=" << GetRuntimeMaxTreeTicksPerFrame()
		<< " frameTreeTicks=" << RuntimeFrameTreeTickCount()
		<< " frameBudgetSkipped=" << RuntimeFrameBudgetSkippedCount()
		<< " traceSamples=" << m_traceSampleCount
		<< " traceSkipped=" << m_traceSkippedCount
		<< " sampleEvery=" << m_debugTraceSampleInterval
		<< " cacheHits=" << m_cacheHitCount
		<< " invalidated=" << m_cacheInvalidatedCount
		<< " activeNodes=" << m_ownedNodes.size()
		<< " pooledNodes=" << GetPooledNodeCount()
		<< " treeBuilds=" << m_treeBuildCount
		<< " storageResets=" << m_treeStorageResetCount
		<< " nodeAllocs=" << m_nodeAllocationCount
		<< " nodeReuses=" << m_nodeReuseCount
		<< " treeAllocs=" << m_treeAllocationCount
		<< " treeReuses=" << m_treeReuseCount
		<< " luaActionAllocs=" << m_luaActionAllocationCount
		<< " retiredLuaActions=" << m_retiredLuaActions.size()
		<< " retiredLuaConditions=" << m_retiredLuaConditions.size()
		<< " lastTraceFrame=" << m_debugTraceFrameIndex
		<< " lastTraceEvents=" << m_traceFrame.GetEventCount();
	return stream.str();
}

void BehaviorTreeDriver::WriteRuntimeStatsToBlackboard()
{
	if (m_blackboard == nullptr)
		return;

	m_blackboard->SetInt("__bt.tickCount", m_totalTickCount);
	m_blackboard->SetInt("__bt.treeTickCount", m_runtimeTreeTickCount);
	m_blackboard->SetInt("__bt.tickSkippedCount", m_runtimeTickSkippedCount);
	m_blackboard->SetInt("__bt.budgetSkippedCount", m_runtimeBudgetSkippedCount);
	m_blackboard->SetFloat("__bt.tickIntervalMs", m_runtimeTickIntervalMs);
	m_blackboard->SetBool("__bt.tickStaggerEnabled", m_runtimeTickStaggerEnabled);
	m_blackboard->SetInt("__bt.maxTreeTicksPerFrame", GetRuntimeMaxTreeTicksPerFrame());
	m_blackboard->SetInt("__bt.frameTreeTickCount", RuntimeFrameTreeTickCount());
	m_blackboard->SetInt("__bt.frameBudgetSkippedCount", RuntimeFrameBudgetSkippedCount());
	m_blackboard->SetFloat("__bt.distanceLodNearDistance", m_runtimeDistanceLodNearDistance);
	m_blackboard->SetFloat("__bt.distanceLodFarDistance", m_runtimeDistanceLodFarDistance);
	m_blackboard->SetFloat("__bt.distanceLodMaxIntervalMultiplier", m_runtimeDistanceLodMaxIntervalMultiplier);
	m_blackboard->SetFloat("__bt.distanceLodDistance", m_runtimeLastDistanceLodDistance);
	m_blackboard->SetFloat("__bt.distanceLodMultiplier", m_runtimeLastDistanceLodMultiplier);
	m_blackboard->SetFloat("__bt.effectiveTickIntervalMs", m_runtimeTickIntervalMs * m_runtimeLastDistanceLodMultiplier);
	m_blackboard->SetInt("__bt.distanceLodSkippedCount", m_runtimeDistanceLodSkippedCount);
	m_blackboard->SetInt("__bt.traceSampleCount", m_traceSampleCount);
	m_blackboard->SetInt("__bt.traceSkippedCount", m_traceSkippedCount);
	m_blackboard->SetInt("__bt.traceSampleInterval", m_debugTraceSampleInterval);
	m_blackboard->SetInt("__bt.cacheHitCount", m_cacheHitCount);
	m_blackboard->SetInt("__bt.cacheInvalidatedCount", m_cacheInvalidatedCount);
	m_blackboard->SetInt("__bt.activeNodeCount", static_cast<int>(m_ownedNodes.size()));
	m_blackboard->SetInt("__bt.pooledNodeCount", GetPooledNodeCount());
	m_blackboard->SetInt("__bt.treeBuildCount", m_treeBuildCount);
	m_blackboard->SetInt("__bt.treeStorageResetCount", m_treeStorageResetCount);
	m_blackboard->SetInt("__bt.nodeAllocationCount", m_nodeAllocationCount);
	m_blackboard->SetInt("__bt.nodeReuseCount", m_nodeReuseCount);
	m_blackboard->SetInt("__bt.treeAllocationCount", m_treeAllocationCount);
	m_blackboard->SetInt("__bt.treeReuseCount", m_treeReuseCount);
	m_blackboard->SetInt("__bt.luaActionAllocationCount", m_luaActionAllocationCount);
	m_blackboard->SetInt("__bt.retiredLuaActionCount", static_cast<int>(m_retiredLuaActions.size()));
	m_blackboard->SetInt("__bt.retiredLuaConditionCount", static_cast<int>(m_retiredLuaConditions.size()));
}

void BehaviorTreeDriver::RefreshRuntimeCacheStats()
{
	int cacheHits = 0;
	int cacheInvalidated = 0;
	for (BehaviorNode* node : m_ownedNodes)
	{
		LuaCondition* condition = dynamic_cast<LuaCondition*>(node);
		if (condition == nullptr)
			continue;
		cacheHits += condition->GetResultCacheHitCount();
		cacheInvalidated += condition->GetResultCacheInvalidatedCount();
	}
	m_cacheHitCount = cacheHits;
	m_cacheInvalidatedCount = cacheInvalidated;
}

void BehaviorTreeDriver::PrepareTreeBuildStorage()
{
	if (!m_treeBuildInProgress && m_tree != nullptr)
	{
		RecycleActiveTreeStorage();
	}
	m_treeBuildInProgress = true;
}

void BehaviorTreeDriver::RecycleActiveTreeStorage()
{
	const bool hadStorage = m_tree != nullptr || !m_ownedNodes.empty() || !m_ownedTrees.empty();
	if (m_tree != nullptr && m_tree->GetRoot() != nullptr)
		m_tree->GetRoot()->Reset();

	for (size_t i = 0; i < m_ownedNodes.size(); ++i)
	{
		BehaviorNode* node = m_ownedNodes[i];
		if (node == nullptr)
			continue;

		if (LuaBehaviorAction* action = dynamic_cast<LuaBehaviorAction*>(node))
		{
			action->Reset();
			m_retiredLuaActions.push_back(action);
		}
		else if (LuaCondition* condition = dynamic_cast<LuaCondition*>(node))
		{
			m_retiredLuaConditions.push_back(condition);
		}
		else if (BehaviorSequence* sequence = dynamic_cast<BehaviorSequence*>(node))
		{
			sequence->ResetForBuild(-1.0f);
			m_sequencePool.push_back(sequence);
		}
		else if (BehaviorSelector* selector = dynamic_cast<BehaviorSelector*>(node))
		{
			selector->ResetForBuild(-1.0f);
			m_selectorPool.push_back(selector);
		}
		else if (BehaviorParallel* parallel = dynamic_cast<BehaviorParallel*>(node))
		{
			parallel->ResetForBuild(BehaviorParallel::POLICY_ALL, BehaviorParallel::POLICY_ONE);
			m_parallelPool.push_back(parallel);
		}
		else if (BehaviorRandomSelector* randomSelector = dynamic_cast<BehaviorRandomSelector*>(node))
		{
			randomSelector->ResetForBuild();
			m_randomSelectorPool.push_back(randomSelector);
		}
		else if (BehaviorWait* wait = dynamic_cast<BehaviorWait*>(node))
		{
			wait->ResetForBuild(0.0f);
			m_waitPool.push_back(wait);
		}
		else if (BehaviorInverter* inverter = dynamic_cast<BehaviorInverter*>(node))
		{
			inverter->ResetForBuild(nullptr);
			m_inverterPool.push_back(inverter);
		}
		else if (BehaviorForceSuccess* forceSuccess = dynamic_cast<BehaviorForceSuccess*>(node))
		{
			forceSuccess->ResetForBuild(nullptr);
			m_forceSuccessPool.push_back(forceSuccess);
		}
		else if (BehaviorForceFailure* forceFailure = dynamic_cast<BehaviorForceFailure*>(node))
		{
			forceFailure->ResetForBuild(nullptr);
			m_forceFailurePool.push_back(forceFailure);
		}
		else
		{
			delete node;
		}
	}
	m_ownedNodes.clear();

	for (BehaviorTree* tree : m_ownedTrees)
	{
		if (tree == nullptr)
			continue;
		tree->SetRoot(nullptr);
		m_treePool.push_back(tree);
	}
	m_ownedTrees.clear();
	m_tree = nullptr;
	if (hadStorage)
		++m_treeStorageResetCount;
}

void BehaviorTreeDriver::DeleteActiveTreeStorage()
{
	m_tree = nullptr;
	DeletePointerVector(m_ownedNodes);
	DeletePointerVector(m_ownedTrees);
	m_treeBuildInProgress = false;
}

void BehaviorTreeDriver::DeleteNodePools()
{
	DeletePointerVector(m_sequencePool);
	DeletePointerVector(m_selectorPool);
	DeletePointerVector(m_parallelPool);
	DeletePointerVector(m_randomSelectorPool);
	DeletePointerVector(m_conditionPool);
	DeletePointerVector(m_waitPool);
	DeletePointerVector(m_inverterPool);
	DeletePointerVector(m_forceSuccessPool);
	DeletePointerVector(m_forceFailurePool);
	DeletePointerVector(m_treePool);
}

void BehaviorTreeDriver::DeleteRetiredLuaActions()
{
	DeletePointerVector(m_retiredLuaActions);
}

void BehaviorTreeDriver::DeleteRetiredLuaConditions()
{
	DeletePointerVector(m_retiredLuaConditions);
}

int BehaviorTreeDriver::GetPooledNodeCount() const
{
	return static_cast<int>(
		m_sequencePool.size()
		+ m_selectorPool.size()
		+ m_parallelPool.size()
		+ m_randomSelectorPool.size()
		+ m_conditionPool.size()
		+ m_waitPool.size()
		+ m_inverterPool.size()
		+ m_forceSuccessPool.size()
		+ m_forceFailurePool.size());
}

void BehaviorTreeDriver::Init()
{
}

float BehaviorTreeDriver::ComputeRuntimeTickPhaseMs() const
{
	if (!m_runtimeTickStaggerEnabled || m_runtimeTickIntervalMs <= 0.0f || m_owner == nullptr)
		return 0.0f;

	const unsigned int intervalMs = static_cast<unsigned int>(std::max(1.0f, m_runtimeTickIntervalMs));
	return static_cast<float>((m_owner->GetObjId() * 37u) % intervalMs);
}

bool BehaviorTreeDriver::IsRuntimeDistanceLodEnabled() const
{
	return m_runtimeDistanceLodMaxIntervalMultiplier > 1.0f
		&& m_runtimeDistanceLodFarDistance > m_runtimeDistanceLodNearDistance;
}

float BehaviorTreeDriver::ResolveRuntimeDistanceLodDistance() const
{
	if (m_blackboard == nullptr)
		return -1.0f;

	float distance = m_blackboard->GetFloat("perception.targetDistance", -1.0f);
	if (distance >= 0.0f)
		return distance;
	distance = m_blackboard->GetFloat("sense.targetDistance", -1.0f);
	if (distance >= 0.0f)
		return distance;
	distance = m_blackboard->GetFloat("knowledge.enemyDistance", -1.0f);
	if (distance >= 0.0f)
		return distance;
	distance = m_blackboard->GetFloat("memory.lastKnownEnemyDistance", -1.0f);
	if (distance >= 0.0f)
		return distance;

	if (m_owner == nullptr)
		return -1.0f;

	if (m_blackboard->Has("memory.snapshot.lastKnownEnemyPos"))
		return static_cast<float>(std::sqrt(std::max(0.0f, m_owner->GetPosition().squaredDistance(m_blackboard->GetVec3("memory.snapshot.lastKnownEnemyPos")))));
	if (m_blackboard->Has("movePos"))
		return static_cast<float>(std::sqrt(std::max(0.0f, m_owner->GetPosition().squaredDistance(m_blackboard->GetVec3("movePos")))));
	return -1.0f;
}

float BehaviorTreeDriver::ComputeRuntimeDistanceLodMultiplier()
{
	m_runtimeLastDistanceLodDistance = -1.0f;
	m_runtimeLastDistanceLodMultiplier = 1.0f;
	if (!IsRuntimeDistanceLodEnabled() || m_blackboard == nullptr)
		return m_runtimeLastDistanceLodMultiplier;

	const float distance = ResolveRuntimeDistanceLodDistance();
	m_runtimeLastDistanceLodDistance = distance;
	if (distance < 0.0f || distance <= m_runtimeDistanceLodNearDistance)
		return m_runtimeLastDistanceLodMultiplier;

	if (distance >= m_runtimeDistanceLodFarDistance)
	{
		m_runtimeLastDistanceLodMultiplier = m_runtimeDistanceLodMaxIntervalMultiplier;
		return m_runtimeLastDistanceLodMultiplier;
	}

	const float range = std::max(0.001f, m_runtimeDistanceLodFarDistance - m_runtimeDistanceLodNearDistance);
	const float ratio = std::max(0.0f, std::min(1.0f, (distance - m_runtimeDistanceLodNearDistance) / range));
	m_runtimeLastDistanceLodMultiplier = 1.0f + (m_runtimeDistanceLodMaxIntervalMultiplier - 1.0f) * ratio;
	return m_runtimeLastDistanceLodMultiplier;
}

float BehaviorTreeDriver::ComputeRuntimeEffectiveTickIntervalMs()
{
	if (m_runtimeTickIntervalMs <= 0.0f)
	{
		m_runtimeLastDistanceLodMultiplier = 1.0f;
		return 0.0f;
	}
	return m_runtimeTickIntervalMs * ComputeRuntimeDistanceLodMultiplier();
}

void BehaviorTreeDriver::ResetRuntimeTickAccumulator()
{
	m_runtimeTickElapsedMs = ComputeRuntimeTickPhaseMs();
}

bool BehaviorTreeDriver::ResolveRuntimeTick(float deltaMs, float& outDeltaMs)
{
	outDeltaMs = deltaMs;
	const float effectiveTickIntervalMs = ComputeRuntimeEffectiveTickIntervalMs();
	if (effectiveTickIntervalMs <= 0.0f)
		return true;

	if (m_runtimeTreeTickCount == 0 && !m_runtimeTickStaggerEnabled)
	{
		m_runtimeTickElapsedMs = 0.0f;
		return true;
	}

	m_runtimeTickElapsedMs += std::max(0.0f, deltaMs);
	if (m_runtimeTickElapsedMs < effectiveTickIntervalMs)
	{
		if (m_runtimeLastDistanceLodMultiplier > 1.0f)
			++m_runtimeDistanceLodSkippedCount;
		return false;
	}

	outDeltaMs = m_runtimeTickElapsedMs;
	return true;
}

void BehaviorTreeDriver::ConsumeResolvedRuntimeTick()
{
	if (m_runtimeTickIntervalMs > 0.0f)
		m_runtimeTickElapsedMs = 0.0f;
}

bool BehaviorTreeDriver::TryConsumeRuntimeFrameBudget()
{
	const int maxTreeTicksPerFrame = GetRuntimeMaxTreeTicksPerFrame();
	if (maxTreeTicksPerFrame <= 0)
		return true;

	if (RuntimeFrameTreeTickCount() >= maxTreeTicksPerFrame)
	{
		++m_runtimeBudgetSkippedCount;
		++RuntimeFrameBudgetSkippedCount();
		return false;
	}

	++RuntimeFrameTreeTickCount();
	return true;
}

void BehaviorTreeDriver::Tick(float deltaMs)
{
	H3D_PROFILE_SCOPE("BehaviorTreeDriver::Tick");
	DeleteRetiredLuaActions();
	DeleteRetiredLuaConditions();
	if (!m_tree) return;
	++m_totalTickCount;
	float runtimeDeltaMs = deltaMs;
	if (!ResolveRuntimeTick(deltaMs, runtimeDeltaMs))
	{
		++m_runtimeTickSkippedCount;
		WriteRuntimeStatsToBlackboard();
		return;
	}
	if (!TryConsumeRuntimeFrameBudget())
	{
		WriteRuntimeStatsToBlackboard();
		return;
	}
	ConsumeResolvedRuntimeTick();
	++m_runtimeTreeTickCount;
	if (!m_debugTraceEnabled)
	{
		m_tree->Tick(runtimeDeltaMs);
		RefreshRuntimeCacheStats();
		WriteRuntimeStatsToBlackboard();
		return;
	}

	const int sampleInterval = std::max(1, m_debugTraceSampleInterval);
	const bool shouldTrace = sampleInterval <= 1 || (m_debugTraceTickCounter % sampleInterval) == 0;
	++m_debugTraceTickCounter;
	if (!shouldTrace)
	{
		++m_traceSkippedCount;
		m_tree->Tick(runtimeDeltaMs);
		RefreshRuntimeCacheStats();
		WriteRuntimeStatsToBlackboard();
		return;
	}

	m_traceFrame.Begin(++m_debugTraceFrameIndex);

	BehaviorTraceFrame* prevFrame = BehaviorTrace::GetCurrentFrame();
	BehaviorTrace::SetCurrentFrame(&m_traceFrame);
	m_tree->Tick(runtimeDeltaMs);
	BehaviorTrace::SetCurrentFrame(prevFrame);
	RefreshRuntimeCacheStats();

	m_lastDebugTrace = m_traceFrame.Format();
	++m_traceSampleCount;
	m_blackboard->SetString("__bt.trace", m_lastDebugTrace);
	m_blackboard->SetInt("__bt.traceFrame", m_debugTraceFrameIndex);
	m_blackboard->SetInt("__bt.traceEventCount", m_traceFrame.GetEventCount());
	WriteRuntimeStatsToBlackboard();

	if (m_debugTracePrintEnabled)
	{
		printf("%s\n", m_lastDebugTrace.c_str());
	}
}
