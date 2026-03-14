#ifndef __SOLDIER_OBJECT__
#define __SOLDIER_OBJECT__

#include "AgentObject.h"
#include "ai/fsm/AgentStateController.h"

class IPlayerInput;
class SoldierCommandController;
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

	//tolua_end

	virtual bool RequestWeaponState(const std::string& weaponStateName, bool forceUpdate = false);

	bool QueueCommand(AgentCommandType commandType, int priority = 0, int arg = 0, const std::string& source = "") override;
	bool ImmediateCommand(AgentCommandType commandType, int priority = 100, int arg = 0, const std::string& source = "") override;
	void ClearCommands() override;
	bool HasPendingCommands() const override;
	bool HasCurrentCommand() const override;
	int GetPendingCommandCount() const override;
	AgentCommandType GetCurrentCommandType() const override;
	AgentCommandType GetPreviousCommandType() const override;

	void DoShootBullet(const Ogre::Vector3& position, const Ogre::Quaternion& orientation);

private:
	void ApplyStanceParams(int stanceType);
	void TryApplyPendingStance();
	void SyncWeaponToHandBone();

protected:
	void CreateEventDispatcher();
	void RemoveEventDispatcher();
	
private:
	RenderableObject* m_pWeapon;
	Ogre::Vector3 m_weaponHandOffsetPos = Ogre::Vector3::ZERO;
	Ogre::Quaternion m_weaponHandOffsetOrientation = Ogre::Quaternion::IDENTITY;
	SOLDIER_STANCE_TYPE m_stanceType;
	int m_pendingStanceType = -1;

	IPlayerInput* m_inputInfo;
	AgentStateController* m_stateController;
	SoldierCommandController* m_commandController;

}; //tolua_exports

REGISTER_LUA_CLASS_NAME(SoldierObject);

#endif  // __SOLDIER_OBJECT__
