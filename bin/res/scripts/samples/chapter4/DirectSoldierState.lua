require("res.scripts.samples.chapter4.SoldierAgent.lua")

function Agent_DeathState(agent, deltaTime)
    agent:RequestState(Soldier.States.SSTATE_DEAD);
end

function Agent_FallingState(agent, deltaTime)
    agent:RequestState(Soldier.States.SSTATE_FALL_IDLE);
end

function Agent_IdleState(agent, deltaTime)
    agent:RequestState(Soldier.States.SSTATE_IDLE_AIM);
end

function Agent_MovingState(agent, deltaTime)
    agent:RequestState(Soldier.States.SSTATE_RUN_FORWARD);
end

function Agent_ShootState(agent, deltaTime)
    agent:RequestState(Soldier.States.SSTATE_FIRE);
end

