#ifndef __ANIM_COMPONENT_H__
#define __ANIM_COMPONENT_H__

#include "component/IComponent.h"
#include <string>
#include <unordered_map>

class AgentAnim;
class AgentAnimStateMachine;
class IAnimContextProvider;
class IAnimController;
class SoldierAnimController;
namespace Ogre {
	class Entity;
}

class AnimComponent : public IComponent //tolua_exports
{ //tolua_exports
public:
	explicit AnimComponent(BaseObject* owner = nullptr);
	virtual ~AnimComponent();

	virtual void onAttach(BaseObject* owner) override;
	virtual void onDetach() override;
	virtual int getUpdateOrder() const override;
	virtual void update(int deltaMs) override;

	IAnimController* GetController() const { return m_controller; }
	SoldierAnimController* GetSoldierController() const;

	void InitBodyAnimations(Ogre::Entity* entity, bool canFireEvent = true);
	void InitWeaponAnimations(Ogre::Entity* entity, bool canFireEvent = false);
	void UpdateController(int deltaMs);
	void UpdateBodyAnimations(int deltaMs);
	void UpdateWeaponAnimations(int deltaMs);
	//tolua_begin
	AgentAnim* GetBodyAnimation(const char* animationName);
	AgentAnimStateMachine* GetBodyAsm() const { return m_bodyAsm; }
	AgentAnim* GetWeaponAnimation(const char* animationName);
	AgentAnimStateMachine* GetWeaponAsm() const { return m_weaponAsm; }

	bool HasNextAnim() const;
	bool IsAnimReadyForMove() const;
	bool IsAnimReadyForShoot() const;
	bool EnterIdleIntent();
	bool EnterMoveIntent();
	bool EnterShootIntent();
	bool EnterReloadIntent();
	bool EnterDeathIntent();
	//tolua_end

private:
	IAnimContextProvider* GetAnimContext() const;
	void EnsureController();
	void SubscribeAnimEvents();
	void UnsubscribeAnimEvents();
	AgentAnim* GetAnimation(Ogre::Entity* entity, std::unordered_map<std::string, AgentAnim*>& animations, const char* animationName);
	void UpdateAsm(AgentAnimStateMachine* asmMachine, int deltaMs);
	void ClearAnimations(std::unordered_map<std::string, AgentAnim*>& animations);

private:
	IAnimController* m_controller;
	Ogre::Entity* m_bodyEntity; // non-owning; owned by RenderComponent/Ogre scene
	Ogre::Entity* m_weaponEntity; // non-owning; owned by RenderComponent/Ogre scene
	AgentAnimStateMachine* m_bodyAsm;
	AgentAnimStateMachine* m_weaponAsm;
	std::unordered_map<std::string, AgentAnim*> m_bodyAnimations;
	std::unordered_map<std::string, AgentAnim*> m_weaponAnimations;
	long long m_localTimeMs;
	int m_asmStateChangeEventToken;
	int m_asmNotifyEventToken;
}; //tolua_exports

#endif // __ANIM_COMPONENT_H__
