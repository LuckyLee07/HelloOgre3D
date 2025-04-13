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
        --agent:DeleteRighdBody();
        agent:SetHealth(0);
    end
end

function Agent_FallingState(agent, deltaTime)
    local currStateName = agent:GetCurStateName()

    if currStateName ~= Soldier.SoldierStates.STAND_IDLE_AIM and
        currStateName ~= Soldier.SoldierStates.STAND_FALL_DEAD then
        agent:RequestState(Soldier.States.SSTATE_IDLE_AIM);
    end

    if not agent:IsFalling() then
        if currStateName ~= Soldier.SoldierStates.STAND_FALL_DEAD then
            agent:RequestState(Soldier.States.SSTATE_FALL_DEAD);
        elseif currStateName == Soldier.SoldierStates.STAND_FALL_DEAD then
            --agent:DeleteRighdBody();
            agent:SetHealth(0);
        end
    end
end

function Agent_IdleState(agent, deltaTime)
    if agent:IsMoving() then
        Soldier_SlowMovement(agent, deltaTime, 2)
    end
    agent:RequestState(Soldier.States.SSTATE_IDLE_AIM);
end

function Agent_MovingState(agent, deltaTime)
    local steeringForces = 0
    local deltaTimeInSec = deltaTime / 1000
    local stance = agent:getStanceType()
    if stance == SOLDIER_STAND then
        steeringForces = Soldier_CalculateSteering(agent, deltaTimeInSec)
    else
        steeringForces = Soldier_CalculateSlowSteering(agent, deltaTimeInSec)
    end
    agent:RequestState(Soldier.States.SSTATE_RUN_FORWARD);
    Soldier_ApplySteering(agent, steeringForces, deltaTimeInSec)
end

function Agent_ShootState(agent, deltaTime)
    if agent:IsMoving() then
        Soldier_SlowMovement(agent, deltaTime)
    end
    agent:RequestState(Soldier.States.SSTATE_FIRE);
end

