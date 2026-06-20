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

function AgentComponentAccess.GetAnim(agent)
	if agent ~= nil and agent.GetAnimComponent ~= nil then
		return agent:GetAnimComponent()
	end
	return nil
end

function AgentComponentAccess.GetBodyAsm(agent)
	local anim = AgentComponentAccess.GetAnim(agent)
	if anim ~= nil then
		return anim:GetBodyAsm()
	end
	if agent ~= nil and agent.GetObjectASM ~= nil then
		return agent:GetObjectASM()
	end
	return nil
end

function AgentComponentAccess.GetEnemy(agent)
	local ai = AgentComponentAccess.GetAI(agent)
	if ai ~= nil then
		return ai:GetEnemy()
	end
	return nil
end

function AgentComponentAccess.HasEnemy(agent, navMeshName)
	local ai = AgentComponentAccess.GetAI(agent)
	if ai ~= nil then
		return ai:HasEnemy(navMeshName or "default")
	end
	return false
end

function AgentComponentAccess.CanShootEnemy(agent, navMeshName, shootDistance)
	local ai = AgentComponentAccess.GetAI(agent)
	if ai ~= nil then
		return ai:CanShootEnemy(navMeshName or "default", shootDistance or 3.0)
	end
	return false
end

function AgentComponentAccess.HasMovePosition(agent, reachDistance)
	local ai = AgentComponentAccess.GetAI(agent)
	if ai ~= nil then
		return ai:HasMovePosition(reachDistance or 1.5)
	end
	if agent ~= nil and agent.HasMovePosition ~= nil then
		return agent:HasMovePosition(reachDistance or 1.5)
	end
	return false
end

function AgentComponentAccess.SetMovePosition(agent, movePos)
	local ai = AgentComponentAccess.GetAI(agent)
	if ai ~= nil then
		ai:SetMovePosition(movePos)
		return true
	end
	if agent ~= nil and agent.SetMovePosition ~= nil then
		agent:SetMovePosition(movePos)
		return true
	end
	return false
end

function AgentComponentAccess.ClearMovePosition(agent)
	local ai = AgentComponentAccess.GetAI(agent)
	if ai ~= nil then
		ai:ClearMovePosition()
		return true
	end
	if agent ~= nil and agent.ClearMovePosition ~= nil then
		agent:ClearMovePosition()
		return true
	end
	return false
end

function AgentComponentAccess.IsTargetReached(agent, threshold)
	local ai = AgentComponentAccess.GetAI(agent)
	if ai ~= nil then
		return ai:IsTargetReached(threshold or 1.5)
	end
	if agent ~= nil and agent.IsTargetReached ~= nil then
		return agent:IsTargetReached(threshold or 1.5)
	end
	return false
end

function AgentComponentAccess.EnterIdleAnim(agent)
	local anim = AgentComponentAccess.GetAnim(agent)
	if anim ~= nil and anim.EnterIdleIntent ~= nil then
		return anim:EnterIdleIntent()
	end
	if agent ~= nil and agent.EnterIdleAnim ~= nil then
		agent:EnterIdleAnim()
		return true
	end
	return false
end

function AgentComponentAccess.EnterMoveAnim(agent)
	local anim = AgentComponentAccess.GetAnim(agent)
	if anim ~= nil and anim.EnterMoveIntent ~= nil then
		return anim:EnterMoveIntent()
	end
	if agent ~= nil and agent.EnterMoveAnim ~= nil then
		agent:EnterMoveAnim()
		return true
	end
	return false
end

function AgentComponentAccess.EnterShootAnim(agent)
	local anim = AgentComponentAccess.GetAnim(agent)
	if anim ~= nil and anim.EnterShootIntent ~= nil then
		return anim:EnterShootIntent()
	end
	if agent ~= nil and agent.EnterShootAnim ~= nil then
		agent:EnterShootAnim()
		return true
	end
	return false
end

function AgentComponentAccess.EnterReloadAnim(agent)
	local anim = AgentComponentAccess.GetAnim(agent)
	if anim ~= nil and anim.EnterReloadIntent ~= nil then
		return anim:EnterReloadIntent()
	end
	if agent ~= nil and agent.EnterReloadAnim ~= nil then
		agent:EnterReloadAnim()
		return true
	end
	return false
end

function AgentComponentAccess.EnterDeathAnim(agent)
	local anim = AgentComponentAccess.GetAnim(agent)
	if anim ~= nil and anim.EnterDeathIntent ~= nil then
		return anim:EnterDeathIntent()
	end
	if agent ~= nil and agent.EnterDeathAnim ~= nil then
		agent:EnterDeathAnim()
		return true
	end
	return false
end

function AgentComponentAccess.ShootBullet(agent)
	local weapon = AgentComponentAccess.GetWeapon(agent)
	if weapon ~= nil then
		weapon:ShootBullet()
		return true
	end
	if agent ~= nil and agent.ShootBullet ~= nil then
		agent:ShootBullet()
		return true
	end
	return false
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
