#include "BehaviorTrace.h"

#include <sstream>

static BehaviorTraceFrame* s_currentBehaviorTraceFrame = nullptr;

BehaviorTraceFrame::BehaviorTraceFrame()
	: m_frameIndex(0)
{
}

void BehaviorTraceFrame::Begin(int frameIndex)
{
	m_frameIndex = frameIndex;
	m_records.clear();
}

void BehaviorTraceFrame::Record(const BehaviorNode* node, BehaviorNode::Status status)
{
	BehaviorTraceRecord record;
	record.label = node ? node->GetDebugLabel() : "Node";
	record.status = status;
	m_records.push_back(record);
}

std::string BehaviorTraceFrame::Format() const
{
	std::ostringstream ss;
	ss << "[BT_TRACE frame=" << m_frameIndex << " events=" << m_records.size() << "]";
	for (std::vector<BehaviorTraceRecord>::const_iterator iter = m_records.begin(); iter != m_records.end(); ++iter)
	{
		ss << " " << iter->label << "=" << BehaviorTrace::StatusToString(iter->status);
		if (iter + 1 != m_records.end()) ss << " |";
	}
	return ss.str();
}

void BehaviorTrace::SetCurrentFrame(BehaviorTraceFrame* frame)
{
	s_currentBehaviorTraceFrame = frame;
}

BehaviorTraceFrame* BehaviorTrace::GetCurrentFrame()
{
	return s_currentBehaviorTraceFrame;
}

void BehaviorTrace::Record(const BehaviorNode* node, BehaviorNode::Status status)
{
	BehaviorTraceFrame* frame = GetCurrentFrame();
	if (!frame) return;
	frame->Record(node, status);
}

const char* BehaviorTrace::StatusToString(BehaviorNode::Status status)
{
	switch (status)
	{
	case BehaviorNode::STATUS_RUNNING:
		return "RUNNING";
	case BehaviorNode::STATUS_SUCCESS:
		return "SUCCESS";
	case BehaviorNode::STATUS_FAILURE:
		return "FAILURE";
	case BehaviorNode::STATUS_INVALID:
	default:
		return "INVALID";
	}
}
