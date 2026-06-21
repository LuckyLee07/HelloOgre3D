-- AIEvents.lua
-- Lua facade for AI event payload conventions. It normalizes the common
-- Chapter 8/9 event table shape before forwarding to existing C++ services.

local AIEvents = {}

AIEvents.EventTypes = {
	EnemySighted = "EnemySighted",
	BulletShot = "BulletShot",
	BulletImpact = "BulletImpact",
	DeadFriendlySighted = "DeadFriendlySighted",
	PositionUpdate = "PositionUpdate",
	EnemySelection = "EnemySelection",
	SupportRequested = "SupportRequested",
	SupportResponded = "SupportResponded",
}

AIEvents.Scope = {
	Local = "local",
	Team = "team",
	Global = "global",
}

local function _GetAgentId(agent)
	if agent ~= nil and agent.GetObjId ~= nil then
		return agent:GetObjId()
	end
	return -1
end

local function _GetTeamId(agent)
	if agent ~= nil and agent.GetTeamId ~= nil then
		return agent:GetTeamId()
	end
	return -1
end

local function _CloneVec3(position)
	if position == nil then
		return nil
	end
	return Vector3(position.x, position.y, position.z)
end

local function _ResolvePosition(event)
	return _CloneVec3(event.position)
		or _CloneVec3(event.seenAt)
		or _CloneVec3(event.targetPos)
		or _CloneVec3(event.supportPos)
		or _CloneVec3(event.retreatPos)
		or (event.agent ~= nil and event.agent.GetPosition ~= nil and event.agent:GetPosition() or nil)
end

local function _ResolveSender(event, eventType)
	local sender = event.sender or event.spotter or event.reporter
	if sender == nil and eventType ~= AIEvents.EventTypes.EnemySighted and eventType ~= AIEvents.EventTypes.DeadFriendlySighted then
		sender = event.agent
	end
	return sender
end

function AIEvents.Normalize(eventType, event, options)
	event = event or {}
	options = options or {}
	eventType = eventType or event.eventType or event.type

	local sender = _ResolveSender(event, eventType)
	local target = event.target or event.agent
	local position = _ResolvePosition(event)
	if position ~= nil and type(options.projectPosition) == "function" then
		position = options.projectPosition(position)
	end

	local senderId = event.senderId or event.spotterId or event.reporterId or event.requesterId or event.responderId or _GetAgentId(sender)
	local targetId = event.targetId or event.enemyId or event.agentId or _GetAgentId(target)
	local teamId = event.teamId or event.senderTeam or _GetTeamId(sender)
	if eventType == AIEvents.EventTypes.DeadFriendlySighted and event.agent ~= nil then
		teamId = event.agent:GetTeamId()
	end
	local timeMs = math.floor(tonumber(event.timeMs or event.lastSeenMs or event.lastSeen or options.timeMs) or 0)
	local scope = event.scope or options.scope or (event.teamOnly == true and AIEvents.Scope.Team or AIEvents.Scope.Global)

	return {
		eventType = eventType,
		type = eventType,
		senderId = senderId,
		targetId = targetId,
		teamId = teamId or -1,
		targetTeamId = event.targetTeamId or _GetTeamId(target),
		position = position,
		targetPos = _CloneVec3(position),
		seenAt = _CloneVec3(position),
		timeMs = timeMs,
		lastSeenMs = timeMs,
		confidence = tonumber(event.confidence or options.confidence) or 1.0,
		scope = scope,
		teamOnly = event.teamOnly == true or scope == AIEvents.Scope.Team,
		queueEvent = event.queueEvent == true or options.queueEvent == true,
		raw = event,
	}
end

function AIEvents.PublishTacticalEvent(eventType, event, options)
	if SandboxTactics == nil or SandboxTactics.publishTacticalEvent == nil then
		return false, nil
	end

	local payload = AIEvents.Normalize(eventType, event, options)
	if payload.eventType == nil or payload.position == nil then
		return false, payload
	end

	SandboxTactics:publishTacticalEvent(
		payload.eventType,
		payload.senderId,
		payload.targetId,
		payload.teamId,
		payload.targetTeamId,
		payload.position,
		payload.timeMs,
		payload.scope,
		payload.queueEvent)
	return true, payload
end

_G.AIEvents = AIEvents
return AIEvents
