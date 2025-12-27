#ifndef __DT_ACTION_DIE_H__
#define __DT_ACTION_DIE_H__

#include "DTActionBase.h"
#include "AgentContext.h

class DTActionDie : public DTNodeBase
{
public:
	DTActionDie() : DTNodeBase("Die") {}
	
	virtual Status OnUpdate(int deltaMs, AgentContext& ctx)
	{
		return DTActionBase::TERMINATED;
	}
};

#endif  // __DT_ACTION_DIE_H__
