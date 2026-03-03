#ifndef __DTEVAL_HAS_MOVE_POSITION_H__
#define __DTEVAL_HAS_MOVE_POSITION_H__

#include "DTEvaluator.h"
#include "AgentContext.h"

class DTEval_CriticalHealth : public DTEvaluator
{
public:
	virtual int Evaluate(AgentContext& ctx)
	{
		return ctx.hasMovePos ? 0 : 1;
	}
};

#endif  // __DTEVAL_HAS_MOVE_POSITION_H__
