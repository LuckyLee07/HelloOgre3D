#ifndef __WEAPON_COMPONENT_H__
#define __WEAPON_COMPONENT_H__

#include "component/IComponent.h"
#include "OgreQuaternion.h"
#include "OgreString.h"
#include "OgreVector3.h"
#include "script/LuaClassNameTraits.h"

class AgentAnim;
class AgentAnimStateMachine;
class RenderComponent;

class WeaponComponent : public IComponent //tolua_exports
{ //tolua_exports
public:
	explicit WeaponComponent(BaseObject* owner = nullptr);
	virtual ~WeaponComponent();

	virtual void onAttach(BaseObject* owner) override;
	virtual void onDetach() override;
	virtual int getUpdateOrder() const override;
	virtual void update(int deltaMs) override;

	void Init(const Ogre::String& meshFile);
	RenderComponent* GetRenderComponent() const { return m_weaponRender; }
	void SetRenderVisible(bool visible);

	//tolua_begin
	AgentAnim* GetAnimation(const char* animationName);
	AgentAnimStateMachine* GetObjectASM() const;
	void SyncToHandBone();
	void ShootBullet();

	void SetAmmo(int ammo);
	int GetAmmo() const { return m_ammo; }
	void SetMaxAmmo(int maxAmmo);
	int GetMaxAmmo() const { return m_maxAmmo; }
	bool HasAmmo() const { return m_ammo > 0; }
	void ConsumeAmmo(int amount);
	void RestoreAmmo();
	//tolua_end

	void DoShootBullet(const Ogre::Vector3& position, const Ogre::Quaternion& orientation);

	const Ogre::Vector3& GetHandOffsetPos() const { return m_handOffsetPos; }
	const Ogre::Quaternion& GetHandOffsetOrientation() const { return m_handOffsetOrientation; }

private:
	RenderComponent* m_weaponRender;
	int m_ammo;
	int m_maxAmmo;
	Ogre::Vector3 m_handOffsetPos;
	Ogre::Quaternion m_handOffsetOrientation;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(WeaponComponent);

#endif // __WEAPON_COMPONENT_H__
