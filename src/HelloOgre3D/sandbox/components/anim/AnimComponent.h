#ifndef __ANIM_COMPONENT_H__
#define __ANIM_COMPONENT_H__

#include "component/IComponent.h"

class AgentAnimStateMachine;
class IAnimController;
class SoldierAnimController;
class SoldierObject;

class AnimComponent : public IComponent
{
public:
	explicit AnimComponent(SoldierObject* owner = nullptr);
	virtual ~AnimComponent();

	virtual void onAttach(GameObject* owner) override;
	virtual void onDetach() override;
	virtual void update(int deltaMs) override;

	IAnimController* GetController() const { return m_controller; }
	SoldierAnimController* GetSoldierController() const;

	bool HasNextAnim() const;
	bool IsAnimReadyForMove() const;
	bool IsAnimReadyForShoot() const;

private:
	void EnsureController();
	void SubscribeAnimEvents();
	void UnsubscribeAnimEvents();
	AgentAnimStateMachine* GetBodyAsm() const;

private:
	SoldierObject* m_owner;
	IAnimController* m_controller;
	int m_asmStateChangeEventToken;
	int m_asmNotifyEventToken;
};

#endif // __ANIM_COMPONENT_H__
