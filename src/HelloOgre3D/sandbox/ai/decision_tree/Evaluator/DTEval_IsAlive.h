#ifndef __DTEVAL_IS_ALIVE_H__
#define __DTEVAL_IS_ALIVE_H__

#include "DTEvaluator.h"
#include "AgentContext.h"

class DTEval_CriticalHealth : public DTEvaluator
{
public:
	virtual int Evaluate(AgentContext& ctx)
	{
		return ctx.health > 0.0f ? 0 : 1;
	}
};

#endif  // __DTEVAL_IS_ALIVE_H__
