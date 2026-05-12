#ifndef __SOLDIER_OBJECT__
#define __SOLDIER_OBJECT__

#include "AgentObject.h"

class IPlayerInput;
class SoldierAnimController;
class DecisionTreeDriver;
class BehaviorTreeDriver;
class IDecisionDriver;
class AgentStateController;
class SoldierObject : public AgentObject //tolua_exports
{ //tolua_exports
public:
	SoldierObject(RenderableObject* pAgentBody, btRigidBody* pRigidBody = nullptr);
	virtual ~SoldierObject();

	virtual void Init() override;
	virtual void Update(int deltaMs) override;

	//tolua_begin
	void initWeapon(const Ogre::String& meshFile);
	RenderableObject* getWeapon() { return m_pWeapon; }

	void changeStanceType(int stanceType);
	int getStanceType() { return m_stanceType; }

	void ShootBullet();

	virtual IPlayerInput* GetInput() { return m_inputInfo; }
	virtual void RequestState(int soldierState, bool forceUpdate = false);

	virtual bool HasNextAnim();
	virtual bool IsAnimReadyForMove();
	virtual bool IsAnimReadyForShoot();
	SoldierAnimController* GetAnimController() const { return m_animController; }

	void SetMaxHealth(Ogre::Real maxHealth);
	Ogre::Real GetMaxHealth() const { return m_maxHealth; }

	void SetAmmo(int ammo);
	int GetAmmo() const { return m_ammo; }
	void SetMaxAmmo(int maxAmmo);
	int GetMaxAmmo() const { return m_maxAmmo; }
	bool HasAmmo() const { return m_ammo > 0; }
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

	// Typed accessor for the FSM driver, when soldier is FSM-driven.
	// Returns nullptr when running under a DT (or future BT) driver.
	AgentStateController* GetFsmController() const;

private:
	void ApplyStanceParams(int stanceType);
	void TryApplyPendingStance();
	void SyncWeaponToHandBone();
	void SetEnemy(AgentObject* enemy);
	AgentObject* FindNearestEnemy(const Ogre::String& navMeshName);
	bool IsEnemyValid(AgentObject* enemy, const Ogre::String& navMeshName, bool requirePath) const;

protected:
	void CreateEventDispatcher();
	void RemoveEventDispatcher();

private:
	RenderableObject* m_pWeapon;
	Ogre::Vector3 m_weaponHandOffsetPos = Ogre::Vector3::ZERO;
	Ogre::Quaternion m_weaponHandOffsetOrientation = Ogre::Quaternion::IDENTITY;
	SOLDIER_STANCE_TYPE m_stanceType;
	int m_pendingStanceType = -1;

	Ogre::Real m_maxHealth = 100.0f;
	int m_ammo = 10;
	int m_maxAmmo = 10;

	AgentObject* m_enemy = nullptr;
	int m_enemyId = -1;
	bool m_hasMovePos = false;
	Ogre::Vector3 m_movePos = Ogre::Vector3::ZERO;

	IPlayerInput* m_inputInfo;
	SoldierAnimController* m_animController;

	// One driver to rule them all (FSM / DT / future BT). All implement IDecisionDriver
	// so Update() just calls m_driver->Tick(); typed accessors above downcast on demand.
	IDecisionDriver* m_driver = nullptr;

}; //tolua_exports

REGISTER_LUA_CLASS_NAME(SoldierObject);

#endif  // __SOLDIER_OBJECT__
