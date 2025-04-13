require("res.scripts.samples.chapter4.SoldierAgent.lua")

function Soldier_SlowMovement(agent, deltaTime, rate)
    rate = rate or 1;

    local horizontalVelocity = agent:GetVelocity();
    local yMovement = horizontalVelocity.y;
    
    horizontalVelocity.y = 0;
    horizontalVelocity = horizontalVelocity * 0.91 * (1 / rate);
    horizontalVelocity.y = yMovement;
    
    agent:SetVelocity(horizontalVelocity);
end


function Agent_DeathState(agent, deltaTime)
    local currStateName = agent:GetCurStateName()

    if agent:IsMoving() then
        Soldier_SlowMovement(agent, deltaTime, 2)
    end
    agent:RequestState(Soldier.States.SSTATE_DEAD);
    
    if currStateName == Soldier.SoldierStates.CROUCH_DEAD or
        currStateName == Soldier.SoldierStates.STAND_DEAD then
        agent:DeleteRighdBody();
        if agent:GetHealth() > 0 then
            agent:SetHealth(0);
        end
    end
end

function Agent_FallingState(agent, deltaTime)
    agent:RequestState(Soldier.States.SSTATE_FALL_IDLE);
end

function Agent_IdleState(agent, deltaTime)
    if agent:IsMoving() then
        Soldier_SlowMovement(agent, deltaTime, 2)
    end
    agent:RequestState(Soldier.States.SSTATE_IDLE_AIM);
end

function Agent_MovingState(agent, deltaTime)
    agent:RequestState(Soldier.States.SSTATE_RUN_FORWARD);
end

function Agent_ShootState(agent, deltaTime)
    if agent:IsMoving() then
        Soldier_SlowMovement(agent, deltaTime)
    end
    agent:RequestState(Soldier.States.SSTATE_FIRE);
end

