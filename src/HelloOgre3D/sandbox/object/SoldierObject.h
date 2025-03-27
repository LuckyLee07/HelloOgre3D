#ifndef __SOLDIER_OBJECT__
#define __SOLDIER_OBJECT__

#include "AgentObject.h"

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

	void ShootBullet();
	//tolua_end
	
	void DoShootBullet(const Ogre::Vector3& position, const Ogre::Vector3& rotation);

protected:
	virtual void CreateEventDispatcher();
	virtual void RemoveEventDispatcher();
	
private:
	EntityObject* m_pWeapon;
}; //tolua_exports

#endif  // __SOLDIER_OBJECT__
