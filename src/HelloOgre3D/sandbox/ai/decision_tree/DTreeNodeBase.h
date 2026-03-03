#ifndef __DTREE_NODE_BASE_H__
#define __DTREE_NODE_BASE_H__

class DTreeActionBase;
struct AgentContext;

class DTreeNodeBase
{
public:
	virtual ~DTreeNodeBase() {}

	// 返回要选择的child下标
	virtual DTreeActionBase* Evaluate(AgentContext& ctx) = 0;
};

#endif  // __DTREE_NODE_BASE_H__
