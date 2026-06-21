#include "BehaviorUtility.h"

BehaviorWait::BehaviorWait(float waitMs)
	: m_waitMs(waitMs), m_elapsedMs(0.0f), m_started(false)
{
	if (m_waitMs < 0.0f) m_waitMs = 0.0f;
}

BehaviorWait::~BehaviorWait()
{
}

BehaviorNode::Status BehaviorWait::Tick(float deltaMs)
{
	if (!m_started)
	{
		m_started = true;
		m_elapsedMs = 0.0f;
	}

	m_elapsedMs += deltaMs;
	if (m_elapsedMs >= m_waitMs)
	{
		Reset();
		return TraceStatus(STATUS_SUCCESS);
	}

	return TraceStatus(STATUS_RUNNING);
}

void BehaviorWait::Reset()
{
	m_started = false;
	m_elapsedMs = 0.0f;
}

void BehaviorWait::ResetForBuild(float waitMs)
{
	Reset();
	m_waitMs = waitMs < 0.0f ? 0.0f : waitMs;
	ClearDebugName();
}
