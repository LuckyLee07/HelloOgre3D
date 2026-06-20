--PathingAgent.lua--
require("res.scripts..agent.AgentUtils.lua")

local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

function Agent_Initialize(agent)
    local randPosx = math.random(-50, 50)
    local randPosz = math.random(-50, 50)
    agent:setPosition(Vector3(randPosx, 0, randPosz))
end

function Agent_Update(agent, deltaTimeInMillis)
    local deltaTimeInSeconds = deltaTimeInMillis / 1000;

    local followForce = AgentComponents.ForceToFollowPath(agent, 1.25);
    local stayForce = AgentComponents.ForceToStayOnPath(agent, 1);
    local wanderForce = AgentComponents.ForceToWander(agent, deltaTimeInMillis);

    local totalForces = Vector.Normalize(followForce) + Vector.Normalize(stayForce) * 0.25
                         + Vector.Normalize(wanderForce) * 0.25;

    local targetSpeed = 3
    -- Accelerate pathing agents to a minimun speed
    if (agent:GetSpeed() < targetSpeed) then
        local speedForce = AgentComponents.ForceToTargetSpeed(agent, targetSpeed)
        totalForces = totalForces + Vector.Normalize(speedForce);
    end

    AgentUtilities_ApplyPhysicsSteeringForce(
        agent, totalForces, deltaTimeInSeconds);
    AgentUtilities_ClampHorizontalSpeed(agent);

    DebugDrawer:drawPath(AgentComponents.GetPath(agent), UtilColors.Blue, true, Vector3(0.0))
end

function Agent_EventHandle(agent, keycode)
    
end
