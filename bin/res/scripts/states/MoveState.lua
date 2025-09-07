-- MoveState --

local function CalculateSteering(agent, deltaTimeInSec)
    local stance = agent:getStanceType()
    if stance == SOLDIER_STAND then
        return Soldier_CalculateSteering(agent, deltaTimeInSec)
    else
        return Soldier_CalculateSlowSteering(agent, deltaTimeInSec)
    end
end

--local m_pAgent = owner;
AgentState_OnEnter = function(agent)
    --print("Info==========>>>OnEnter")
end

AgentState_OnLeave = function(agent)
    --print("Info==========>>>OnLeave")
end

AgentState_OnUpdate = function(agent, deltaTime)
    local deltaTimeInSec = deltaTime / 1000
    local steeringForces = CalculateSteering(agent, deltaTimeInSec)
    Soldier_ApplySteering(agent, steeringForces, deltaTimeInSec)

    agent:RequestState(SSTATE_RUN_FORWARD);
    
    if not agent:IsAnimReadyForMove() then
        return "";
    end
    
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

