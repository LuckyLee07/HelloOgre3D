#ifndef __DT_EVALUATOR_H__
#define __DT_EVALUATOR_H__

struct AgentContext;

class DTEvaluator
{
public:
	virtual ~DTEvaluator() {}

	// 返回要选择的child下标
	virtual int Evaluate(AgentContext& ctx) = 0;
};

#endif  // __DT_EVALUATOR_H__
