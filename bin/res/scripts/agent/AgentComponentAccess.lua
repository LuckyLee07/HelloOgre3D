local AgentComponentAccess = {}

function AgentComponentAccess.GetAI(agent)
	if agent ~= nil and agent.GetAIComponent ~= nil then
		return agent:GetAIComponent()
	end
	return nil
end

function AgentComponentAccess.GetAttrib(agent)
	if agent ~= nil and agent.GetAttribComponent ~= nil then
		return agent:GetAttribComponent()
	end
	return nil
end

function AgentComponentAccess.GetWeapon(agent)
	if agent ~= nil and agent.GetWeaponComponent ~= nil then
		return agent:GetWeaponComponent()
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
	return defaultValue or 0
end

function AgentComponentAccess.HasAmmo(agent)
	local weapon = AgentComponentAccess.GetWeapon(agent)
	if weapon ~= nil then
		return weapon:HasAmmo()
	end
	return false
end

function AgentComponentAccess.GetAmmo(agent, defaultValue)
	local weapon = AgentComponentAccess.GetWeapon(agent)
	if weapon ~= nil then
		return weapon:GetAmmo()
	end
	return defaultValue or 0
end

function AgentComponentAccess.SetAmmo(agent, ammo)
	local weapon = AgentComponentAccess.GetWeapon(agent)
	if weapon ~= nil then
		weapon:SetAmmo(ammo)
		return true
	end
	return false
end

function AgentComponentAccess.SetMaxAmmo(agent, maxAmmo)
	local weapon = AgentComponentAccess.GetWeapon(agent)
	if weapon ~= nil then
		weapon:SetMaxAmmo(maxAmmo)
		return true
	end
	return false
end

function AgentComponentAccess.ConsumeAmmo(agent, amount)
	local weapon = AgentComponentAccess.GetWeapon(agent)
	if weapon ~= nil then
		weapon:ConsumeAmmo(amount or 1)
		return true
	end
	return false
end

function AgentComponentAccess.RestoreAmmo(agent)
	local weapon = AgentComponentAccess.GetWeapon(agent)
	if weapon ~= nil then
		weapon:RestoreAmmo()
		return true
	end
	return false
end

return AgentComponentAccess
