Soldier = {}

Soldier.SoldierStates = {
    CROUCH_DEAD =               "crouch_dead",
    CROUCH_FIRE =               "crouch_fire",
    CROUCH_FORWARD =            "crouch_forward",
    CROUCH_IDLE_AIM =           "crouch_idle_aim",
    STAND_DEAD =                "dead",
    STAND_DEAD_HEADSHOT =       "dead_headshot",
    STAND_FALL_DEAD =           "fall_dead",
    STAND_FALL_IDLE =           "fall_idle",
    STAND_FIRE =                "fire",
    STAND_IDLE_AIM =            "idle_aim",
    STAND_JUMP_LAND =           "jump_land",
    STAND_JUMP_UP =             "jump_up",
    STAND_MELEE =               "melee",
    STAND_RELOAD =              "reload",
    STAND_RUN_BACKWARD =        "run_backward",
    STAND_RUN_FORWARD =         "run_forward",
    STAND_SMG_TRANSFORM =       "smg_transform",
    STAND_SNIPER_TRANSFORM =    "sniper_transform"
}

Soldier.weaponStates = {
    SMG_IDLE =                  "smg_idle",
    SMG_TRANSFORM =             "smg_transform",
    SNIPER_IDLE =               "sniper_idle",
    SNIPER_RELOAD =             "sniper_reload",
    SNIPER_TRANSFORM =          "sniper_transform"
}

function Soldier_InitSoldierAsm(agent)
    local soldier = agent:getAgentBody()

    -- Create an animation state machine to handle soldier animations.
    local soldierAsm = soldier:GetObjectASM();

    local crouchIdleAnim = soldier:GetAnimation("crouch_idle_aim");
    local crouchIdleAnimLength = crouchIdleAnim:GetLength();
    local crouchForward = soldier:GetAnimation("crouch_forward_aim");
    local crouchForwardLength = crouchForward:GetLength();
    local idleAnim = soldier:GetAnimation("stand_idle_aim");
    local idleAnimLength = idleAnim:GetLength();
    local runforward = soldier:GetAnimation("stand_run_forward_aim");
    local runForwardLength = runforward:GetLength();

    soldierAsm:AddState("idle_aim", soldier:GetAnimation("stand_idle_aim"), true);
    soldierAsm:AddState("crouch_idle_aim", soldier:GetAnimation("crouch_idle_aim"), true);
    soldierAsm:AddState("crouch_dead", soldier:GetAnimation("stand_dead_2"), nil, 0.8);
    soldierAsm:AddState("crouch_fire", soldier:GetAnimation("crouch_fire_one_shot"), true);
    soldierAsm:AddState("crouch_forward", soldier:GetAnimation("crouch_forward_aim"), true);
    soldierAsm:AddState("dead", soldier:GetAnimation("stand_dead_2"));
    soldierAsm:AddState("dead_headshot", soldier:GetAnimation("stand_dead_headshot"));
    soldierAsm:AddState("fall_dead", soldier:GetAnimation("stand_dead_2"));
    soldierAsm:AddState("fall_idle", soldier:GetAnimation("stand_idle_aim"), true);
    soldierAsm:AddState("fire", soldier:GetAnimation("stand_fire_one_shot"), true);
    soldierAsm:AddState("jump_land", soldier:GetAnimation("stand_jump_land"));
    soldierAsm:AddState("jump_up", soldier:GetAnimation("stand_jump_up"));
    soldierAsm:AddState("melee", soldier:GetAnimation("stand_melee_1_with_weapon"));
    soldierAsm:AddState("reload", soldier:GetAnimation("stand_reload"));
    soldierAsm:AddState("run_backward", soldier:GetAnimation("stand_run_backward_aim"), true);
    soldierAsm:AddState("run_forward", soldier:GetAnimation("stand_run_forward_aim"), true);
    soldierAsm:AddState("smg_transform", soldier:GetAnimation("stand_smg_transform"));
    soldierAsm:AddState("sniper_transform", soldier:GetAnimation("stand_sniper_transform"));

    soldierAsm:AddTransition("idle_aim", "crouch_idle_aim", idleAnimLength, 0.3);
    soldierAsm:AddTransition("idle_aim", "dead", idleAnimLength, 0.1);
    soldierAsm:AddTransition("idle_aim", "fall_dead", idleAnimLength, 0.15, 1.0);
    soldierAsm:AddTransition("idle_aim", "dead_headshot", idleAnimLength, 0.2);
    soldierAsm:AddTransition("idle_aim", "fire", idleAnimLength, 0.1);
    soldierAsm:AddTransition("idle_aim", "melee", idleAnimLength, 0.2);
    soldierAsm:AddTransition("idle_aim", "reload", idleAnimLength, 0.2);
    soldierAsm:AddTransition("idle_aim", "run_backward", idleAnimLength, 0.2);
    soldierAsm:AddTransition("idle_aim", "run_forward", idleAnimLength, 0.5);
    soldierAsm:AddTransition("idle_aim", "smg_transform", idleAnimLength, 0.2);
    soldierAsm:AddTransition("idle_aim", "sniper_transform", idleAnimLength, 0.2);
    
    soldierAsm:AddTransition("crouch_idle_aim", "idle_aim", crouchIdleAnimLength, 0.3);
    soldierAsm:AddTransition("crouch_idle_aim", "crouch_dead", crouchIdleAnimLength, 0.2, 0.5);
    soldierAsm:AddTransition("crouch_idle_aim", "crouch_fire", crouchIdleAnimLength, 0.1);
    soldierAsm:AddTransition("crouch_idle_aim", "crouch_forward", crouchIdleAnimLength, 0.5);
    soldierAsm:AddTransition("crouch_idle_aim", "fire", crouchIdleAnimLength, 0.5);
    soldierAsm:AddTransition("crouch_idle_aim", "reload", crouchIdleAnimLength, 0.3);
    soldierAsm:AddTransition("crouch_idle_aim", "run_forward", crouchIdleAnimLength, 0.5);
    
    soldierAsm:AddTransition("crouch_fire", "crouch_dead", 0.2, 0.2, 0.5);
    soldierAsm:AddTransition("crouch_fire", "crouch_forward", 0.5, 0.5);
    soldierAsm:AddTransition("crouch_fire", "crouch_idle_aim", 0.1, 0.1);
    soldierAsm:AddTransition("crouch_fire", "fire", 0.3, 0.3);
    soldierAsm:AddTransition("crouch_fire", "idle_aim", 0.3, 0.3);
    soldierAsm:AddTransition("crouch_fire", "reload", 0.3, 0.3);
    soldierAsm:AddTransition("crouch_fire", "run_forward", 0.5, 0.5);
    
    soldierAsm:AddTransition("crouch_forward", "crouch_dead", crouchForwardLength, 0.2, 0.5);
    soldierAsm:AddTransition("crouch_forward", "crouch_fire", crouchForwardLength, 0.2);
    soldierAsm:AddTransition("crouch_forward", "crouch_idle_aim", crouchForwardLength, 0.2);
    soldierAsm:AddTransition("crouch_forward", "fall_idle", crouchForwardLength, 0.2);
    soldierAsm:AddTransition("crouch_forward", "fire", crouchForwardLength, 0.5);
    soldierAsm:AddTransition("crouch_forward", "idle_aim", crouchForwardLength, 0.5);
    soldierAsm:AddTransition("crouch_forward", "reload", crouchForwardLength, 0.5);
    soldierAsm:AddTransition("crouch_forward", "run_forward", crouchForwardLength, 0.4);
    
    soldierAsm:AddTransition("run_forward", "crouch_fire", runForwardLength, 0.5);
    soldierAsm:AddTransition("run_forward", "crouch_forward", runForwardLength, 0.2);
    soldierAsm:AddTransition("run_forward", "crouch_idle_aim", runForwardLength, 0.5);
    soldierAsm:AddTransition("run_forward", "dead", runForwardLength, 0.2);
    soldierAsm:AddTransition("run_forward", "fall_idle", runForwardLength, 0.1);
    soldierAsm:AddTransition("run_forward", "fire", runForwardLength, 0.5);
    soldierAsm:AddTransition("run_forward", "idle_aim", runForwardLength, 0.5);
    soldierAsm:AddTransition("run_forward", "reload", runForwardLength, 0.5);
    
    soldierAsm:AddTransition("fall_idle", "fall_dead", idleAnimLength, 0.15, 1.0);
    
    soldierAsm:AddTransition("fire", "idle_aim", 0.1, 0.1);
    soldierAsm:AddTransition("fire", "reload", 0.1, 0.1);
    soldierAsm:AddTransition("fire", "run_forward", 0.5, 0.5);
    soldierAsm:AddTransition("fire", "run_backward", 0.5, 0.5);
    soldierAsm:AddTransition("fire", "crouch_idle_aim", 0.5, 0.5);
    soldierAsm:AddTransition("fire", "crouch_fire", 0.3, 0.3);
    soldierAsm:AddTransition("fire", "crouch_forward", 0.5, 0.5);
    
    soldierAsm:AddTransition("melee", "idle_aim", 0.2, 0.2);
    soldierAsm:AddTransition("reload", "idle_aim", 0.2, 0.2);
    soldierAsm:AddTransition("run_backward", "idle_aim", 0.2, 0.2);
    soldierAsm:AddTransition("smg_transform", "idle_aim", 0.2, 0.2);
    soldierAsm:AddTransition("sniper_transform", "idle_aim", 0.2, 0.2);
    
    soldierAsm:RequestState("idle_aim");
end

function Soldier_InitWeaponAsm(agent)
    local weapon = agent:getAgentWeapon();

    -- Create an animation state machine to handle weapon animations.
    local weaponAsm = weapon:GetObjectASM();

    local sniperIdle = weapon:GetAnimation("sniper_idle")
    local smgIdle = weapon:GetAnimation("smg_idle")

    weaponAsm:AddState("smg_idle", smgIdle, true);
    weaponAsm:AddState("sniper_idle", sniperIdle, true);
    weaponAsm:AddState("smg_transform", weapon:GetAnimation("smg_transform"));
    weaponAsm:AddState("sniper_reload", weapon:GetAnimation("sniper_reload"));
    weaponAsm:AddState("sniper_transform", weapon:GetAnimation("sniper_transform"));

    weaponAsm:AddTransition("sniper_idle", "sniper_reload", sniperIdle:GetLength(), 0.2);
    weaponAsm:AddTransition("sniper_idle", "sniper_transform", sniperIdle:GetLength(), 0.2);
    weaponAsm:AddTransition("sniper_reload", "sniper_idle", 0.2, 0.2);
    weaponAsm:AddTransition("sniper_transform", "sniper_idle", 0.2, 0.2);
    weaponAsm:AddTransition("smg_idle", "smg_transform", smgIdle:GetLength(), 0.2);
    weaponAsm:AddTransition("smg_transform", "smg_idle", 0.2, 0.2);

    weaponAsm:RequestState("sniper_idle");
end