#ifndef __WEAPON_COMPONENT_H__
#define __WEAPON_COMPONENT_H__

class GameObject;
class WeaponComponent
{
public:
	WeaponComponent() : m_owner(nullptr) {}
	virtual ~WeaponComponent() {}

	virtual void onAttach(GameObject* owner);
	virtual void onDetach();

	virtual void start() {}
	virtual void update(int deltaMs);

	GameObject* getOwner() const;

protected:
	GameObject* m_owner;
};

#endif // __WEAPON_COMPONENT_H__