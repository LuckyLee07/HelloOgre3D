require("res.scripts.samples.chapter4.SoldierAgent.lua")
require("res.scripts.samples.chapter4.DirectSoldierState.lua")
require("res.scripts.samples.AgentUtils.lua")

local _soldierStates = {
    DEATH = "DEATH",
    FALLING = "FALLING",
    IDLE = "IDLE",
    MOVING = "MOVING",
    SHOOTING = "SHOOTING",
}

local _soldierState = nil

-- Accumulates acceleration to smooth out jerks in movement.
local _agentAccumulators = {};
function Soldier_ApplySteering(agent, steeringForces, deltaTimeInSeconds)
    local agentId = agent:getObjId();

    if (_agentAccumulators[agentId] == nil) then
        _agentAccumulators[agentId] = Vector3(0.0);
    end
    
    AgentUtilities_ApplySteeringForce2(agent, steeringForces, _agentAccumulators[agentId], deltaTimeInSeconds);
    AgentUtilities_ClampHorizontalSpeed(agent);
end


function Agent_Initialize(agent)
    local height = agent:GetHeight()
    local posoffset = Vector3(0, height/2, 10)
    local position = agent:GetPosition()
    local randomVec = Vector3(math.random(-5, 5), 0, math.random(-5, 5))
    agent:setPosition(position + posoffset + randomVec)

    agent:SetPath(SandboxUtilities_GetLevelPath(), true)
    agent:SetMaxSpeed(agent:GetMaxSpeed() * 0.5);

    _soldierState = _soldierStates.IDLE
end


function Agent_EventHandle(agent, keycode)
    if keycode == OIS.KC_1 then
        _soldierState = _soldierStates.IDLE
    elseif keycode == OIS.KC_2 then
        _soldierState = _soldierStates.SHOOTING
    elseif keycode == OIS.KC_3 then
        _soldierState = _soldierStates.MOVING
    elseif keycode == OIS.KC_4 then
        _soldierState = _soldierStates.DEATH
    elseif keycode == OIS.KC_6 then
        _soldierState = _soldierStates.FALLING
    elseif keycode == OIS.KC_5 then
        local stanceType = agent:getStanceType()
        if stanceType == SOLDIER_STAND then
            agent:changeStanceType(SOLDIER_CROUCH)
        else
            agent:changeStanceType(SOLDIER_STAND)
        end
    end
end


function Agent_Update(agent, deltaTimeInMillis)
    if agent:GetHealth() <= 0 then
        return --已经处于DEAD状态
    end

    if agent:IsFalling() then
        _soldierState = _soldierStates.FALLING
    end

    if _soldierState == _soldierStates.DEATH then
        Agent_DeathState(agent, deltaTimeInMillis)
    elseif _soldierState == _soldierStates.FALLING then
        Agent_FallingState(agent, deltaTimeInMillis)
    elseif _soldierState == _soldierStates.IDLE then
        Agent_IdleState(agent, deltaTimeInMillis)
    elseif _soldierState == _soldierStates.MOVING then
        Agent_MovingState(agent, deltaTimeInMillis)
    elseif _soldierState == _soldierStates.SHOOTING then
        Agent_ShootState(agent, deltaTimeInMillis)
    end
end
