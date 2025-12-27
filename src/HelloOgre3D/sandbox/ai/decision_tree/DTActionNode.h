#ifndef __DT_ACTION_NODE_H__
#define __DT_ACTION_NODE_H__

class DTActionNode : public DTNodeBase
{
public:
	DTActionNode(DTActionBase* action) : m_action(action) {}

	virtual DTActionBase* Evaluate(AgentContext& ctx) { return m_action; }
private:
	DTActionBase* m_action;
};

#endif  // __DT_ACTION_LEAF_H__
