--PursuingAgent.lua--
require("res.scripts..agent.AgentUtils")

local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

local enemy;
function Agent_Initialize(agent)
    AgentComponents.SetTargetRadius(agent, 1.0);

    local randPosx = math.random(-50, 50)
    local randPosz = math.random(-50, 50)
    agent:setPosition(Vector3(randPosx, 0, randPosz))

    seekings = ObjectManager:getSpecifyAgents(AGENT_OBJ_SEEKING)
    if seekings:size() > 0 then enemy = seekings[0] end
    AgentComponents.SetTarget(agent, enemy:GetPosition());
    AgentComponents.SetMaxSpeed(agent, AgentComponents.GetMaxSpeed(enemy) * 0.8)
end

function Agent_Update(agent, deltaTimeInMillis)
    AgentComponents.SetTarget(agent, enemy:PredictFuturePosition(1))

    local destination = AgentComponents.GetTarget(agent)
    local deltaTimeInSeconds = deltaTimeInMillis / 1000;
    local seekForce = AgentComponents.ForceToPosition(agent, destination);
    local targetRadius = AgentComponents.GetTargetRadius(agent);
    local position = agent:GetPosition();

    AgentUtilities_ApplyPhysicsSteeringForce(
        agent, seekForce, deltaTimeInSeconds);
    AgentUtilities_ClampHorizontalSpeed(agent);

    DebugDrawer:drawCircle(destination, targetRadius, 10, UtilColors.Orange);
    DebugDrawer:drawLine(position, destination, UtilColors.Yellow);
end

function Agent_EventHandle(agent, keycode)
    
end
