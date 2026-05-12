#include "DecisionTreeDriver.h"

#include "DecisionTree.h"
#include "DecisionBranch.h"
#include "LuaDecisionAction.h"

DecisionTreeDriver::DecisionTreeDriver(SoldierObject* owner)
	: m_owner(owner), m_blackboard(owner), m_tree(nullptr)
{
}

DecisionTreeDriver::~DecisionTreeDriver()
{
	m_tree = nullptr;
	for (DecisionNode* node : m_ownedNodes) delete node;
	m_ownedNodes.clear();
	for (DecisionTree* tree : m_ownedTrees) delete tree;
	m_ownedTrees.clear();
}

DecisionTree* DecisionTreeDriver::NewTree()
{
	DecisionTree* t = new DecisionTree();
	m_ownedTrees.push_back(t);
	return t;
}

DecisionBranch* DecisionTreeDriver::NewBranch()
{
	DecisionBranch* b = new DecisionBranch();
	m_ownedNodes.push_back(b);
	return b;
}

LuaDecisionAction* DecisionTreeDriver::NewLuaAction(const std::string& name)
{
	LuaDecisionAction* a = new LuaDecisionAction(name, m_owner);
	m_ownedNodes.push_back(a);
	return a;
}

void DecisionTreeDriver::SetTree(DecisionTree* tree)
{
	m_tree = tree;
}

void DecisionTreeDriver::Init()
{
}

void DecisionTreeDriver::Tick(float deltaMs)
{
	if (!m_tree) return;
	m_tree->Tick(deltaMs);
}
