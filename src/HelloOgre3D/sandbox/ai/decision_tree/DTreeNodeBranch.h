#ifndef __DTREE_NODE_BRANCH_H__
#define __DTREE_NODE_BRANCH_H__

#include <vector>
#include "DTreeNodeBase.h"

class DTreeEvaluator;
class DTreeActionBase;

class DTreeNodeBranch : public DTreeNodeBase
{
public:
	DTreeNodeBranch() : m_pEval(nullptr) {}
	virtual ~DTreeNodeBranch() {}

	void SetEvaluator(DTreeEvaluator* pEval);
	void AddChild(DTreeNodeBase* pNode);

	// 返回要选择的child下标
	virtual DTreeActionBase* Evaluate(AgentContext& ctx);

private:
	DTreeEvaluator* m_pEval;
	std::vector<DTreeNodeBase*> m_children;
};

#endif  // __DTREE_NODE_BRANCH_H__
