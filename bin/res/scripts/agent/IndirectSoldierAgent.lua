require("res.scripts.agent.AgentUtils.lua")
require("res.scripts.agent.SoldierAgent.lua")

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
    agent:SetMaxSpeed(SOLDIER_STAND_SPEED);
end


function Agent_EventHandle(agent, keycode)
    if keycode == OIS.KC_5 then
        local stanceType = agent:getStanceType()
        if stanceType == SOLDIER_STAND then
            agent:changeStanceType(SOLDIER_CROUCH)
        else
            agent:changeStanceType(SOLDIER_STAND)
        end
    end
end


function Agent_Update(agent, deltaTimeInMillis)
    
end