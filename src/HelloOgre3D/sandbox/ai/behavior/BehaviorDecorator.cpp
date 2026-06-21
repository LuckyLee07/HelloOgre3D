#include "BehaviorDecorator.h"

BehaviorDecorator::BehaviorDecorator(BehaviorNode* child)
	: m_child(child)
{
}

BehaviorDecorator::~BehaviorDecorator()
{
	m_child = nullptr;
}

void BehaviorDecorator::Reset()
{
	if (m_child) m_child->Reset();
}

void BehaviorDecorator::ResetForBuild(BehaviorNode* child)
{
	m_child = child;
	ClearDebugName();
}

BehaviorInverter::BehaviorInverter(BehaviorNode* child)
	: BehaviorDecorator(child)
{
}

BehaviorNode::Status BehaviorInverter::Tick(float deltaMs)
{
	if (!m_child) return TraceStatus(STATUS_FAILURE);

	const Status status = m_child->Tick(deltaMs);
	if (status == STATUS_RUNNING) return TraceStatus(STATUS_RUNNING);
	if (status == STATUS_SUCCESS) return TraceStatus(STATUS_FAILURE);
	if (status == STATUS_FAILURE) return TraceStatus(STATUS_SUCCESS);
	return TraceStatus(STATUS_FAILURE);
}

BehaviorForceSuccess::BehaviorForceSuccess(BehaviorNode* child)
	: BehaviorDecorator(child)
{
}

BehaviorNode::Status BehaviorForceSuccess::Tick(float deltaMs)
{
	if (!m_child) return TraceStatus(STATUS_SUCCESS);

	const Status status = m_child->Tick(deltaMs);
	if (status == STATUS_RUNNING) return TraceStatus(STATUS_RUNNING);
	return TraceStatus(STATUS_SUCCESS);
}

BehaviorForceFailure::BehaviorForceFailure(BehaviorNode* child)
	: BehaviorDecorator(child)
{
}

BehaviorNode::Status BehaviorForceFailure::Tick(float deltaMs)
{
	if (!m_child) return TraceStatus(STATUS_FAILURE);

	const Status status = m_child->Tick(deltaMs);
	if (status == STATUS_RUNNING) return TraceStatus(STATUS_RUNNING);
	return TraceStatus(STATUS_FAILURE);
}
