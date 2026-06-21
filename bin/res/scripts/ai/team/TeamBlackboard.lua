-- TeamBlackboard.lua
-- Small shared-memory service for team-level AI samples.

local AIEvents = require("res.scripts.ai.events.AIEvents.lua")

local TeamBlackboard = {}

TeamBlackboard.EventTypes = {
	EnemySighted = AIEvents.EventTypes.EnemySighted,
	SupportRequested = AIEvents.EventTypes.SupportRequested,
	SupportResponded = AIEvents.EventTypes.SupportResponded,
	FocusTarget = "FocusTarget",
	RetreatPoint = "RetreatPoint",
	FormationSlot = "FormationSlot",
}

local _teams = {}
local _CppService

local function _CloneVec3(pos)
	if pos == nil then
		return nil
	end
	return Vector3(pos.x, pos.y, pos.z)
end

local function _HasCppService()
	return _CppService() ~= nil
end

_CppService = function()
	if SandboxTeam ~= nil
		and SandboxTeam.rememberTeamEnemyFact ~= nil
		and SandboxTeam.writeBestTeamEnemyFactToBlackboard ~= nil then
		return SandboxTeam
	end
	return nil
end

local function _RememberCppEnemyFact(sighting)
	local service = _CppService()
	if service == nil or sighting == nil or sighting.targetPos == nil then
		return false
	end
	return service:rememberTeamEnemyFact(
		tonumber(sighting.teamId) or 0,
		tonumber(sighting.spotterId) or -1,
		tonumber(sighting.targetId) or -1,
		sighting.targetPos,
		tonumber(sighting.lastSeenMs) or 0,
		tonumber(sighting.confidence) or 1.0)
end

local function _RememberCppTeamFact(factType, teamId, sourceAgentId, targetAgentId, position, timeMs, confidence, priority, ttlMs, key)
	local service = _CppService()
	if service == nil or service.rememberTeamFact == nil or factType == nil then
		return false
	end
	return service:rememberTeamFact(
		tostring(factType),
		tonumber(teamId) or 0,
		tonumber(sourceAgentId) or -1,
		tonumber(targetAgentId) or -1,
		position or Vector3(0, 0, 0),
		tonumber(timeMs) or 0,
		tonumber(confidence) or 1.0,
		tonumber(priority) or 0,
		tonumber(ttlMs) or 0,
		key ~= nil and tostring(key) or "")
end

local function _NewTeamState(teamId)
	return {
		teamId = tonumber(teamId) or 0,
		visibleEnemies = {},
		supportResponses = {},
		supportRequest = nil,
		focusTarget = nil,
		retreatPoints = {},
		lastEvent = nil,
	}
end

local function _IsRetreatKey(key)
	return type(key) == "string" and string.sub(key, 1, 8) == "retreat:"
end

local function _GetTypedLegacyValue(team, key)
	if key == "backupRequest" then
		return team.supportRequest
	end
	if key == "focusTarget" then
		return team.focusTarget
	end
	if _IsRetreatKey(key) then
		return team.retreatPoints[key]
	end
	return nil
end

function TeamBlackboard:Reset()
	_teams = {}
	local service = _CppService()
	if service ~= nil and service.clearTeamBlackboardFacts ~= nil then
		service:clearTeamBlackboardFacts()
	end
end

function TeamBlackboard:GetTeam(teamId)
	teamId = tonumber(teamId) or 0
	if _teams[teamId] == nil then
		_teams[teamId] = _NewTeamState(teamId)
	end
	return _teams[teamId]
end

function TeamBlackboard:RememberVisibleEnemy(teamId, sighting)
	if sighting == nil or sighting.targetId == nil then
		return nil, false
	end

	local team = self:GetTeam(teamId)
	local existing = team.visibleEnemies[sighting.targetId]
	local stored = {
		eventType = TeamBlackboard.EventTypes.EnemySighted,
		teamId = tonumber(sighting.teamId) or tonumber(teamId) or 0,
		spotterId = sighting.spotterId,
		targetId = sighting.targetId,
		targetPos = _CloneVec3(sighting.targetPos),
		lastSeenMs = tonumber(sighting.lastSeenMs) or 0,
		confidence = tonumber(sighting.confidence) or 1.0,
	}
	team.visibleEnemies[sighting.targetId] = stored
	_RememberCppEnemyFact(stored)
	self:RememberFocusTarget(stored.teamId, {
		sourceAgentId = stored.spotterId,
		targetId = stored.targetId,
		targetPos = stored.targetPos,
		timeMs = stored.lastSeenMs,
		confidence = stored.confidence,
		key = "visibleEnemy:" .. tostring(stored.targetId),
	})
	return stored, existing == nil
end

function TeamBlackboard:BuildEnemySightedEvent(sighting, timeMs)
	if sighting == nil then
		return nil
	end
	local payload = AIEvents.Normalize(AIEvents.EventTypes.EnemySighted, sighting, {
		scope = AIEvents.Scope.Team,
		timeMs = timeMs,
	})
	if payload.position == nil then
		return nil
	end
	return payload
end

function TeamBlackboard:ForgetVisibleEnemy(teamId, targetId)
	local team = self:GetTeam(teamId)
	team.visibleEnemies[targetId] = nil
end

function TeamBlackboard:SetLastEvent(teamId, event)
	local team = self:GetTeam(teamId)
	team.lastEvent = event
end

function TeamBlackboard:GetVisibleEnemies(teamId)
	return self:GetTeam(teamId).visibleEnemies
end

function TeamBlackboard:ConfigureCppService(ttlMs)
	local service = _CppService()
	if service ~= nil and service.configureTeamBlackboard ~= nil then
		service:configureTeamBlackboard(tonumber(ttlMs) or 0)
		return true
	end
	return false
end

function TeamBlackboard:WriteBestCppEnemyToBlackboard(agent, keyPrefix, allowOwnReport)
	local service = _CppService()
	if service == nil or service.writeBestTeamEnemyFactToBlackboard == nil or agent == nil then
		return false
	end
	return service:writeBestTeamEnemyFactToBlackboard(agent, keyPrefix or "team.cpp", allowOwnReport == true)
end

function TeamBlackboard:GetCppFactCount()
	local service = _CppService()
	if service ~= nil and service.getTeamBlackboardFactCount ~= nil then
		return service:getTeamBlackboardFactCount()
	end
	return 0
end

function TeamBlackboard:GetCppReportCount()
	local service = _CppService()
	if service ~= nil and service.getTeamBlackboardReportCount ~= nil then
		return service:getTeamBlackboardReportCount()
	end
	return 0
end

function TeamBlackboard:BuildCppDebugSummary()
	local service = _CppService()
	if service ~= nil and service.buildTeamBlackboardDebugSummary ~= nil then
		return service:buildTeamBlackboardDebugSummary()
	end
	return "[TeamBlackboardService] unavailable"
end

function TeamBlackboard:GetCppEnemyFactCount()
	local service = _CppService()
	if service ~= nil and service.getTeamBlackboardEnemyFactCount ~= nil then
		return service:getTeamBlackboardEnemyFactCount()
	end
	return 0
end

function TeamBlackboard:GetCppTypedFactCount()
	local service = _CppService()
	if service ~= nil and service.getTeamBlackboardTypedFactCount ~= nil then
		return service:getTeamBlackboardTypedFactCount()
	end
	return 0
end

function TeamBlackboard:GetCppTypedReportCount()
	local service = _CppService()
	if service ~= nil and service.getTeamBlackboardTypedReportCount ~= nil then
		return service:getTeamBlackboardTypedReportCount()
	end
	return 0
end

function TeamBlackboard:WriteBestCppFactToBlackboard(agent, factType, keyPrefix, allowOwnReport)
	local service = _CppService()
	if service == nil or service.writeBestTeamFactToBlackboard == nil or agent == nil then
		return false
	end
	return service:writeBestTeamFactToBlackboard(agent, tostring(factType or ""), keyPrefix or "team.fact", allowOwnReport ~= false)
end

function TeamBlackboard:GetBestCppFact(teamId, factType, ignoredSourceAgentId)
	local service = _CppService()
	if service == nil or service.hasBestTeamFact == nil or service.getBestTeamFactPosition == nil then
		return nil
	end

	local numericTeamId = tonumber(teamId) or 0
	local factTypeText = tostring(factType or "")
	local ignored = tonumber(ignoredSourceAgentId) or -1
	if factTypeText == "" or not service:hasBestTeamFact(numericTeamId, factTypeText, ignored) then
		return nil
	end

	local position = service:getBestTeamFactPosition(numericTeamId, factTypeText, ignored)
	local sourceAgentId = service:getBestTeamFactSourceAgentId(numericTeamId, factTypeText, ignored)
	local targetId = service:getBestTeamFactTargetId(numericTeamId, factTypeText, ignored)
	return {
		eventType = factTypeText,
		factType = factTypeText,
		teamId = numericTeamId,
		sourceAgentId = sourceAgentId,
		spotterId = sourceAgentId,
		requesterId = sourceAgentId,
		responderId = sourceAgentId,
		agentId = sourceAgentId,
		targetId = targetId,
		targetAgentId = targetId,
		targetPos = _CloneVec3(position),
		position = _CloneVec3(position),
		key = service:getBestTeamFactKey(numericTeamId, factTypeText, ignored),
		confidence = service:getBestTeamFactConfidence(numericTeamId, factTypeText, ignored),
		reportCount = service:getBestTeamFactReportCount(numericTeamId, factTypeText, ignored),
		priority = service:getBestTeamFactPriority(numericTeamId, factTypeText, ignored),
		timeMs = service:getBestTeamFactTimeMs(numericTeamId, factTypeText, ignored),
		ageMs = service:getBestTeamFactAgeMs(numericTeamId, factTypeText, ignored),
		cpp = true,
	}
end

function TeamBlackboard:RunLifecycleSelfTest()
	local service = _CppService()
	if service == nil or service.clearTeamBlackboardFacts == nil or service.hasBestTeamFact == nil then
		print("[TeamBlackboardLifecycleSelfTest] FAIL serviceUnavailable=true")
		return false
	end

	local testTeamId = 9091
	self:Reset()
	local stored = self:RememberFocusTarget(testTeamId, {
		sourceAgentId = 101,
		targetId = 202,
		targetPos = Vector3(1, 0, 2),
		timeMs = 10,
		confidence = 0.8,
		key = "lifecycle:selftest",
	})
	local factBeforeReset = self:GetBestCppFact(testTeamId, TeamBlackboard.EventTypes.FocusTarget)
	local countBeforeReset = self:GetCppFactCount()
	self:Reset()
	local factAfterReset = self:GetBestCppFact(testTeamId, TeamBlackboard.EventTypes.FocusTarget)
	local countAfterReset = self:GetCppFactCount()
	local luaAfterReset = self:GetValue(testTeamId, "focusTarget", nil)
	local passed = stored ~= nil
		and factBeforeReset ~= nil
		and countBeforeReset > 0
		and factAfterReset == nil
		and countAfterReset == 0
		and luaAfterReset == nil
	print("[TeamBlackboardLifecycleSelfTest]", passed and "PASS" or "FAIL",
		"beforeFacts=", countBeforeReset,
		"afterFacts=", countAfterReset,
		"cppCleared=", factAfterReset == nil,
		"luaCleared=", luaAfterReset == nil)
	return passed
end

function TeamBlackboard:RememberSupportRequest(teamId, request)
	if request == nil or request.requesterId == nil then
		return nil, false
	end

	local team = self:GetTeam(teamId)
	local existing = team.supportRequest
	local stored = {
		eventType = TeamBlackboard.EventTypes.SupportRequested,
		teamId = tonumber(request.teamId) or tonumber(teamId) or 0,
		requesterId = request.requesterId,
		targetId = request.targetId,
		targetPos = _CloneVec3(request.targetPos),
		timeMs = tonumber(request.timeMs) or 0,
	}
	team.supportRequest = stored
	_RememberCppTeamFact(
		TeamBlackboard.EventTypes.SupportRequested,
		stored.teamId,
		stored.requesterId,
		stored.targetId,
		stored.targetPos,
		stored.timeMs,
		1.0,
		0,
		0,
		"backupRequest")
	return stored, existing == nil or existing.targetId ~= stored.targetId
end

function TeamBlackboard:RememberFocusTarget(teamId, focus)
	if focus == nil or focus.targetPos == nil then
		return nil, false
	end

	local team = self:GetTeam(teamId)
	local existing = team.focusTarget
	local stored = {
		eventType = TeamBlackboard.EventTypes.FocusTarget,
		teamId = tonumber(focus.teamId) or tonumber(teamId) or 0,
		sourceAgentId = tonumber(focus.sourceAgentId) or tonumber(focus.spotterId) or -1,
		targetId = tonumber(focus.targetId) or -1,
		targetPos = _CloneVec3(focus.targetPos),
		timeMs = tonumber(focus.timeMs) or 0,
		confidence = tonumber(focus.confidence) or 1.0,
	}
	team.focusTarget = stored
	_RememberCppTeamFact(
		TeamBlackboard.EventTypes.FocusTarget,
		stored.teamId,
		stored.sourceAgentId,
		stored.targetId,
		stored.targetPos,
		stored.timeMs,
		stored.confidence,
		tonumber(focus.priority) or 0,
		tonumber(focus.ttlMs) or 0,
		focus.key or "focusTarget")
	return stored, existing == nil or existing.targetId ~= stored.targetId
end

function TeamBlackboard:RememberRetreatPoint(teamId, retreat)
	if retreat == nil or retreat.retreatPos == nil then
		return nil, false
	end

	local team = self:GetTeam(teamId)
	local key = retreat.key ~= nil and tostring(retreat.key) or ("retreat:" .. tostring(retreat.agentId or retreat.sourceAgentId or "team"))
	local existing = team.retreatPoints[key]
	local stored = {
		eventType = TeamBlackboard.EventTypes.RetreatPoint,
		teamId = tonumber(retreat.teamId) or tonumber(teamId) or 0,
		agentId = tonumber(retreat.agentId) or tonumber(retreat.sourceAgentId) or -1,
		threatId = tonumber(retreat.threatId) or tonumber(retreat.targetId) or -1,
		threatPos = _CloneVec3(retreat.threatPos),
		retreatPos = _CloneVec3(retreat.retreatPos),
		timeMs = tonumber(retreat.timeMs) or 0,
		confidence = tonumber(retreat.confidence) or 1.0,
	}
	team.retreatPoints[key] = stored
	_RememberCppTeamFact(
		TeamBlackboard.EventTypes.RetreatPoint,
		stored.teamId,
		stored.agentId,
		stored.threatId,
		stored.retreatPos,
		stored.timeMs,
		stored.confidence,
		tonumber(retreat.priority) or 0,
		tonumber(retreat.ttlMs) or 0,
		key)
	return stored, existing == nil or existing.threatId ~= stored.threatId
end

function TeamBlackboard:RememberSupportResponse(teamId, response)
	if response == nil or response.responderId == nil then
		return nil, false
	end

	local team = self:GetTeam(teamId)
	local existing = team.supportResponses[response.responderId]
	local stored = {
		eventType = TeamBlackboard.EventTypes.SupportResponded,
		teamId = tonumber(response.teamId) or tonumber(teamId) or 0,
		responderId = response.responderId,
		fromAgentId = response.fromAgentId,
		targetId = response.targetId,
		targetPos = _CloneVec3(response.targetPos),
		supportPos = _CloneVec3(response.supportPos),
		timeMs = tonumber(response.timeMs) or 0,
	}
	team.supportResponses[response.responderId] = stored
	_RememberCppTeamFact(
		TeamBlackboard.EventTypes.SupportResponded,
		stored.teamId,
		stored.responderId,
		stored.targetId,
		stored.supportPos or stored.targetPos,
		stored.timeMs,
		1.0,
		0,
		0,
		"responder:" .. tostring(stored.responderId))
	return stored, existing == nil or existing.targetId ~= stored.targetId
end

function TeamBlackboard:RememberFormationSlot(teamId, slot)
	if slot == nil or slot.agentId == nil or slot.slotPos == nil then
		return false
	end
	return _RememberCppTeamFact(
		TeamBlackboard.EventTypes.FormationSlot,
		tonumber(slot.teamId) or tonumber(teamId) or 0,
		tonumber(slot.agentId) or -1,
		tonumber(slot.focusTargetId) or -1,
		slot.slotPos,
		tonumber(slot.timeMs) or 0,
		tonumber(slot.confidence) or 1.0,
		tonumber(slot.priority) or 0,
		tonumber(slot.ttlMs) or 0,
		"slot:" .. tostring(slot.agentId))
end

function TeamBlackboard:GetSupportResponses(teamId)
	return self:GetTeam(teamId).supportResponses
end

function TeamBlackboard:PruneVisibleEnemies(nowMs, ttlMs)
	nowMs = tonumber(nowMs) or 0
	ttlMs = tonumber(ttlMs) or 0
	local activeCount = 0
	for _, team in pairs(_teams) do
		for targetId, sighting in pairs(team.visibleEnemies) do
			if (nowMs - (tonumber(sighting.lastSeenMs) or 0)) > ttlMs then
				team.visibleEnemies[targetId] = nil
			else
				activeCount = activeCount + 1
			end
		end
	end
	return activeCount
end

function TeamBlackboard:PruneSupportResponses(nowMs, ttlMs)
	nowMs = tonumber(nowMs) or 0
	ttlMs = tonumber(ttlMs) or 0
	local activeCount = 0
	for _, team in pairs(_teams) do
		for responderId, response in pairs(team.supportResponses) do
			if (nowMs - (tonumber(response.timeMs) or 0)) > ttlMs then
				team.supportResponses[responderId] = nil
			else
				activeCount = activeCount + 1
			end
		end
	end
	return activeCount
end

function TeamBlackboard:SetValue(teamId, key, value)
	if key == nil then
		return false
	end
	if key == "backupRequest" and type(value) == "table" then
		self:RememberSupportRequest(teamId, value)
		return true
	end
	if key == "focusTarget" and type(value) == "table" then
		self:RememberFocusTarget(teamId, value)
		return true
	end
	if _IsRetreatKey(key) and type(value) == "table" then
		if value.key == nil then
			value.key = key
		end
		self:RememberRetreatPoint(teamId, value)
		return true
	end
	return false
end

function TeamBlackboard:GetValue(teamId, key, defaultValue)
	if key == nil then
		return defaultValue
	end
	local value = _GetTypedLegacyValue(self:GetTeam(teamId), key)
	if value == nil then
		return defaultValue
	end
	return value
end

_G.TeamBlackboard = TeamBlackboard
return TeamBlackboard
