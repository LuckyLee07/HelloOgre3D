-- Sandbox17.lua
-- Chapter 9 tactics sample, Lua-first version.

require("res.scripts.agent.SoldierAgent.lua")
require("res.scripts.agent.IndirectSoldierAgent.lua")
require("res.scripts.agent.DecisionSoldierAgent.lua")

local InfluenceMap = require("res.scripts.ai.tactics.InfluenceMap.lua")
local ParityTrace = require("res.scripts.samples.parity_trace")
local Chapter9Profile = require("res.scripts.config.chapter9_tactics_profile")
local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

local _sampleName = "Sandbox17"
local _agents = {}
local _navMesh = nil
local _influenceMap = nil
local _panel = nil
local _drawNavMesh = false
local _drawInfluence = true
local _elapsedMs = 0
local _panelElapsedMs = 0
local _drawCache = {}
local _projectionCache = {}
local _EnsureInfluenceMap = nil
local _GetDrawCellLimit = nil
local _GetCppDrawCellLimit = nil
local _parityTrace = nil
local _uiEnabled = true

local _colors = {
	team1 = ColourValue(0.2, 0.95, 0.4),
	team0 = ColourValue(0.95, 0.25, 0.2),
	teamPositive = ColourValue(0.1, 0.45, 1.0),
	teamNegative = ColourValue(1.0, 0.18, 0.12),
	danger = ColourValue(1.0, 0.16, 0.08),
	bullet = ColourValue(1.0, 0.75, 0.15),
	dead = ColourValue(0.85, 0.85, 0.85),
	grid = ColourValue(0.0, 0.0, 0.0, 0.14),
	targetRadius = ColourValue(1.0, 0.12, 0.12),
}

local _influencePalette = {
	positive = { 0.0, 0.0, 1.0, 0.9 },
	zero = { 0.0, 0.0, 0.0, 0.75 },
	negative = { 1.0, 0.0, 0.0, 0.9 },
}
local _gridCoverageRedSpec = { 1.0, 0.0, 0.0, 0.9 }

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
	lastCellWrites = 0,
	eventTypes = {},
	eventPairs = {},
	recentEvents = {},
	lastBurstAtMs = -1,
	deadFriendlyPublished = false,
	smokePrinted = false,
}

local textSize = { w = 390, h = 230 }
local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
	"[Sandbox17 - Chapter 9 Tactics]" .. GUI.MarkupNewline ..
	"DangerousAreas: shots, impacts, sightings" .. GUI.MarkupNewline ..
	"TeamAreas: team control influence" .. GUI.MarkupNewline ..
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
	if config == nil then
		return defaultValue
	end
	local value = config[key]
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

local function _ReadEnvBool(name, defaultValue)
	local value = os.getenv and os.getenv(name) or nil
	if value == nil then
		return defaultValue
	end
	return value == "1" or value == "true" or value == "TRUE" or value == "yes"
end

-- 影响图网格轮廓线颜色。alpha 默认 0.14（teaching 偏淡），legacy parity 用
-- gridLineAlpha=0.5 复刻 chapter-9 旧版那种明显的黑色网格线。
local function _GetGridColor()
	local alpha = _ReadNumber(_GetConfig(), "gridLineAlpha", 0.14)
	return ColourValue(0.0, 0.0, 0.0, alpha)
end

local function _IsVisualIsolationEnabled(config)
	return _ReadBool(config, "visualIsolation", false)
		or _ReadEnvBool("HELLO_CH9_VISUAL_ISOLATION", false)
end

local function _ShouldHideUi(config)
	return _IsVisualIsolationEnabled(config)
		or _ReadBool(config, "hideUi", false)
		or _ReadEnvBool("HELLO_CH9_HIDE_UI", false)
end

local function _ShouldHideAgentRender(config)
	return _IsVisualIsolationEnabled(config)
		or _ReadBool(config, "hideAgentRender", false)
		or _ReadEnvBool("HELLO_CH9_HIDE_AGENT_RENDER", false)
end

local function _ShouldForceGridCoverageRed(config)
	return _ReadBool(config, "forceGridCoverageRed", false)
		or _ReadEnvBool("HELLO_CH9_FORCE_GRID_RED", false)
end

local function _GetCameraPreset(config)
	local preset = os.getenv and os.getenv("HELLO_CH9_CAMERA_PRESET") or nil
	if preset ~= nil and preset ~= "" then
		return string.lower(tostring(preset))
	end
	return string.lower(_ReadString(config, "cameraPreset", "current"))
end

local function _ApplyCameraPreset(config)
	local camera = SandboxCamera:GetCamera()
	local preset = _GetCameraPreset(config)
	if preset == "top" then
		camera:setPosition(Vector3(12, 95, 27))
		camera:setOrientation(Quaternion(-90, 0, -180))
		return
	end
	if preset == "agent" then
		camera:setPosition(Vector3(-43, 10, -3))
		camera:setOrientation(Quaternion(-135, -25, -147))
		return
	end
	camera:setPosition(Vector3(-30, 18, -17))
	camera:setOrientation(Quaternion(-146, -40, -157))
end

local function _CopyTable(value)
	local result = {}
	if value == nil then
		return result
	end
	for key, item in pairs(value) do
		result[key] = item
	end
	return result
end

local function _CopyArray(value)
	local result = {}
	if value == nil then
		return result
	end
	for index, item in ipairs(value) do
		if type(item) == "table" then
			result[index] = _CopyTable(item)
		else
			result[index] = item
		end
	end
	return result
end

local function _CountTable(value)
	if type(value) ~= "table" then
		return 0
	end
	local count = 0
	for _, _ in pairs(value) do
		count = count + 1
	end
	return count
end

local function _GetInfoPanelOptions(config)
	if _ReadBool(config, "useLegacyInfoPanel", false) then
		return nil, { w = 300, h = 180 }
	end
	return infoText, textSize
end

local function _ReadVector3(config, key, defaultValue)
	local value = config ~= nil and config[key] or nil
	if value == nil then
		return defaultValue
	end
	return Vector3(
		tonumber(value.x or value[1]) or defaultValue.x,
		tonumber(value.y or value[2]) or defaultValue.y,
		tonumber(value.z or value[3]) or defaultValue.z)
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
	if _ShouldForceGridCoverageRed(_GetConfig()) then
		return ColourValue(_gridCoverageRedSpec[1], _gridCoverageRedSpec[2], _gridCoverageRedSpec[3], _gridCoverageRedSpec[4])
	end
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

local function _ColorFromSpec(spec)
	spec = spec or _influencePalette.zero
	return ColourValue(
		tonumber(spec[1]) or 0.0,
		tonumber(spec[2]) or 0.0,
		tonumber(spec[3]) or 0.0,
		tonumber(spec[4]) or 1.0)
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

local function _GetEventTraceInfo(eventType, event)
	event = event or {}
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

	return sender, senderId, targetId, teamId, targetTeamId
end

local function _RecordTacticEventTrace(eventType, event)
	local _, senderId, targetId, teamId, targetTeamId = _GetEventTraceInfo(eventType, event)
	_tactics.eventTypes[eventType] = (_tactics.eventTypes[eventType] or 0) + 1

	local pairKey = tostring(eventType) .. ":" .. tostring(senderId) .. ">" .. tostring(targetId)
	_tactics.eventPairs[pairKey] = (_tactics.eventPairs[pairKey] or 0) + 1

	table.insert(_tactics.recentEvents, {
		tMs = math.floor(_elapsedMs),
		type = eventType,
		sender = senderId,
		target = targetId,
		team = teamId,
		targetTeam = targetTeamId,
	})
	local maxRecentEvents = _ReadNumber(_GetConfig(), "traceRecentEventLimit", 16)
	while #_tactics.recentEvents > maxRecentEvents do
		table.remove(_tactics.recentEvents, 1)
	end
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

local function _ClearInfluenceDrawCache()
	_drawCache = {}
	_projectionCache = {}
end

local function _MarkInfluenceDrawLayerDirty(layerName)
	if layerName == nil then
		_drawCache = {}
	else
		_drawCache[layerName] = nil
	end
end

local function _ResolveInfluenceDrawPosition(pos, yOffset, cellSize)
	if pos == nil then
		return nil
	end
	local config = _GetConfig()
	if _ReadBool(config, "projectInfluenceToNav", true) then
		local maxDistance = _ReadNumber(config, "drawNavProjectionMaxDistance", cellSize * 0.9)
		local cacheKey = tostring(pos.x) .. ":" .. tostring(pos.z) .. ":" .. tostring(maxDistance)
		local cached = _projectionCache[cacheKey]
		if cached == false then
			return nil
		end
		if cached == nil then
			local navPos = Sandbox:FindClosestPoint("default", pos)
			if navPos == nil then
				_projectionCache[cacheKey] = false
				return nil
			end
			local dx = navPos.x - pos.x
			local dz = navPos.z - pos.z
			if maxDistance > 0.0 and dx * dx + dz * dz > maxDistance * maxDistance then
				_projectionCache[cacheKey] = false
				return nil
			end
			cached = { x = navPos.x, y = navPos.y, z = navPos.z }
			_projectionCache[cacheKey] = cached
		end
		if cached == nil then
			return nil
		end
		return Vector3(cached.x, cached.y + yOffset, cached.z)
	end
	return Vector3(pos.x, yOffset, pos.z)
end

local function _HasCppTacticalEvents()
	return ObjectManager ~= nil
		and ObjectManager.configureTacticalEvents ~= nil
		and ObjectManager.getTacticalEventDebugRecordCount ~= nil
		and ObjectManager.getTacticalEventDebugType ~= nil
		and ObjectManager.getTacticalEventDebugPosition ~= nil
end

local function _HasCppInfluenceDraw()
	return ObjectManager ~= nil
		and ObjectManager.configureTacticalInfluence ~= nil
		and ObjectManager.clearTacticalInfluenceLayer ~= nil
		and ObjectManager.addTacticalInfluenceSource ~= nil
		and ObjectManager.addTacticalInfluencePoint ~= nil
		and ObjectManager.spreadTacticalInfluenceLayer ~= nil
		and ObjectManager.setTacticalInfluenceLayerOptions ~= nil
		and ObjectManager.rebuildTacticalInfluenceLayerDebugVisual ~= nil
		and ObjectManager.setTacticalInfluenceDebugVisible ~= nil
end

local function _ConfigureCppTacticalEvents()
	if not _HasCppTacticalEvents() then
		return
	end
	local config = _GetConfig()
	ObjectManager:clearTacticalEvents()
	ObjectManager:configureTacticalEvents(_ReadNumber(config, "eventTtlMs", 1800))
end

local function _ConfigureCppInfluenceDraw()
	if not _HasCppInfluenceDraw() then
		return
	end
	local config = _GetConfig()
	local mapConfig = config.influenceMap or {}
	ObjectManager:clearTacticalInfluence()
	-- 3D 影响图：从 navmesh 几何体素化建图，cell 自动贴在可走面上（对齐 chapter-9）。
	ObjectManager:configureTacticalInfluenceFromNavMesh(
		"default",
		_ReadNumber(mapConfig, "cellSize", 2.0),
		_ReadNumber(mapConfig, "cellHeight", 1.0),
		_ReadVector3(mapConfig, "boundaryMinOffset", Vector3(0.0, 0.0, 0.0)),
		_ReadVector3(mapConfig, "boundaryMaxOffset", Vector3(0.0, 0.0, 0.0)))
	ObjectManager:setTacticalInfluenceLayerOptions("danger", _ReadNumber(config, "dangerFalloff", 0.2), _ReadNumber(config, "dangerInertia", 0.5))
	ObjectManager:setTacticalInfluenceLayerOptions("team", _ReadNumber(config, "teamFalloff", 0.2), _ReadNumber(config, "teamInertia", 0.5))
end

local function _AddCppInfluenceSource(layerName, position, strength, radius)
	if position == nil or not _HasCppInfluenceDraw() then
		return 0
	end
	return ObjectManager:addTacticalInfluenceSource(layerName, position, strength, radius)
end

local function _AddCppInfluencePoint(layerName, position, strength)
	if position == nil or not _HasCppInfluenceDraw() then
		return 0
	end
	return ObjectManager:addTacticalInfluencePoint(layerName, position, strength)
end

local function _SpreadCppInfluenceLayer(layerName, passCount)
	if not _HasCppInfluenceDraw() then
		return 0
	end
	return ObjectManager:spreadTacticalInfluenceLayer(layerName, passCount)
end

local function _RebuildCppInfluenceLayerVisual(layerName, y, positiveSpec, negativeSpec, drawNeutralDefault)
	if not _HasCppInfluenceDraw() then
		return false
	end
	local config = _GetConfig()
	local map = _EnsureInfluenceMap()
	local threshold = _ReadNumber(config, "drawThreshold", 0.08)
	local drawNeutral = _ReadBool(config, layerName .. "DrawNeutralCells", drawNeutralDefault)
	local zeroSpec = _influencePalette.zero
	if _ShouldForceGridCoverageRed(config) then
		positiveSpec = _gridCoverageRedSpec
		negativeSpec = _gridCoverageRedSpec
		zeroSpec = _gridCoverageRedSpec
	end
	ObjectManager:rebuildTacticalInfluenceLayerDebugVisual(
		layerName,
		y,
		_ColorFromSpec(positiveSpec),
		_ColorFromSpec(zeroSpec),
		_ColorFromSpec(negativeSpec),
		_GetGridColor(),
		threshold,
		_GetCppDrawCellLimit(config),
		drawNeutral,
		-- 注意：projectInfluenceToNav / drawNavProjectionMaxDistance 当前是 C++ 端死参数（已改 2D 拍平绘制），
		-- 这里仍传是为兼容绑定签名；待 Tactical 收口重构时一并从签名和此处删除。
		_ReadBool(config, "projectInfluenceToNav", true),
		_ReadNumber(config, "drawNavProjectionMaxDistance", map.cellSize * 0.9),
		"default")
	return true
end

local function _GetCppTacticalEventCount()
	if not _HasCppTacticalEvents() then
		return 0
	end
	return ObjectManager:getTacticalEventDebugRecordCount()
end

local function _UseCppEventSource(config)
	return _ReadBool(config, "useCppEventSource", true) == true and _GetCppTacticalEventCount() > 0
end

_EnsureInfluenceMap = function()
	if _influenceMap == nil then
		local config = _GetConfig()
		_influenceMap = InfluenceMap.New(config.influenceMap or {})
		_ClearInfluenceDrawCache()
	end
	return _influenceMap
end

local function _CreatePanel()
	_panel = SandboxUI:CreateUIFrame()
	_panel:setPosition(Vector2(20, 280))
	_panel:setDimension(Vector2(600, 145))
	_panel:setTextMargin(12, 10)
	_panel:setGradientColor(Gorilla.Gradient_NorthSouth, ColourValue(0.05, 0.07, 0.09, 0.78), ColourValue(0.0, 0.0, 0.0, 0.82))
end

local function _RefreshPanel()
	if _panel == nil then
		return
	end

	local text = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
		"Chapter9 Tactics: events=" .. tostring(_tactics.eventCount) ..
		" writes=" .. tostring(_tactics.lastCellWrites) .. GUI.MarkupNewline ..
		"Danger: cells=" .. tostring(_tactics.dangerCells) ..
		" updates=" .. tostring(_tactics.dangerRunCount) ..
		" skips=" .. tostring(_tactics.dangerSkipCount) .. GUI.MarkupNewline ..
		"Team: cells=" .. tostring(_tactics.teamCells) ..
		" updates=" .. tostring(_tactics.teamRunCount) ..
		" skips=" .. tostring(_tactics.teamSkipCount) .. GUI.MarkupNewline ..
		"Draw influence=" .. tostring(_drawInfluence) ..
		" navmesh=" .. tostring(_drawNavMesh)
	_panel:setMarkupText(text)
end

local function _PublishTacticEvent(eventType, event)
	event = event or {}
	event.type = eventType
	_tactics.eventCount = _tactics.eventCount + 1
	_RecordTacticEventTrace(eventType, event)

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

	if _HasCppTacticalEvents() then
		local position = event.position or event.seenAt
		if position == nil and event.agent ~= nil then
			position = event.agent:GetPosition()
		end
		if position ~= nil then
			local _, senderId, targetId, teamId, targetTeamId = _GetEventTraceInfo(eventType, event)
			ObjectManager:publishTacticalEvent(eventType, senderId, targetId, teamId, targetTeamId, _ProjectToNav(position), math.floor(_elapsedMs), "global", false)
		end
	end
end

_G.Chapter9Legacy_OnAgentTacticEvent = function(eventType, event)
	local config = _GetConfig()
	if _ReadBool(config, "useLegacyAgentEvents", false) ~= true then
		return
	end
	_PublishTacticEvent(eventType, event)
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
	if _ReadBool(config, "useLegacyAgentEvents", false) == true then
		return
	end
	local scriptedEnabled = _ReadBool(config, "scriptedEvents", false)
	if _G.HELLO_SANDBOX_SMOKE_MODE == true then
		scriptedEnabled = scriptedEnabled or _ReadBool(config, "scriptedEventsInSmoke", true)
	end
	if scriptedEnabled ~= true then
		return
	end
	if _G.HELLO_SANDBOX_SMOKE_MODE ~= true and _UseCppEventSource(config) then
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

local function _UpdateDangerousAreas(deltaTimeInMillis)
	local config = _GetConfig()
	local intervalMs = _ReadNumber(config, "dangerUpdateIntervalMs", 500)
	_tactics.dangerElapsedMs = _tactics.dangerElapsedMs + deltaTimeInMillis
	if _tactics.dangerElapsedMs < intervalMs then
		_tactics.dangerSkipCount = _tactics.dangerSkipCount + 1
		return
	end
	_tactics.dangerElapsedMs = 0
	_tactics.dangerRunCount = _tactics.dangerRunCount + 1

	local map = _EnsureInfluenceMap()
	map:ClearLayer("danger")
	if _HasCppInfluenceDraw() then
		ObjectManager:clearTacticalInfluenceLayer("danger")
	end

	_tactics.bulletImpacts = _PruneTimedEvents(_tactics.bulletImpacts, intervalMs)
	_tactics.bulletShots = _PruneTimedEvents(_tactics.bulletShots, intervalMs)

	local perspectiveTeamId = _ReadNumber(config, "dangerPerspectiveTeamId", 0)
	local dangerStrength = _ReadNumber(config, "dangerStrength", -1.0)
	local shotRadius = _ReadNumber(config, "bulletShotRadius", 10.0)
	local impactRadius = _ReadNumber(config, "bulletImpactRadius", 8.0)
	local corpseRadius = _ReadNumber(config, "deadFriendlyRadius", 12.0)
	local sightingRadius = _ReadNumber(config, "enemySightingRadius", 14.0)
	local writes = 0
	local function addDangerSource(position, radius)
		local navPosition = _ProjectToNav(position)
		writes = writes + map:AddRadialSource("danger", navPosition, dangerStrength, radius)
		_AddCppInfluenceSource("danger", navPosition, dangerStrength, radius)
	end

	if _UseCppEventSource(config) then
		local count = _GetCppTacticalEventCount()
		for index = 1, count do
			local eventType = ObjectManager:getTacticalEventDebugType(index)
			local position = ObjectManager:getTacticalEventDebugPosition(index)
			if eventType == "BulletImpact" then
				addDangerSource(position, impactRadius)
			elseif eventType == "BulletShot" then
				addDangerSource(position, shotRadius)
			elseif eventType == "DeadFriendlySighted" then
				if ObjectManager:getTacticalEventDebugTeamId(index) == perspectiveTeamId then
					addDangerSource(position, corpseRadius)
				end
			elseif eventType == "EnemySighted" then
				local targetTeamId = ObjectManager:getTacticalEventDebugTargetTeamId(index)
				if targetTeamId < 0 then
					targetTeamId = perspectiveTeamId == 0 and 1 or 0
				end
				if targetTeamId ~= perspectiveTeamId then
					addDangerSource(position, sightingRadius)
				end
			end
		end
	else
		for _, event in ipairs(_tactics.bulletImpacts) do
			addDangerSource(event.position, impactRadius)
		end
		for _, event in ipairs(_tactics.bulletShots) do
			addDangerSource(event.position, shotRadius)
		end
		for _, event in pairs(_tactics.deadFriendlies) do
			if event.agent ~= nil and event.agent:GetTeamId() == perspectiveTeamId then
				addDangerSource(event.agent:GetPosition(), corpseRadius)
			end
		end
		for _, event in pairs(_tactics.seenEnemies) do
			if event.agent ~= nil and event.agent:GetTeamId() ~= perspectiveTeamId then
				addDangerSource(event.seenAt or event.agent:GetPosition(), sightingRadius)
			end
		end
	end

	local threshold = _ReadNumber(config, "drawThreshold", 0.08)
	_tactics.dangerCells = #map:GetActiveCells("danger", threshold)
	_tactics.lastCellWrites = writes
	_MarkInfluenceDrawLayerDirty("danger")
	if _ReadBool(config, "drawDangerLayer", false) then
		_RebuildCppInfluenceLayerVisual("danger", 0.0, _influencePalette.positive, _influencePalette.negative, false)
	end
end

local function _UpdateTeamAreas(deltaTimeInMillis)
	local config = _GetConfig()
	local intervalMs = _ReadNumber(config, "teamUpdateIntervalMs", 500)
	_tactics.teamElapsedMs = _tactics.teamElapsedMs + deltaTimeInMillis
	if _tactics.teamElapsedMs < intervalMs then
		_tactics.teamSkipCount = _tactics.teamSkipCount + 1
		return
	end
	_tactics.teamElapsedMs = 0
	_tactics.teamRunCount = _tactics.teamRunCount + 1

	local map = _EnsureInfluenceMap()
	map:ClearLayer("team")
	if _HasCppInfluenceDraw() then
		ObjectManager:clearTacticalInfluenceLayer("team")
	end

	local positiveTeamId = _ReadNumber(config, "teamPositiveTeamId", 0)
	local teamRadius = _ReadNumber(config, "teamInfluenceRadius", 11.0)
	local teamStrength = _ReadNumber(config, "teamStrength", 1.0)
	local useLegacyPointSpread = _ReadBool(config, "teamUseLegacyPointSpread", false)
	local writes = 0
	for _, agent in ipairs(_agents) do
		if _IsAlive(agent) then
			local value = agent:GetTeamId() == positiveTeamId and teamStrength or -teamStrength
			local navPosition = _ProjectToNav(agent:GetPosition())
			if useLegacyPointSpread then
				writes = writes + map:AddPointSource("team", navPosition, value)
				_AddCppInfluencePoint("team", navPosition, value)
			else
				writes = writes + map:AddRadialSource("team", navPosition, value, teamRadius)
				_AddCppInfluenceSource("team", navPosition, value, teamRadius)
			end
		end
	end
	if useLegacyPointSpread then
		local spreadPasses = _ReadNumber(config, "teamSpreadPasses", 20)
		writes = writes + map:SpreadLayer("team", spreadPasses, _ReadNumber(config, "teamFalloff", 0.2), _ReadNumber(config, "teamInertia", 0.5))
		_SpreadCppInfluenceLayer("team", spreadPasses)
	end

	local threshold = _ReadNumber(config, "drawThreshold", 0.08)
	_tactics.teamCells = #map:GetActiveCells("team", threshold)
	_tactics.lastCellWrites = _tactics.lastCellWrites + writes
	_MarkInfluenceDrawLayerDirty("team")
	if _ReadBool(config, "drawTeamLayer", true) then
		_RebuildCppInfluenceLayerVisual("team", _ReadNumber(_GetConfig(), "teamDrawYOffset", 0.0), _influencePalette.positive, _influencePalette.negative, true)
	end
end

local function _DrawCachedCell(cell)
	if cell == nil or cell.position == nil or cell.color == nil then
		return
	end
	DebugDrawer:drawSquare(cell.position, cell.cellSize, cell.color, true)
	DebugDrawer:drawSquare(cell.position + Vector3(0, 0.01, 0), cell.cellSize, _GetGridColor(), false)
end

_GetDrawCellLimit = function(config, map)
	local limit = tonumber(config ~= nil and config.maxDrawCellsPerLayer or nil)
	if limit == nil or limit <= 0 then
		return map.width * map.height
	end
	return math.max(1, limit)
end

_GetCppDrawCellLimit = function(config)
	local limit = tonumber(config ~= nil and config.maxDrawCellsPerLayer or nil)
	if limit == nil or limit <= 0 then
		return 0
	end
	return math.max(1, limit)
end

local function _BuildInfluenceLayerDrawCache(layerName, y, positiveSpec, negativeSpec, drawNeutralDefault)
	local config = _GetConfig()
	local map = _EnsureInfluenceMap()
	local threshold = _ReadNumber(config, "drawThreshold", 0.08)
	local drawNeutral = _ReadBool(config, layerName .. "DrawNeutralCells", drawNeutralDefault)
	local maxCells = _GetDrawCellLimit(config, map)
	local cells = {}

	for iz = 0, map.height - 1 do
		for ix = 0, map.width - 1 do
			local position = Vector3(
				map.minX + (ix + 0.5) * map.cellSize,
				y,
				map.minZ + (iz + 0.5) * map.cellSize)
			local value = map:Sample(layerName, position)
			if drawNeutral or math.abs(value) >= threshold then
				local drawPosition = _ResolveInfluenceDrawPosition(position, y, map.cellSize)
				if drawPosition ~= nil then
					table.insert(cells, {
						position = drawPosition,
						color = _InfluenceColor(value, positiveSpec, negativeSpec),
						cellSize = map.cellSize,
					})
					if #cells >= maxCells then
						return cells
					end
				end
			end
		end
	end
	return cells
end

local function _DrawInfluenceLayer(layerName, y, positiveSpec, negativeSpec, drawNeutralDefault)
	if _HasCppInfluenceDraw() then
		local config = _GetConfig()
		local map = _EnsureInfluenceMap()
		local threshold = _ReadNumber(config, "drawThreshold", 0.08)
		local drawNeutral = _ReadBool(config, layerName .. "DrawNeutralCells", drawNeutralDefault)
		local zeroSpec = _influencePalette.zero
		if _ShouldForceGridCoverageRed(config) then
			positiveSpec = _gridCoverageRedSpec
			negativeSpec = _gridCoverageRedSpec
			zeroSpec = _gridCoverageRedSpec
		end
		ObjectManager:drawTacticalInfluenceLayer(
			layerName,
			y,
			_ColorFromSpec(positiveSpec),
			_ColorFromSpec(zeroSpec),
			_ColorFromSpec(negativeSpec),
			_GetGridColor(),
			threshold,
			_GetCppDrawCellLimit(config),
			drawNeutral,
			_ReadBool(config, "projectInfluenceToNav", true),
			_ReadNumber(config, "drawNavProjectionMaxDistance", map.cellSize * 0.9),
			"default")
		return
	end
	local cells = _drawCache[layerName]
	if cells == nil then
		cells = _BuildInfluenceLayerDrawCache(layerName, y, positiveSpec, negativeSpec, drawNeutralDefault)
		_drawCache[layerName] = cells
	end
	for _, cell in ipairs(cells) do
		_DrawCachedCell(cell)
	end
end

local function _DrawInfluenceMap()
	local config = _GetConfig()
	local visible = _drawInfluence and _influenceMap ~= nil
	if _G.HELLO_SANDBOX_SMOKE_MODE == true and _ReadBool(config, "drawInSmoke", false) ~= true then
		visible = false
	end
	if _HasCppInfluenceDraw() then
		ObjectManager:setTacticalInfluenceDebugVisible(visible)
		return
	end
	if not visible then
		return
	end

	if _ReadBool(config, "drawDangerLayer", false) then
		_DrawInfluenceLayer("danger", 0.12, _influencePalette.positive, _influencePalette.negative, false)
	end
	if _ReadBool(config, "drawTeamLayer", true) then
		_DrawInfluenceLayer("team", _ReadNumber(_GetConfig(), "teamDrawYOffset", 0.0), _influencePalette.positive, _influencePalette.negative, true)
	end
end

local function _DrawTacticEvents()
	local config = _GetConfig()
	if _IsVisualIsolationEnabled(config) then
		return
	end
	if _ReadBool(config, "drawEventMarkers", false) ~= true then
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
end

local function _DrawAgents()
	local config = _GetConfig()
	if _IsVisualIsolationEnabled(config) then
		return
	end
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

local function _DrawFootRing(agent, radius, yOffset)
	if agent == nil or radius <= 0.0 then
		return
	end
	local position = agent:GetPosition()
	if position ~= nil then
		position.y = position.y - agent:GetHeight() * 0.5 + yOffset
		DebugDrawer:drawCircle(position, radius, 28, _colors.targetRadius, false)
	end
end

local function _FindAgentByObjectId(agentId)
	for _, agent in ipairs(_agents) do
		if _GetAgentId(agent) == agentId then
			return agent
		end
	end
	return nil
end

local function _DrawPursuedAgentFootRings(ringRadius, yOffset)
	local drawnTargets = {}
	for _, agent in ipairs(_agents) do
		local ai = AgentComponents.GetAI(agent)
		local bb = ai ~= nil and ai.GetBlackboard ~= nil and ai:GetBlackboard() or nil
		if bb ~= nil and bb:GetString("legacy.action") == "pursue" then
			local enemyId = bb:GetInt("legacy.enemyId", -1)
			if enemyId >= 0 and drawnTargets[enemyId] ~= true then
				drawnTargets[enemyId] = true
				_DrawFootRing(_FindAgentByObjectId(enemyId), ringRadius, yOffset)
			end
		end
	end
end

-- 对齐 chapter-9 的视觉：legacy parity 的红圈跟随被追击的 agent 脚下；
-- 普通 sample 仍可选择保留移动目标点 target ring。
local function _DrawTargetRadius()
	local config = _GetConfig()
	if _IsVisualIsolationEnabled(config) then
		return
	end
	if _ReadBool(config, "drawTargetRadius", true) ~= true then
		return
	end
	local ringRadius = _ReadNumber(config, "targetRingRadius", _ReadNumber(_GetPreset(), "targetRadius", 1.0))
	local yOffset = _ReadNumber(config, "agentFootRingYOffset", 0.08)
	local mode = _ReadString(config, "targetRingMode", "target")
	if mode == "pursuedAgent" then
		_DrawPursuedAgentFootRings(ringRadius, yOffset)
		return
	end
	if mode == "agentFoot" then
		for _, agent in ipairs(_agents) do
			if _IsAlive(agent) then
				_DrawFootRing(agent, ringRadius, yOffset)
			end
		end
		return
	end
	for _, agent in ipairs(_agents) do
		if _IsAlive(agent) then
			local target = agent:GetTarget()
			if target ~= nil and ringRadius > 0.0 then
				DebugDrawer:drawCircle(target + Vector3(0, 0.12, 0), ringRadius, 28, _colors.targetRadius, false)
			end
		end
	end
end

local function _MaybePrintSmoke()
	if _tactics.smokePrinted or _G.HELLO_SANDBOX_SMOKE_MODE ~= true then
		return
	end
	if _tactics.dangerRunCount > 0 and _tactics.teamRunCount > 0 and _tactics.dangerCells > 0 and _tactics.teamCells > 0 and _tactics.eventCount >= 3 then
		_tactics.smokePrinted = true
		print("[Chapter9TacticsSmoke] PASS",
			"events=", _tactics.eventCount,
			"dangerCells=", _tactics.dangerCells,
			"teamCells=", _tactics.teamCells,
			"dangerUpdates=", _tactics.dangerRunCount,
			"teamUpdates=", _tactics.teamRunCount,
			"dangerSkips=", _tactics.dangerSkipCount,
			"teamSkips=", _tactics.teamSkipCount)
	end
end

local function _BuildAgentParityExtra(agent)
	local extra = {
		alive = _IsAlive(agent),
		tacticDead = _IsTacticDead(agent),
		intent = ParityTrace.AgentIntentSnapshot(agent),
	}

	local ai = AgentComponents.GetAI(agent)
	local bb = ai ~= nil and ai.GetBlackboard ~= nil and ai:GetBlackboard() or nil
	if bb ~= nil then
		extra.legacyAction = bb:GetString("legacy.action")
		extra.legacyEnemyId = bb:GetInt("legacy.enemyId", -1)
		extra.legacyAgentIndex = bb:GetInt("legacy.agentIndex", -1)
		extra.legacyRandomMoveCount = bb:GetInt("legacy.randomMoveCount", 0)
		extra.legacyMoveSource = bb:GetString("legacy.moveSource")
		extra.legacyVisibleCount = bb:GetInt("legacy.visibleCount", 0)
		extra.legacyUseHeadBoneVision = bb:GetBool("legacy.useHeadBoneVision", false)

		local visibleIds = {}
		if bb.GetIntArrayCount ~= nil and bb.GetIntArrayValue ~= nil then
			for i = 1, bb:GetIntArrayCount("legacy.visibleIds") do
				visibleIds[#visibleIds + 1] = bb:GetIntArrayValue("legacy.visibleIds", i, -1)
			end
		end
		extra.legacyVisibleIds = visibleIds
	end

	return extra
end

local function _BuildParitySnapshot(state)
	local map = _EnsureInfluenceMap()
	local config = _GetConfig()
	local threshold = _ReadNumber(config, "drawThreshold", 0.08)
	local agents = {}
	local maxAgents = state ~= nil and state.maxAgents or #_agents
	for index, agent in ipairs(_agents) do
		if index > maxAgents then
			break
		end
		agents[#agents + 1] = ParityTrace.AgentSnapshot(agent, index, _BuildAgentParityExtra(agent))
	end

	local payload = {
		agents = agents,
		tactics = {
			events = _tactics.eventCount,
			dangerCells = _tactics.dangerCells,
			teamCells = _tactics.teamCells,
			dangerUpdates = _tactics.dangerRunCount,
			teamUpdates = _tactics.teamRunCount,
			dangerSkips = _tactics.dangerSkipCount,
			teamSkips = _tactics.teamSkipCount,
			bulletImpacts = #_tactics.bulletImpacts,
			bulletShots = #_tactics.bulletShots,
			deadFriendlies = _CountTable(_tactics.deadFriendlies),
			seenEnemies = _CountTable(_tactics.seenEnemies),
			lastCellWrites = _tactics.lastCellWrites,
			eventTypes = _CopyTable(_tactics.eventTypes),
			eventPairs = _CopyTable(_tactics.eventPairs),
			recentEvents = _CopyArray(_tactics.recentEvents),
			luaDangerActive = #(map:GetActiveCells("danger", threshold)),
			luaTeamActive = #(map:GetActiveCells("team", threshold)),
			cppEvents = _GetCppTacticalEventCount(),
		},
	}
	if state == nil or state.includeAiSummary then
		if ObjectManager ~= nil and ObjectManager.buildAiDebugSummary ~= nil then
			payload.aiSummary = ParityTrace.SplitLines(ObjectManager:buildAiDebugSummary(maxAgents), state.aiSummaryMaxLines, state.aiSummaryMaxLineLength)
		end
	end
	return payload
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
	local hideAgentRender = _ShouldHideAgentRender(config)
	print(ConfigManager:BuildDebugSummary(_sampleName))
	Chapter9Profile.PrintStartupSummary(_sampleName, preset)

	for i = 1, agentCount do
		local teamId = _GetTeamIdForAgent(config, i)
		local agentType = _GetAppearanceForTeam(teamId)
		local agent = Create_Soldier(agentLuafile, agentType, teamId)
		_PlaceAgentLikeChapter9(agent, preset, config, i)
		if hideAgentRender and agent.SetRenderVisible ~= nil then
			agent:SetRenderVisible(false)
		end
		table.insert(_agents, agent)
	end
end

function EventHandle_Keyboard(keycode, pressed)
	if _uiEnabled then
		GUI_HandleKeyEvent(keycode, pressed)
	end

	if not pressed then return end
	if keycode == OIS.KC_F1 then
		_ApplyCameraPreset(_GetConfig())
	elseif keycode == OIS.KC_F3 then
		_drawNavMesh = not _drawNavMesh
		if _navMesh ~= nil then
			_navMesh:SetDebugVisible(_drawNavMesh)
		end
	elseif keycode == OIS.KC_F4 then
		_drawInfluence = not _drawInfluence
	end
end

function EventHandle_Mouse(ctype)
end

function EventHandle_WindowResized(width, height)
	if _uiEnabled then
		GUI_WindowResized(width, height)
	end
end

function Sandbox_Initialize()
	local config = _GetConfig()
	_G.HELLO_SUPPRESS_AI_PATH_DRAW = _ReadBool(config, "suppressPathDraw", true)
	_uiEnabled = not _ShouldHideUi(config)

	if _uiEnabled then
		GUI_CreateCameraAndProfileInfo()
		local panelText, panelSize = _GetInfoPanelOptions(config)
		GUI_CreateSandboxText(panelText, panelSize)
		if _ReadBool(config, "showTacticsPanel", false) then
			_CreatePanel()
		end
	end

	Sandbox:SetUseCppFsmFlag(true)

	_ApplyCameraPreset(config)

	Sandbox:SetSkyBox("ThickCloudsWaterSkyBox", Vector3(0, 180, 0))

	local plane = SandboxObjects:CreatePlane(200, 200)
	plane:setPosition(Vector3(0, -10, 0))
	plane:setMaterial("Ground2")

	Sandbox:SetAmbientLight(Vector3(0.30))
	local directLight = Sandbox:CreateDirectionalLight(Vector3(1, -1, 1))
	directLight:setDiffuseColour(ColourValue(1.8, 1.4, 0.9))
	directLight:setSpecularColour(ColourValue(1.8, 1.4, 0.9))

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
	_EnsureInfluenceMap()
	_ConfigureCppInfluenceDraw()
	_ConfigureCppTacticalEvents()

	_tactics.dangerElapsedMs = _ReadNumber(config, "dangerUpdateIntervalMs", 500)
	_tactics.teamElapsedMs = _ReadNumber(config, "teamUpdateIntervalMs", 500)
	_PublishScriptedBurst(true)
	local preset = _GetPreset()
	_parityTrace = ParityTrace.Start({
		sample = _sampleName,
		preset = preset.name,
		seed = preset.seed,
		profile = Chapter9Profile.BuildTraceMetadata(preset),
		config = preset.parityTrace,
	})
	_RefreshPanel()
end

function Sandbox_Update(deltaTimeInMillis)
	if _uiEnabled then
		GUI_UpdateCameraInfo()
		GUI_UpdateProfileInfo()
	end

	_elapsedMs = _elapsedMs + math.max(0, tonumber(deltaTimeInMillis) or 0)
	_panelElapsedMs = _panelElapsedMs + math.max(0, tonumber(deltaTimeInMillis) or 0)
	_PublishScriptedBurst(false)
	_UpdateDangerousAreas(deltaTimeInMillis)
	_UpdateTeamAreas(deltaTimeInMillis)
	_DrawAgents()
	_DrawTargetRadius()
	_DrawTacticEvents()
	_DrawInfluenceMap()
	if _uiEnabled and _panelElapsedMs >= 250 then
		_panelElapsedMs = 0
		_RefreshPanel()
	end
	ParityTrace.Tick(_parityTrace, deltaTimeInMillis, _BuildParitySnapshot)
	_MaybePrintSmoke()
end
