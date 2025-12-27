#ifndef __DT_NODE_BRANCH_H__
#define __DT_NODE_BRANCH_H__

#include <vector>
#include "DTNodeBase.h"

class DTEvaluator;

class DTNodeBranch : public DTNodeBase
{
public:
	DTNodeBranch() : m_pEval(nullptr) {}
	virtual ~DTNodeBranch() {}

	void SetEvaluator(DTEvaluator* pEval);
	void AddChild(DTNodeBase* pNode);

	// 返回要选择的child下标
	virtual DTActionBase* Evaluate(AgentContext& ctx);

private:
	DTEvaluator* m_pEval;
	std::vector<DTNodeBase*> m_children;
};

#endif  // __DT_NODE_BRANCH_H__
