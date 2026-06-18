#ifndef __I_ANIM_CONTEXT_PROVIDER_H__
#define __I_ANIM_CONTEXT_PROVIDER_H__

class AgentAnimStateMachine;
class AgentStateController;
class BaseObject;

class IAnimContextProvider
{
public:
	virtual ~IAnimContextProvider() {}

	virtual BaseObject* GetAnimOwnerObject() = 0;
	virtual AgentAnimStateMachine* GetBodyAnimStateMachine() const = 0;
	virtual AgentAnimStateMachine* GetWeaponAnimStateMachine() const = 0;
	virtual int GetAnimStanceType() const = 0;
	virtual bool IsCppAnimControllerEnabled() const = 0;
	virtual void ExecuteAnimShoot() = 0;
	virtual AgentStateController* GetAnimFsmController() const = 0;
};

#endif // __I_ANIM_CONTEXT_PROVIDER_H__
