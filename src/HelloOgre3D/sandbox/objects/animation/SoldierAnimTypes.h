#ifndef __SOLDIER_ANIM_TYPES_H__
#define __SOLDIER_ANIM_TYPES_H__

enum class SoldierLocomotionIntent
{
	None = 0,
	Idle,
	Move,
	Fall,
};

enum class SoldierActionIntent
{
	None = 0,
	Shoot,
	Reload,
	Death,
};

#endif  // __SOLDIER_ANIM_TYPES_H__