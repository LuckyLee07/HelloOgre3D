-- TeamBlackboard.lua
-- Small shared-memory service for team-level AI samples.

local TeamBlackboard = {}

TeamBlackboard.EventTypes = {
	EnemySighted = "EnemySighted",
	SupportResponded = "SupportResponded",
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

local function _NewTeamState(teamId)
	return {
		teamId = tonumber(teamId) or 0,
		visibleEnemies = {},
		supportResponses = {},
		lastEvent = nil,
		values = {},
	}
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
	return stored, existing == nil
end

function TeamBlackboard:BuildEnemySightedEvent(sighting, timeMs)
	if sighting == nil then
		return nil
	end
	return {
		eventType = TeamBlackboard.EventTypes.EnemySighted,
		teamId = sighting.teamId,
		senderId = sighting.spotterId,
		targetId = sighting.targetId,
		targetPos = _CloneVec3(sighting.targetPos),
		timeMs = tonumber(timeMs) or 0,
	}
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
	return stored, existing == nil or existing.targetId ~= stored.targetId
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
		return
	end
	self:GetTeam(teamId).values[key] = value
end

function TeamBlackboard:GetValue(teamId, key, defaultValue)
	if key == nil then
		return defaultValue
	end
	local value = self:GetTeam(teamId).values[key]
	if value == nil then
		return defaultValue
	end
	return value
end

_G.TeamBlackboard = TeamBlackboard
return TeamBlackboard
