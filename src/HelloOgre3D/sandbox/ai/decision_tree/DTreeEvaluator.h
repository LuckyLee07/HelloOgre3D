#ifndef __DTREE_EVALUATOR_H__
#define __DTREE_EVALUATOR_H__

struct AgentContext;

class DTreeEvaluator
{
public:
	virtual ~DTreeEvaluator() {}

	// 返回要选择的child下标
	virtual int Evaluate(AgentContext& ctx) = 0;
};

#endif  // __DTREE_EVALUATOR_H__
