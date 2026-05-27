#ifndef __I_ANIM_CONTROLLER_H__
#define __I_ANIM_CONTROLLER_H__

#include <string>

#include "objects/animation/SoldierAnimTypes.h"

class IAnimController
{
public:
	virtual ~IAnimController() {}

	virtual void Update(float deltaTimeInMillis) = 0;
	virtual void OnBodyStateChanged(int stateId) = 0;
	virtual void OnBodyNotify(const std::string& eventName, int stateId, float normalizedTime) = 0;

	virtual void SetLocomotionIntent(SoldierLocomotionIntent intent) = 0;
	virtual SoldierLocomotionIntent GetLocomotionIntent() const = 0;

	virtual bool RequestAction(SoldierActionIntent intent, bool forceRestart = false) = 0;
	virtual void ClearAction(SoldierActionIntent intent) = 0;
	virtual void ClearAllActions() = 0;
	virtual SoldierActionIntent GetActionIntent() const = 0;

	virtual bool ConsumeShootExecution() = 0;
	virtual bool IsMovePresentationReady() const = 0;
	virtual bool IsShootPresentationReady() const = 0;
	virtual bool IsShootPresentationFinished() const = 0;
	virtual bool IsReloadPresentationFinished() const = 0;
	virtual bool HasPendingPresentation() const = 0;
};

#endif // __I_ANIM_CONTROLLER_H__
