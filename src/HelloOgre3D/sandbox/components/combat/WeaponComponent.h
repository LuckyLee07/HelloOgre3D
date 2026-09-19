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

// Shared by firing and read-only projectile queries; capsule height includes caps.
namespace WeaponProjectileGeometry
{
	const float Height = 0.30f;
	const float Radius = 0.01f;
	const float SpawnOffset = 0.20f;
}

// Immutable shot data is owned by the projectile, not by the firing agent.
// Crossfire projectiles never retain a raw pointer to an agent that can despawn.
class CrossfireProjectileComponent : public IComponent
{
public:
	CrossfireProjectileComponent(unsigned int sourceId, unsigned int sourceTeam, float damage, const Ogre::Vector3& direction);
	bool Consume();
	virtual void update(int deltaMs) override;

	const unsigned int sourceId;
	const unsigned int sourceTeam;
	const float damage;
	const Ogre::Vector3 direction;

private:
	bool m_consumed;
	int m_remainingMs;
};

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
	// Factory configuration; call before Init. The original mesh keeps animation and muzzle ownership.
	void SetCommanderRifleShellEnabled(bool enabled) { m_commanderRifleShellEnabled = enabled; }
	void SetRenderVisible(bool visible);

	//tolua_begin
	AgentAnim* GetAnimation(const char* animationName);
	AgentAnimStateMachine* GetObjectASM() const;
	void SyncToHandBone();
	void ShootBullet();
	void ShootBulletAt(const Ogre::Vector3& worldTarget);
	// A value snapshot, with non-finite components if no muzzle can be resolved.
	Ogre::Vector3 GetMuzzlePosition();

	void SetAmmo(int ammo);
	int GetAmmo() const { return m_ammo; }
	void SetMaxAmmo(int maxAmmo);
	int GetMaxAmmo() const { return m_maxAmmo; }
	bool HasAmmo() const { return m_ammo > 0; }
	void ConsumeAmmo(int amount);
	void RestoreAmmo();
	//tolua_end

	void ShootBulletTowards(const Ogre::Vector3& direction);
	void DoShootBullet(const Ogre::Vector3& position, const Ogre::Quaternion& orientation);

	const Ogre::Vector3& GetHandOffsetPos() const { return m_handOffsetPos; }
	const Ogre::Quaternion& GetHandOffsetOrientation() const { return m_handOffsetOrientation; }

private:
	bool ResolveMuzzleTransform(Ogre::Vector3& position, Ogre::Quaternion& orientation);

	RenderComponent* m_weaponRender;
	RenderComponent* m_weaponShellRender;
	int m_ammo;
	int m_maxAmmo;
	Ogre::Vector3 m_handOffsetPos;
	Ogre::Quaternion m_handOffsetOrientation;
	bool m_commanderRifleShellEnabled;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(WeaponComponent);

#endif // __WEAPON_COMPONENT_H__
