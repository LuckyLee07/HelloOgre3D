#include "BehaviorTree.h"

#include "BehaviorNode.h"

BehaviorTree::BehaviorTree() : m_root(nullptr) {}

BehaviorTree::~BehaviorTree()
{
	// Root 是 driver 所有；这里不 delete。
	m_root = nullptr;
}

void BehaviorTree::SetRoot(BehaviorNode* root)
{
	m_root = root;
}

void BehaviorTree::Tick(float deltaMs)
{
	if (!m_root) return;
	m_root->Tick(deltaMs);
}
