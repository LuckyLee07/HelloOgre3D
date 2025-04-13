#ifndef __SOLDIER_OBJECT__
#define __SOLDIER_OBJECT__

#include "AgentObject.h"
#include "SoldierObjDef.h"

//tolua_begin
enum SoldierStance 
{
	SOLDIER_STAND = 0,
	SOLDIER_CROUCH,
};
//tolua_end

class SoldierObject : public AgentObject //tolua_exports
{ //tolua_exports
public:
	SoldierObject(EntityObject* pAgentBody, btRigidBody* pRigidBody = nullptr);
	virtual ~SoldierObject();

	virtual void Initialize();
	virtual void update(int deltaMilisec);
	virtual void handleEventByLua(OIS::KeyCode keycode);

	//tolua_begin
	void initWeapon(const Ogre::String& meshFile);
	EntityObject* getWeapon() { return m_pWeapon; }

	void changeStanceType(int stanceType);
	int getStanceType() { return m_eStance; }

	void RequestState(int soldierState);

	void ShootBullet();
	//tolua_end
	
	void DoShootBullet(const Ogre::Vector3& position, const Ogre::Vector3& rotation);

protected:
	virtual void CreateEventDispatcher();
	virtual void RemoveEventDispatcher();
	
private:
	EntityObject* m_pWeapon;
	SoldierStance m_eStance;
}; //tolua_exports

#endif  // __SOLDIER_OBJECT__
