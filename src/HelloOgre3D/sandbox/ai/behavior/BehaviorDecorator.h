#ifndef __BEHAVIOR_DECORATOR_H__
#define __BEHAVIOR_DECORATOR_H__

#include "BehaviorNode.h"

class BehaviorDecorator : public BehaviorNode
{
public:
	explicit BehaviorDecorator(BehaviorNode* child);
	virtual ~BehaviorDecorator();

	virtual void Reset() override;
	void ResetForBuild(BehaviorNode* child);

protected:
	BehaviorNode* m_child;  // non-owning; driver owns all nodes
};

class BehaviorInverter : public BehaviorDecorator
{
public:
	explicit BehaviorInverter(BehaviorNode* child);
	virtual Status Tick(float deltaMs) override;
	virtual const char* GetDebugType() const override { return "Inverter"; }
};

class BehaviorForceSuccess : public BehaviorDecorator
{
public:
	explicit BehaviorForceSuccess(BehaviorNode* child);
	virtual Status Tick(float deltaMs) override;
	virtual const char* GetDebugType() const override { return "ForceSuccess"; }
};

class BehaviorForceFailure : public BehaviorDecorator
{
public:
	explicit BehaviorForceFailure(BehaviorNode* child);
	virtual Status Tick(float deltaMs) override;
	virtual const char* GetDebugType() const override { return "ForceFailure"; }
};

#endif  // __BEHAVIOR_DECORATOR_H__
