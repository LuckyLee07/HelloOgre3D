#ifndef __BEHAVIOR_TRACE_H__
#define __BEHAVIOR_TRACE_H__

#include <string>
#include <vector>

#include "BehaviorNode.h"

struct BehaviorTraceRecord
{
	std::string label;
	BehaviorNode::Status status;
};

class BehaviorTraceFrame
{
public:
	BehaviorTraceFrame();

	void Begin(int frameIndex);
	void Record(const BehaviorNode* node, BehaviorNode::Status status);
	std::string Format() const;
	int GetFrameIndex() const { return m_frameIndex; }
	int GetEventCount() const { return (int)m_records.size(); }

private:
	int m_frameIndex;
	std::vector<BehaviorTraceRecord> m_records;
};

class BehaviorTrace
{
public:
	static void SetCurrentFrame(BehaviorTraceFrame* frame);
	static BehaviorTraceFrame* GetCurrentFrame();
	static void Record(const BehaviorNode* node, BehaviorNode::Status status);
	static const char* StatusToString(BehaviorNode::Status status);
};

#endif  // __BEHAVIOR_TRACE_H__
