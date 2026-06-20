#ifndef __SOLDIER_OBJECT__
#define __SOLDIER_OBJECT__

#include "AgentObject.h"
#include "components/anim/IAnimContextProvider.h"

class IPlayerInput;
class SoldierAnimController;
class AnimComponent;
class AIController;
class WeaponComponent;
class AgentStateController;
struct AICommand;
class SoldierObject : public AgentObject, public IAnimContextProvider //tolua_exports
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
	//tolua_end
	WeaponComponent* getWeapon();
	//tolua_begin
	virtual void SetRenderVisible(bool visible) override;

	void changeStanceType(int stanceType);
	int getStanceType() const;
	//tolua_end

	void ShootBullet();

	//tolua_begin
	virtual IPlayerInput* GetInput() { return m_inputInfo; }
	virtual void RequestState(int soldierState, bool forceUpdate = false);

	virtual bool HasNextAnim();
	virtual bool IsAnimReadyForMove();
	virtual bool IsAnimReadyForShoot();
	SoldierAnimController* GetAnimController() const;
	//tolua_end

	AIController* GetAIController() const;
	AIController* GetAI() const;

	// Legacy C++ compatibility forwards. Lua scripts should use BaseObject typed
	// component getters or AgentComponentAccess helpers instead.
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

	//tolua_begin
	Ogre::Vector3 GetBonePosition(const Ogre::String& boneName) const;
	Ogre::Vector3 GetBoneForward(const Ogre::String& boneName) const;
	//tolua_end

	bool HasMovePosition(float reachDistance = 1.5f) const;
	void SetMovePosition(const Ogre::Vector3& movePos);
	void ClearMovePosition();
	bool IsTargetReached(float threshold) const;

	// High-level anim intents for Lua actions. SoldierAnimController re-evaluates
	// intent each frame, so using these (rather than RequestState(SSTATE_*)) is
	// what actually makes run/idle/fire/reload/death animations stick.
	void EnterIdleAnim();
	void EnterMoveAnim();
	void EnterShootAnim();
	void EnterReloadAnim();
	void EnterDeathAnim();

	void DoShootBullet(const Ogre::Vector3& position, const Ogre::Quaternion& orientation);
	AnimComponent* GetAnimComponent() const;
	virtual void ApplyCommand(const AICommand& command) override;

	// Typed accessor for the FSM driver, when soldier is FSM-driven.
	// Returns nullptr when running under a DT (or future BT) driver.
	AgentStateController* GetFsmController() const;

	virtual BaseObject* GetAnimOwnerObject() override;
	virtual AgentAnimStateMachine* GetBodyAnimStateMachine() const override;
	virtual AgentAnimStateMachine* GetWeaponAnimStateMachine() const override;
	virtual int GetAnimStanceType() const override;
	virtual bool IsCppAnimControllerEnabled() const override;
	virtual void ExecuteAnimShoot() override;
	virtual AgentStateController* GetAnimFsmController() const override;

private:
	void ApplyStanceParams(int stanceType);
	void TryApplyPendingStance();
	void SyncWeaponToHandBone();
	void ApplyIdleCommand();
	void ApplyMoveCommand();
	void ApplyAttackCommand();
	void ApplyReloadCommand();
	void ApplyDeathCommand();
	void ApplyFireWeaponCommand();
	void ApplyRequestStateCommand(int soldierState, bool forceUpdate);
	void ApplyMoveToCommand(const Ogre::Vector3& targetPosition);
	void ApplyStopCommand();
	void ReportUnsupportedCommand(const AICommand& command) const;

	IPlayerInput* m_inputInfo; // owned; created from injected InputManager and deleted by SoldierObject

}; //tolua_exports

REGISTER_LUA_CLASS_NAME(SoldierObject);

#endif  // __SOLDIER_OBJECT__
