#ifndef __IDECISION_DRIVER_H__
#define __IDECISION_DRIVER_H__

// Common driver interface for FSM / DecisionTree / BehaviorTree.
// SoldierObject holds one IDecisionDriver* and calls Tick() every frame.
// All three decision engines (C++ side) implement this, Lua only provides leaves.
class IDecisionDriver
{
public:
	virtual ~IDecisionDriver() {}

	virtual void Init() {}
	virtual void Tick(float deltaMs) = 0;
};

#endif  // __IDECISION_DRIVER_H__
