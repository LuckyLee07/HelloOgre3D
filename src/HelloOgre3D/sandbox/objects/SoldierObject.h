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
	AgentObject* GetEnemy() const { return m_enemy; }

	bool HasMovePosition(float reachDistance = 1.5f) const;
	void SetMovePosition(const Ogre::Vector3& movePos);
	void ClearMovePosition();
	bool IsTargetReached(float threshold) const;
	//tolua_end

	void DoShootBullet(const Ogre::Vector3& position, const Ogre::Quaternion& orientation);

private:
	void ApplyStanceParams(int stanceType);
	void TryApplyPendingStance();
	void SyncWeaponToHandBone();
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
	bool m_hasMovePos = false;
	Ogre::Vector3 m_movePos = Ogre::Vector3::ZERO;

	IPlayerInput* m_inputInfo;
	AgentStateController* m_stateController;

}; //tolua_exports

REGISTER_LUA_CLASS_NAME(SoldierObject);

#endif  // __SOLDIER_OBJECT__
