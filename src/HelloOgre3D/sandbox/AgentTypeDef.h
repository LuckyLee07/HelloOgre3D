#ifndef __AGENT_TYPE_DEF__
#define __AGENT_TYPE_DEF__

//tolua_begin
#define SOLDIER_STAND_SPEED		3.0f
#define SOLDIER_STAND_HEIGHT	1.6f
#define SOLDIER_CROUCH_SPEED	1.0f
#define SOLDIER_CROUCH_HEIGHT	1.3f

enum SOLDIER_STATE
{
    SSTATE_DEAD = 0,
    SSTATE_FIRE,
    SSTATE_IDLE_AIM,
    SSTATE_RUN_FORWARD,
    SSTATE_RUN_BACKWARD,
    SSTATE_DEAD_HEADSHOT,
    SSTATE_FALL_DEAD,
    SSTATE_FALL_IDLE,
    SSTATE_JUMP_LAND,
    SSTATE_JUMP_UP,
    SSTATE_MELEE,
    SSTATE_RELOAD,
    SSTATE_SMG_TRANSFORM,
    SSTATE_SNIPER_TRANSFORM,

    // 不可直接使用
    CROUCH_SSTATE_DEAD,
    CROUCH_SSTATE_FIRE,
    CROUCH_SSTATE_IDLE_AIM,
    CROUCH_SSTATE_FORWARD,

    SSTATE_MAXCOUNT, // 标记位
};

enum SOLDIER_STANCE_TYPE
{
	SOLDIER_STAND = 0,
	SOLDIER_CROUCH,
};

//tolua_end

inline int ConvertAnimID(int animId, int stype)
{
	if (stype == SOLDIER_STAND)
	{
		if (animId == CROUCH_SSTATE_DEAD || animId == CROUCH_SSTATE_FIRE ||
			animId == CROUCH_SSTATE_IDLE_AIM || animId == CROUCH_SSTATE_FORWARD)
		{
			return (animId - SSTATE_MAXCOUNT + 4);
		}
	}
	else if (stype == SOLDIER_CROUCH)
	{
		if (animId == SSTATE_DEAD || animId == SSTATE_FIRE ||
			animId == SSTATE_IDLE_AIM || animId == SSTATE_RUN_FORWARD)
		{
			return (SSTATE_MAXCOUNT - 4 + animId);
		}
	}
	return animId; // 其他状态不处理
}

#endif  // __AGENT_TYPE_DEF__
