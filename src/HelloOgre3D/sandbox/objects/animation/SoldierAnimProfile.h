#ifndef __SOLDIER_ANIM_PROFILE_H__
#define __SOLDIER_ANIM_PROFILE_H__

#include "SoldierAnimTypes.h"
#include "AgentAnimStateMachine.h"
#include "AgentAnimState.h"
#include "GameDefine.h"
#include "GameFunction.h"

namespace SoldierAnimProfile
{
	inline int ResolveBodyLocomotionState(int stanceType, SoldierLocomotionIntent locomotionIntent)
	{
		switch (locomotionIntent)
		{
		case SoldierLocomotionIntent::Move:
			return ConvertAnimID(SSTATE_RUN_FORWARD, stanceType);
		case SoldierLocomotionIntent::Fall:
			return SSTATE_FALL_IDLE;
		case SoldierLocomotionIntent::None:
		case SoldierLocomotionIntent::Idle:
		default:
			return ConvertAnimID(SSTATE_IDLE_AIM, stanceType);
		}
	}

	inline int ResolveBodyActionState(int stanceType, SoldierActionIntent actionIntent)
	{
		switch (actionIntent)
		{
		case SoldierActionIntent::Shoot:
			return ConvertAnimID(SSTATE_FIRE, stanceType);
		case SoldierActionIntent::Reload:
			return ConvertAnimID(SSTATE_RELOAD, stanceType);
		case SoldierActionIntent::Death:
			return ConvertAnimID(SSTATE_DEAD, stanceType);
		case SoldierActionIntent::None:
		default:
			return -1;
		}
	}

	inline const char* ResolveWeaponState(SoldierActionIntent actionIntent)
	{
		switch (actionIntent)
		{
		case SoldierActionIntent::Reload:
			return "sniper_reload";
		case SoldierActionIntent::None:
		case SoldierActionIntent::Shoot:
		case SoldierActionIntent::Death:
		default:
			return "sniper_idle";
		}
	}

	inline void RegisterDefaultBodyNotifies(AgentAnimStateMachine& animStateMachine)
	{
		animStateMachine.AddNotify("fire", "shoot_fire", 0.18f, true);
		animStateMachine.AddNotify("fire", "shoot_complete", 0.90f, true);
		animStateMachine.AddNotify("crouch_fire", "shoot_fire", 0.16f, true);
		animStateMachine.AddNotify("crouch_fire", "shoot_complete", 0.88f, true);
		animStateMachine.AddNotify("reload", "reload_complete", 0.92f, true);
	}

	inline const std::string& GetStateName(int stateId)
	{
		static std::string emptyState = "";
		const std::string stateName = AgentAnimState::GetNameByID(stateId);
		if (stateName.empty())
		{
			return emptyState;
		}

		static std::string cachedState;
		cachedState = stateName;
		return cachedState;
	}
}

#endif  // __SOLDIER_ANIM_PROFILE_H__