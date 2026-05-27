#ifndef __SOLDIER_ANIM_PROFILE_H__
#define __SOLDIER_ANIM_PROFILE_H__

#include <string>

#include "SoldierAnimTypes.h"
#include "GameDefine.h"
#include "GameFunction.h"
#include "components/anim/IAnimProfile.h"

class AgentAnimStateMachine;

class SoldierAnimProfile : public IAnimProfile
{
public:
	// Soldier 专属的 state id <-> state name 映射（与 SOLDIER_STATE 枚举顺序对应）。
	// 以前这张表硬编码在通用的 AgentAnimState 里，导致"通用 ASM 知道士兵状态名"；
	// 现在作为 soldier 侧数据收敛在此，通过 resolver 注入给 ASM。
	static std::string GetStateNameById(int stateId);
	static int GetStateIdByName(const std::string& stateName);

	static int ResolveBodyLocomotionState(int stanceType, SoldierLocomotionIntent locomotionIntent);
	static int ResolveBodyActionState(int stanceType, SoldierActionIntent actionIntent);

	// 根据当前武器 ASM 的状态名（如 "sniper_idle" / "smg_idle"）解析出目标武器状态。
	// 若 intent 在当前武器下没有对应动画（例如 SMG 没有 reload 动画），返回空串，
	// 交由调用方决定是否跳过武器 ASM 切换。
	static std::string ResolveWeaponState(SoldierActionIntent actionIntent, const std::string& currentWeaponState);

	static void RegisterDefaultBodyNotifies(AgentAnimStateMachine& animStateMachine);
};

#endif  // __SOLDIER_ANIM_PROFILE_H__
