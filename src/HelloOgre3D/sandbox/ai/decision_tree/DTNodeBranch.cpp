#include "DTNodeBranch.h"
#include "DTEvaluator.h"
#include <assert.h>

void DTNodeBranch::SetEvaluator(DTEvaluator* pEval)
{
	m_pEval = pEval;
}

void DTNodeBranch::AddChild(DTNodeBase* pNode)
{
	m_children.push_back(pNode);
}

DTActionBase* DTNodeBranch::Evaluate(AgentContext& ctx)
{
	assert(m_pEval != nullptr);
	int idx = m_pEval->Evaluate(ctx);

	if (idx < 0 || idx >= (int)m_children.size())
	{
		assert(false); return nullptr;
	}
	return m_children[idx]->Evaluate(ctx);
}
