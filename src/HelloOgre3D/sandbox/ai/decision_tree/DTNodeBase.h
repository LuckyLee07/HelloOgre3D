#ifndef __DT_NODE_BASE_H__
#define __DT_NODE_BASE_H__

class DTActionBase;
struct AgentContext;

class DTNodeBase
{
public:
	virtual ~DTNodeBase() {}

	// 返回要选择的child下标
	virtual DTActionBase* Evaluate(AgentContext& ctx) = 0;
};

#endif  // __DT_NODE_BASE_H__
