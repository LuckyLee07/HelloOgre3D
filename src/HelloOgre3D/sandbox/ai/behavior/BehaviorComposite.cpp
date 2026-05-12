#include "BehaviorComposite.h"

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
	if (m_children.empty()) return STATUS_SUCCESS;

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
			return STATUS_RUNNING;
		}
		if (s == STATUS_FAILURE)
		{
			m_runningIdx = -1;
			return STATUS_FAILURE;
		}
		// SUCCESS：继续下一个
	}
	m_runningIdx = -1;
	return STATUS_SUCCESS;
}


BehaviorNode::Status BehaviorSelector::Tick(float deltaMs)
{
	if (m_children.empty()) return STATUS_FAILURE;

	int i = (m_runningIdx >= 0) ? m_runningIdx : 0;
	for (; i < (int)m_children.size(); ++i)
	{
		BehaviorNode* child = m_children[i];
		if (!child) continue;

		const Status s = child->Tick(deltaMs);
		if (s == STATUS_RUNNING)
		{
			m_runningIdx = i;
			return STATUS_RUNNING;
		}
		if (s == STATUS_SUCCESS)
		{
			m_runningIdx = -1;
			return STATUS_SUCCESS;
		}
		// FAILURE：尝试下一个
	}
	m_runningIdx = -1;
	return STATUS_FAILURE;
}
