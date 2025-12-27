#ifndef __DECISION_TREE_H__
#define __DECISION_TREE_H__

class DTNodeBase;
class DTActionBase;
struct AgentContext;

class DecisionTree
{
public:
	DecisionTree();

	void SetRootNode(DTNodeBase* pRootNode);
	void Update(int deltaMs, AgentContext& ctx);
	
private:
	DTNodeBase* m_pRootNode;
	DTActionBase* m_pCurrAction;
};

#endif  // __DECISION_TREE_H__
