local AgentComponentAccess = {}

function AgentComponentAccess.GetAI(agent)
	if agent ~= nil and agent.GetAIComponent ~= nil then
		return agent:GetAIComponent()
	end
	if agent ~= nil and agent.GetAI ~= nil then
		return agent:GetAI()
	end
	return nil
end

function AgentComponentAccess.GetAttrib(agent)
	if agent ~= nil and agent.GetAttribComponent ~= nil then
		return agent:GetAttribComponent()
	end
	return nil
end

function AgentComponentAccess.GetHealth(agent, defaultValue)
	local attrib = AgentComponentAccess.GetAttrib(agent)
	if attrib ~= nil then
		return attrib:GetHealth()
	end
	if agent ~= nil and agent.GetHealth ~= nil then
		return agent:GetHealth()
	end
	return defaultValue or 0
end

function AgentComponentAccess.GetMaxHealth(agent, defaultValue)
	local attrib = AgentComponentAccess.GetAttrib(agent)
	if attrib ~= nil then
		return attrib:GetMaxHealth()
	end
	if agent ~= nil and agent.GetMaxHealth ~= nil then
		return agent:GetMaxHealth()
	end
	return defaultValue or 0
end

return AgentComponentAccess
