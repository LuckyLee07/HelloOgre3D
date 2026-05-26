#ifndef __WEAPON_COMPONENT_H__
#define __WEAPON_COMPONENT_H__

#include "component/IComponent.h"

class WeaponComponent : public IComponent
{
public:
	WeaponComponent() {}
	virtual ~WeaponComponent() {}
};

#endif // __WEAPON_COMPONENT_H__
