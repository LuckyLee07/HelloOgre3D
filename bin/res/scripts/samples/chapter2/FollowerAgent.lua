--FollowerAgent.lua--
require("res.scripts.samples.AgentUtils.lua")

local leaders;
function Agent_Initialize(agent)
    local randPosx = math.random(-50, 50)
    local randPosz = math.random(-50, 50)
    agent:setPosition(Vector3(randPosx, 0, randPosz))

    leaders = ObjectManager:getSpecifyAgents(AGENT_OBJ_SEEKING)
end

function Agent_Update(agent, deltaTimeInMillis)
    local deltaTimeInSeconds = deltaTimeInMillis / 1000;

    local forceToCombine = agent:ForceToCombine(leaders, 100, 180);

    local agents = ObjectManager:getAllAgents()
    local forceToSeparate = agent:ForceToSeparate(agents, 2, 180);
    
    local forceToAlign = agent:ForceToSeparate(leaders, 5, 45);

    local totalForces = forceToCombine + forceToSeparate * 1.15 + forceToAlign;

    AgentUtilities_ApplyPhysicsSteeringForce(
        agent, totalForces, deltaTimeInSeconds);
    AgentUtilities_ClampHorizontalSpeed(agent);


    local position = agent:GetPosition();
    local destination = leaders[0]:GetPosition();
    local targetRadius = agent:GetTargetRadius();

    DebugDrawer:drawCircle(position, 1, 10, UtilColors.Yellow);
    DebugDrawer:drawCircle(destination, targetRadius, 10, UtilColors.White);
    DebugDrawer:drawLine(position, destination, UtilColors.Green);
end

function Agent_EventHandle(agent, keycode)
    
end