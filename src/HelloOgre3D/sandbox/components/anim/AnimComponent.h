#ifndef __ANIM_COMPONENT_H__
#define __ANIM_COMPONENT_H__

#include "component/IComponent.h"
#include <string>
#include <unordered_map>

class AgentAnim;
class AgentAnimStateMachine;
class IAnimController;
class SoldierAnimController;
class SoldierObject;
namespace Ogre {
	class Entity;
}

class AnimComponent : public IComponent
{
public:
	explicit AnimComponent(SoldierObject* owner = nullptr);
	virtual ~AnimComponent();

	virtual void onAttach(BaseObject* owner) override;
	virtual void onDetach() override;
	virtual void update(int deltaMs) override;

	IAnimController* GetController() const { return m_controller; }
	SoldierAnimController* GetSoldierController() const;

	void InitBodyAnimations(Ogre::Entity* entity, bool canFireEvent = true);
	void InitWeaponAnimations(Ogre::Entity* entity, bool canFireEvent = false);
	void UpdateController(int deltaMs);
	void UpdateBodyAnimations(int deltaMs);
	void UpdateWeaponAnimations(int deltaMs);
	AgentAnim* GetBodyAnimation(const char* animationName);
	AgentAnimStateMachine* GetBodyAsm() const { return m_bodyAsm; }
	AgentAnim* GetWeaponAnimation(const char* animationName);
	AgentAnimStateMachine* GetWeaponAsm() const { return m_weaponAsm; }

	bool HasNextAnim() const;
	bool IsAnimReadyForMove() const;
	bool IsAnimReadyForShoot() const;

private:
	void EnsureController();
	void SubscribeAnimEvents();
	void UnsubscribeAnimEvents();
	AgentAnim* GetAnimation(Ogre::Entity* entity, std::unordered_map<std::string, AgentAnim*>& animations, const char* animationName);
	void UpdateAsm(AgentAnimStateMachine* asmMachine, int deltaMs);
	void ClearAnimations(std::unordered_map<std::string, AgentAnim*>& animations);

private:
	SoldierObject* m_owner;
	IAnimController* m_controller;
	Ogre::Entity* m_bodyEntity;
	Ogre::Entity* m_weaponEntity;
	AgentAnimStateMachine* m_bodyAsm;
	AgentAnimStateMachine* m_weaponAsm;
	std::unordered_map<std::string, AgentAnim*> m_bodyAnimations;
	std::unordered_map<std::string, AgentAnim*> m_weaponAnimations;
	int m_asmStateChangeEventToken;
	int m_asmNotifyEventToken;
};

#endif // __ANIM_COMPONENT_H__
