#include "DecisionTree.h"
#include "DTActionBase.h"
#include "DTNodeBase.h"

DecisionTree::DecisionTree() : m_pRootNode(nullptr), m_pCurrAction(nullptr)
{

}

void DecisionTree::SetRootNode(DTNodeBase* pRootNode)
{
	m_pRootNode = pRootNode;
}

void DecisionTree::Update(int deltaMs, AgentContext& ctx)
{
	if (!m_pRootNode)
		return;

	if (!m_pCurrAction)
	{
		m_pCurrAction = m_pRootNode->Evaluate(ctx);
		if (m_pCurrAction)
			m_pCurrAction->Initialize(ctx);
	}

	if (!m_pCurrAction)
		return;

	DTActionBase::Status result = m_pCurrAction->Update(deltaMs, ctx);
	if (result == DTActionBase::TERMINATED)
	{
		m_pCurrAction->ClearUp(ctx);
		m_pCurrAction = nullptr;
	}
}