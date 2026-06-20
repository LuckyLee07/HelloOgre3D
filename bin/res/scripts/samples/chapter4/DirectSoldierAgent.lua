require("res.scripts.agent.AgentUtils.lua")
require("res.scripts.agent.SoldierAgent.lua")
require("res.scripts.samples.chapter4.DirectSoldierState.lua")

local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

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
    local agentId = agent:GetObjId();

    if (_agentAccumulators[agentId] == nil) then
        _agentAccumulators[agentId] = Vector3(0.0);
    end
    
    AgentUtilities_ApplySteeringForce2(agent, steeringForces, _agentAccumulators[agentId], deltaTimeInSeconds);
    AgentUtilities_ClampHorizontalSpeed(agent);
end

function Agent_Initialize(agent)
    local height = AgentComponents.GetHeight(agent)
    local posoffset = Vector3(0, height/2, 10)
    local position = agent:GetPosition()
    local randomVec = Vector3(math.random(-5, 5), 0, math.random(-5, 5))
    agent:setPosition(position + posoffset + randomVec)
    AgentComponents.SetMaxSpeed(agent, AgentComponents.GetMaxSpeed(agent) * 0.5);

    Agent_SetPath(agent, SandboxUtilities_GetLevelPath(), true)

    _soldierState = _soldierStates.IDLE
end


function Agent_EventHandle(agent, keycode)
    if IsNumKey(keycode, 1) then
        _soldierState = _soldierStates.IDLE
    elseif IsNumKey(keycode, 2) then
        _soldierState = _soldierStates.SHOOTING
    elseif IsNumKey(keycode, 3) then
        _soldierState = _soldierStates.MOVING
    elseif IsNumKey(keycode, 4) then
        _soldierState = _soldierStates.DEATH
    elseif IsNumKey(keycode, 6) then
        _soldierState = _soldierStates.FALLING
    elseif IsNumKey(keycode, 5) then
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
    if agent:GetUseCppFSM() then 
        return --使用C++端的FSM
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
    --]]
end
