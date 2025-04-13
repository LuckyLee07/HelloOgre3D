require("res.scripts.samples.chapter4.SoldierAgent.lua")
require("res.scripts.samples.chapter4.DirectSoldierState.lua")
require("res.scripts.samples.AgentUtils.lua")

local soldierState = nil

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

    soldierState = "Move"
    agent:RequestState(Soldier.States.SSTATE_RUN_FORWARD);

    agent:SetPath(SandboxUtilities_GetLevelPath(), true)
    agent:SetMaxSpeed(agent:GetMaxSpeed() * 0.5);
end


function Agent_EventHandle(agent, keycode)
    if keycode == OIS.KC_2 then
        soldierState = "Fire"
    elseif keycode == OIS.KC_3 then
        soldierState = "Idle"
    elseif keycode == OIS.KC_4 then
        soldierState = "Move"
    elseif keycode == OIS.KC_5 then
        soldierState = "Dead"
    end
end


function Agent_Update(agent, deltaTimeInMillis)
    
    if soldierState == "Fire" then
        Agent_ShootState(agent)
    elseif soldierState == "Idle" then
        Agent_IdleState(agent)
    elseif soldierState == "Move" then
        Agent_MovingState(agent)
    elseif soldierState == "Dead" then
        Agent_DeathState(agent)
    end
    
    -- Draw the agent's cyclic path, offset slightly above the level geometry.
    --DebugDrawer:drawPath(agent:GetPath(), UtilColors.Blue, true, Vector3(0.0, 0.02, 0.0))

    -- Apply a steering force to move the agent along the path.
    if (agent:HasPath() and soldierState == "Move") then
        local deltaTimeInSeconds = deltaTimeInMillis / 1000;
        local steeringForces = Soldier_CalculateSteering(agent, deltaTimeInSeconds);
        Soldier_ApplySteering(agent, steeringForces, deltaTimeInSeconds);
    end
end
