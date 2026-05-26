#ifndef __WEAPON_COMPONENT_H__
#define __WEAPON_COMPONENT_H__

#include "component/IComponent.h"
#include "OgreQuaternion.h"
#include "OgreString.h"
#include "OgreVector3.h"

class RenderableObject;
class SoldierObject;

class WeaponComponent : public IComponent
{
public:
	explicit WeaponComponent(SoldierObject* owner = nullptr);
	virtual ~WeaponComponent();

	virtual void onAttach(GameObject* owner) override;
	virtual void onDetach() override;
	virtual void update(int deltaMs) override;

	void Init(const Ogre::String& meshFile);
	RenderableObject* GetWeapon() const { return m_weaponBody; }

	void SyncToHandBone();
	void ShootBullet();
	void DoShootBullet(const Ogre::Vector3& position, const Ogre::Quaternion& orientation);

	void SetAmmo(int ammo);
	int GetAmmo() const { return m_ammo; }
	void SetMaxAmmo(int maxAmmo);
	int GetMaxAmmo() const { return m_maxAmmo; }
	bool HasAmmo() const { return m_ammo > 0; }
	void ConsumeAmmo(int amount);
	void RestoreAmmo();

	const Ogre::Vector3& GetHandOffsetPos() const { return m_handOffsetPos; }
	const Ogre::Quaternion& GetHandOffsetOrientation() const { return m_handOffsetOrientation; }

private:
	SoldierObject* m_owner;
	RenderableObject* m_weaponBody;
	int m_ammo;
	int m_maxAmmo;
	Ogre::Vector3 m_handOffsetPos;
	Ogre::Quaternion m_handOffsetOrientation;
};

#endif // __WEAPON_COMPONENT_H__
