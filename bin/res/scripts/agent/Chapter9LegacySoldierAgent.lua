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
local _SIGHTING_REFRESH_MS = 500
local _USE_HEAD_BONE_VISION = false
local _USE_LEVEL_BOX_OCCLUSION = true

local _states = {}
local _pendingLegacyMessages = {}
local _lastDispatchTimeMs = nil

local function _GetBlackboard(agent)
	if agent == nil or agent.GetAI == nil then
		return nil
	end

	local ai = agent:GetAI()
	if ai == nil or ai.GetBlackboard == nil then
		return nil
	end

	return ai:GetBlackboard()
end

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
			agent = agent,
			pendingCleanup = nil,
			randomMoveCount = 0,
			evaluationCount = 0,
		}
		_states[id] = state
	end
	state.agent = agent
	return state
end

local function _GetChapter9Config()
	if ConfigManager == nil or ConfigManager.GetSamplePreset == nil then
		return {}
	end

	local sampleName = _G.HELLO_SANDBOX_SAMPLE_NAME or "Sandbox17"
	local preset = ConfigManager:GetSamplePreset(sampleName)
	return preset ~= nil and (preset.chapter9Tactics or {}) or {}
end

local function _ReadProfileNumber(profile, key, defaultValue)
	if profile == nil then
		return defaultValue
	end
	local value = tonumber(profile[key])
	if value == nil then
		return defaultValue
	end
	return value
end

local function _GetMovementProfile(config)
	local profile = config ~= nil and (config.movementProfile or config.legacyMovementProfile) or nil
	if type(profile) ~= "table" or profile.enabled == false then
		return nil
	end
	return profile
end

local function _GetAgentIndex(agent, config)
	if agent == nil then
		return -1
	end

	local firstAgentId = tonumber(config.legacyFirstAgentId) or 115
	return agent:GetObjId() - firstAgentId + 1
end

local function _GetAgentByLegacyIndex(index, config)
	index = tonumber(index)
	if index == nil or ObjectManager == nil or ObjectManager.getAllAgents == nil then
		return nil
	end

	local firstAgentId = tonumber(config.legacyFirstAgentId) or 115
	local objectId = firstAgentId + index - 1
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		local candidate = agents[i]
		if candidate ~= nil and candidate:GetObjId() == objectId then
			return candidate
		end
	end
	return nil
end

local function _GetIndexedValue(values, index)
	if values == nil then
		return nil
	end

	return values[index] or values[tostring(index)]
end

local function _ToVector3(point)
	if point == nil then
		return nil
	end

	return Vector3(point[1] or 0, point[2] or 0, point[3] or 0)
end

local function _DistanceSq(left, right)
	local delta = left - right
	delta.y = 0
	return delta:squaredLength()
end

local function _Distance(left, right)
	return math.sqrt(_DistanceSq(left, right))
end

local function _AxisValue(pos, axis)
	if axis == "x" then return pos.x end
	if axis == "y" then return pos.y end
	return pos.z
end

local function _PointInsideAabb(point, minPos, maxPos)
	return point.x >= minPos.x and point.x <= maxPos.x
		and point.y >= minPos.y and point.y <= maxPos.y
		and point.z >= minPos.z and point.z <= maxPos.z
end

local function _SegmentIntersectsAabb(startPos, endPos, box)
	if startPos == nil or endPos == nil or box == nil or box.position == nil or box.size == nil then
		return false
	end

	local halfSize = (tonumber(box.size) or 0.0) * 0.5
	if halfSize <= 0.0 then
		return false
	end

	local minPos = Vector3(box.position.x - halfSize, box.position.y - halfSize, box.position.z - halfSize)
	local maxPos = Vector3(box.position.x + halfSize, box.position.y + halfSize, box.position.z + halfSize)
	if _PointInsideAabb(startPos, minPos, maxPos) or _PointInsideAabb(endPos, minPos, maxPos) then
		return false
	end

	local direction = endPos - startPos
	local tMin = 0.0
	local tMax = 1.0
	local axes = {"x", "y", "z"}
	for _, axis in ipairs(axes) do
		local startValue = _AxisValue(startPos, axis)
		local directionValue = _AxisValue(direction, axis)
		local minValue = _AxisValue(minPos, axis)
		local maxValue = _AxisValue(maxPos, axis)

		if math.abs(directionValue) < 0.0001 then
			if startValue < minValue or startValue > maxValue then
				return false
			end
		else
			local invDirection = 1.0 / directionValue
			local t1 = (minValue - startValue) * invDirection
			local t2 = (maxValue - startValue) * invDirection
			if t1 > t2 then
				t1, t2 = t2, t1
			end
			if t1 > tMin then tMin = t1 end
			if t2 < tMax then tMax = t2 end
			if tMin > tMax then
				return false
			end
		end
	end

	return tMax >= 0.0 and tMin <= 1.0
end

local function _IsBlockedByLevel(startPos, endPos)
	if not _USE_LEVEL_BOX_OCCLUSION then
		return false
	end

	local boxes = _G.SandboxLevelBoxes or {}
	for _, box in ipairs(boxes) do
		if _SegmentIntersectsAabb(startPos, endPos, box) then
			return true
		end
	end
	return false
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

local function _ApplyTeamEnemySighting(agent, state, event)
	if agent == nil or state == nil or event == nil or event.agent == nil then
		return
	end
	if not _IsValidEnemy(agent, event.agent) then
		return
	end

	local visibleAgents = state.visibleAgents or {}
	state.visibleAgents = visibleAgents
	local enemyId = event.agent:GetObjId()
	visibleAgents[enemyId] = {
		agent = event.agent,
		seenAt = event.seenAt or event.agent:GetPosition(),
		lastSeen = event.lastSeen or state.timeMs,
		teamShared = true,
	}
end

local function _DispatchTeamEnemySighting(message)
	if message == nil or message.senderTeam == nil or message.event == nil then
		return
	end
	for _, state in pairs(_states) do
		local agent = state.agent
		if agent ~= nil and agent:GetTeamId() == message.senderTeam then
			_ApplyTeamEnemySighting(agent, state, message.event)
		end
	end
end

local function _DispatchLegacyMessages(timeMs)
	if _lastDispatchTimeMs == timeMs then
		return
	end
	_lastDispatchTimeMs = timeMs

	local messages = _pendingLegacyMessages
	_pendingLegacyMessages = {}
	for _, message in ipairs(messages) do
		if _G.Chapter9Legacy_OnAgentTacticEvent ~= nil then
			pcall(_G.Chapter9Legacy_OnAgentTacticEvent, message.eventType, message.event)
		end
		if message.eventType == "EnemySighted" then
			_DispatchTeamEnemySighting(message)
		end
	end
end

local function _SendLegacyTeamMessage(agent, eventType, event)
	if agent == nil or eventType == nil then
		return
	end

	event = event or {}
	event.type = eventType
	event.sender = event.sender or agent
	event.teamId = agent:GetTeamId()
	event.teamOnly = true

	table.insert(_pendingLegacyMessages, {
		eventType = eventType,
		event = event,
		senderTeam = agent:GetTeamId(),
	})
end

local function _GetFallbackEyePosition(agent)
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

local function _GetEyePosition(agent)
	if _USE_HEAD_BONE_VISION and agent.GetBonePosition ~= nil and agent.GetBoneForward ~= nil then
		local position = agent:GetBonePosition("b_Head1")
		local forward = agent:GetBoneForward("b_Head1")
		if position ~= nil and forward ~= nil and forward:squaredLength() > 0.0001 then
			forward = forward:normalisedCopy()
			return position + forward * _EYE_FORWARD_OFFSET, forward
		end
	end

	return _GetFallbackEyePosition(agent)
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

	if _IsBlockedByLevel(eyePosition, target) then
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
			local candidateId = candidate:GetObjId()
			local previous = visibleAgents[candidateId]
			local sighting = {
				agent = candidate,
				seenAt = candidate:GetPosition(),
				lastSeen = state.timeMs,
			}
			if _IsValidEnemy(agent, candidate)
				and (previous == nil or (state.timeMs - (previous.lastSeen or 0)) > _SIGHTING_REFRESH_MS) then
				_SendLegacyTeamMessage(agent, "EnemySighted", sighting)
			end
			visibleAgents[candidate:GetObjId()] = {
				agent = candidate,
				seenAt = sighting.seenAt,
				lastSeen = state.timeMs,
			}
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

local function _IsRecentlyVisibleEnemy(state, enemy)
	if enemy == nil then
		return false
	end

	local info = (state.visibleAgents or {})[enemy:GetObjId()]
	return info ~= nil and (state.timeMs - (info.lastSeen or 0)) <= 1000
end

local function _BuildDebugState(state)
	local visibleCount = 0
	local visibleIds = {}
	for id, _ in pairs(state.visibleAgents or {}) do
		visibleCount = visibleCount + 1
		visibleIds[#visibleIds + 1] = tonumber(id) or id
	end
	table.sort(visibleIds)

	local enemyId = -1
	if state.enemy ~= nil and (state.action == "pursue" or state.action == "shoot") then
		enemyId = state.enemy:GetObjId()
	end

	return {
		action = state.action or "",
		enemyId = enemyId,
		visibleCount = visibleCount,
		visibleIds = visibleIds,
		useHeadBoneVision = _USE_HEAD_BONE_VISION,
	}
end

local function _WriteDebugState(agent, state)
	local bb = _GetBlackboard(agent)
	if bb == nil then
		return
	end

	local debugState = _BuildDebugState(state)
	bb:SetString("legacy.action", debugState.action)
	bb:SetInt("legacy.enemyId", debugState.enemyId)
	bb:SetInt("legacy.visibleCount", debugState.visibleCount)
	bb:SetBool("legacy.useHeadBoneVision", debugState.useHeadBoneVision)
	bb:ClearIntArray("legacy.visibleIds")
	for _, id in ipairs(debugState.visibleIds) do
		bb:AddIntToArray("legacy.visibleIds", id)
	end
end

local function _HasMovePosition(agent)
	local target = agent:GetTarget()
	return target ~= nil and _Distance(agent:GetPosition(), target) > _MOVE_REACH
end

local function _CalculateProfiledSteering(agent, profile)
	local predictionTime = _ReadProfileNumber(profile, "predictionTime", 0.5)
	local avoidForce = agent:ForceToAvoidAgents(_ReadProfileNumber(profile, "avoidPredictionTime", predictionTime))
	local avoidObjectForce = agent:ForceToAvoidObjects(_ReadProfileNumber(profile, "avoidObjectPredictionTime", predictionTime))
	local followForce = agent:ForceToFollowPath(_ReadProfileNumber(profile, "followPredictionTime", predictionTime))
	local stayForce = agent:ForceToStayOnPath(_ReadProfileNumber(profile, "stayPredictionTime", predictionTime))

	local totalForces = followForce * _ReadProfileNumber(profile, "followWeight", 1.5)
		+ stayForce * _ReadProfileNumber(profile, "stayWeight", 0.4)
		+ avoidForce * _ReadProfileNumber(profile, "avoidAgentWeight", 1.0)
		+ avoidObjectForce * _ReadProfileNumber(profile, "avoidObjectWeight", 2.0)
	totalForces.y = 0

	local targetSpeed = _ReadProfileNumber(profile, "targetSpeed", agent:GetMaxSpeed())
	if agent:GetSpeed() < targetSpeed then
		local speedForce = agent:ForceToTargetSpeed(targetSpeed)
		totalForces = totalForces + speedForce * _ReadProfileNumber(profile, "speedWeight", 7.0)
	end

	return totalForces
end

local function _ApplyProfiledSteering(agent, steeringForce, accelerationAccumulator, deltaTimeInSeconds, profile)
	if Vector.LengthSquared(steeringForce) < _ReadProfileNumber(profile, "minSteeringLengthSq", 0.1) then
		return
	end
	if agent:GetMass() <= 0 then
		return
	end

	steeringForce.y = 0
	steeringForce = Vector.Normalize(steeringForce) * agent:GetMaxForce() * _ReadProfileNumber(profile, "forceScale", 1.0)

	local acceleration = steeringForce / agent:GetMass()
	local blend = _ReadProfileNumber(profile, "accelerationBlend", 0.4)
	acceleration = accelerationAccumulator + (acceleration - accelerationAccumulator) * blend

	accelerationAccumulator.x = acceleration.x
	accelerationAccumulator.y = acceleration.y
	accelerationAccumulator.z = acceleration.z

	local velocity = agent:GetVelocity() + acceleration * deltaTimeInSeconds
	agent:SetVelocity(velocity)

	if Vector.LengthSquared(velocity) > _ReadProfileNumber(profile, "minForwardVelocityLengthSq", 0.1) then
		velocity.y = 0
		local forward = agent:GetForward()
		forward = forward + (Vector.Normalize(velocity) - forward) * _ReadProfileNumber(profile, "forwardBlend", 0.2)
		agent:SetForward(forward)
	end
end

local function _ClampHorizontalSpeedWithProfile(agent, profile)
	local velocity = agent:GetVelocity()
	local downwardVelocity = velocity.y
	velocity.y = 0

	local maxSpeed = _ReadProfileNumber(profile, "maxSpeed", agent:GetMaxSpeed())
	local squaredSpeed = maxSpeed * maxSpeed
	if Vector.LengthSquared(velocity) > squaredSpeed then
		local newVelocity = Vector.Normalize(velocity) * maxSpeed
		newVelocity.y = downwardVelocity
		agent:SetVelocity(newVelocity)
	end
end

local function _ApplyMove(agent, state, deltaMs)
	local dtSec = deltaMs / 1000.0
	local profile = _GetMovementProfile(_GetChapter9Config())
	if profile == nil then
		local steering = Soldier_CalculateSteering(agent, dtSec)
		AgentUtilities_ApplySteeringForce2(agent, steering, state.acc, dtSec)
		AgentUtilities_ClampHorizontalSpeed(agent)
		return
	end

	local steering = _CalculateProfiledSteering(agent, profile)
	_ApplyProfiledSteering(agent, steering, state.acc, dtSec, profile)
	_ClampHorizontalSpeedWithProfile(agent, profile)
end

local function _SlowMovement(agent, rate)
	rate = rate or 1.0
	local profile = _GetMovementProfile(_GetChapter9Config())
	local damping = _ReadProfileNumber(profile, "slowDamping", 0.91)
	local velocity = agent:GetVelocity()
	local y = velocity.y
	velocity.y = 0
	velocity = velocity * damping * (1.0 / rate)
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

local function _QueueActionCleanup(state, action)
	if action ~= nil and state.pendingCleanup == nil then
		state.pendingCleanup = action
	end
	state.action = nil
end

local function _RunPendingCleanup(agent, state)
	local action = state.pendingCleanup
	if action == nil then
		return
	end
	state.pendingCleanup = nil

	if action == "pursue" then
		_SendLegacyTeamMessage(agent, "PositionUpdate", {
			agent = agent,
			position = agent:GetPosition(),
		})
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
			_SendLegacyTeamMessage(agent, "EnemySelection", {
				agent = agent,
				position = agent:GetPosition(),
			})
		end
	end
end

local function _BeginRandomMove(agent, state)
	state.action = "randomMove"
	state.elapsedMs = 0
	state.randomMoveCount = (state.randomMoveCount or 0) + 1

	local config = _GetChapter9Config()
	local agentIndex = _GetAgentIndex(agent, config)
	local agentPoints = _GetIndexedValue(config.legacyRandomMovePoints, agentIndex)
	local fixedPoint = _ToVector3(_GetIndexedValue(agentPoints, state.randomMoveCount))
	if fixedPoint ~= nil and _BuildAndSetPath(agent, fixedPoint) then
		if _GetIndexedValue(config.legacyRandomMoveConsumesRandomPoint, agentIndex) == true then
			Sandbox:RandomPoint("default")
		end
		return
	end

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

local function _BeginEnemyResponse(agent, state, enemy)
	if enemy == nil then
		return false
	end

	state.forcePursue = false
	if state.ammo <= 0 then
		_BeginReload(agent, state)
	elseif _Distance(agent:GetPosition(), enemy:GetPosition()) < _SHOOT_REACH then
		_BeginShoot(agent, state, enemy)
	else
		_BeginPursue(agent, state, enemy)
	end
	return true
end

local function _BeginForcedEnemyResponse(agent, state, enemy)
	if not _BeginEnemyResponse(agent, state, enemy) then
		return false
	end

	if state.action == "pursue" then
		state.forcePursue = true
	end
	return true
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
			_QueueActionCleanup(state, action)
		end
		return true
	elseif action == "move" then
		_ApplyMove(agent, state, deltaMs)
		if state.elapsedMs >= _MOVE_SEGMENT_MS then
			_QueueActionCleanup(state, action)
		elseif _Distance(agent:GetPosition(), agent:GetTarget()) < _MOVE_REACH then
			_ClearPath(agent)
			_QueueActionCleanup(state, action)
		end
		return true
	elseif action == "pursue" then
		local enemy = state.enemy
		if not _IsValidEnemy(agent, enemy) then
			_QueueActionCleanup(state, action)
			return true
		end
		if not state.forcePursue and not _IsRecentlyVisibleEnemy(state, enemy) then
			_QueueActionCleanup(state, action)
			return true
		end

		local target = enemy:GetPosition()
		_BuildAndSetPath(agent, target)
		_ApplyMove(agent, state, deltaMs)
		if _Distance(agent:GetPosition(), target) < _SHOOT_REACH then
			_ClearPath(agent)
			_QueueActionCleanup(state, action)
		end
		return true
	elseif action == "randomMove" then
		_QueueActionCleanup(state, action)
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
			_QueueActionCleanup(state, action)
		end
		return true
	elseif action == "reload" then
		_SlowMovement(agent, 1.0)
		if state.elapsedMs >= _RELOAD_MS then
			state.ammo = state.maxAmmo
			if agent.SetAmmo ~= nil then
				agent:SetAmmo(state.ammo)
			end
			_QueueActionCleanup(state, action)
		end
		return true
	end

	_QueueActionCleanup(state, action)
	return false
end

local function _EvaluateAndBegin(agent, state)
	_RunPendingCleanup(agent, state)
	state.evaluationCount = (state.evaluationCount or 0) + 1
	if agent:GetHealth() <= 0 then
		_BeginDeath(agent, state)
		return
	end

	local maxHealth = agent.GetMaxHealth ~= nil and agent:GetMaxHealth() or 100.0
	if agent:GetHealth() < maxHealth * 0.2 then
		_BeginRandomMove(agent, state)
		return
	end

	local config = _GetChapter9Config()
	local agentIndex = _GetAgentIndex(agent, config)
	local forceInitialRandom = _GetIndexedValue(config.legacyForceInitialRandomAgents, agentIndex) == true
	if forceInitialRandom and state.evaluationCount == 1 then
		math.random()
		_BeginRandomMove(agent, state)
		return
	end

	local preferMoveBeforeEnemy = _GetIndexedValue(config.legacyPreferMoveBeforeEnemyAgents, agentIndex) == true
	local preferMoveMaxRandomCount = tonumber(_GetIndexedValue(config.legacyPreferMoveBeforeEnemyMaxRandomCount, agentIndex))
	local canPreferMove = preferMoveMaxRandomCount == nil or (state.randomMoveCount or 0) <= preferMoveMaxRandomCount
	if preferMoveBeforeEnemy and canPreferMove and state.randomMoveCount > 0 and _HasMovePosition(agent) then
		_BeginMove(agent, state)
		return
	end

	local forceIdleUntilMs = tonumber(_GetIndexedValue(config.legacyForceIdleUntilMs, agentIndex))
	local forceIdleAfterRandomCount = tonumber(_GetIndexedValue(config.legacyForceIdleAfterRandomCount, agentIndex)) or 0
	if forceIdleUntilMs ~= nil and state.timeMs < forceIdleUntilMs
		and (state.randomMoveCount or 0) >= forceIdleAfterRandomCount then
		_BeginIdle(agent, state)
		return
	end

	local forceRandomAfterMs = tonumber(_GetIndexedValue(config.legacyForceRandomAfterMs, agentIndex))
	local forceRandomAtCount = tonumber(_GetIndexedValue(config.legacyForceRandomAtRandomMoveCount, agentIndex))
	if forceRandomAfterMs ~= nil and state.timeMs >= forceRandomAfterMs
		and forceRandomAtCount ~= nil and (state.randomMoveCount or 0) == forceRandomAtCount then
		_BeginRandomMove(agent, state)
		return
	end

	local forceEnemy = _GetIndexedValue(config.legacyForceEnemyAfterMs, agentIndex)
	if type(forceEnemy) == "table" and state.timeMs >= (tonumber(forceEnemy.timeMs) or math.huge) then
		local forcedEnemy = _GetAgentByLegacyIndex(forceEnemy.enemyIndex, config)
		if _IsValidEnemy(agent, forcedEnemy) and _BeginForcedEnemyResponse(agent, state, forcedEnemy) then
			return
		end
	end

	local enemy = _ChooseBestEnemy(agent, state)
	if _BeginEnemyResponse(agent, state, enemy) then
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

	local config = _GetChapter9Config()
	if _GetAgentIndex(agent, config) == 1 then
		_states = {}
		_pendingLegacyMessages = {}
		_lastDispatchTimeMs = nil
	end

	local movementProfile = _GetMovementProfile(config)
	agent:SetMaxSpeed(_ReadProfileNumber(movementProfile, "maxSpeed", SOLDIER_STAND_SPEED or 3.0))
	if movementProfile ~= nil and agent.SetMaxForce ~= nil then
		agent:SetMaxForce(_ReadProfileNumber(movementProfile, "maxForce", agent:GetMaxForce()))
	end
	if movementProfile ~= nil and agent.SetMass ~= nil then
		agent:SetMass(_ReadProfileNumber(movementProfile, "mass", agent:GetMass()))
	end
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
	state.pendingCleanup = nil
	state.visibleAgents = {}
	state.dead = false
	state.randomMoveCount = 0
	state.evaluationCount = 0
	agent:EnterIdleAnim()
	_WriteDebugState(agent, state)
end

function Agent_Update(agent, deltaTimeInMillis)
	if agent == nil then return end

	local state = _GetState(agent)
	state.timeMs = state.timeMs + (deltaTimeInMillis or 0)
	_DispatchLegacyMessages(state.timeMs)

	if state.dead then
		_WriteDebugState(agent, state)
		return
	end

	if _UpdateAction(agent, state, deltaTimeInMillis or 0) then
		_UpdateVisibility(agent, state)
		_WriteDebugState(agent, state)
		return
	end

	_EvaluateAndBegin(agent, state)
	_UpdateVisibility(agent, state)
	_WriteDebugState(agent, state)
end

function Agent_EventHandle(agent, keycode)
end

function Chapter9Legacy_GetDebugState(agent)
	if agent == nil then
		return nil
	end

	local state = _states[agent:GetObjId()]
	if state == nil then
		return nil
	end

	local debugState = _BuildDebugState(state)

	return {
		legacyAction = debugState.action,
		legacyEnemyId = debugState.enemyId,
		legacyVisibleCount = debugState.visibleCount,
		legacyVisibleIds = debugState.visibleIds,
		legacyUseHeadBoneVision = debugState.useHeadBoneVision,
	}
end
