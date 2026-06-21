#ifndef __BEHAVIOR_UTILITY_H__
#define __BEHAVIOR_UTILITY_H__

#include "BehaviorNode.h"

class BehaviorWait : public BehaviorNode
{
public:
	explicit BehaviorWait(float waitMs);
	virtual ~BehaviorWait();

	virtual Status Tick(float deltaMs) override;
	virtual void Reset() override;
	void ResetForBuild(float waitMs);
	virtual const char* GetDebugType() const override { return "Wait"; }

private:
	float m_waitMs;
	float m_elapsedMs;
	bool m_started;
};

#endif  // __BEHAVIOR_UTILITY_H__
