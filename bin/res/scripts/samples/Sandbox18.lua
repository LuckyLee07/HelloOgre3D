-- Sandbox18.lua
-- Chapter 9 tactics sample, C++ influence map system version.

require("res.scripts.agent.SoldierAgent.lua")
require("res.scripts.agent.IndirectSoldierAgent.lua")

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
	queryCount = 0,
	bestPosition = nil,
	bestScore = 0.0,
	lastBurstAtMs = -1,
	deadFriendlyPublished = false,
	smokePrinted = false,
}

local textSize = { w = 410, h = 230 }
local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
	"[Sandbox18 - Chapter 9 Tactics C++]" .. GUI.MarkupNewline ..
	"C++ InfluenceMapSystem + TacticalQuery" .. GUI.MarkupNewline ..
	"Lua feeds events, C++ scores positions" .. GUI.MarkupNewline ..
	GUI.MarkupNewline ..
	"F1: reset camera" .. GUI.MarkupNewline ..
	"F3: toggle navigation mesh" .. GUI.MarkupNewline ..
	"F4: toggle tactical draw" .. GUI.MarkupNewline ..
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

local function _HasCppTactics()
	return ObjectManager ~= nil
		and ObjectManager.configureTacticalInfluence ~= nil
		and ObjectManager.addTacticalInfluenceSource ~= nil
		and ObjectManager.findBestTacticalPosition ~= nil
end

local function _ConfigureCppTactics()
	if not _HasCppTactics() then
		print("[Sandbox18] C++ tactical influence API unavailable")
		return
	end

	local config = _GetConfig()
	local mapConfig = config.influenceMap or {}
	ObjectManager:clearTacticalInfluence()
	ObjectManager:configureTacticalInfluence(
		_ReadNumber(mapConfig, "minX", -32.0),
		_ReadNumber(mapConfig, "maxX", 56.0),
		_ReadNumber(mapConfig, "minZ", -8.0),
		_ReadNumber(mapConfig, "maxZ", 62.0),
		_ReadNumber(mapConfig, "cellSize", 4.0))
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
	if _ReadBool(config, "scriptedEvents", true) ~= true then
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

	_PublishTacticEvent("BulletShot", { position = shotPos, agent = shooter })
	_PublishTacticEvent("BulletImpact", { position = impactPos, agent = shooter })
	_PublishTacticEvent("EnemySighted", { agent = target, seenAt = targetPos, lastSeen = _elapsedMs })

	local deadDelayMs = _ReadNumber(config, "deadFriendlyDelayMs", 2600)
	if not _tactics.deadFriendlyPublished and _elapsedMs >= deadDelayMs then
		local deadFriendly = _FindAgentByTeam(shooterTeam, shooter) or shooter
		if deadFriendly ~= nil then
			_PublishTacticEvent("DeadFriendlySighted", { agent = deadFriendly, seenAt = deadFriendly:GetPosition(), lastSeen = _elapsedMs })
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

	ObjectManager:clearTacticalInfluenceLayer("danger")
	_tactics.bulletImpacts = _PruneTimedEvents(_tactics.bulletImpacts, intervalMs)
	_tactics.bulletShots = _PruneTimedEvents(_tactics.bulletShots, intervalMs)

	local perspectiveTeamId = _ReadNumber(config, "dangerPerspectiveTeamId", 0)
	local dangerStrength = _ReadNumber(config, "dangerStrength", -1.0)
	local shotRadius = _ReadNumber(config, "bulletShotRadius", 10.0)
	local impactRadius = _ReadNumber(config, "bulletImpactRadius", 8.0)
	local corpseRadius = _ReadNumber(config, "deadFriendlyRadius", 12.0)
	local sightingRadius = _ReadNumber(config, "enemySightingRadius", 14.0)
	local writes = 0

	for _, event in ipairs(_tactics.bulletImpacts) do
		writes = writes + _AddSource("danger", event.position, dangerStrength, impactRadius)
	end
	for _, event in ipairs(_tactics.bulletShots) do
		writes = writes + _AddSource("danger", event.position, dangerStrength, shotRadius)
	end
	for _, event in pairs(_tactics.deadFriendlies) do
		if event.agent ~= nil and event.agent:GetTeamId() == perspectiveTeamId then
			writes = writes + _AddSource("danger", event.agent:GetPosition(), dangerStrength, corpseRadius)
		end
	end
	for _, event in pairs(_tactics.seenEnemies) do
		if event.agent ~= nil and event.agent:GetTeamId() ~= perspectiveTeamId then
			writes = writes + _AddSource("danger", event.seenAt or event.agent:GetPosition(), dangerStrength, sightingRadius)
		end
	end

	_tactics.dangerCells = ObjectManager:getTacticalInfluenceLayerActiveCellCount("danger")
	_tactics.lastCellWrites = writes
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

	ObjectManager:clearTacticalInfluenceLayer("team")
	local positiveTeamId = _ReadNumber(config, "teamPositiveTeamId", 0)
	local teamRadius = _ReadNumber(config, "teamInfluenceRadius", 11.0)
	local teamStrength = _ReadNumber(config, "teamStrength", 1.0)
	local writes = 0
	for _, agent in ipairs(_agents) do
		if _IsAlive(agent) then
			local value = agent:GetTeamId() == positiveTeamId and teamStrength or -teamStrength
			writes = writes + _AddSource("team", agent:GetPosition(), value, teamRadius)
		end
	end

	_tactics.teamCells = ObjectManager:getTacticalInfluenceLayerActiveCellCount("team")
	_tactics.lastCellWrites = _tactics.lastCellWrites + writes
end

local function _FindObjectiveCenter()
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
	local center = _ProjectToNav(_FindObjectiveCenter())
	ObjectManager:clearTacticalInfluenceLayer("objective")
	_tactics.lastCellWrites = _tactics.lastCellWrites + _AddSource(
		"objective",
		center,
		_ReadNumber(config, "objectiveStrength", 1.0),
		_ReadNumber(config, "objectiveRadius", 16.0))
	_tactics.objectiveCells = ObjectManager:getTacticalInfluenceLayerActiveCellCount("objective")

	_tactics.bestPosition = ObjectManager:findBestTacticalPosition(
		center,
		_ReadNumber(config, "tacticalQueryRadius", 24.0),
		_ReadNumber(config, "tacticalQueryStep", 4.0),
		_ReadNumber(config, "dangerWeight", 1.0),
		_ReadNumber(config, "teamWeight", 1.0),
		_ReadNumber(config, "objectiveWeight", 1.0))
	_tactics.bestScore = ObjectManager:scoreTacticalPosition(
		_tactics.bestPosition,
		_ReadNumber(config, "dangerWeight", 1.0),
		_ReadNumber(config, "teamWeight", 1.0),
		_ReadNumber(config, "objectiveWeight", 1.0))
	_tactics.queryCount = ObjectManager:getTacticalInfluenceQueryCount()
end

local function _DrawTacticEvents()
	if not _drawTactics then
		return
	end
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
	if _tactics.bestPosition ~= nil then
		local pos = _tactics.bestPosition
		DebugDrawer:drawCircle(pos + Vector3(0, 0.34, 0), 2.2, 36, _colors.best, false)
		DebugDrawer:drawLine(pos + Vector3(-1.8, 0.55, 0), pos + Vector3(1.8, 0.55, 0), _colors.best)
		DebugDrawer:drawLine(pos + Vector3(0, 0.55, -1.8), pos + Vector3(0, 0.55, 1.8), _colors.best)
	end
end

local function _DrawAgents()
	for _, agent in ipairs(_agents) do
		local pos = agent:GetPosition()
		local color = agent:GetTeamId() == 1 and _colors.team1 or _colors.team0
		DebugDrawer:drawCircle(pos + Vector3(0, 0.16, 0), 1.2, 24, color, false)
		DebugDrawer:drawLine(pos + Vector3(0, 0.2, 0), pos + Vector3(0, 3.0, 0), color)
	end
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
		and _tactics.eventCount >= 3 then
		_tactics.smokePrinted = true
		print("[Chapter9TacticsCppSmoke] PASS",
			"events=", _tactics.eventCount,
			"dangerCells=", _tactics.dangerCells,
			"teamCells=", _tactics.teamCells,
			"objectiveCells=", _tactics.objectiveCells,
			"writes=", _tactics.lastCellWrites,
			"queries=", _tactics.queryCount,
			"bestScore=", string.format("%.2f", _tactics.bestScore),
			"summary=", ObjectManager:buildTacticalInfluenceDebugSummary())
	end
end

local function _CreateAgents()
	local preset = _GetPreset()
	local agentCount = ConfigManager:GetAgentCount(_sampleName, 6)
	local agentLuafile = "res/scripts/agent/IndirectSoldierAgent.lua"
	print(ConfigManager:BuildDebugSummary(_sampleName))

	for i = 1, agentCount do
		local teamId = ConfigManager:GetAgentTeamId(_sampleName, i)
		local agentType = ConfigManager:GetAgentAppearance(_sampleName, i, Soldier.AppearanceTypes)
		local agent = Create_Soldier(agentLuafile, agentType, teamId)
		table.insert(_agents, agent)
		ConfigManager:PlaceAgentOnPresetSpawn(agent, _sampleName, i, "default")
		agent:SetTarget(agent:GetPosition())
		agent:SetTargetRadius(preset.targetRadius or 1)
		agent:SetMaxSpeed(0)
		agent:SetVelocity(Vector3(0, 0, 0))
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
	GUI_CreateCameraAndProfileInfo()
	GUI_CreateSandboxText(infoText, textSize)
	_CreatePanel()

	Sandbox:SetUseCppFsmFlag(false)

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

	local config = _GetConfig()
	_tactics.dangerElapsedMs = _ReadNumber(config, "dangerUpdateIntervalMs", 500)
	_tactics.teamElapsedMs = _ReadNumber(config, "teamUpdateIntervalMs", 500)
	_PublishScriptedBurst(true)
	_RefreshPanel()
end

function Sandbox_Update(deltaTimeInMillis)
	GUI_UpdateCameraInfo()
	GUI_UpdateProfileInfo()

	_elapsedMs = _elapsedMs + math.max(0, tonumber(deltaTimeInMillis) or 0)
	_panelElapsedMs = _panelElapsedMs + math.max(0, tonumber(deltaTimeInMillis) or 0)
	_PublishScriptedBurst(false)
	_UpdateDangerousAreas(deltaTimeInMillis)
	_UpdateTeamAreas(deltaTimeInMillis)
	_UpdateObjectiveAndQuery()
	_DrawAgents()
	_DrawTacticEvents()
	if _panelElapsedMs >= 250 then
		_panelElapsedMs = 0
		_RefreshPanel()
	end
	_MaybePrintSmoke()
end
