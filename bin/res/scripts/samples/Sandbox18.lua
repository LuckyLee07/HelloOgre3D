-- Sandbox18.lua
-- Chapter 9 tactics sample, C++ influence map system version.

require("res.scripts.agent.SoldierAgent.lua")
require("res.scripts.agent.IndirectSoldierAgent.lua")
require("res.scripts.agent.DecisionSoldierAgent.lua")
require("res.scripts.agent.AgentUtils.lua")

local _sampleName = "Sandbox18"
local _agents = {}
local _navMesh = nil
local _panel = nil
local _drawNavMesh = false
local _drawTactics = true
local _elapsedMs = 0
local _panelElapsedMs = 0

local _colors = {
	team1 = ColourValue(0.2, 0.95, 0.4),
	team0 = ColourValue(0.95, 0.25, 0.2),
	danger = ColourValue(1.0, 0.16, 0.08),
	bullet = ColourValue(1.0, 0.75, 0.15),
	dead = ColourValue(0.85, 0.85, 0.85),
	best = ColourValue(0.15, 0.9, 1.0),
	objective = ColourValue(0.95, 0.9, 0.18),
	grid = ColourValue(0.0, 0.0, 0.0, 0.6),
}

local _influencePalette = {
	positive = { 0.0, 0.0, 1.0, 0.9 },
	zero = { 0.0, 0.0, 0.0, 0.75 },
	negative = { 1.0, 0.0, 0.0, 0.9 },
	objective = { 0.95, 0.9, 0.18, 0.85 },
}

local _tactics = {
	bulletImpacts = {},
	bulletShots = {},
	deadFriendlies = {},
	seenEnemies = {},
	dangerElapsedMs = 0,
	teamElapsedMs = 0,
	dangerRunCount = 0,
	dangerSkipCount = 0,
	teamRunCount = 0,
	teamSkipCount = 0,
	eventCount = 0,
	dangerCells = 0,
	teamCells = 0,
	objectiveCells = 0,
	lastCellWrites = 0,
	cppEventCount = 0,
	queryCount = 0,
	bestPosition = nil,
	bestScore = 0.0,
	lastBurstAtMs = -1,
	deadFriendlyPublished = false,
	smokePrinted = false,
	smokeComplete = false,
}

local _tacticalDriver = {
	agent = nil,
	teamId = -1,
	lastTarget = nil,
	acc = Vector3(0.0),
	moveCount = 0,
}

local textSize = { w = 410, h = 230 }
local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
	"[Sandbox18 - Chapter 9 Tactics C++]" .. GUI.MarkupNewline ..
	"C++ InfluenceMapSystem + TacticalQuery" .. GUI.MarkupNewline ..
	"Lua feeds events, C++ scores positions" .. GUI.MarkupNewline ..
	GUI.MarkupNewline ..
	"F1: reset camera" .. GUI.MarkupNewline ..
	"F3: toggle navigation mesh" .. GUI.MarkupNewline ..
	"F4: toggle influence map" .. GUI.MarkupNewline ..
	"F5/F6/F7: profile/camera/physics"

local function _GetPreset()
	if ConfigManager ~= nil and ConfigManager.GetSamplePreset ~= nil then
		return ConfigManager:GetSamplePreset(_sampleName)
	end
	return {}
end

local function _GetConfig()
	local preset = _GetPreset()
	return preset.chapter9Tactics or {}
end

local function _ReadNumber(config, key, defaultValue)
	local value = config ~= nil and tonumber(config[key]) or nil
	if value == nil then
		return defaultValue
	end
	return value
end

local function _ReadBool(config, key, defaultValue)
	local value = config ~= nil and config[key] or nil
	if value == nil then
		return defaultValue
	end
	return value == true or value == "1" or value == "true" or value == "TRUE" or value == "yes"
end

local function _ReadString(config, key, defaultValue)
	local value = config ~= nil and config[key] or nil
	if value == nil then
		return defaultValue
	end
	return tostring(value)
end

local function _Clamp01(value)
	value = tonumber(value) or 0.0
	if value < 0.0 then return 0.0 end
	if value > 1.0 then return 1.0 end
	return value
end

local function _Lerp(startValue, endValue, t)
	return startValue + (endValue - startValue) * t
end

local function _InfluenceColor(value, positiveSpec, negativeSpec)
	local numberValue = tonumber(value) or 0.0
	local amount = _Clamp01(math.abs(numberValue))
	local target = numberValue >= 0.0 and positiveSpec or negativeSpec
	local zero = _influencePalette.zero
	return ColourValue(
		_Lerp(zero[1], target[1], amount),
		_Lerp(zero[2], target[2], amount),
		_Lerp(zero[3], target[3], amount),
		_Lerp(zero[4], target[4], amount))
end

local function _GetAgentId(agent)
	if agent == nil then return -1 end
	if agent.GetObjId ~= nil then return agent:GetObjId() end
	if agent.GetId ~= nil then return agent:GetId() end
	return -1
end

local function _IsTacticDead(agent)
	return agent ~= nil and _tactics.deadFriendlies[_GetAgentId(agent)] ~= nil
end

local function _IsAlive(agent)
	return agent ~= nil and agent:GetHealth() > 0 and not _IsTacticDead(agent)
end

local function _ProjectToNav(pos)
	if pos == nil then
		return nil
	end
	local navPos = Sandbox:FindClosestPoint("default", pos)
	if navPos ~= nil then
		return navPos
	end
	return pos
end

local function _ResolveInfluenceDrawPosition(pos, yOffset, cellSize)
	if pos == nil then
		return nil
	end
	local config = _GetConfig()
	if _ReadBool(config, "projectInfluenceToNav", true) then
		local navPos = Sandbox:FindClosestPoint("default", pos)
		if navPos == nil then
			return nil
		end
		local dx = navPos.x - pos.x
		local dz = navPos.z - pos.z
		local maxDistance = _ReadNumber(config, "drawNavProjectionMaxDistance", cellSize * 0.9)
		if maxDistance > 0.0 and dx * dx + dz * dz > maxDistance * maxDistance then
			return nil
		end
		return Vector3(navPos.x, navPos.y + yOffset, navPos.z)
	end
	return Vector3(pos.x, yOffset, pos.z)
end

local function _HasCppTactics()
	return ObjectManager ~= nil
		and ObjectManager.configureTacticalInfluence ~= nil
		and ObjectManager.publishTacticalEvent ~= nil
		and ObjectManager.rebuildTacticalDangerLayer ~= nil
		and ObjectManager.findBestTacticalQueryPosition ~= nil
end

local function _ShouldRunObjectiveAndQuery(config)
	return _G.HELLO_SANDBOX_SMOKE_MODE == true
		or _ReadBool(config, "objectiveEnabled", false) == true
		or _ReadBool(config, "tacticalQueryEnabled", false) == true
end

local function _ShouldRunTacticalAgent(config)
	return _G.HELLO_SANDBOX_SMOKE_MODE == true
		or _ReadBool(config, "tacticalAgentEnabled", false) == true
end

local function _GetTacticalDriverTeamId(config)
	return _ReadNumber(config, "tacticalDriverTeamId", _ReadNumber(config, "teamPositiveTeamId", 0))
end

local function _ConfigureCppTactics()
	if not _HasCppTactics() then
		print("[Sandbox18] C++ tactical influence API unavailable")
		return
	end

	local config = _GetConfig()
	local mapConfig = config.influenceMap or {}
	ObjectManager:clearTacticalInfluence()
	ObjectManager:clearTacticalEvents()
	ObjectManager:configureTacticalEvents(_ReadNumber(config, "eventTtlMs", 1800))
	ObjectManager:configureTacticalInfluence(
		_ReadNumber(mapConfig, "minX", -32.0),
		_ReadNumber(mapConfig, "maxX", 56.0),
		_ReadNumber(mapConfig, "minZ", -8.0),
		_ReadNumber(mapConfig, "maxZ", 62.0),
		_ReadNumber(mapConfig, "cellSize", 4.0))
	local falloff = _ReadNumber(config, "influenceFalloff", 0.2)
	local inertia = _ReadNumber(config, "influenceInertia", 0.5)
	ObjectManager:setTacticalInfluenceLayerOptions("danger", falloff, inertia)
	ObjectManager:setTacticalInfluenceLayerOptions("team", falloff, inertia)
	ObjectManager:setTacticalInfluenceLayerOptions("objective", falloff, inertia)
end

local function _CreatePanel()
	_panel = Sandbox:CreateUIFrame()
	_panel:setPosition(Vector2(20, 280))
	_panel:setDimension(Vector2(640, 160))
	_panel:setTextMargin(12, 10)
	_panel:setGradientColor(Gorilla.Gradient_NorthSouth, ColourValue(0.05, 0.07, 0.09, 0.78), ColourValue(0.0, 0.0, 0.0, 0.82))
end

local function _RefreshPanel()
	if _panel == nil then
		return
	end

	local summary = ""
	if ObjectManager ~= nil and ObjectManager.buildTacticalInfluenceDebugSummary ~= nil then
		summary = ObjectManager:buildTacticalInfluenceDebugSummary()
	end
	local text = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
		"Chapter9 C++ Tactics: events=" .. tostring(_tactics.eventCount) ..
		" cppEvents=" .. tostring(_tactics.cppEventCount) ..
		" writes=" .. tostring(_tactics.lastCellWrites) ..
		" queries=" .. tostring(_tactics.queryCount) .. GUI.MarkupNewline ..
		"Danger: cells=" .. tostring(_tactics.dangerCells) ..
		" updates=" .. tostring(_tactics.dangerRunCount) ..
		" skips=" .. tostring(_tactics.dangerSkipCount) .. GUI.MarkupNewline ..
		"Team: cells=" .. tostring(_tactics.teamCells) ..
		" objective=" .. tostring(_tactics.objectiveCells) ..
		" updates=" .. tostring(_tactics.teamRunCount) .. GUI.MarkupNewline ..
		"Best: " .. tostring(_tactics.bestPosition) ..
		" score=" .. string.format("%.2f", tonumber(_tactics.bestScore) or 0.0) .. GUI.MarkupNewline ..
		summary
	_panel:setMarkupText(text)
end

local function _PublishTacticEvent(eventType, event)
	event = event or {}
	event.type = eventType
	_tactics.eventCount = _tactics.eventCount + 1

	local config = _GetConfig()
	local ttlMs = _ReadNumber(config, "eventTtlMs", 1500)
	if eventType == "BulletImpact" and event.position ~= nil then
		table.insert(_tactics.bulletImpacts, { position = event.position, ttl = ttlMs })
	elseif eventType == "BulletShot" and event.position ~= nil then
		table.insert(_tactics.bulletShots, { position = event.position, ttl = ttlMs })
	elseif eventType == "DeadFriendlySighted" and event.agent ~= nil then
		_tactics.deadFriendlies[_GetAgentId(event.agent)] = event
	elseif eventType == "EnemySighted" and event.agent ~= nil then
		_tactics.seenEnemies[_GetAgentId(event.agent)] = event
	end

	if _HasCppTactics() then
		local position = event.position or event.seenAt
		if position == nil and event.agent ~= nil then
			position = event.agent:GetPosition()
		end
		if position ~= nil then
			local sender = event.sender or event.spotter or event.reporter
			if sender == nil and eventType ~= "EnemySighted" and eventType ~= "DeadFriendlySighted" then
				sender = event.agent
			end
			local senderId = _GetAgentId(sender)
			local targetId = event.agent ~= nil and _GetAgentId(event.agent) or -1
			local teamId = sender ~= nil and sender:GetTeamId() or -1
			local targetTeamId = event.agent ~= nil and event.agent:GetTeamId() or -1
			if eventType == "DeadFriendlySighted" and event.agent ~= nil then
				teamId = event.agent:GetTeamId()
			end
			ObjectManager:publishTacticalEvent(eventType, senderId, targetId, teamId, targetTeamId, _ProjectToNav(position), math.floor(_elapsedMs), "global", false)
			_tactics.cppEventCount = ObjectManager:getTacticalEventCount()
		end
	end
end

local function _PruneTimedEvents(events, deltaTimeInMillis)
	local validEvents = {}
	for _, event in ipairs(events) do
		event.ttl = event.ttl - deltaTimeInMillis
		if event.ttl > 0 then
			table.insert(validEvents, event)
		end
	end
	return validEvents
end

local function _FindAgentByTeam(teamId, skipAgent)
	for _, agent in ipairs(_agents) do
		if agent ~= skipAgent and _IsAlive(agent) and agent:GetTeamId() == teamId then
			return agent
		end
	end
	return nil
end

local function _PublishScriptedBurst(force)
	local config = _GetConfig()
	local scriptedEnabled = _ReadBool(config, "scriptedEvents", false)
	if _G.HELLO_SANDBOX_SMOKE_MODE == true then
		scriptedEnabled = scriptedEnabled or _ReadBool(config, "scriptedEventsInSmoke", true)
	end
	if scriptedEnabled ~= true then
		return
	end
	if _G.HELLO_SANDBOX_SMOKE_MODE ~= true and _HasCppTactics() and ObjectManager:getTacticalEventCount() > 0 then
		return
	end

	local intervalMs = _ReadNumber(config, "scriptedEventIntervalMs", 1800)
	if not force and _elapsedMs - _tactics.lastBurstAtMs < intervalMs then
		return
	end
	_tactics.lastBurstAtMs = _elapsedMs

	local shooterTeam = _ReadNumber(config, "dangerPerspectiveTeamId", 0)
	local enemyTeam = shooterTeam == 0 and 1 or 0
	local shooter = _FindAgentByTeam(shooterTeam, nil) or _agents[1]
	local target = _FindAgentByTeam(enemyTeam, shooter) or _agents[2]
	if shooter == nil or target == nil then
		return
	end

	local shotPos = shooter:GetPosition()
	local targetPos = target:GetPosition()
	local impactOffset = Vector3(math.sin(_tactics.eventCount + 1) * 2.5, 0, math.cos(_tactics.eventCount + 1) * 2.5)
	local impactPos = _ProjectToNav(targetPos + impactOffset)

	_PublishTacticEvent("BulletShot", { position = shotPos, agent = shooter, sender = shooter })
	_PublishTacticEvent("BulletImpact", { position = impactPos, agent = shooter, sender = shooter })
	_PublishTacticEvent("EnemySighted", { agent = target, sender = shooter, seenAt = targetPos, lastSeen = _elapsedMs })

	local deadDelayMs = _ReadNumber(config, "deadFriendlyDelayMs", 2600)
	if not _tactics.deadFriendlyPublished and _elapsedMs >= deadDelayMs then
		local deadFriendly = _FindAgentByTeam(shooterTeam, shooter) or shooter
		if deadFriendly ~= nil then
			_PublishTacticEvent("DeadFriendlySighted", { agent = deadFriendly, sender = shooter, seenAt = deadFriendly:GetPosition(), lastSeen = _elapsedMs })
			_tactics.deadFriendlyPublished = true
		end
	end
end

local function _AddSource(layerName, position, strength, radius)
	if position == nil or not _HasCppTactics() then
		return 0
	end
	return ObjectManager:addTacticalInfluenceSource(layerName, _ProjectToNav(position), strength, radius)
end

local function _UpdateDangerousAreas(deltaTimeInMillis)
	if not _HasCppTactics() then
		return
	end

	local config = _GetConfig()
	local intervalMs = _ReadNumber(config, "dangerUpdateIntervalMs", 500)
	_tactics.dangerElapsedMs = _tactics.dangerElapsedMs + deltaTimeInMillis
	if _tactics.dangerElapsedMs < intervalMs then
		_tactics.dangerSkipCount = _tactics.dangerSkipCount + 1
		return
	end
	_tactics.dangerElapsedMs = 0
	_tactics.dangerRunCount = _tactics.dangerRunCount + 1

	_tactics.bulletImpacts = _PruneTimedEvents(_tactics.bulletImpacts, intervalMs)
	_tactics.bulletShots = _PruneTimedEvents(_tactics.bulletShots, intervalMs)

	local perspectiveTeamId = _ReadNumber(config, "dangerPerspectiveTeamId", 0)
	local dangerStrength = _ReadNumber(config, "dangerStrength", -1.0)
	local shotRadius = _ReadNumber(config, "bulletShotRadius", 10.0)
	local impactRadius = _ReadNumber(config, "bulletImpactRadius", 8.0)
	local corpseRadius = _ReadNumber(config, "deadFriendlyRadius", 12.0)
	local sightingRadius = _ReadNumber(config, "enemySightingRadius", 14.0)
	local writes = ObjectManager:rebuildTacticalDangerLayer(
		perspectiveTeamId,
		dangerStrength,
		shotRadius,
		impactRadius,
		corpseRadius,
		sightingRadius,
		_ReadNumber(config, "influenceSpreadPasses", 2))

	_tactics.dangerCells = ObjectManager:getTacticalInfluenceLayerActiveCellCount("danger")
	_tactics.lastCellWrites = writes
	_tactics.cppEventCount = ObjectManager:getTacticalEventCount()
end

local function _UpdateTeamAreas(deltaTimeInMillis)
	if not _HasCppTactics() then
		return
	end

	local config = _GetConfig()
	local intervalMs = _ReadNumber(config, "teamUpdateIntervalMs", 500)
	_tactics.teamElapsedMs = _tactics.teamElapsedMs + deltaTimeInMillis
	if _tactics.teamElapsedMs < intervalMs then
		_tactics.teamSkipCount = _tactics.teamSkipCount + 1
		return
	end
	_tactics.teamElapsedMs = 0
	_tactics.teamRunCount = _tactics.teamRunCount + 1

	local positiveTeamId = _tacticalDriver.teamId >= 0 and _tacticalDriver.teamId or _GetTacticalDriverTeamId(config)
	local teamRadius = _ReadNumber(config, "teamInfluenceRadius", 11.0)
	local teamStrength = _ReadNumber(config, "teamStrength", 1.0)
	local writes = ObjectManager:rebuildTacticalTeamLayer(
		positiveTeamId,
		teamStrength,
		teamRadius,
		_ReadNumber(config, "influenceSpreadPasses", 2))

	_tactics.teamCells = ObjectManager:getTacticalInfluenceLayerActiveCellCount("team")
	_tactics.lastCellWrites = _tactics.lastCellWrites + writes
end

local function _FindObjectiveCenter()
	if _HasCppTactics() then
		local fallback = _agents[1] ~= nil and _agents[1]:GetPosition() or Vector3(0, 0, 0)
		return ObjectManager:getLastTacticalEventPosition("EnemySighted", fallback)
	end
	for _, event in pairs(_tactics.seenEnemies) do
		if event.seenAt ~= nil then
			return event.seenAt
		end
		if event.agent ~= nil then
			return event.agent:GetPosition()
		end
	end
	return _agents[1] ~= nil and _agents[1]:GetPosition() or Vector3(0, 0, 0)
end

local function _UpdateObjectiveAndQuery()
	if not _HasCppTactics() then
		return
	end

	local config = _GetConfig()
	if not _ShouldRunObjectiveAndQuery(config) then
		return
	end
	local center = _ProjectToNav(_FindObjectiveCenter())
	_tactics.lastCellWrites = _tactics.lastCellWrites + ObjectManager:rebuildTacticalObjectiveLayer(
		center,
		_ReadNumber(config, "objectiveStrength", 1.0),
		_ReadNumber(config, "objectiveRadius", 16.0),
		_ReadNumber(config, "influenceSpreadPasses", 2))
	_tactics.objectiveCells = ObjectManager:getTacticalInfluenceLayerActiveCellCount("objective")

	_tactics.bestPosition = ObjectManager:findBestTacticalQueryPosition(
		_ReadString(config, "tacticalQueryType", "support"),
		center,
		_ReadNumber(config, "tacticalQueryRadius", 24.0),
		_ReadNumber(config, "tacticalQueryStep", 4.0))
	_tactics.bestScore = ObjectManager:scoreTacticalQueryPosition(
		_ReadString(config, "tacticalQueryType", "support"),
		_tactics.bestPosition)
	_tactics.queryCount = ObjectManager:getTacticalInfluenceQueryCount()
end

local function _DrawTacticEvents()
	if not _drawTactics then
		return
	end
	local config = _GetConfig()
	if _ReadBool(config, "drawEventMarkers", false) then
		for _, event in ipairs(_tactics.bulletShots) do
			DebugDrawer:drawCircle(event.position + Vector3(0, 0.24, 0), 1.2, 24, _colors.bullet, false)
		end
		for _, event in ipairs(_tactics.bulletImpacts) do
			DebugDrawer:drawCircle(event.position + Vector3(0, 0.28, 0), 2.0, 32, _colors.danger, false)
		end
		for _, event in pairs(_tactics.deadFriendlies) do
			if event.agent ~= nil then
				local pos = event.agent:GetPosition()
				DebugDrawer:drawCircle(pos + Vector3(0, 0.28, 0), 1.6, 32, _colors.dead, false)
				DebugDrawer:drawLine(pos + Vector3(-1.2, 0.45, 0), pos + Vector3(1.2, 0.45, 0), _colors.dead)
			end
		end
	end
	if _ReadBool(config, "drawBestPosition", false) and _tactics.bestPosition ~= nil then
		local pos = _tactics.bestPosition
		DebugDrawer:drawCircle(pos + Vector3(0, 0.34, 0), 2.2, 36, _colors.best, false)
		DebugDrawer:drawLine(pos + Vector3(-1.8, 0.55, 0), pos + Vector3(1.8, 0.55, 0), _colors.best)
		DebugDrawer:drawLine(pos + Vector3(0, 0.55, -1.8), pos + Vector3(0, 0.55, 1.8), _colors.best)
	end
end

local function _DrawAgents()
	local config = _GetConfig()
	if _ReadBool(config, "drawAgentMarkers", false) ~= true then
		return
	end
	for _, agent in ipairs(_agents) do
		local pos = agent:GetPosition()
		local color = agent:GetTeamId() == 1 and _colors.team1 or _colors.team0
		DebugDrawer:drawCircle(pos + Vector3(0, 0.16, 0), 1.2, 24, color, false)
		DebugDrawer:drawLine(pos + Vector3(0, 0.2, 0), pos + Vector3(0, 3.0, 0), color)
	end
end

local function _DrawCell(center, color, y, cellSize)
	if center == nil then
		return
	end
	local p = _ResolveInfluenceDrawPosition(center, y, cellSize)
	if p == nil then
		return
	end
	DebugDrawer:drawSquare(p, cellSize, color, true)
	DebugDrawer:drawSquare(p + Vector3(0, 0.01, 0), cellSize, _colors.grid, false)
end

local function _GetDrawCellLimit(config)
	local limit = tonumber(config ~= nil and config.maxDrawCellsPerLayer or nil)
	if limit == nil or limit <= 0 then
		return 100000
	end
	return math.max(1, limit)
end

local function _DrawCppInfluenceLayer(layerName, y, positiveSpec, negativeSpec, drawNeutralDefault)
	if not _drawTactics or not _HasCppTactics() then
		return
	end
	local config = _GetConfig()
	if _G.HELLO_SANDBOX_SMOKE_MODE == true and _ReadBool(config, "drawInSmoke", false) ~= true then
		return
	end
	local mapConfig = config.influenceMap or {}
	local cellSize = _ReadNumber(mapConfig, "cellSize", 4.0)
	local threshold = _ReadNumber(config, "drawThreshold", 0.08)
	local drawNeutral = _ReadBool(config, layerName .. "DrawNeutralCells", drawNeutralDefault)
	local queryThreshold = drawNeutral and 0.0 or threshold
	local maxCells = _GetDrawCellLimit(config)
	local count = ObjectManager:getTacticalInfluenceLayerDebugCellCount(layerName, queryThreshold, maxCells)
	for index = 1, count do
		local pos = ObjectManager:getTacticalInfluenceLayerDebugCellPosition(layerName, index, queryThreshold)
		local value = ObjectManager:getTacticalInfluenceLayerDebugCellValue(layerName, index, queryThreshold)
		_DrawCell(pos, _InfluenceColor(value, positiveSpec, negativeSpec), y, cellSize)
	end
end

local function _DrawCppInfluenceMap()
	local config = _GetConfig()
	if _ReadBool(config, "drawDangerLayer", false) then
		_DrawCppInfluenceLayer("danger", 0.12, _influencePalette.positive, _influencePalette.negative, false)
	end
	if _ReadBool(config, "drawTeamLayer", true) then
		_DrawCppInfluenceLayer("team", 0.22, _influencePalette.positive, _influencePalette.negative, true)
	end
	if _ReadBool(config, "drawObjectiveLayer", false) then
		_DrawCppInfluenceLayer("objective", 0.32, _influencePalette.objective, _influencePalette.negative, false)
	end
end

local function _BuildAndSetTacticalPath(agent, target)
	local path = std.vector_Ogre__Vector3_()
	local ok = Sandbox:FindPath("default", agent:GetPosition(), target, path)
	if ok and path:size() > 0 then
		Agent_SetPath(agent, path, false)
		agent:SetTarget(target)
		agent:SetTargetRadius(1.0)
		return true
	end
	agent:SetTarget(target)
	agent:SetTargetRadius(1.0)
	return false
end

local function _UpdateTacticalAgent(deltaTimeInMillis)
	local config = _GetConfig()
	if not _ShouldRunTacticalAgent(config) then
		return
	end
	local agent = _tacticalDriver.agent
	local target = _ProjectToNav(_tactics.bestPosition)
	if agent == nil or target == nil or not _IsAlive(agent) then
		return
	end
	if _tacticalDriver.teamId >= 0 and agent:GetTeamId() ~= _tacticalDriver.teamId then
		return
	end

	agent:SetMaxSpeed(_ReadNumber(config, "tacticalAgentMaxSpeed", 2.4))
	local rebuildSq = _ReadNumber(config, "tacticalAgentRepathDistance", 1.0)
	rebuildSq = rebuildSq * rebuildSq
	if _tacticalDriver.lastTarget == nil or (target - _tacticalDriver.lastTarget):squaredLength() > rebuildSq then
		_BuildAndSetTacticalPath(agent, target)
		_tacticalDriver.lastTarget = target
		_tacticalDriver.moveCount = _tacticalDriver.moveCount + 1
	end

	local dtSec = math.max(0, tonumber(deltaTimeInMillis) or 0) / 1000.0
	if dtSec <= 0 then
		return
	end
	local force = agent:HasPath() and Soldier_CalculateSteering(agent, dtSec) or agent:ForceToPosition(target)
	AgentUtilities_ApplySteeringForce2(agent, force, _tacticalDriver.acc, dtSec)
	AgentUtilities_ClampHorizontalSpeed(agent)
end

local function _MaybePrintSmoke()
	if _tactics.smokePrinted or _G.HELLO_SANDBOX_SMOKE_MODE ~= true then
		return
	end
	if _tactics.dangerRunCount > 0
		and _tactics.teamRunCount > 0
		and _tactics.dangerCells > 0
		and _tactics.teamCells > 0
		and _tactics.queryCount > 0
		and _tacticalDriver.moveCount > 0
		and _tacticalDriver.agent ~= nil
		and _tacticalDriver.agent:GetTeamId() == _tacticalDriver.teamId
		and _tactics.eventCount >= 3 then
		_tactics.smokePrinted = true
		print("[Chapter9TacticsCppSmoke] PASS",
			"events=", _tactics.eventCount,
			"dangerCells=", _tactics.dangerCells,
			"teamCells=", _tactics.teamCells,
			"objectiveCells=", _tactics.objectiveCells,
			"writes=", _tactics.lastCellWrites,
			"queries=", _tactics.queryCount,
			"moves=", _tacticalDriver.moveCount,
			"driverTeam=", _tacticalDriver.teamId,
			"bestScore=", string.format("%.2f", _tactics.bestScore),
			"summary=", ObjectManager:buildTacticalInfluenceDebugSummary())
		_tactics.smokeComplete = true
		if _tacticalDriver.agent ~= nil then
			_tacticalDriver.agent:SetMaxSpeed(0)
			_tacticalDriver.agent:SetVelocity(Vector3(0, 0, 0))
			_tacticalDriver.agent:SetTarget(_tacticalDriver.agent:GetPosition())
		end
	end
end

local function _GetTeamIdForAgent(config, index)
	if _ReadBool(config, "alternateTeams", true) then
		return (index % 2 == 0) and 0 or 1
	end
	return ConfigManager:GetAgentTeamId(_sampleName, index)
end

local function _GetAppearanceForTeam(teamId)
	return teamId == 0 and Soldier.AppearanceTypes.DARK or Soldier.AppearanceTypes.LIGHT
end

local function _PlaceAgentLikeChapter9(agent, preset, config, index)
	if agent == nil then
		return nil
	end
	if preset.spawnMode ~= "random" then
		return ConfigManager:PlaceAgentOnPresetSpawn(agent, _sampleName, index, "default")
	end

	local minDistanceSq = _ReadNumber(config, "minSpawnDistanceSq", 725.0)
	local maxAttempts = math.max(1, _ReadNumber(config, "maxSpawnAttempts", 4096))
	local position = nil
	for attempt = 1, maxAttempts do
		position = Sandbox:RandomPoint("default")
		if position ~= nil then
			local found = true
			for _, other in ipairs(_agents) do
				local delta = position - other:GetPosition()
				delta.y = 0
				if delta:squaredLength() < minDistanceSq then
					found = false
					break
				end
			end
			if found then
				break
			end
		end
		position = nil
	end
	if position == nil then
		return ConfigManager:PlaceAgentOnPresetSpawn(agent, _sampleName, index, "default")
	end

	position.y = position.y + agent:GetHeight() * 0.5
	agent:setPosition(position)
	local navPosition = Sandbox:FindClosestPoint("default", agent:GetPosition()) or position
	agent:SetTarget(navPosition)
	agent:SetTargetRadius(preset.targetRadius or 1)
	return position
end

local function _CreateAgents()
	local preset = _GetPreset()
	local config = _GetConfig()
	local agentCount = ConfigManager:GetAgentCount(_sampleName, 6)
	local agentLuafile = _ReadString(config, "agentScript", "res/scripts/agent/DecisionSoldierAgent.lua")
	print(ConfigManager:BuildDebugSummary(_sampleName))

	for i = 1, agentCount do
		local teamId = _GetTeamIdForAgent(config, i)
		local agentType = _GetAppearanceForTeam(teamId)
		local agent = Create_Soldier(agentLuafile, agentType, teamId)
		_PlaceAgentLikeChapter9(agent, preset, config, i)
		table.insert(_agents, agent)
	end
	_tacticalDriver.teamId = _GetTacticalDriverTeamId(config)
	_tacticalDriver.agent = _FindAgentByTeam(_tacticalDriver.teamId, nil) or _agents[1]
	if _tacticalDriver.agent ~= nil and _tacticalDriver.agent:GetTeamId() ~= _tacticalDriver.teamId then
		_tacticalDriver.teamId = _tacticalDriver.agent:GetTeamId()
	end
	if _tacticalDriver.agent ~= nil and _ShouldRunTacticalAgent(config) then
		_tacticalDriver.agent:SetMaxSpeed(_ReadNumber(config, "tacticalAgentMaxSpeed", 2.4))
	end
end

function EventHandle_Keyboard(keycode, pressed)
	GUI_HandleKeyEvent(keycode, pressed)

	if not pressed then return end
	if keycode == OIS.KC_F1 then
		local camera = Sandbox:GetCamera()
		camera:setPosition(Vector3(-30, 18, -17))
		camera:setOrientation(Quaternion(-146, -40, -157))
	elseif keycode == OIS.KC_F3 then
		_drawNavMesh = not _drawNavMesh
		if _navMesh ~= nil then
			_navMesh:SetDebugVisible(_drawNavMesh)
		end
	elseif keycode == OIS.KC_F4 then
		_drawTactics = not _drawTactics
	end
end

function EventHandle_Mouse(ctype)
end

function EventHandle_WindowResized(width, height)
	GUI_WindowResized(width, height)
end

function Sandbox_Initialize()
	local config = _GetConfig()
	_G.HELLO_SUPPRESS_AI_PATH_DRAW = _ReadBool(config, "suppressPathDraw", true)

	GUI_CreateCameraAndProfileInfo()
	GUI_CreateSandboxText(infoText, textSize)
	if _ReadBool(config, "showTacticsPanel", false) then
		_CreatePanel()
	end

	Sandbox:SetUseCppFsmFlag(true)

	local camera = Sandbox:GetCamera()
	camera:setPosition(Vector3(-30, 18, -17))
	camera:setOrientation(Quaternion(-146, -40, -157))

	Sandbox:SetSkyBox("ThickCloudsWaterSkyBox", Vector3(0, 180, 0))

	local plane = Sandbox:CreatePlane(200, 200)
	plane:setPosition(Vector3(0, -10, 0))
	plane:setMaterial("Ground2")

	Sandbox:SetAmbientLight(Vector3(0.35))
	local directLight = Sandbox:CreateDirectionalLight(Vector3(1, -1, 1))
	directLight:setDiffuseColour(ColourValue(1.6, 1.4, 1.0))
	directLight:setSpecularColour(ColourValue(1.3, 1.2, 0.9))

	SandboxUtilities_CreateLevel()
	Sandbox:UpdateSceneGraph()

	local navMeshConfig = rcConfig()
	Sandbox:DefaultConfig(navMeshConfig)
	Sandbox:ApplySettingConfig(navMeshConfig, 0.0, 0.4, 0.2)
	navMeshConfig.minRegionArea = math.pow(250, 2)
	navMeshConfig.walkableSlopeAngle = 45

	_navMesh = Sandbox:CreateNavigationMesh(navMeshConfig, "default")
	if _navMesh ~= nil then _navMesh:SetDebugVisible(_drawNavMesh) end

	_CreateAgents()
	_ConfigureCppTactics()

	_tactics.dangerElapsedMs = _ReadNumber(config, "dangerUpdateIntervalMs", 500)
	_tactics.teamElapsedMs = _ReadNumber(config, "teamUpdateIntervalMs", 500)
	_PublishScriptedBurst(true)
	_RefreshPanel()
end

function Sandbox_Update(deltaTimeInMillis)
	GUI_UpdateCameraInfo()
	GUI_UpdateProfileInfo()

	if _G.HELLO_SANDBOX_SMOKE_MODE == true and _tactics.smokeComplete then
		return
	end

	_elapsedMs = _elapsedMs + math.max(0, tonumber(deltaTimeInMillis) or 0)
	_panelElapsedMs = _panelElapsedMs + math.max(0, tonumber(deltaTimeInMillis) or 0)
	_PublishScriptedBurst(false)
	_UpdateDangerousAreas(deltaTimeInMillis)
	_UpdateTeamAreas(deltaTimeInMillis)
	_UpdateObjectiveAndQuery()
	_UpdateTacticalAgent(deltaTimeInMillis)
	_DrawAgents()
	_DrawCppInfluenceMap()
	_DrawTacticEvents()
	if _panelElapsedMs >= 250 then
		_panelElapsedMs = 0
		_RefreshPanel()
	end
	_MaybePrintSmoke()
end
