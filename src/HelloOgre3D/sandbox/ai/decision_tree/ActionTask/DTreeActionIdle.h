#ifndef __DT_ACTION_IDLE_H__
#define __DT_ACTION_IDLE_H__

#include "DTActionBase.h"
#include "AgentContext.h

class DTActionDie : public DTNodeBase
{
public:
	DTActionDie() : DTNodeBase("Idle") {}

	virtual Status OnUpdate(int deltaMs, AgentContext& ctx)
	{
		return DTActionBase::TERMINATED;
	}
};

#endif  // __DT_ACTION_IDLE_H__
