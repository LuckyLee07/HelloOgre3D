-- MoveState --

--local m_pAgent = owner;
AgentState_OnEnter = function(agent)
    --print("Info==========>>>OnEnter")
end

AgentState_OnLeave = function(agent)
    --print("Info==========>>>OnLeave")
end

AgentState_OnUpdate = function(agent, deltaTime)
    local steeringForces = 0
    local deltaTimeInSec = deltaTime / 1000
    local stance = agent:getStanceType()
    if stance == SOLDIER_STAND then
        steeringForces = Soldier_CalculateSteering(agent, deltaTimeInSec)
    else
        steeringForces = Soldier_CalculateSlowSteering(agent, deltaTimeInSec)
    end
    agent:RequestState(SSTATE_RUN_FORWARD);
    Soldier_ApplySteering(agent, steeringForces, deltaTimeInSec)
    
    local pInput = agent:GetInput();
    if (pInput:isKeyDown(OIS.KC_2)) then
        return "toShoot";
    elseif pInput:isKeyDown(OIS.KC_3) then
        return ""; -- 继续Walk
    elseif (pInput:isKeyDown(OIS.KC_4)) then
        return "toDeath";
    end

    return "toIdle";
end

