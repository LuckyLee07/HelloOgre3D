#ifndef __DECISION_TREE_H__
#define __DECISION_TREE_H__

class DTreeNodeBase;
class DTreeActionBase;
struct AgentContext;

class DecisionTree
{
public:
	DecisionTree();

	void SetRootNode(DTreeNodeBase* pRootNode);
	void Update(int deltaMs, AgentContext& ctx);
	
private:
	DTreeNodeBase* m_pRootNode;
	DTreeActionBase* m_pCurrAction;
};

#endif  // __DECISION_TREE_H__
