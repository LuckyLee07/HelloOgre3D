-- TeamBlackboard.lua
-- Small shared-memory service for team-level AI samples.

local TeamBlackboard = {}

local _teams = {}

local function _CloneVec3(pos)
	if pos == nil then
		return nil
	end
	return Vector3(pos.x, pos.y, pos.z)
end

local function _NewTeamState(teamId)
	return {
		teamId = tonumber(teamId) or 0,
		visibleEnemies = {},
		lastEvent = nil,
		values = {},
	}
end

function TeamBlackboard:Reset()
	_teams = {}
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
		teamId = tonumber(sighting.teamId) or tonumber(teamId) or 0,
		spotterId = sighting.spotterId,
		targetId = sighting.targetId,
		targetPos = _CloneVec3(sighting.targetPos),
		lastSeenMs = tonumber(sighting.lastSeenMs) or 0,
	}
	team.visibleEnemies[sighting.targetId] = stored
	return stored, existing == nil
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
