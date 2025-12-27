#ifndef __DTEVAL_CRITICAL_HEALTH_H__
#define __DTEVAL_CRITICAL_HEALTH_H__

#include "DTEvaluator.h"
#include "AgentContext.h"

class DTEval_CriticalHealth : public DTEvaluator
{
public:
	virtual int Evaluate(AgentContext& ctx)
	{
		float ratio = 0.0f;
		if (ctx.maxHealth > 0.0f)
		{
			ratio = ctx.health / ctx.maxHealth;
		}
		return ratio < 0.3f ? 0 : 1;
	}
};

#endif  // __DTEVAL_CRITICAL_HEALTH_H__
