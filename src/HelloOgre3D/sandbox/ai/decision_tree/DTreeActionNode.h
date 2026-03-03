#ifndef __DTREE_ACTION_NODE_H__
#define __DTREE_ACTION_NODE_H__

#include "DTreeNodeBase.h"

class DTreeActionNode : public DTreeNodeBase
{
public:
	DTreeActionNode(DTreeActionBase* action) : m_action(action) {}

	virtual DTreeActionBase* Evaluate(AgentContext& ctx) { return m_action; }
private:
	DTreeActionBase* m_action;
};

#endif  // __DTREE_ACTION_NODE_H__
