--PursuingAgent.lua--
require("res.scripts.samples.AgentUtils")

local enemy;
function Agent_Initialize(agent)
    agent:SetTargetRadius(1.0);

    local randPosx = math.random(-50, 50)
    local randPosz = math.random(-50, 50)
    agent:setPosition(Vector3(randPosx, 0, randPosz))

    seekings = ObjectManager:getSpecifyAgents(AGENT_OBJ_SEEKING)
    if seekings:size() > 0 then enemy = seekings[0] end
    agent:SetTarget(enemy:GetPosition());
    agent:SetMaxSpeed(enemy:GetMaxSpeed() * 0.8)
end

function Agent_Update(agent, deltaTimeInMillis)
    agent:SetTarget(enemy:PredictFuturePosition(1))

    local destination = agent:GetTarget()
    local deltaTimeInSeconds = deltaTimeInMillis / 1000;
    local seekForce = agent:ForceToPosition(destination);
    local targetRadius = agent:GetTargetRadius();
    local position = agent:GetPosition();

    AgentUtilities_ApplyPhysicsSteeringForce(
        agent, seekForce, deltaTimeInSeconds);
    AgentUtilities_ClampHorizontalSpeed(agent);

    DebugDrawer:drawCircle(destination, targetRadius, 10, UtilColors.Orange);
    DebugDrawer:drawLine(position, destination, UtilColors.Yellow);
end

function Agent_EventHandle(agent, keycode)
    
end