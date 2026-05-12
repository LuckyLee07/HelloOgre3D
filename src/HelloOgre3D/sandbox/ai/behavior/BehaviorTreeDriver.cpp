#include "BehaviorTreeDriver.h"

#include "BehaviorTree.h"
#include "BehaviorComposite.h"
#include "LuaBehaviorAction.h"
#include "LuaCondition.h"

BehaviorTreeDriver::BehaviorTreeDriver(SoldierObject* owner)
	: m_owner(owner), m_blackboard(owner), m_tree(nullptr)
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

void BehaviorTreeDriver::SetTree(BehaviorTree* tree)
{
	m_tree = tree;
}

void BehaviorTreeDriver::Init()
{
}

void BehaviorTreeDriver::Tick(float deltaMs)
{
	if (!m_tree) return;
	m_tree->Tick(deltaMs);
}
