#ifndef __SOLDIER_OBJECT__
#define __SOLDIER_OBJECT__

#include "AgentObject.h"
#include "components/anim/IAnimContextProvider.h"

class IPlayerInput;
class SoldierAnimController;
class AnimComponent;
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
	//tolua_begin
	virtual void SetRenderVisible(bool visible) override;

	void changeStanceType(int stanceType);
	int getStanceType() const;
	//tolua_end

	//tolua_begin
	virtual IPlayerInput* GetInput() { return m_inputInfo; }
	virtual void RequestState(int soldierState, bool forceUpdate = false);

	virtual bool HasNextAnim();
	virtual bool IsAnimReadyForMove();
	virtual bool IsAnimReadyForShoot();
	SoldierAnimController* GetAnimController() const;
	//tolua_end

	//tolua_begin
	Ogre::Vector3 GetBonePosition(const Ogre::String& boneName) const;
	Ogre::Vector3 GetBoneForward(const Ogre::String& boneName) const;
	//tolua_end

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
