local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

function Agent_Initialize(agent)
    if agent == nil then return end
    AgentComponents.SetMaxSpeed(agent, SOLDIER_STAND_SPEED)
end

function Agent_Update(agent, deltaTimeInMillis)
end

function Agent_EventHandle(agent, keycode, key)
end
