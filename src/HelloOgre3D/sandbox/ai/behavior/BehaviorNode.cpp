#include "BehaviorNode.h"

#include "BehaviorTrace.h"

BehaviorNode::BehaviorNode()
{
}

void BehaviorNode::SetDebugName(const std::string& name)
{
	m_debugName = name;
}

void BehaviorNode::ClearDebugName()
{
	m_debugName.clear();
}

std::string BehaviorNode::GetDebugLabel() const
{
	if (!m_debugName.empty()) return m_debugName;
	return GetDebugType();
}

BehaviorNode::Status BehaviorNode::TraceStatus(Status status) const
{
	BehaviorTrace::Record(this, status);
	return status;
}
