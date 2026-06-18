local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

function Agent_Initialize(agent)
	if agent == nil then return end

	if agent.SetMaxSpeed ~= nil then
		agent:SetMaxSpeed(0)
	end

	local attrib = AgentComponents.GetAttrib(agent)
	if attrib ~= nil then
		attrib:SetMaxHealth(123)
		attrib:SetHealth(77)
	end
end

function Agent_Update(agent, deltaTimeInMillis)
end

function Agent_EventHandle(agent, keycode)
end
