#include "BehaviorTreeDriver.h"

#include <cstdio>

#include "BehaviorTree.h"
#include "BehaviorComposite.h"
#include "BehaviorDecorator.h"
#include "BehaviorUtility.h"
#include "LuaBehaviorAction.h"
#include "LuaCondition.h"
#include "profiling/Profile.h"

BehaviorTreeDriver::BehaviorTreeDriver(SoldierObject* owner, Blackboard* blackboard)
	: m_owner(owner)
	, m_fallbackBlackboard(owner)
	, m_blackboard(blackboard != nullptr ? blackboard : &m_fallbackBlackboard)
	, m_tree(nullptr)
	, m_debugTraceEnabled(false)
	, m_debugTracePrintEnabled(false)
	, m_debugTraceFrameIndex(0)
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

BehaviorSequence* BehaviorTreeDriver::NewSequence()
{
	BehaviorSequence* s = new BehaviorSequence();
	m_ownedNodes.push_back(s);
	return s;
}

BehaviorSelector* BehaviorTreeDriver::NewSelector()
{
	BehaviorSelector* s = new BehaviorSelector();
	m_ownedNodes.push_back(s);
	return s;
}

LuaBehaviorAction* BehaviorTreeDriver::NewLuaAction(const std::string& name)
{
	LuaBehaviorAction* a = new LuaBehaviorAction(name, m_owner);
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
	}
}

void BehaviorTreeDriver::SetDebugTracePrintEnabled(bool enabled)
{
	m_debugTracePrintEnabled = enabled;
}

void BehaviorTreeDriver::SetNodeDebugName(BehaviorNode* node, const std::string& name)
{
	if (node) node->SetDebugName(name);
}

void BehaviorTreeDriver::Init()
{
}

void BehaviorTreeDriver::Tick(float deltaMs)
{
	H3D_PROFILE_SCOPE("BehaviorTreeDriver::Tick");
	if (!m_tree) return;
	if (!m_debugTraceEnabled)
	{
		m_tree->Tick(deltaMs);
		return;
	}

	m_traceFrame.Begin(++m_debugTraceFrameIndex);

	BehaviorTraceFrame* prevFrame = BehaviorTrace::GetCurrentFrame();
	BehaviorTrace::SetCurrentFrame(&m_traceFrame);
	m_tree->Tick(deltaMs);
	BehaviorTrace::SetCurrentFrame(prevFrame);

	m_lastDebugTrace = m_traceFrame.Format();
	m_blackboard->SetString("__bt.trace", m_lastDebugTrace);
	m_blackboard->SetInt("__bt.traceFrame", m_debugTraceFrameIndex);
	m_blackboard->SetInt("__bt.traceEventCount", m_traceFrame.GetEventCount());

	if (m_debugTracePrintEnabled)
	{
		printf("%s\n", m_lastDebugTrace.c_str());
	}
}
