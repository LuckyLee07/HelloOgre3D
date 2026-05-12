#include "DecisionTree.h"

#include "DecisionNode.h"
#include "DecisionAction.h"

DecisionTree::DecisionTree()
	: m_root(nullptr), m_currentAction(nullptr)
{
}

DecisionTree::~DecisionTree()
{
	// Nodes are owned by Lua; do not delete anything here. If an action is mid-run
	// we still want to call CleanUp so it can release transient state before the
	// agent dies. Tolerate a partially-torn-down state (the VM may be closing).
	if (m_currentAction && m_currentAction->GetStatus() == DecisionAction::STATUS_TERMINATED)
	{
		m_currentAction->CleanUp();
	}
	m_currentAction = nullptr;
	m_root = nullptr;
}

void DecisionTree::SetRoot(DecisionNode* root)
{
	m_root = root;
}

void DecisionTree::Tick(float deltaMs)
{
	if (!m_root) return;

	if (!m_currentAction)
	{
		m_currentAction = m_root->Resolve();
		if (!m_currentAction) return;
		m_currentAction->Initialize();
	}

	const DecisionAction::Status status = m_currentAction->Update(deltaMs);
	if (status == DecisionAction::STATUS_TERMINATED)
	{
		m_currentAction->CleanUp();
		m_currentAction = nullptr;
	}
}
