#ifndef __DECISION_TREE_INSTANCE_H__
#define __DECISION_TREE_INSTANCE_H__

#include <vector>
#include "DTEvaluator.h"
#include "DTActionBase.h"
#include "DTNodeBase.h"

class DecisionTreeInstance
{
public:
	DecisionTreeInstance() {}
	~DecisionTreeInstance()
	{
		for (std::size_t i = 0; i < m_evals.size(); ++i)
			SAFE_DELETE(m_evals[i]);
		for (std::size_t i = 0; i < m_actions.size(); ++i)
			SAFE_DELETE(m_actions[i]);
		for (std::size_t i = 0; i < m_nodes.size(); ++i)
			SAFE_DELETE(m_nodes[i]);
	}

public:
	DecisionTree m_dtree;

	std::vector<DTNodeBase*> m_nodes;
	std::vector<DTActionBase*> m_actions;
	std::vector<DTEvaluator*> m_evals;
};

#endif  // __DECISION_TREE_CREATOR_H__
