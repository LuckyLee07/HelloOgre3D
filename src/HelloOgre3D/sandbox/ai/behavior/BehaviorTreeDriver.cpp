#include "BehaviorTreeDriver.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
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
	, m_traceSampleCount(0)
	, m_traceSkippedCount(0)
	, m_cacheHitCount(0)
	, m_cacheInvalidatedCount(0)
{
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
	LuaCondition* c = new LuaCondition();
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

void BehaviorTreeDriver::SetNodeDebugName(BehaviorNode* node, const std::string& name)
{
	if (node) node->SetDebugName(name);
}

std::string BehaviorTreeDriver::BuildRuntimeDebugSummary() const
{
	std::ostringstream stream;
	stream << "[BTStats] ticks=" << m_totalTickCount
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
	m_blackboard->SetInt("__bt.traceSampleCount", m_traceSampleCount);
	m_blackboard->SetInt("__bt.traceSkippedCount", m_traceSkippedCount);
	m_blackboard->SetInt("__bt.traceSampleInterval", m_debugTraceSampleInterval);
	m_blackboard->SetInt("__bt.cacheHitCount", m_cacheHitCount);
	m_blackboard->SetInt("__bt.cacheInvalidatedCount", m_cacheInvalidatedCount);
}

void BehaviorTreeDriver::Init()
{
}

void BehaviorTreeDriver::Tick(float deltaMs)
{
	H3D_PROFILE_SCOPE("BehaviorTreeDriver::Tick");
	if (!m_tree) return;
	++m_totalTickCount;
	if (!m_debugTraceEnabled)
	{
		m_tree->Tick(deltaMs);
		return;
	}

	const int sampleInterval = std::max(1, m_debugTraceSampleInterval);
	const bool shouldTrace = sampleInterval <= 1 || (m_debugTraceTickCounter % sampleInterval) == 0;
	++m_debugTraceTickCounter;
	if (!shouldTrace)
	{
		++m_traceSkippedCount;
		m_tree->Tick(deltaMs);
		WriteRuntimeStatsToBlackboard();
		return;
	}

	m_traceFrame.Begin(++m_debugTraceFrameIndex);

	BehaviorTraceFrame* prevFrame = BehaviorTrace::GetCurrentFrame();
	BehaviorTrace::SetCurrentFrame(&m_traceFrame);
	m_tree->Tick(deltaMs);
	BehaviorTrace::SetCurrentFrame(prevFrame);

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
