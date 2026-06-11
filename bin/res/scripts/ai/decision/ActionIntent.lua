-- ActionIntent.lua
-- Trace-only intent recorder for AI action / movement / animation diagnostics.
-- It writes to Blackboard only when parity or intent tracing is enabled.

local ActionIntent = {}

local function _GetEnv(name)
	return os.getenv and os.getenv(name) or nil
end

local function _Truthy(value)
	return value == true or value == "1" or value == "true" or value == "TRUE" or value == "True" or value == "yes" or value == "YES"
end

local function _SafeCall(object, methodName, defaultValue, ...)
	if object == nil or object[methodName] == nil then
		return defaultValue
	end
	local ok, result = pcall(object[methodName], object, ...)
	if ok then
		return result
	end
	return defaultValue
end

local function _HorizontalDistance(a, b)
	if a == nil or b == nil then
		return -1.0
	end
	local delta = a - b
	delta.y = 0
	return delta:length()
end

function ActionIntent.IsEnabled(bb)
	if _Truthy(_GetEnv("HELLO_INTENT_TRACE")) or _Truthy(_GetEnv("HELLO_PARITY_TRACE")) then
		return true
	end
	if bb ~= nil and bb.GetBool ~= nil then
		return bb:GetBool("intent.traceEnabled", false)
	end
	return false
end

function ActionIntent.Record(owner, bb, data)
	if bb == nil or not ActionIntent.IsEnabled(bb) then
		return false
	end
	data = data or {}

	local target = data.target
	if target == nil and owner ~= nil then
		target = _SafeCall(owner, "GetTarget", nil)
	end
	local position = _SafeCall(owner, "GetPosition", nil)
	local velocity = _SafeCall(owner, "GetVelocity", nil)
	local distance = data.distance
	if distance == nil then
		distance = _HorizontalDistance(position, target)
	end

	local sequence = bb:GetInt("intent.sequence", 0) + 1
	bb:SetInt("intent.sequence", sequence)
	bb:SetString("intent.action", tostring(data.action or ""))
	bb:SetString("intent.phase", tostring(data.phase or ""))
	bb:SetString("intent.movement", tostring(data.movement or ""))
	bb:SetString("intent.animation", tostring(data.animation or ""))
	bb:SetString("intent.reason", tostring(data.reason or ""))
	bb:SetFloat("intent.elapsedMs", tonumber(data.elapsedMs) or 0.0)
	bb:SetFloat("intent.durationMs", tonumber(data.durationMs) or 0.0)
	bb:SetFloat("intent.distance", tonumber(distance) or -1.0)
	bb:SetFloat("intent.speed", _SafeCall(owner, "GetSpeed", 0.0))
	bb:SetFloat("intent.maxSpeed", _SafeCall(owner, "GetMaxSpeed", 0.0))
	bb:SetBool("intent.moving", _SafeCall(owner, "IsMoving", false) == true)
	if position ~= nil then
		bb:SetVec3("intent.position", position)
	end
	if target ~= nil then
		bb:SetVec3("intent.target", target)
	end
	if velocity ~= nil then
		bb:SetVec3("intent.velocity", velocity)
	end

	local enemy = data.enemy
	if enemy ~= nil then
		bb:SetInt("intent.enemyId", _SafeCall(enemy, "GetObjId", -1))
		bb:SetInt("intent.enemyTeam", _SafeCall(enemy, "GetTeamId", -1))
		local enemyPos = _SafeCall(enemy, "GetPosition", nil)
		if enemyPos ~= nil then
			bb:SetVec3("intent.enemyPosition", enemyPos)
			bb:SetFloat("intent.enemyDistance", _HorizontalDistance(position, enemyPos))
		end
	else
		bb:SetInt("intent.enemyId", -1)
		bb:SetInt("intent.enemyTeam", -1)
		bb:SetFloat("intent.enemyDistance", -1.0)
	end
	return true
end

return ActionIntent
