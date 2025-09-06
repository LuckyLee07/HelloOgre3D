#ifndef __SOLDIER_OBJECT__
#define __SOLDIER_OBJECT__

#include "AgentObject.h"
#include "AgentTypeDef.h"
#include "state/AgentStateController.h"

class IPlayerInput;
class SoldierObject : public AgentObject //tolua_exports
{ //tolua_exports
public:
	SoldierObject(EntityObject* pAgentBody, btRigidBody* pRigidBody = nullptr);
	virtual ~SoldierObject();

	virtual void Initialize();
	virtual void update(int deltaMilisec);

	//tolua_begin
	void initWeapon(const Ogre::String& meshFile);
	EntityObject* getWeapon() { return m_pWeapon; }

	void changeStanceType(int stanceType);
	int getStanceType() { return m_stanceType; }

	void ShootBullet();

	virtual IPlayerInput* GetInput() { return m_inputInfo; }
	virtual void RequestState(int soldierState, bool forceUpdate = false);

	virtual bool IsHasNextAnim();
	virtual bool IsAnimReadyForMove();
	virtual bool IsAnimReadyForShoot();

	//tolua_end

	void DoShootBullet(const Ogre::Vector3& position, const Ogre::Vector3& rotation);

protected:
	void CreateEventDispatcher();
	void RemoveEventDispatcher();
	
private:
	EntityObject* m_pWeapon;
	SOLDIER_STANCE_TYPE m_stanceType;

	IPlayerInput* m_inputInfo;
	AgentStateController* m_stateController;

}; //tolua_exports

REGISTER_LUA_CLASS_NAME(SoldierObject);

#endif  // __SOLDIER_OBJECT__
