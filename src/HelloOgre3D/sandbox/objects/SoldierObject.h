#ifndef __SOLDIER_OBJECT__
#define __SOLDIER_OBJECT__

#include "AgentObject.h"
#include "ai/fsm/AgentStateController.h"

class IPlayerInput;
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

	//tolua_end

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

}; //tolua_exports

REGISTER_LUA_CLASS_NAME(SoldierObject);

#endif  // __SOLDIER_OBJECT__
