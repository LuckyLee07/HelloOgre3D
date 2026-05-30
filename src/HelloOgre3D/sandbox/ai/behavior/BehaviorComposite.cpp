#include "BehaviorComposite.h"

#include <cstdlib>

BehaviorComposite::BehaviorComposite()
	: m_runningIdx(-1)
{
}

BehaviorComposite::~BehaviorComposite()
{
	// 节点所有权在 driver 那边，析构这里只断引用。
	m_children.clear();
}

void BehaviorComposite::AddChild(BehaviorNode* child)
{
	if (child) m_children.push_back(child);
}

void BehaviorComposite::Reset()
{
	if (m_runningIdx >= 0 && m_runningIdx < (int)m_children.size())
	{
		BehaviorNode* running = m_children[m_runningIdx];
		if (running) running->Reset();
	}
	m_runningIdx = -1;
}


BehaviorNode::Status BehaviorSequence::Tick(float deltaMs)
{
	if (m_children.empty()) return TraceStatus(STATUS_SUCCESS);

	// 从游标处恢复（首次或全新评估时 m_runningIdx == -1，从 0 开始）
	int i = (m_runningIdx >= 0) ? m_runningIdx : 0;
	for (; i < (int)m_children.size(); ++i)
	{
		BehaviorNode* child = m_children[i];
		if (!child) continue;

		const Status s = child->Tick(deltaMs);
		if (s == STATUS_RUNNING)
		{
			m_runningIdx = i;
			return TraceStatus(STATUS_RUNNING);
		}
		if (s == STATUS_FAILURE)
		{
			m_runningIdx = -1;
			return TraceStatus(STATUS_FAILURE);
		}
		// SUCCESS：继续下一个
	}
	m_runningIdx = -1;
	return TraceStatus(STATUS_SUCCESS);
}


BehaviorNode::Status BehaviorSelector::Tick(float deltaMs)
{
	if (m_children.empty()) return TraceStatus(STATUS_FAILURE);

	int i = (m_runningIdx >= 0) ? m_runningIdx : 0;
	for (; i < (int)m_children.size(); ++i)
	{
		BehaviorNode* child = m_children[i];
		if (!child) continue;

		const Status s = child->Tick(deltaMs);
		if (s == STATUS_RUNNING)
		{
			m_runningIdx = i;
			return TraceStatus(STATUS_RUNNING);
		}
		if (s == STATUS_SUCCESS)
		{
			m_runningIdx = -1;
			return TraceStatus(STATUS_SUCCESS);
		}
		// FAILURE：尝试下一个
	}
	m_runningIdx = -1;
	return TraceStatus(STATUS_FAILURE);
}


BehaviorParallel::BehaviorParallel(int successPolicy, int failurePolicy)
	: m_successPolicy(successPolicy == POLICY_ONE ? POLICY_ONE : POLICY_ALL)
	, m_failurePolicy(failurePolicy == POLICY_ALL ? POLICY_ALL : POLICY_ONE)
{
}

BehaviorNode::Status BehaviorParallel::Tick(float deltaMs)
{
	if (m_children.empty()) return TraceStatus(STATUS_SUCCESS);
	if (m_childStatus.size() != m_children.size())
	{
		m_childStatus.assign(m_children.size(), STATUS_INVALID);
	}

	int successCount = 0;
	int failureCount = 0;
	int runningCount = 0;
	int validCount = 0;

	for (int i = 0; i < (int)m_children.size(); ++i)
	{
		BehaviorNode* child = m_children[i];
		if (!child) continue;

		++validCount;
		Status s = m_childStatus[i];
		if (s == STATUS_INVALID || s == STATUS_RUNNING)
		{
			s = child->Tick(deltaMs);
			m_childStatus[i] = s;
		}

		if (s == STATUS_SUCCESS) ++successCount;
		else if (s == STATUS_FAILURE) ++failureCount;
		else if (s == STATUS_RUNNING) ++runningCount;
	}

	if (validCount == 0)
	{
		Reset();
		return TraceStatus(STATUS_SUCCESS);
	}

	const bool shouldFail =
		(m_failurePolicy == POLICY_ONE && failureCount > 0) ||
		(m_failurePolicy == POLICY_ALL && failureCount == validCount);
	if (shouldFail)
	{
		Reset();
		return TraceStatus(STATUS_FAILURE);
	}

	const bool shouldSucceed =
		(m_successPolicy == POLICY_ONE && successCount > 0) ||
		(m_successPolicy == POLICY_ALL && successCount == validCount);
	if (shouldSucceed)
	{
		Reset();
		return TraceStatus(STATUS_SUCCESS);
	}

	if (runningCount == 0)
	{
		Reset();
		return TraceStatus(STATUS_FAILURE);
	}

	return TraceStatus(STATUS_RUNNING);
}

void BehaviorParallel::Reset()
{
	for (int i = 0; i < (int)m_children.size(); ++i)
	{
		BehaviorNode* child = m_children[i];
		if (child) child->Reset();
	}
	m_childStatus.clear();
	m_runningIdx = -1;
}


void BehaviorRandomSelector::BuildOrder()
{
	m_order.clear();
	for (int i = 0; i < (int)m_children.size(); ++i)
	{
		if (m_children[i]) m_order.push_back(i);
	}

	for (int i = (int)m_order.size() - 1; i > 0; --i)
	{
		const int j = std::rand() % (i + 1);
		const int tmp = m_order[i];
		m_order[i] = m_order[j];
		m_order[j] = tmp;
	}
}

BehaviorNode::Status BehaviorRandomSelector::Tick(float deltaMs)
{
	if (m_children.empty()) return TraceStatus(STATUS_FAILURE);
	if (m_order.empty()) BuildOrder();
	if (m_order.empty()) return TraceStatus(STATUS_FAILURE);

	int orderIdx = (m_runningIdx >= 0) ? m_runningIdx : 0;
	for (; orderIdx < (int)m_order.size(); ++orderIdx)
	{
		const int childIdx = m_order[orderIdx];
		if (childIdx < 0 || childIdx >= (int)m_children.size()) continue;

		BehaviorNode* child = m_children[childIdx];
		if (!child) continue;

		const Status s = child->Tick(deltaMs);
		if (s == STATUS_RUNNING)
		{
			m_runningIdx = orderIdx;
			return TraceStatus(STATUS_RUNNING);
		}
		if (s == STATUS_SUCCESS)
		{
			Reset();
			return TraceStatus(STATUS_SUCCESS);
		}
	}

	Reset();
	return TraceStatus(STATUS_FAILURE);
}

void BehaviorRandomSelector::Reset()
{
	if (m_runningIdx >= 0 && m_runningIdx < (int)m_order.size())
	{
		const int childIdx = m_order[m_runningIdx];
		if (childIdx >= 0 && childIdx < (int)m_children.size())
		{
			BehaviorNode* running = m_children[childIdx];
			if (running) running->Reset();
		}
	}
	m_runningIdx = -1;
	m_order.clear();
}
