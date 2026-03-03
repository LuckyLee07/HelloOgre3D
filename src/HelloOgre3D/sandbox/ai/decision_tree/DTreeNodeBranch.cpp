#include "DTreeNodeBranch.h"
#include "DTreeEvaluator.h"
#include <assert.h>

void DTreeNodeBranch::SetEvaluator(DTreeEvaluator* pEval)
{
	m_pEval = pEval;
}

void DTreeNodeBranch::AddChild(DTreeNodeBase* pNode)
{
	m_children.push_back(pNode);
}

DTreeActionBase* DTreeNodeBranch::Evaluate(AgentContext& ctx)
{
	assert(m_pEval != nullptr);
	int idx = m_pEval->Evaluate(ctx);

	if (idx < 0 || idx >= (int)m_children.size())
	{
		assert(false); return nullptr;
	}
	return m_children[idx]->Evaluate(ctx);
}
