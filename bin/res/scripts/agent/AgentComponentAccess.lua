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

function AgentComponentAccess.GetLocomotion(agent)
	if agent ~= nil and agent.GetLocomotionComponent ~= nil then
		return agent:GetLocomotionComponent()
	end
	return nil
end

local function _GetLocomotionValue(agent, methodName, defaultValue, ...)
	local locomotion = AgentComponentAccess.GetLocomotion(agent)
	if locomotion ~= nil and locomotion[methodName] ~= nil then
		return locomotion[methodName](locomotion, ...)
	end
	if agent ~= nil and agent[methodName] ~= nil then
		return agent[methodName](agent, ...)
	end
	return defaultValue
end

local function _CallLocomotion(agent, methodName, ...)
	local locomotion = AgentComponentAccess.GetLocomotion(agent)
	if locomotion ~= nil and locomotion[methodName] ~= nil then
		return locomotion[methodName](locomotion, ...)
	end
	if agent ~= nil and agent[methodName] ~= nil then
		return agent[methodName](agent, ...)
	end
	return nil
end

local function _SetLocomotion(agent, methodName, ...)
	if _CallLocomotion(agent, methodName, ...) ~= nil then
		return true
	end
	local locomotion = AgentComponentAccess.GetLocomotion(agent)
	if locomotion ~= nil and locomotion[methodName] ~= nil then
		return true
	end
	if agent ~= nil and agent[methodName] ~= nil then
		return true
	end
	return false
end

function AgentComponentAccess.GetBodyAsm(agent)
	local anim = AgentComponentAccess.GetAnim(agent)
	if anim ~= nil then
		return anim:GetBodyAsm()
	end
	return nil
end

function AgentComponentAccess.GetBodyAnimation(agent, animationName)
	local anim = AgentComponentAccess.GetAnim(agent)
	if anim ~= nil then
		return anim:GetBodyAnimation(animationName)
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
	return false
end

function AgentComponentAccess.SetMovePosition(agent, movePos)
	local ai = AgentComponentAccess.GetAI(agent)
	if ai ~= nil then
		ai:SetMovePosition(movePos)
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
	return false
end

function AgentComponentAccess.IsTargetReached(agent, threshold)
	local ai = AgentComponentAccess.GetAI(agent)
	if ai ~= nil then
		return ai:IsTargetReached(threshold or 1.5)
	end
	return false
end

function AgentComponentAccess.EnterIdleAnim(agent)
	local anim = AgentComponentAccess.GetAnim(agent)
	if anim ~= nil and anim.EnterIdleIntent ~= nil then
		return anim:EnterIdleIntent()
	end
	return false
end

function AgentComponentAccess.EnterMoveAnim(agent)
	local anim = AgentComponentAccess.GetAnim(agent)
	if anim ~= nil and anim.EnterMoveIntent ~= nil then
		return anim:EnterMoveIntent()
	end
	return false
end

function AgentComponentAccess.EnterShootAnim(agent)
	local anim = AgentComponentAccess.GetAnim(agent)
	if anim ~= nil and anim.EnterShootIntent ~= nil then
		return anim:EnterShootIntent()
	end
	return false
end

function AgentComponentAccess.EnterReloadAnim(agent)
	local anim = AgentComponentAccess.GetAnim(agent)
	if anim ~= nil and anim.EnterReloadIntent ~= nil then
		return anim:EnterReloadIntent()
	end
	return false
end

function AgentComponentAccess.EnterDeathAnim(agent)
	local anim = AgentComponentAccess.GetAnim(agent)
	if anim ~= nil and anim.EnterDeathIntent ~= nil then
		return anim:EnterDeathIntent()
	end
	return false
end

function AgentComponentAccess.ShootBullet(agent)
	local weapon = AgentComponentAccess.GetWeapon(agent)
	if weapon ~= nil then
		weapon:ShootBullet()
		return true
	end
	return false
end

function AgentComponentAccess.SetPath(agent, points, cyclic)
	return _SetLocomotion(agent, "SetPath", points, cyclic or false)
end

function AgentComponentAccess.GetPath(agent)
	return _CallLocomotion(agent, "GetPath")
end

function AgentComponentAccess.HasPath(agent)
	local result = _CallLocomotion(agent, "HasPath")
	return result == true
end

function AgentComponentAccess.GetDistanceAlongPath(agent, position)
	return _GetLocomotionValue(agent, "GetDistanceAlongPath", 0, position)
end

function AgentComponentAccess.GetNearestPointOnPath(agent, position)
	return _CallLocomotion(agent, "GetNearestPointOnPath", position)
end

function AgentComponentAccess.GetPointOnPath(agent, distance)
	return _CallLocomotion(agent, "GetPointOnPath", distance)
end

function AgentComponentAccess.SetTarget(agent, target)
	return _SetLocomotion(agent, "SetTarget", target)
end

function AgentComponentAccess.GetTarget(agent)
	return _CallLocomotion(agent, "GetTarget")
end

function AgentComponentAccess.SetTargetRadius(agent, radius)
	return _SetLocomotion(agent, "SetTargetRadius", radius)
end

function AgentComponentAccess.GetTargetRadius(agent, defaultValue)
	return _GetLocomotionValue(agent, "GetTargetRadius", defaultValue or 0)
end

function AgentComponentAccess.SetMaxForce(agent, maxForce)
	return _SetLocomotion(agent, "SetMaxForce", maxForce)
end

function AgentComponentAccess.GetMaxForce(agent, defaultValue)
	return _GetLocomotionValue(agent, "GetMaxForce", defaultValue or 0)
end

function AgentComponentAccess.SetMaxSpeed(agent, maxSpeed)
	return _SetLocomotion(agent, "SetMaxSpeed", maxSpeed)
end

function AgentComponentAccess.GetMaxSpeed(agent, defaultValue)
	return _GetLocomotionValue(agent, "GetMaxSpeed", defaultValue or 0)
end

function AgentComponentAccess.GetMass(agent, defaultValue)
	return _GetLocomotionValue(agent, "GetMass", defaultValue or 0)
end

function AgentComponentAccess.SetMass(agent, mass)
	if agent ~= nil and agent.SetMass ~= nil then
		agent:SetMass(mass)
		return true
	end
	return _SetLocomotion(agent, "SetMass", mass)
end

function AgentComponentAccess.GetHeight(agent, defaultValue)
	return _GetLocomotionValue(agent, "GetHeight", defaultValue or 0)
end

function AgentComponentAccess.GetRadius(agent, defaultValue)
	return _GetLocomotionValue(agent, "GetRadius", defaultValue or 0)
end

function AgentComponentAccess.GetSpeed(agent, defaultValue)
	if agent ~= nil and agent.GetSpeed ~= nil then
		return agent:GetSpeed()
	end
	return _GetLocomotionValue(agent, "GetSpeed", defaultValue or 0)
end

function AgentComponentAccess.ForceToPosition(agent, position)
	return _CallLocomotion(agent, "ForceToPosition", position) or Vector3(0, 0, 0)
end

function AgentComponentAccess.ForceToFollowPath(agent, predictionTime)
	return _CallLocomotion(agent, "ForceToFollowPath", predictionTime) or Vector3(0, 0, 0)
end

function AgentComponentAccess.ForceToStayOnPath(agent, predictionTime)
	return _CallLocomotion(agent, "ForceToStayOnPath", predictionTime) or Vector3(0, 0, 0)
end

function AgentComponentAccess.ForceToWander(agent, deltaMilliSeconds)
	return _CallLocomotion(agent, "ForceToWander", deltaMilliSeconds) or Vector3(0, 0, 0)
end

function AgentComponentAccess.ForceToTargetSpeed(agent, targetSpeed)
	return _CallLocomotion(agent, "ForceToTargetSpeed", targetSpeed) or Vector3(0, 0, 0)
end

function AgentComponentAccess.ForceToAvoidAgents(agent, predictionTime)
	return _CallLocomotion(agent, "ForceToAvoidAgents", predictionTime) or Vector3(0, 0, 0)
end

function AgentComponentAccess.ForceToAvoidObjects(agent, predictionTime)
	return _CallLocomotion(agent, "ForceToAvoidObjects", predictionTime) or Vector3(0, 0, 0)
end

function AgentComponentAccess.ForceToCombine(agent, agents, distance, angle)
	return _CallLocomotion(agent, "ForceToCombine", agents, distance, angle) or Vector3(0, 0, 0)
end

function AgentComponentAccess.ForceToSeparate(agent, agents, distance, angle)
	return _CallLocomotion(agent, "ForceToSeparate", agents, distance, angle) or Vector3(0, 0, 0)
end

function AgentComponentAccess.ApplyForce(agent, force)
	return _SetLocomotion(agent, "ApplyForce", force)
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
