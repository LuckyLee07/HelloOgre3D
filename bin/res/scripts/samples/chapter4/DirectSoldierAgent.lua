require("res.scripts.samples.chapter4.SoldierAgent.lua")

function Agent_DeathState(agent)
end


function Agent_Initialize(agent)
    local height = agent:GetHeight()
    local posoffset = Vector3(0, height/2, 10)
    local position = agent:GetPosition()
    local randomVec = Vector3(math.random(-5, 5), 0, math.random(-5, 5))
    agent:setPosition(position + posoffset + randomVec)
end

function Agent_EventHandle(agent)
    local position = agent:GetPosition()
end

function Agent_Update(agent, deltaTime)
    
end