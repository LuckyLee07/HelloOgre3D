require("res.scripts.agent.AgentUtils.lua")

local _MOVE_SEGMENT_MS = 500
local _IDLE_MS = 2000
local _SHOOT_MS = 600
local _SHOOT_FIRE_MS = 200
local _RELOAD_MS = 900
local _MOVE_REACH = 1.5
local _SHOOT_REACH = 3.0
local _MAX_RANDOM_ATTEMPTS = 64
local _COS_45_DEGREES = 0.707
local _EYE_FORWARD_OFFSET = 0.5
local _EYE_HEIGHT_RATIO = 0.5

local _states = {}

local function _GetState(agent)
	local id = agent:GetObjId()
	local state = _states[id]
	if state == nil then
		state = {
			action = nil,
			elapsedMs = 0,
			timeMs = 0,
			acc = Vector3(0, 0, 0),
			ammo = 10,
			maxAmmo = 10,
			fired = false,
			enemy = nil,
			visibleAgents = {},
			dead = false,
		}
		_states[id] = state
	end
	return state
end

local function _DistanceSq(left, right)
	local delta = left - right
	delta.y = 0
	return delta:squaredLength()
end

local function _Distance(left, right)
	return math.sqrt(_DistanceSq(left, right))
end

local function _ClearPath(agent)
	if agent.ClearMovePosition ~= nil then
		agent:ClearMovePosition()
	end
	local empty = std.vector_Ogre__Vector3_()
	agent:SetPath(empty, false)
end

local function _BuildAndSetPath(agent, target)
	local path = std.vector_Ogre__Vector3_()
	if not Sandbox:FindPath("default", agent:GetPosition(), target, path) or path:size() == 0 then
		return false
	end

	Agent_SetPath(agent, path, false)
	agent:SetTarget(target)
	agent:SetTargetRadius(1.0)
	if agent.SetMovePosition ~= nil then
		agent:SetMovePosition(target)
	end
	return true
end

local function _HasPath(agent, target)
	local path = std.vector_Ogre__Vector3_()
	return Sandbox:FindPath("default", agent:GetPosition(), target, path) and path:size() > 0
end

local function _IsValidEnemy(agent, candidate)
	return candidate ~= nil
		and candidate ~= agent
		and candidate:GetHealth() > 0
		and candidate:GetTeamId() ~= agent:GetTeamId()
end

local function _GetEyePosition(agent)
	local forward = agent:GetForward()
	forward.y = 0
	if forward:squaredLength() <= 0.0001 then
		forward = Vector3(0, 0, 1)
	else
		forward = forward:normalisedCopy()
	end

	local height = agent.GetHeight ~= nil and agent:GetHeight() or 1.6
	return agent:GetPosition() + Vector3(0, height * _EYE_HEIGHT_RATIO, 0) + forward * _EYE_FORWARD_OFFSET, forward
end

local function _CanSeeAgent(agent, candidate)
	if Sandbox == nil or Sandbox.RayCastObjectId == nil then
		return true
	end

	local eyePosition, forward = _GetEyePosition(agent)
	local target = candidate:GetPosition()
	local toTarget = target - eyePosition
	if toTarget:squaredLength() <= 0.0001 then
		return true
	end

	local rayVector = toTarget:normalisedCopy()
	local dotProduct = rayVector:dotProduct(forward)
	if dotProduct < _COS_45_DEGREES then
		return false
	end

	local hitObjectId = Sandbox:RayCastObjectId(eyePosition, target)
	return hitObjectId == 0 or hitObjectId == candidate:GetObjId()
end

local function _UpdateVisibility(agent, state)
	if ObjectManager == nil or ObjectManager.getAllAgents == nil then
		return
	end

	local agents = ObjectManager:getAllAgents()
	local visibleAgents = state.visibleAgents or {}
	state.visibleAgents = visibleAgents

	for i = 0, agents:size() - 1 do
		local candidate = agents[i]
		if candidate ~= nil
			and candidate ~= agent
			and candidate:GetHealth() > 0
			and _CanSeeAgent(agent, candidate) then
			visibleAgents[candidate:GetObjId()] = {
				agent = candidate,
				seenAt = candidate:GetPosition(),
				lastSeen = state.timeMs,
			}
		end
	end

	for id, info in pairs(visibleAgents) do
		if info == nil or info.agent == nil or (state.timeMs - (info.lastSeen or 0)) > 1000 then
			visibleAgents[id] = nil
		end
	end
end

local function _ChooseBestEnemy(agent, state)
	local visibleAgents = state.visibleAgents or {}
	local bestEnemy = nil
	local bestDistanceSq = nil
	for id, info in pairs(visibleAgents) do
		local enemy = info ~= nil and info.agent or nil
		if _IsValidEnemy(agent, enemy)
			and (state.timeMs - (info.lastSeen or 0)) <= 1000
			and _HasPath(agent, enemy:GetPosition()) then
			local distanceSq = _DistanceSq(agent:GetPosition(), enemy:GetPosition())
			if bestEnemy == nil or distanceSq < bestDistanceSq then
				bestEnemy = enemy
				bestDistanceSq = distanceSq
			end
		end
	end
	return bestEnemy
end

local function _HasMovePosition(agent)
	local target = agent:GetTarget()
	return target ~= nil and _Distance(agent:GetPosition(), target) > _MOVE_REACH
end

local function _ApplyMove(agent, state, deltaMs)
	local dtSec = deltaMs / 1000.0
	local steering = Soldier_CalculateSteering(agent, dtSec)
	AgentUtilities_ApplySteeringForce2(agent, steering, state.acc, dtSec)
	AgentUtilities_ClampHorizontalSpeed(agent)
end

local function _SlowMovement(agent, rate)
	rate = rate or 1.0
	local velocity = agent:GetVelocity()
	local y = velocity.y
	velocity.y = 0
	velocity = velocity * 0.91 * (1.0 / rate)
	velocity.y = y
	agent:SetVelocity(velocity)
end

local function _Face(agent, target)
	if target == nil then return end
	local forward = target - agent:GetPosition()
	forward.y = 0
	if forward:squaredLength() > 0.0001 then
		agent:SetForward(forward)
	end
end

local function _BeginIdle(agent, state)
	state.action = "idle"
	state.elapsedMs = 0
	agent:EnterIdleAnim()
end

local function _BeginMove(agent, state)
	state.action = "move"
	state.elapsedMs = 0
	agent:EnterMoveAnim()
end

local function _BeginPursue(agent, state, enemy)
	state.action = "pursue"
	state.elapsedMs = 0
	state.enemy = enemy
	if enemy ~= nil then
		local target = enemy:GetPosition()
		if _BuildAndSetPath(agent, target) then
			agent:EnterMoveAnim()
		end
	end
end

local function _BeginRandomMove(agent, state)
	state.action = "randomMove"
	state.elapsedMs = 0

	for attempt = 1, _MAX_RANDOM_ATTEMPTS do
		local target = Sandbox:RandomPoint("default")
		if target ~= nil and _BuildAndSetPath(agent, target) then
			return
		end
	end
end

local function _BeginShoot(agent, state, enemy)
	state.action = "shoot"
	state.elapsedMs = 0
	state.enemy = enemy
	state.fired = false
	-- EnterShootAnim fires the current weapon through an ASM callback. Legacy
	-- parity keeps shoot as decision timing only so modern bullets do not skew
	-- the old Chapter9 trace.
	agent:EnterIdleAnim()
	_Face(agent, enemy ~= nil and enemy:GetPosition() or nil)
end

local function _BeginReload(agent, state)
	state.action = "reload"
	state.elapsedMs = 0
	if agent.EnterReloadAnim ~= nil then
		agent:EnterReloadAnim()
	else
		agent:EnterIdleAnim()
	end
end

local function _BeginDeath(agent, state)
	state.action = "dead"
	state.dead = true
	state.elapsedMs = 0
	_ClearPath(agent)
	agent:SetVelocity(Vector3(0, 0, 0))
	if agent.EnterDeathAnim ~= nil then
		agent:EnterDeathAnim()
	end
end

local function _UpdateAction(agent, state, deltaMs)
	local action = state.action
	if action == nil then
		return false
	end

	state.elapsedMs = state.elapsedMs + deltaMs

	if action == "dead" then
		return true
	elseif action == "idle" then
		_SlowMovement(agent, 1.0)
		if state.elapsedMs >= _IDLE_MS then
			state.action = nil
		end
		return true
	elseif action == "move" then
		_ApplyMove(agent, state, deltaMs)
		if state.elapsedMs >= _MOVE_SEGMENT_MS or _Distance(agent:GetPosition(), agent:GetTarget()) < _MOVE_REACH then
			_ClearPath(agent)
			state.action = nil
		end
		return true
	elseif action == "pursue" then
		local enemy = state.enemy
		if not _IsValidEnemy(agent, enemy) then
			state.action = nil
			return true
		end

		local target = enemy:GetPosition()
		_BuildAndSetPath(agent, target)
		_ApplyMove(agent, state, deltaMs)
		if _Distance(agent:GetPosition(), target) < _SHOOT_REACH then
			_ClearPath(agent)
			state.action = nil
		end
		return true
	elseif action == "randomMove" then
		state.action = nil
		return true
	elseif action == "shoot" then
		local enemy = state.enemy
		if _IsValidEnemy(agent, enemy) then
			_Face(agent, enemy:GetPosition())
		end
		_SlowMovement(agent, 1.0)
		if not state.fired and state.elapsedMs >= _SHOOT_FIRE_MS then
			state.fired = true
			if state.ammo > 0 then
				state.ammo = state.ammo - 1
				if agent.ConsumeAmmo ~= nil then
					agent:ConsumeAmmo(1)
				end
			end
		end
		if state.elapsedMs >= _SHOOT_MS then
			state.action = nil
		end
		return true
	elseif action == "reload" then
		_SlowMovement(agent, 1.0)
		if state.elapsedMs >= _RELOAD_MS then
			state.ammo = state.maxAmmo
			if agent.SetAmmo ~= nil then
				agent:SetAmmo(state.ammo)
			end
			state.action = nil
		end
		return true
	end

	state.action = nil
	return false
end

local function _EvaluateAndBegin(agent, state)
	if agent:GetHealth() <= 0 then
		_BeginDeath(agent, state)
		return
	end

	local maxHealth = agent.GetMaxHealth ~= nil and agent:GetMaxHealth() or 100.0
	if agent:GetHealth() < maxHealth * 0.2 then
		_BeginRandomMove(agent, state)
		return
	end

	local enemy = _ChooseBestEnemy(agent, state)
	if enemy ~= nil then
		if state.ammo <= 0 then
			_BeginReload(agent, state)
		elseif _Distance(agent:GetPosition(), enemy:GetPosition()) < _SHOOT_REACH then
			_BeginShoot(agent, state, enemy)
		else
			_BeginPursue(agent, state, enemy)
		end
		return
	end

	if _HasMovePosition(agent) then
		_BeginMove(agent, state)
	elseif math.random() >= 0.5 then
		_BeginRandomMove(agent, state)
	else
		_BeginIdle(agent, state)
	end
end

function Agent_Initialize(agent)
	if agent == nil then return end

	agent:SetMaxSpeed(SOLDIER_STAND_SPEED or 3.0)
	if agent.SetMaxAmmo ~= nil then agent:SetMaxAmmo(10) end
	if agent.SetAmmo ~= nil then agent:SetAmmo(10) end

	local ai = agent:GetAI()
	if ai ~= nil then
		ai:SetDriverByType("dt")
		local driver = ai:GetDecisionTreeDriver()
		local bb = driver ~= nil and driver:GetBlackboard() or ai:GetBlackboard()
		if bb ~= nil then
			bb:SetFloat("maxHealth", agent:GetMaxHealth())
			local sampleName = _G.HELLO_SANDBOX_SAMPLE_NAME or "Sandbox17"
			if ConfigManager ~= nil and ConfigManager.ApplyAiBlackboardDefaults ~= nil then
				ConfigManager:ApplyAiBlackboardDefaults(bb, sampleName)
			end
		end
	end

	local state = _GetState(agent)
	state.action = nil
	state.elapsedMs = 0
	state.timeMs = 0
	state.ammo = 10
	state.maxAmmo = 10
	state.fired = false
	state.enemy = nil
	state.visibleAgents = {}
	state.dead = false
	agent:EnterIdleAnim()
end

function Agent_Update(agent, deltaTimeInMillis)
	if agent == nil then return end

	local state = _GetState(agent)
	state.timeMs = state.timeMs + (deltaTimeInMillis or 0)

	if state.dead then
		return
	end

	if _UpdateAction(agent, state, deltaTimeInMillis or 0) then
		_UpdateVisibility(agent, state)
		return
	end

	_EvaluateAndBegin(agent, state)
	_UpdateVisibility(agent, state)
end

function Agent_EventHandle(agent, keycode)
end
