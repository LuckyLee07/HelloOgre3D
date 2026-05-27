#ifndef __SOLDIER_OBJECT__
#define __SOLDIER_OBJECT__

#include "AgentObject.h"

class IPlayerInput;
class SoldierAnimController;
class AnimComponent;
class AIController;
class AgentAttrib;
class WeaponComponent;
class DecisionTreeDriver;
class BehaviorTreeDriver;
class AgentStateController;
class SoldierObject : public AgentObject //tolua_exports
{ //tolua_exports
public:
	SoldierObject(RenderComponent* renderComp, btRigidBody* pRigidBody = nullptr);
	virtual ~SoldierObject();

	virtual void Init() override;
	virtual void Update(int deltaMs) override;
	void TickAi(int deltaMs);
	void SetAiTickInUpdateEnabled(bool enabled);

	//tolua_begin
	void initWeapon(const Ogre::String& meshFile);
	WeaponComponent* getWeapon();

	void changeStanceType(int stanceType);
	int getStanceType() const;

	void ShootBullet();

	virtual IPlayerInput* GetInput() { return m_inputInfo; }
	virtual void RequestState(int soldierState, bool forceUpdate = false);

	virtual bool HasNextAnim();
	virtual bool IsAnimReadyForMove();
	virtual bool IsAnimReadyForShoot();
	SoldierAnimController* GetAnimController() const;
	AIController* GetAIController() const { return m_ai; }
	AIController* GetAI() const { return m_ai; }

	void SetMaxHealth(Ogre::Real maxHealth);
	Ogre::Real GetMaxHealth() const;

	void SetAmmo(int ammo);
	int GetAmmo() const;
	void SetMaxAmmo(int maxAmmo);
	int GetMaxAmmo() const;
	bool HasAmmo() const;
	void ConsumeAmmo(int amount);
	void RestoreAmmo();

	bool HasEnemy(const Ogre::String& navMeshName = "default");
	bool CanShootEnemy(const Ogre::String& navMeshName = "default", float shootDistance = 3.0f);
	AgentObject* GetEnemy() const;

	bool HasMovePosition(float reachDistance = 1.5f) const;
	void SetMovePosition(const Ogre::Vector3& movePos);
	void ClearMovePosition();
	bool IsTargetReached(float threshold) const;

	// Decision tree wiring. When UseDecisionTreeDriver() is called, the existing
	// FSM state controller is torn down and the soldier is driven by a DT instead.
	// Lua then builds the tree and attaches it via GetDecisionTreeDriver()->SetTree(...).
	void UseDecisionTreeDriver();
	DecisionTreeDriver* GetDecisionTreeDriver() const;

	// Behavior tree wiring — mirror of UseDecisionTreeDriver。
	void UseBehaviorTreeDriver();
	BehaviorTreeDriver* GetBehaviorTreeDriver() const;

	// High-level anim intents for Lua actions. SoldierAnimController re-evaluates
	// intent each frame, so using these (rather than RequestState(SSTATE_*)) is
	// what actually makes run/idle/fire/reload/death animations stick.
	void EnterIdleAnim();
	void EnterMoveAnim();
	void EnterShootAnim();
	void EnterReloadAnim();
	void EnterDeathAnim();
	//tolua_end

	void DoShootBullet(const Ogre::Vector3& position, const Ogre::Quaternion& orientation);
	AnimComponent* GetAnimComponent() const { return m_animComp; }

	// Typed accessor for the FSM driver, when soldier is FSM-driven.
	// Returns nullptr when running under a DT (or future BT) driver.
	AgentStateController* GetFsmController() const;

private:
	void ApplyStanceParams(int stanceType);
	void TryApplyPendingStance();
	void SyncWeaponToHandBone();

private:
	AgentAttrib* m_attrib;
	WeaponComponent* m_weaponComp;
	AIController* m_ai;
	AnimComponent* m_animComp;
	IPlayerInput* m_inputInfo;

}; //tolua_exports

REGISTER_LUA_CLASS_NAME(SoldierObject);

#endif  // __SOLDIER_OBJECT__
