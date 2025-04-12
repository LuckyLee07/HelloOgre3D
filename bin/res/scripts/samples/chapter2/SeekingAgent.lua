--SeekingAgent.lua--
require("res.scripts.samples.AgentUtils")

function Agent_Initialize(agent)
    agent:SetTarget(Vector3(50.0, 0.0, 0.0))
    agent:SetTargetRadius(1.5)
end

function Agent_Update(agent, deltaTimeInMillis)
    local destination = agent:GetTarget();
    local deltaTimeInSeconds = deltaTimeInMillis / 1000;
    local avoidAgentForce = agent:ForceToAvoidAgents(1.5);
    local avoidObjectForce = agent:ForceToAvoidObjects(1.5);
    local seekForce = agent:ForceToPosition(destination);
    local targetRadius = agent:GetTargetRadius();
    local radius = agent:GetRadius();
    local position = agent:GetPosition();
    local avoidanceMultiplier = 3;
    
    -- Sum all forces and apply higher priorty to avoidance forces.
    local steeringForces = seekForce +
        avoidAgentForce * avoidanceMultiplier +
        avoidObjectForce * avoidanceMultiplier;

    -- Apply all steering forces.
    AgentUtilities_ApplyPhysicsSteeringForce(
        agent, steeringForces, deltaTimeInSeconds);
    AgentUtilities_ClampHorizontalSpeed(agent);

    local targetRadiusSquared = (targetRadius + radius) * (targetRadius + radius);
    
    -- Calculate the position where the Agent touches the ground.
    local adjustedPosition = agent:GetPosition() - Vector3(0, agent:GetHeight()/2, 0);

    -- If the agent is within the target radius pick a new
    -- random position to move to.
    if (DistanceSquared(adjustedPosition, destination) < targetRadiusSquared) then

        -- New target is within the 100 meter squared movement space.
        local targetPos = agent:GetTarget();
        targetPos.x = math.random(-50, 50);
        targetPos.z = math.random(-50, 50);
        
        agent:SetTarget(targetPos);
    end

    -- Draw debug information for target and target radius.
    DebugDrawer:drawCircle(destination, targetRadius, 10, UtilColors.Red);
    DebugDrawer:drawLine(position, destination, UtilColors.Green);

    -- Debug outline representing the space the Agent can move within.
    DebugDrawer:drawSquare(Vector3(0, 0, 0), 100, UtilColors.Red);
end

function Agent_EventHandle(agent, keycode)
    
end