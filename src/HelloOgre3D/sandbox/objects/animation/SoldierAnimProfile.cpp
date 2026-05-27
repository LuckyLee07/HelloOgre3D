#include "SoldierAnimProfile.h"

#include <cstring>

#include "AgentAnimStateMachine.h"

namespace
{
	// 顺序必须与 GameDefine.h 中 SOLDIER_STATE 枚举严格对应
	const char* kSoldierStateNames[] =
	{
		"dead",
		"fire",
		"idle_aim",
		"run_forward",
		"run_backward",
		"dead_headshot",
		"fall_dead",
		"fall_idle",
		"jump_land",
		"jump_up",
		"melee",
		"reload",
		"smg_transform",
		"sniper_transform",

		"crouch_dead",
		"crouch_fire",
		"crouch_idle_aim",
		"crouch_forward",
	};
	constexpr int kSoldierStateCount = (int)(sizeof(kSoldierStateNames) / sizeof(kSoldierStateNames[0]));

	bool StartsWith(const std::string& value, const char* prefix)
	{
		const size_t prefixLen = std::strlen(prefix);
		return value.size() >= prefixLen && std::strncmp(value.c_str(), prefix, prefixLen) == 0;
	}
}

std::string SoldierAnimProfile::GetStateNameById(int stateId)
{
	if (stateId >= 0 && stateId < kSoldierStateCount)
	{
		return kSoldierStateNames[stateId];
	}
	return "";
}

int SoldierAnimProfile::GetStateIdByName(const std::string& stateName)
{
	for (int index = 0; index < kSoldierStateCount; ++index)
	{
		if (std::strcmp(kSoldierStateNames[index], stateName.c_str()) == 0)
		{
			return index;
		}
	}
	return -1;
}

int SoldierAnimProfile::ResolveBodyLocomotionState(int stanceType, SoldierLocomotionIntent locomotionIntent)
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

int SoldierAnimProfile::ResolveBodyActionState(int stanceType, SoldierActionIntent actionIntent)
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

std::string SoldierAnimProfile::ResolveWeaponState(SoldierActionIntent actionIntent, const std::string& currentWeaponState)
{
	// 根据当前武器状态前缀判断武器族类，避免硬编码 sniper 导致 SMG 武器状态错乱。
	const bool isSmg = StartsWith(currentWeaponState, "smg_");
	const bool isSniper = StartsWith(currentWeaponState, "sniper_");

	switch (actionIntent)
	{
	case SoldierActionIntent::Reload:
		// 当前仅 sniper 有 reload 动画资源；SMG 的 reload 目标状态不存在，返回空表示不切换武器 ASM。
		if (isSniper) return "sniper_reload";
		return "";
	case SoldierActionIntent::None:
	case SoldierActionIntent::Shoot:
	case SoldierActionIntent::Death:
	default:
		if (isSmg) return "smg_idle";
		if (isSniper) return "sniper_idle";
		// 未识别武器类型（极少数情况，如 ASM 尚未初始化）：沿用 sniper_idle 作为兜底
		return "sniper_idle";
	}
}

void SoldierAnimProfile::RegisterDefaultBodyNotifies(AgentAnimStateMachine& animStateMachine)
{
	animStateMachine.AddNotify("fire", "shoot_fire", 0.18f, true);
	animStateMachine.AddNotify("fire", "shoot_complete", 0.90f, true);
	animStateMachine.AddNotify("crouch_fire", "shoot_fire", 0.16f, true);
	animStateMachine.AddNotify("crouch_fire", "shoot_complete", 0.88f, true);
	animStateMachine.AddNotify("reload", "reload_complete", 0.92f, true);
}
