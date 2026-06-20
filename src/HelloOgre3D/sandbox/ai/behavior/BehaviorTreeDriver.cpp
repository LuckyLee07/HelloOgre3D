#include "BehaviorTreeDriver.h"

#include <algorithm>
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
}

BehaviorTreeDriver::BehaviorTreeDriver(AgentObject* owner, Blackboard* blackboard)
	: m_owner(owner)
	, m_fallbackBlackboard(owner)
	, m_blackboard(blackboard != nullptr ? blackboard : &m_fallbackBlackboard)
	, m_tree(nullptr)
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
	, m_traceSampleCount(0)
	, m_traceSkippedCount(0)
	, m_cacheHitCount(0)
	, m_cacheInvalidatedCount(0)
{
	ResetRuntimeTickAccumulator();
}

BehaviorTreeDriver::~BehaviorTreeDriver()
{
	m_tree = nullptr;
	for (BehaviorNode* node : m_ownedNodes) delete node;
	m_ownedNodes.clear();
	for (BehaviorTree* tree : m_ownedTrees) delete tree;
	m_ownedTrees.clear();
}

BehaviorTree* BehaviorTreeDriver::NewTree()
{
	BehaviorTree* t = new BehaviorTree();
	m_ownedTrees.push_back(t);
	return t;
}

BehaviorSequence* BehaviorTreeDriver::NewSequence(float reevaluateMs)
{
	BehaviorSequence* s = new BehaviorSequence(reevaluateMs);
	m_ownedNodes.push_back(s);
	return s;
}

BehaviorSelector* BehaviorTreeDriver::NewSelector(float reevaluateMs)
{
	BehaviorSelector* s = new BehaviorSelector(reevaluateMs);
	m_ownedNodes.push_back(s);
	return s;
}

BehaviorParallel* BehaviorTreeDriver::NewParallel(int successPolicy, int failurePolicy)
{
	BehaviorParallel* p = new BehaviorParallel(successPolicy, failurePolicy);
	m_ownedNodes.push_back(p);
	return p;
}

BehaviorRandomSelector* BehaviorTreeDriver::NewRandomSelector()
{
	BehaviorRandomSelector* r = new BehaviorRandomSelector();
	m_ownedNodes.push_back(r);
	return r;
}

LuaBehaviorAction* BehaviorTreeDriver::NewLuaAction(const std::string& name)
{
	LuaBehaviorAction* a = new LuaBehaviorAction(name, m_owner, m_blackboard);
	m_ownedNodes.push_back(a);
	return a;
}

LuaCondition* BehaviorTreeDriver::NewCondition()
{
	LuaCondition* c = new LuaCondition(m_blackboard);
	m_ownedNodes.push_back(c);
	return c;
}

BehaviorNode* BehaviorTreeDriver::NewWait(float waitMs)
{
	BehaviorWait* node = new BehaviorWait(waitMs);
	m_ownedNodes.push_back(node);
	return node;
}

BehaviorNode* BehaviorTreeDriver::NewInverter(BehaviorNode* child)
{
	BehaviorInverter* node = new BehaviorInverter(child);
	m_ownedNodes.push_back(node);
	return node;
}

BehaviorNode* BehaviorTreeDriver::NewForceSuccess(BehaviorNode* child)
{
	BehaviorForceSuccess* node = new BehaviorForceSuccess(child);
	m_ownedNodes.push_back(node);
	return node;
}

BehaviorNode* BehaviorTreeDriver::NewForceFailure(BehaviorNode* child)
{
	BehaviorForceFailure* node = new BehaviorForceFailure(child);
	m_ownedNodes.push_back(node);
	return node;
}

void BehaviorTreeDriver::SetTree(BehaviorTree* tree)
{
	m_tree = tree;
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
		m_blackboard->Remove("__bt.cacheHitCount");
		m_blackboard->Remove("__bt.cacheInvalidatedCount");
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
		<< " budgetMax=" << GetRuntimeMaxTreeTicksPerFrame()
		<< " frameTreeTicks=" << RuntimeFrameTreeTickCount()
		<< " frameBudgetSkipped=" << RuntimeFrameBudgetSkippedCount()
		<< " traceSamples=" << m_traceSampleCount
		<< " traceSkipped=" << m_traceSkippedCount
		<< " sampleEvery=" << m_debugTraceSampleInterval
		<< " cacheHits=" << m_cacheHitCount
		<< " invalidated=" << m_cacheInvalidatedCount
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
	m_blackboard->SetInt("__bt.traceSampleCount", m_traceSampleCount);
	m_blackboard->SetInt("__bt.traceSkippedCount", m_traceSkippedCount);
	m_blackboard->SetInt("__bt.traceSampleInterval", m_debugTraceSampleInterval);
	m_blackboard->SetInt("__bt.cacheHitCount", m_cacheHitCount);
	m_blackboard->SetInt("__bt.cacheInvalidatedCount", m_cacheInvalidatedCount);
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

void BehaviorTreeDriver::ResetRuntimeTickAccumulator()
{
	m_runtimeTickElapsedMs = ComputeRuntimeTickPhaseMs();
}

bool BehaviorTreeDriver::ResolveRuntimeTick(float deltaMs, float& outDeltaMs)
{
	outDeltaMs = deltaMs;
	if (m_runtimeTickIntervalMs <= 0.0f)
		return true;

	if (m_runtimeTreeTickCount == 0 && !m_runtimeTickStaggerEnabled)
	{
		m_runtimeTickElapsedMs = 0.0f;
		return true;
	}

	m_runtimeTickElapsedMs += std::max(0.0f, deltaMs);
	if (m_runtimeTickElapsedMs < m_runtimeTickIntervalMs)
		return false;

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
