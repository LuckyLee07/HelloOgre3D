require("res.scripts.samples.chapter4.SoldierAgent.lua")
require("res.scripts.samples.chapter4.DirectSoldierState.lua")
require("res.scripts.samples.AgentUtils.lua")

local soldierState = nil

function Soldier_CalculateSteering(agent, deltaTimeInSeconds)
    local avoidForce = agent:ForceToAvoidAgents(0.5);
    local avoidObjectForce = agent:ForceToAvoidObjects(0.5);
    local followForce = agent:ForceToFollowPath(0.5);
    local stayForce = agent:ForceToStayOnPath(0.5);
    
    local totalForces = followForce * 1.5 + stayForce * 0.4 + avoidForce * 1 + avoidObjectForce * 2;
    totalForces.y = 0;

    local targetSpeed = agent:GetMaxSpeed();

    if (agent:GetSpeed() < targetSpeed) then
        local speedForce = agent:ForceToTargetSpeed(targetSpeed);
        totalForces = totalForces + speedForce * 7;
    end
    
    return totalForces;
end

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
    local soldierASM = agent:getBody():GetObjectASM()
    soldierASM:RequestState(Soldier.SoldierStates.STAND_RUN_FORWARD)

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
    end
end

function Agent_Update(agent, deltaTime)
    
    if soldierState == "Fire" then
        Agent_ShootState(agent)
    elseif soldierState == "Idle" then
        Agent_IdleState(agent)
    elseif soldierState == "Move" then
        Agent_MovingState(agent)
    end
    
    -- Draw the agent's cyclic path, offset slightly above the level geometry.
    DebugDrawer:drawPath(agent:GetPath(), UtilColors.Blue, true, Vector3(0.0, 0.02, 0.0))

    -- Apply a steering force to move the agent along the path.
    if (agent:HasPath()) then
        local steeringForces = Soldier_CalculateSteering(agent, deltaTime);
        Soldier_ApplySteering(agent, steeringForces, deltaTime);
    end
end
