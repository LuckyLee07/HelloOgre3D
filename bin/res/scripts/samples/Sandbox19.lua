-- Sandbox19.lua
-- Playable tactical command encounter: one unarmed commander leads AI squadmates through three waves.

require("res.scripts.agent.SoldierAgent.lua")
require("res.scripts.agent.BehaviorSoldierAgent.lua")
-- 指令条件表必须在 agent 建树前进入 _G：BehaviorSoldierAgent 按 preset 的
-- conditionsGlobal 名字查 _G，查不到会静默回落到 SoldierConditions。
require("res.scripts.ai.behavior.Sandbox19CommandConditions.lua")

local RetreatPolicy = require("res.scripts.samples.sandbox19_retreat.lua")
local Observer = require("res.scripts.samples.ai_observer")
local _observer = Observer.New()
_observer.enabled = false
local _observerPanel = nil
local _profiles = {}
local _observationTest = nil
if os.getenv("HELLO_SANDBOX19_OBSERVATION_SELF_TEST") == "1" then
	_observer.enabled = true
	_observationTest = require("res.scripts.samples.ai_observer_selftest").New()
end

local _stabilityTest = require("res.scripts.samples.sandbox19_stability_selftest.lua").New()
local _sampleName = "Sandbox19"
local _agents = {}
local _player = nil
local _hud = nil
local _crosshair = nil
local _matchState = "PREPARE"
local _restartRequested = false
local _waveIndex = 0
local _phaseDeadlineMs = 0
local _matchStartedMs = 0
local _matchEndedMs = nil
local _matchEndReason = ""
local _waveLastProgressMs = 0
local _waveLastEnemyAlive = 0
local _waveConvergeCount = 0

-- Gorilla 圆盘雷达（SandboxUI:CreatePolygon → UIPolygon，与 HP 面板同一 Gorilla 层，确保渲染）：
-- 浅蓝圆盘（高边数≈圆，深色描边）+ 中心三角箭头（静止朝上，player-up）+ 复用圆点 blip 池
-- （每帧按玩家朝向投影重定位/上色/显隐；敌红友绿）。对齐 code-master Chapter12 观感。
-- cx/cy 为雷达圆心（屏幕像素，置于左上角），radius 为圆盘半径，range 为覆盖世界半径。
local _radar = { disc = nil, arrow = nil, blips = {}, ok = false }
local RADAR = {
	cx = 104, cy = 104, radius = 84, range = 60,
	discSides = 48, blipSides = 14, arrowSides = 3,
	blipRadius = 5, arrowRadius = 11, poolSize = 16,
}

-- 指挥层：玩家是无武器小队长，RMB 用于选择友军（点选/框选），F/T/G 下令。
-- 指令写进 agent blackboard 的 command.* 命名空间（由 Sandbox19CommandBT 消费），
-- 同时写一份 TeamBlackboard typed fact（团队级 + TTL + 可被 RuntimeDiag 观测）。
local COMMAND = {
	ttlMs = 8000,          -- 与 Sandbox19CommandConditions._COMMAND_TTL_MS 保持一致
	pickRadiusPx = 48,     -- 点选命中阈值；超出视为空点选并清空选择集
	dragThresholdPx = 8,   -- 拖拽超过此像素才算框选，否则按点选处理
	fallbackFovDeg = 45,   -- 集火无点选目标时，玩家朝向 ±45° 扇形内取最近敌人
	rallySpacing = 2.5,    -- 编队时各单位在玩家周围的间距
	groundY = 0.0,
}

-- 一局节奏由 preset.commanderMatch 覆盖；默认值保证旧 preset/局部运行仍可用。
local MATCH = {
	prepareMs = 6000,
	intermissionMs = 7000,
	stalemateMs = 20000,
	criticalRetreatMs = 6000,
	allyCount = 2,
	waveEnemyCounts = { 2, 3, 4 },
	waveSpawnIndices = {
		{ 4, 5 },
		{ 6, 7, 4 },
		{ 4, 5, 6, 7 },
	},
}

local _selection = {}          -- objId -> true
local _dragging = false
local _dragStart = { x = 0, y = 0 }
local _dragNow = { x = 0, y = 0 }
local _lastPickedEnemyId = 0
local _commandHint = ""
local _commandHintUntilMs = 0
local function _SetHint(text)
	_commandHint = text
	_commandHintUntilMs = GameManager:getTimeInMillis() + 5000
end
local _inputDiagEnabled = os.getenv ~= nil and os.getenv("HELLO_SANDBOX19_INPUT_DIAG") == "1"

local SELECT_MARK_POOL = 4       -- Sandbox19 友方 AI 只有 2 个，留一倍余量
local SELECT_MARK_SIZE = 18
local _dragRect = nil
local _selectMarks = {}

-- W2 玩家可读意图：头顶卡片说明“正在做什么 + 为什么”，目标菱形说明“要去哪/打谁”。
local INTENT_CARD_POOL = 4
local INTENT_CARD_WIDTH = 164
local INTENT_CARD_HEIGHT = 34
local INTENT_CARD_GAP = 6
local INTENT_TARGET_RADIUS = 8
local INTENT_HUD_BOUNDS = { x = 20, y = 188, w = 410, h = 150 }
local _intentCards = {}
local _intentTargets = {}
local _focusTargetLabel = nil
local _intentSnapshot = {}
local _intentCounts = { focus = 0, retreat = 0, rally = 0 }
local _IntentColour = nil

local infoText = GUI.MarkupColor.White .. GUI.Markup.MediumMono ..
	"LEAD YOUR SQUAD THROUGH 3 WAVES" .. GUI.MarkupNewline ..
	"Unarmed: W/S move | A/D turn" .. GUI.MarkupNewline ..
	"Tab: select all living allies" .. GUI.MarkupNewline ..
	"RMB: select ally / mark enemy" .. GUI.MarkupNewline ..
	"Shift sprint | RMB drag: box select" .. GUI.MarkupNewline ..
	"F focus | T fall back | G rally here" .. GUI.MarkupNewline ..
	"Orders last 8s, then AI takes over" .. GUI.MarkupNewline ..
	"Enter restart | I observer | F2 help"

local function _CreateHud()
	_hud = SandboxUI:CreateUIFrame()
	_hud:setPosition(Vector2(20, 188))   -- 下移避开左上角雷达
	_hud:setDimension(Vector2(410, 150))
	_hud:setTextMargin(12, 10)
	_hud:setGradientColor(Gorilla.Gradient_NorthSouth,
		ColourValue(0.05, 0.08, 0.09, 0.82),
		ColourValue(0.0, 0.0, 0.0, 0.84))

	_observerPanel = SandboxUI:CreateUIFrame()
	_observerPanel:setDimension(Vector2(460, 290))
	_observerPanel:setTextMargin(8, 8)
	_observerPanel:setBackgroundColor(ColourValue(0.03, 0.05, 0.08, 0.90))

	_crosshair = SandboxUI:CreateUIFrame()
	_crosshair:setDimension(Vector2(32, 32))
	_crosshair:setTextMargin(4, 8)
	_crosshair:setText("+")

	-- 指挥 UI：框选矩形 + 选中高亮池，都复用 UIFrame（setPosition/setDimension/
	-- setBackgroundColor/setVisible），不新增 Gorilla 图元。
	_dragRect = SandboxUI:CreateUIFrame()
	_dragRect:setBackgroundColor(ColourValue(0.35, 0.75, 1.0, 0.18))
	_dragRect:setVisible(false)

	_selectMarks = {}
	for i = 1, SELECT_MARK_POOL do
		local m = SandboxUI:CreateUIFrame()
		m:setDimension(Vector2(SELECT_MARK_SIZE, SELECT_MARK_SIZE))
		m:setBackgroundColor(ColourValue(0.30, 1.0, 0.40, 0.55))
		m:setVisible(false)
		table.insert(_selectMarks, m)
	end

	_intentCards = {}
	_intentTargets = {}
	for i = 1, INTENT_CARD_POOL do
		local card = SandboxUI:CreateUIFrame()
		card:setDimension(Vector2(INTENT_CARD_WIDTH, INTENT_CARD_HEIGHT))
		card:setTextMargin(3, 6)
		card:setVisible(false)
		table.insert(_intentCards, card)

		local target = SandboxUI:CreatePolygon()
		target:setSides(4)
		target:setRadius(INTENT_TARGET_RADIUS)
		target:setAngleDegrees(45)
		target:setBorder(2, ColourValue(0.05, 0.05, 0.05, 0.95))
		target:setVisible(false)
		table.insert(_intentTargets, target)
	end

	_focusTargetLabel = SandboxUI:CreateUIFrame()
	_focusTargetLabel:setDimension(Vector2(118, 22))
	_focusTargetLabel:setTextMargin(3, 6)
	_focusTargetLabel:setBackgroundColor(ColourValue(0.80, 0.08, 0.05, 0.88))
	_focusTargetLabel:setMarkupText(GUI.MarkupColor.White .. GUI.Markup.SmallMono .. "FOCUS TARGET")
	_focusTargetLabel:setVisible(false)
end

local function _LayoutCrosshair(width, height)
	if _crosshair == nil then return end
	_crosshair:setPosition(Vector2(width * 0.5 - 16, height * 0.5 - 16))
end

local function _LoadMatchConfig()
	local preset = ConfigManager:GetSamplePreset(_sampleName)
	local cfg = preset ~= nil and preset.commanderMatch or nil
	if cfg == nil then return end
	MATCH.prepareMs = math.max(0, tonumber(cfg.prepareMs) or MATCH.prepareMs)
	MATCH.intermissionMs = math.max(0, tonumber(cfg.intermissionMs) or MATCH.intermissionMs)
	MATCH.stalemateMs = math.max(5000, tonumber(cfg.stalemateMs) or MATCH.stalemateMs)
	MATCH.criticalRetreatMs = math.max(0, tonumber(cfg.criticalRetreatMs) or MATCH.criticalRetreatMs)
	MATCH.allyCount = math.max(1, tonumber(cfg.allyCount) or MATCH.allyCount)
	if type(cfg.waveEnemyCounts) == "table" and #cfg.waveEnemyCounts > 0 then
		MATCH.waveEnemyCounts = {}
		for i, count in ipairs(cfg.waveEnemyCounts) do
			MATCH.waveEnemyCounts[i] = math.max(1, tonumber(count) or 1)
		end
	end
end

local function _CountAliveAgents()
	local allyAlive = 0
	local enemyAlive = 0
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		local agent = agents[i]
		if agent ~= nil and agent:GetHealth() > 0 then
			if _player ~= nil and agent ~= _player and agent:GetTeamId() == _player:GetTeamId() then
				allyAlive = allyAlive + 1
			else
				if _player ~= nil and agent:GetTeamId() ~= _player:GetTeamId() then
					enemyAlive = enemyAlive + 1
				end
			end
		end
	end
	return allyAlive, enemyAlive
end

local function _FormatSeconds(ms)
	return string.format("%.1fs", math.max(0, tonumber(ms) or 0) / 1000.0)
end

local function _BuildPhaseText(nowMs)
	if _matchState == "PREPARE" then
		return "DEPLOY - wave 1 in " .. _FormatSeconds(_phaseDeadlineMs - nowMs)
	elseif _matchState == "INTERMISSION" then
		return "REGROUP - wave " .. tostring(_waveIndex + 1) .. " in " .. _FormatSeconds(_phaseDeadlineMs - nowMs)
	elseif _matchState == "WAVE" then
		return "WAVE " .. tostring(_waveIndex) .. " / " .. tostring(#MATCH.waveEnemyCounts)
	end
	return _matchState .. " - press Enter to restart"
end

local function _UpdateHud()
	if _hud == nil or _player == nil then return end
	local nowMs = GameManager:getTimeInMillis()
	local allyAlive, enemyAlive = _CountAliveAgents()
	local selCount = 0
	for _ in pairs(_selection) do selCount = selCount + 1 end
	local active, nextExpiry = 0, COMMAND.ttlMs
	for _, intent in pairs(_intentSnapshot) do
		if intent.commandKind ~= nil then
			active = active + 1
			nextExpiry = math.min(nextExpiry, intent.remainingMs or 0)
		end
	end
	local orders = active > 0 and string.format("Orders: %d | next expiry %s", active, _FormatSeconds(nextExpiry))
		or "Orders: none - autonomous AI"
	local hint = _commandHint
	if nowMs >= _commandHintUntilMs then
		hint = selCount == 0 and "Tab selects squad | F / T / G orders" or "RMB enemy then F to focus fire"
	end
	if _matchEndedMs ~= nil then
		orders = _matchEndReason
		hint = "Enter: restart with a fresh squad"
	end
	local text = GUI.MarkupColor.White .. GUI.Markup.MediumMono .. string.format(
		"%s\nHP %d/100 | Time %s\nAllies %d | Enemies %d | Selected %d\n%s\n",
		_BuildPhaseText(nowMs), math.max(0, math.floor(_player:GetHealth())),
		_FormatSeconds((_matchEndedMs or nowMs) - _matchStartedMs), allyAlive, enemyAlive, selCount, orders)
		.. GUI.MarkupColor.Yellow .. hint
	_hud:setMarkupText(text)
end

local function _CreateRadar()
	if SandboxUI == nil then return end
	-- 浅蓝圆盘（高边数≈圆）+ 深色描边，对齐 code-master Radar.png 观感
	_radar.disc = SandboxUI:CreatePolygon()
	_radar.disc:setSides(RADAR.discSides)
	_radar.disc:setRadius(RADAR.radius)
	_radar.disc:setPosition(Vector2(RADAR.cx, RADAR.cy))
	_radar.disc:setBackgroundColor(ColourValue(0.60, 0.75, 0.88, 0.82))
	_radar.disc:setBorder(3, ColourValue(0.08, 0.12, 0.18, 0.95))
	-- 圆点 blip 池（复用；每帧按存活单位重定位/上色/显隐）。先建，位于箭头之下。
	_radar.blips = {}
	for i = 1, RADAR.poolSize do
		local b = SandboxUI:CreatePolygon()
		b:setSides(RADAR.blipSides)
		b:setRadius(RADAR.blipRadius)
		b:setVisible(false)
		table.insert(_radar.blips, b)
	end
	-- 玩家箭头：蓝色三角，居中静止朝上（player-up）。最后建 → 叠在最上层。
	_radar.arrow = SandboxUI:CreatePolygon()
	_radar.arrow:setSides(RADAR.arrowSides)
	_radar.arrow:setRadius(RADAR.arrowRadius)
	_radar.arrow:setAngleDegrees(-90)   -- 首顶点朝屏幕上方，三角尖朝上
	_radar.arrow:setPosition(Vector2(RADAR.cx, RADAR.cy))
	_radar.arrow:setBackgroundColor(ColourValue(0.13, 0.34, 0.85, 1.0))
	_radar.ok = true
end

local function _UpdateRadar()
	if not _radar.ok or _player == nil then return end
	local focusTargetIds = {}
	for _, intent in pairs(_intentSnapshot) do
		if intent.commandKind == "focus" and intent.targetId ~= nil then
			focusTargetIds[intent.targetId] = true
		end
	end
	local pp = _player:GetPosition()
	local pf = _player:GetForward()
	local flen = math.sqrt(pf.x * pf.x + pf.z * pf.z)
	if flen < 1e-4 then return end
	local fx, fz = pf.x / flen, pf.z / flen   -- 角色前向(XZ)
	local rx, rz = -fz, fx                     -- 屏幕右向（按已观察手性校正，敌在视觉右→雷达右）
	-- UIPolygon:setPosition 设的是圆心，无需再减半宽
	local scale = (RADAR.radius - RADAR.blipRadius) / RADAR.range
	local slot = 1
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		local agent = agents[i]
		if agent ~= nil and agent ~= _player and agent:GetHealth() > 0 and slot <= RADAR.poolSize then
			local ap = agent:GetPosition()
			local dx, dz = ap.x - pp.x, ap.z - pp.z
			local fwdC = dx * fx + dz * fz         -- 前向分量 → 屏幕上(-y)
			local rgtC = dx * rx + dz * rz         -- 右向分量 → 屏幕右(+x)
			if fwdC * fwdC + rgtC * rgtC <= RADAR.range * RADAR.range then
				local enemy = agent:GetTeamId() ~= _player:GetTeamId()
				local b = _radar.blips[slot]
				if enemy and focusTargetIds[agent:GetObjId()] then
					b:setRadius(RADAR.blipRadius + 3)
					b:setBackgroundColor(ColourValue(1.0, 0.78, 0.08, 1.0))  -- 集火目标黄
				elseif enemy then
					b:setRadius(RADAR.blipRadius)
					b:setBackgroundColor(ColourValue(0.92, 0.20, 0.15, 1.0))  -- 敌红
				else
					b:setRadius(RADAR.blipRadius)
					local intent = _intentSnapshot[agent:GetObjId()]
					b:setBackgroundColor(intent ~= nil and _IntentColour(intent)
						or ColourValue(0.20, 0.80, 0.30, 1.0))
				end
				b:setPosition(Vector2(RADAR.cx + rgtC * scale, RADAR.cy - fwdC * scale))
				b:setVisible(true)
				slot = slot + 1
			end
		end
	end
	for i = slot, RADAR.poolSize do
		_radar.blips[i]:setVisible(false)
	end
end

-- ===== 指挥层：选择 =====

local function _GetBlackboard(agent)
	if agent == nil then return nil end
	local ai = agent:GetAIComponent()
	return ai ~= nil and ai:GetBlackboard() or nil
end

-- ObjectManager:getObjectById 未导出给 Lua；且跨帧持有 agent userdata 有悬垂风险
-- （对象可能已被销毁）。选择集只存 objId，每次按 id 扫 getAllAgents 解析（7 个 agent，可忽略）。
local function _FindAgentById(objId)
	if objId == nil or objId <= 0 then return nil end
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		local agent = agents[i]
		if agent ~= nil and agent:GetObjId() == objId then
			return agent
		end
	end
	return nil
end

local function _IsSelectableAlly(agent)
	return agent ~= nil and agent ~= _player and agent:GetHealth() > 0
		and _player ~= nil and agent:GetTeamId() == _player:GetTeamId()
end

-- 用 WorldToScreen 投影做拾取：对 2-6 个单位比射线更准（不受胶囊碰撞体形状影响），
-- 点选与框选共用同一次投影。返回 (-1,-1) 表示在相机后方，直接跳过。
local function _ScreenPosOfWorld(worldPos)
	local sp = SandboxCamera:WorldToScreen(worldPos)
	if sp.x < 0 and sp.y < 0 then return nil end
	local width, height = GameManager:getScreenWidth(), GameManager:getScreenHeight()
	if sp.x < 0 or sp.y < 0 or sp.x > width or sp.y > height then return nil end
	return sp
end

local function _ScreenPosOf(agent, yOffset)
	if agent == nil then return nil end
	local worldPos = agent:GetPosition()
	worldPos.y = worldPos.y + (tonumber(yOffset) or 0)
	return _ScreenPosOfWorld(worldPos)
end

local function _ClearSelection()
	_selection = {}
end

local function _PickAt(x, y)
	-- 保留 W1 手感排查能力，但只在显式环境变量开启时输出，避免正式 build 每次点选刷日志。
	if _inputDiagEnabled then
		local sw, sh = GameManager:getScreenWidth(), GameManager:getScreenHeight()
		local parts = {}
		local agents = ObjectManager:getAllAgents()
		for i = 0, agents:size() - 1 do
			local a = agents[i]
			if a ~= nil and a ~= _player and a:GetHealth() > 0 then
				local sp = SandboxCamera:WorldToScreen(a:GetPosition())
				local d = -1
				if not (sp.x < 0 and sp.y < 0) then
					d = math.floor(math.sqrt((sp.x - x) ^ 2 + (sp.y - y) ^ 2))
				end
				table.insert(parts, string.format("id%d(t%d)@%.0f,%.0f d=%d",
					a:GetObjId(), a:GetTeamId(), sp.x, sp.y, d))
			end
		end
		print(string.format("[PickDiag] click=%.0f,%.0f screen=%.0fx%.0f playerTeam=%d | %s",
			x, y, sw, sh, _player ~= nil and _player:GetTeamId() or -1,
			table.concat(parts, " | ")))
	end

	local bestId, bestDistSq = 0, COMMAND.pickRadiusPx * COMMAND.pickRadiusPx
	local bestEnemyId, bestEnemyDistSq = 0, COMMAND.pickRadiusPx * COMMAND.pickRadiusPx
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		local agent = agents[i]
		if agent ~= nil and agent ~= _player and agent:GetHealth() > 0 then
			local sp = _ScreenPosOf(agent)
			if sp ~= nil then
				local dx, dy = sp.x - x, sp.y - y
				local distSq = dx * dx + dy * dy
				if agent:GetTeamId() == _player:GetTeamId() then
					if distSq < bestDistSq then
						bestDistSq = distSq
						bestId = agent:GetObjId()
					end
				elseif distSq < bestEnemyDistSq then
					bestEnemyDistSq = distSq
					bestEnemyId = agent:GetObjId()
				end
			end
		end
	end

	-- 点到敌人：记下来供集火用，不改变友军选择集。
	if bestEnemyId > 0 and (bestId == 0 or bestEnemyDistSq < bestDistSq) then
		_lastPickedEnemyId = bestEnemyId
		_SetHint("Enemy marked - F to focus fire")
		return
	end

	_ClearSelection()
	if bestId > 0 then
		_selection[bestId] = true
		_SetHint("1 ally selected")
	else
		_SetHint("Selection cleared - Tab: select all")
	end
end

local function _BoxSelect(x0, y0, x1, y1)
	local minX, maxX = math.min(x0, x1), math.max(x0, x1)
	local minY, maxY = math.min(y0, y1), math.max(y0, y1)
	_ClearSelection()
	local count = 0
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		local agent = agents[i]
		if _IsSelectableAlly(agent) then
			local sp = _ScreenPosOf(agent)
			if sp ~= nil and sp.x >= minX and sp.x <= maxX and sp.y >= minY and sp.y <= maxY then
				_selection[agent:GetObjId()] = true
				count = count + 1
			end
		end
	end
	_SetHint(count .. " allies selected")
end

-- ===== 指挥层：下令 =====

local function _SelectionCount()
	local n = 0
	for _ in pairs(_selection) do n = n + 1 end
	return n
end

-- 集火兜底：玩家朝向 ±fallbackFovDeg 扇形内最近的存活敌人。
local function _FindEnemyInPlayerCone()
	if _player == nil then return 0 end
	local pp = _player:GetPosition()
	local pf = _player:GetForward()
	local flen = math.sqrt(pf.x * pf.x + pf.z * pf.z)
	if flen < 1e-4 then return 0 end
	local fx, fz = pf.x / flen, pf.z / flen
	local cosLimit = math.cos(math.rad(COMMAND.fallbackFovDeg))

	local bestId, bestDistSq = 0, math.huge
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		local agent = agents[i]
		if agent ~= nil and agent ~= _player and agent:GetHealth() > 0
			and agent:GetTeamId() ~= _player:GetTeamId() then
			local ap = agent:GetPosition()
			local dx, dz = ap.x - pp.x, ap.z - pp.z
			local distSq = dx * dx + dz * dz
			if distSq > 1e-4 then
				local dist = math.sqrt(distSq)
				if (dx / dist * fx + dz / dist * fz) >= cosLimit and distSq < bestDistSq then
					bestDistSq = distSq
					bestId = agent:GetObjId()
				end
			end
		end
	end
	return bestId
end

local function _ClearAgentCommand(bb, reason)
	if bb == nil then return false end
	local hadCommand = bb:Has("command.kind") or bb:Has("command.issuedMs")
	local ownedMovePos = bb:Has("command.targetPos")
	bb:Remove("command.kind")
	bb:Remove("command.issuedMs")
	bb:Remove("command.focusTargetId")
	bb:Remove("command.targetPos")
	if ownedMovePos then
		bb:Remove("movePos")
	end
	if hadCommand and reason ~= nil then
		bb:SetString("__debug.commandClearReason", reason)
		print("[Sandbox19CommandLifecycle] cleared reason=" .. tostring(reason))
	end
	return hadCommand
end

_IntentColour = function(intent)
	local key = intent ~= nil and (intent.commandKind or string.lower(intent.code or "")) or ""
	if key == "focus" or key == "engage" then
		return ColourValue(0.88, 0.20, 0.12, 0.90)
	elseif key == "retreat" then
		return ColourValue(0.10, 0.58, 0.92, 0.90)
	elseif key == "rally" or key == "hold" then
		return ColourValue(0.62, 0.25, 0.88, 0.90)
	elseif key == "search" or key == "investigate" then
		return ColourValue(0.92, 0.68, 0.12, 0.90)
	end
	return ColourValue(0.16, 0.38, 0.52, 0.86)
end

local function _BuildIntentForAgent(agent, nowMs)
	local bb = _GetBlackboard(agent)
	if bb == nil then
		return { code = "NO AI", reason = "controller unavailable" }
	end

	if bb:Has("command.issuedMs") then
		local elapsed = nowMs - bb:GetInt("command.issuedMs", nowMs)
		if elapsed >= 0 and elapsed <= COMMAND.ttlMs then
			local kind = bb:GetString("command.kind")
			local remainingMs = COMMAND.ttlMs - elapsed
			if kind == "focus" then
				local targetId = bb:GetObjectId("command.focusTargetId", 0)
				local target = _FindAgentById(targetId)
				if target ~= nil and target:GetHealth() > 0 then
					return {
						code = "FOCUS",
						reason = "player -> enemy #" .. tostring(targetId),
						commandKind = kind,
						targetId = targetId,
						targetPos = target:GetPosition(),
						remainingMs = remainingMs,
					}
				end
			elseif (kind == "retreat" or kind == "rally") and bb:Has("command.targetPos") then
				local targetPos = bb:GetVec3("command.targetPos")
				local delta = targetPos - agent:GetPosition()
				delta.y = 0
				local reached = delta:squaredLength() <= 2.25
				return {
					code = reached and "HOLD" or string.upper(kind),
					reason = reached and (kind .. " position reached") or ("player " .. kind .. " order"),
					commandKind = kind,
					targetPos = targetPos,
					remainingMs = remainingMs,
				}
			end
		end
	end

	if bb:GetBool("perception.hasTarget", false) then
		local targetId = bb:GetObjectId("perception.targetId", -1)
		return { code = "ENGAGE", reason = "vision -> enemy #" .. tostring(targetId), targetId = targetId }
	end
	if bb:GetBool("memory.snapshot.hasLastKnownEnemy", false) then
		local targetId = bb:GetObjectId("memory.snapshot.lastKnownEnemyId", -1)
		return { code = "SEARCH", reason = "last seen enemy #" .. tostring(targetId) }
	end
	if bb:Has("sense.heardSoundPos") then
		return { code = "INVESTIGATE", reason = "heard danger" }
	end
	if bb:Has("movePos") then
		return { code = "MOVE", reason = "autonomous destination", targetPos = bb:GetVec3("movePos") }
	end
	return { code = "PATROL", reason = "no contact" }
end

local function _MaintainCommands(nowMs)
	for id in pairs(_selection) do
		local agent = _FindAgentById(id)
		if agent == nil or agent:GetHealth() <= 0 then _selection[id] = nil end
	end
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		local agent = agents[i]
		local bb = _GetBlackboard(agent)
		if bb ~= nil then
			local command = Observer.Command(bb, nowMs, COMMAND.ttlMs, _FindAgentById)
			if agent:GetHealth() <= 0 then
				_ClearAgentCommand(bb, "owner-dead")
				_selection[agent:GetObjId()] = nil
			elseif command.reason ~= nil then
				_ClearAgentCommand(bb, command.reason)
			end
		end
	end
end

local function _UpdateObserver(nowMs)
	_observer:Update(_selection, _FindAgentById, nowMs, COMMAND.ttlMs, _profiles)
	_observerPanel:setVisible(_observer.enabled)
	if not _observer.enabled then return end
	_observerPanel:setPosition(Vector2(math.max(4, GameManager:getScreenWidth() - 470), 10))
	_observerPanel:setMarkupText(GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
		table.concat(Observer.Lines(_observer.snapshot), GUI.MarkupNewline))
end

local function _RefreshIntentSnapshot(nowMs)
	_intentSnapshot = {}
	_intentCounts = { focus = 0, retreat = 0, rally = 0 }
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		local agent = agents[i]
		if _IsSelectableAlly(agent) then
			local intent = _BuildIntentForAgent(agent, nowMs)
			_intentSnapshot[agent:GetObjId()] = intent
			if intent.commandKind ~= nil then
				_intentCounts[intent.commandKind] = (_intentCounts[intent.commandKind] or 0) + 1
			end
		end
	end
end

-- 三个指令互斥：写入前清掉另两个键，避免旧指令残留把条件误判成活跃。
local function _WriteCommand(bb, kind, focusTargetId, targetPos, nowMs)
	bb:SetString("command.kind", kind)
	bb:SetInt("command.issuedMs", nowMs)
	if kind == "focus" then
		bb:SetObjectId("command.focusTargetId", focusTargetId)
		bb:Remove("command.targetPos")
		bb:Remove("movePos")
	else
		bb:Remove("command.focusTargetId")
		bb:SetVec3("command.targetPos", targetPos)
		bb:SetVec3("movePos", targetPos)
	end
end

local function _IssueCommand(kind, focusTargetId, basePos)
	if _player == nil then return end
	if _matchState == "VICTORY" or _matchState == "DEFEAT" then
		_SetHint("match finished - press Enter")
		return
	end
	if _SelectionCount() == 0 then
		_SetHint("No allies selected - press Tab")
		return
	end

	local nowMs = GameManager:getTimeInMillis()
	local teamId = _player:GetTeamId()
	local focusTarget = kind == "focus" and _FindAgentById(focusTargetId) or nil
	local focusTargetPos = focusTarget ~= nil and focusTarget:GetPosition() or _player:GetPosition()
	local slotIndex = 0
	local issued = 0

	for objId in pairs(_selection) do
		local agent = _FindAgentById(objId)
		if agent ~= nil and agent:GetHealth() > 0 then
			local bb = _GetBlackboard(agent)
			if bb ~= nil then
				local targetPos = basePos
				if kind == "rally" then
					-- 在玩家周围一字排开，避免所有单位挤同一点。
					local offset = (slotIndex - 0.5) * COMMAND.rallySpacing
					local pf = _player:GetForward()
					local flen = math.sqrt(pf.x * pf.x + pf.z * pf.z)
					if flen > 1e-4 then
						local rx, rz = -pf.z / flen, pf.x / flen
						targetPos = Vector3(basePos.x + rx * offset, basePos.y, basePos.z + rz * offset)
					end
				end
				_WriteCommand(bb, kind, focusTargetId, targetPos, nowMs)

				-- 同步 typed fact：团队级记录 + TTL，供 RuntimeDiag / 后续可视化读取。
				if kind == "focus" then
					TeamBlackboard:RememberFocusTarget(teamId, {
						teamId = teamId,
						sourceAgentId = _player:GetObjId(),
						targetId = focusTargetId,
						targetPos = focusTargetPos,
						timeMs = nowMs,
						confidence = 1.0,
						ttlMs = COMMAND.ttlMs,
						key = "playerCommand",
					})
				elseif kind == "retreat" then
					TeamBlackboard:RememberRetreatPoint(teamId, {
						teamId = teamId,
						agentId = objId,
						retreatPos = targetPos,
						timeMs = nowMs,
						confidence = 1.0,
						ttlMs = COMMAND.ttlMs,
						key = "playerCommand:" .. tostring(objId),
					})
				else
					TeamBlackboard:RememberFormationSlot(teamId, {
						teamId = teamId,
						agentId = objId,
						focusTargetId = -1,
						slotPos = targetPos,
						timeMs = nowMs,
						ttlMs = COMMAND.ttlMs,
					})
				end

				slotIndex = slotIndex + 1
				issued = issued + 1
			end
		end
	end

	_SetHint(string.upper(kind) .. ": " .. issued .. " allies | lasts 8s")
	print("[Sandbox19Command] kind=" .. kind .. " issued=" .. issued ..
		" focusTargetId=" .. tostring(focusTargetId))
end

local function _IssueFocus()
	if _SelectionCount() == 0 then
		_SetHint("No allies selected - press Tab")
		return
	end
	local targetId = _lastPickedEnemyId
	if targetId <= 0 then
		targetId = _FindEnemyInPlayerCone()
	else
		local marked = _FindAgentById(targetId)
		if marked == nil or marked:GetHealth() <= 0 then
			_lastPickedEnemyId = 0
			targetId = _FindEnemyInPlayerCone()
		end
	end
	if targetId <= 0 then
		_SetHint("No target - RMB an enemy, then F")
		return
	end
	_IssueCommand("focus", targetId, nil)
end

-- 每帧刷新指挥 UI：框选矩形跟随拖拽，选中单位头顶投影处摆一个绿框。
local function _UpdateCommandUi()
	if _dragRect ~= nil then
		if _dragging then
			local x0 = math.min(_dragStart.x, _dragNow.x)
			local y0 = math.min(_dragStart.y, _dragNow.y)
			local w = math.abs(_dragNow.x - _dragStart.x)
			local h = math.abs(_dragNow.y - _dragStart.y)
			_dragRect:setPosition(Vector2(x0, y0))
			_dragRect:setDimension(Vector2(math.max(w, 1), math.max(h, 1)))
			_dragRect:setVisible(true)
		else
			_dragRect:setVisible(false)
		end
	end

	local slot = 1
	for objId in pairs(_selection) do
		if slot > SELECT_MARK_POOL then break end
		local agent = _FindAgentById(objId)
		if agent ~= nil and agent:GetHealth() > 0 then
			local sp = _ScreenPosOf(agent)
			if sp ~= nil then
				local m = _selectMarks[slot]
				m:setPosition(Vector2(sp.x - SELECT_MARK_SIZE * 0.5, sp.y - SELECT_MARK_SIZE * 0.5))
				m:setVisible(true)
				slot = slot + 1
			end
		end
	end
	for i = slot, SELECT_MARK_POOL do
		if _selectMarks[i] ~= nil then
			_selectMarks[i]:setVisible(false)
		end
	end
end

local function _RectsOverlap(a, b)
	return a.x < b.x + b.w and a.x + a.w > b.x
		and a.y < b.y + b.h and a.y + a.h > b.y
end

local function _ResolveIntentCardRect(screen, placedCards)
	local screenWidth, screenHeight = GameManager:getScreenWidth(), GameManager:getScreenHeight()
	local rect = {
		x = math.max(4, math.min(screenWidth - INTENT_CARD_WIDTH - 4,
			screen.x - INTENT_CARD_WIDTH * 0.5)),
		y = math.max(4, math.min(screenHeight - INTENT_CARD_HEIGHT - 4,
			screen.y - INTENT_CARD_HEIGHT)),
		w = INTENT_CARD_WIDTH,
		h = INTENT_CARD_HEIGHT,
	}

	-- 靠近左侧 HUD 时先移到 HUD 右边；友军聚在一起时再逐张向上堆叠。
	if _RectsOverlap(rect, INTENT_HUD_BOUNDS) then
		rect.x = math.min(screenWidth - rect.w - 4,
			INTENT_HUD_BOUNDS.x + INTENT_HUD_BOUNDS.w + INTENT_CARD_GAP)
	end
	if _observer.enabled then
		local panel = { x = math.max(4, screenWidth - 470), y = 10, w = 460, h = 290 }
		if _RectsOverlap(rect, panel) then
			rect.y = math.min(screenHeight - rect.h - 4, panel.y + panel.h + INTENT_CARD_GAP)
		end
	end
	for i = 1, #placedCards do
		if _RectsOverlap(rect, placedCards[i]) then
			local above = placedCards[i].y - rect.h - INTENT_CARD_GAP
			if above >= 4 then
				rect.y = above
			else
				rect.y = math.min(screenHeight - rect.h - 4,
					placedCards[i].y + placedCards[i].h + INTENT_CARD_GAP)
			end
		end
	end
	return rect
end

local function _UpdateIntentVisuals()
	local slot = 1
	local focusLabelShown = false
	local placedCards = {}
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		local agent = agents[i]
		if _IsSelectableAlly(agent) and slot <= INTENT_CARD_POOL then
			local intent = _intentSnapshot[agent:GetObjId()]
			local screen = _ScreenPosOf(agent, 2.5)
			if intent ~= nil and screen ~= nil then
				local colour = _IntentColour(intent)
				local card = _intentCards[slot]
				local cardRect = _ResolveIntentCardRect(screen, placedCards)
				local remaining = intent.remainingMs ~= nil and ("  " .. _FormatSeconds(intent.remainingMs)) or ""
				card:setPosition(Vector2(cardRect.x, cardRect.y))
				card:setBackgroundColor(colour)
				card:setMarkupText(GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
					"#" .. tostring(agent:GetObjId()) .. "  " .. intent.code .. remaining ..
					GUI.MarkupNewline .. intent.reason)
				card:setVisible(true)
				table.insert(placedCards, cardRect)

				local targetMarker = _intentTargets[slot]
				if intent.commandKind ~= nil and intent.commandKind ~= "focus" and intent.targetPos ~= nil then
					local targetWorld = intent.targetPos
					targetWorld.y = targetWorld.y + 0.2
					local targetScreen = _ScreenPosOfWorld(targetWorld)
					if targetScreen ~= nil then
						targetMarker:setPosition(targetScreen)
						targetMarker:setBackgroundColor(colour)
						targetMarker:setVisible(true)
					else
						targetMarker:setVisible(false)
					end
				else
					targetMarker:setVisible(false)
				end

				if not focusLabelShown and intent.commandKind == "focus" and intent.targetId ~= nil then
					local target = _FindAgentById(intent.targetId)
					local targetScreen = target ~= nil and _ScreenPosOf(target, 2.6) or nil
					if targetScreen ~= nil then
						_focusTargetLabel:setPosition(Vector2(targetScreen.x - 59, targetScreen.y - 22))
						_focusTargetLabel:setVisible(true)
						focusLabelShown = true
					end
				end
				slot = slot + 1
			end
		end
	end

	for i = slot, INTENT_CARD_POOL do
		if _intentCards[i] ~= nil then _intentCards[i]:setVisible(false) end
		if _intentTargets[i] ~= nil then _intentTargets[i]:setVisible(false) end
	end
	if not focusLabelShown and _focusTargetLabel ~= nil then
		_focusTargetLabel:setVisible(false)
	end
end

-- smoke-only 自测：headless smoke 驱动不了鼠标键盘，这里用合成指令走通
-- "写 blackboard -> 指令条件命中 -> TTL 过期后回落" 全链路，作为指令层的自动化证据。
local function _RunCommandSelfTest()
	local ally = nil
	for _, agent in ipairs(_agents) do
		if _IsSelectableAlly(agent) then ally = agent break end
	end
	if ally == nil then
		print("[Sandbox19CommandSelfTest] FAIL reason=noAlly")
		return
	end

	local bb = _GetBlackboard(ally)
	local conds = _G["Sandbox19CommandConditions"]
	if bb == nil or conds == nil then
		print("[Sandbox19CommandSelfTest] FAIL reason=noBlackboardOrConditions")
		return
	end

	-- 1) 撤退指令：写 movePos + command.kind，条件应命中
	_ClearSelection()
	_selection[ally:GetObjId()] = true
	_IssueCommand("retreat", -1, _player:GetPosition())
	local retreatOk = conds.HasCommandRetreat(ally, bb) == true
	local rallyOff = conds.HasCommandRally(ally, bb) == false
	local retreatIntent = _BuildIntentForAgent(ally, GameManager:getTimeInMillis())
	local retreatIntentOk = retreatIntent.commandKind == "retreat"
	require("res.scripts.samples.sandbox19_retreat_selftest.lua").Run(ally, bb, conds)

	-- 2) 集火指令：条件命中时应把 blackboard.enemy 覆写为指定目标
	local enemyId = _FindEnemyInPlayerCone()
	local focusOk, enemyOverridden = false, false
	if enemyId <= 0 then
		local agents = ObjectManager:getAllAgents()
		for i = 0, agents:size() - 1 do
			local a = agents[i]
			if a ~= nil and a:GetHealth() > 0 and a:GetTeamId() ~= _player:GetTeamId() then
				enemyId = a:GetObjId()
				break
			end
		end
	end
	if enemyId > 0 then
		_IssueCommand("focus", enemyId, nil)
		focusOk = conds.HasCommandFocus(ally, bb) == true
		local overridden = bb:GetAgent("enemy")
		enemyOverridden = overridden ~= nil and overridden:GetObjId() == enemyId
	end
	local focusIntent = _BuildIntentForAgent(ally, GameManager:getTimeInMillis())
	local focusIntentOk = focusIntent.commandKind == "focus" and focusIntent.targetId == enemyId

	-- 3) TTL：把 issuedMs 推到超出 TTL 的过去，条件应全部落空
	bb:SetInt("command.issuedMs", GameManager:getTimeInMillis() - COMMAND.ttlMs - 1000)
	local expiredOk = conds.HasCommandFocus(ally, bb) == false
		and conds.HasCommandRetreat(ally, bb) == false
		and conds.HasCommandRally(ally, bb) == false
	_MaintainCommands(GameManager:getTimeInMillis())
	local lifecycleCleared = not bb:Has("command.kind") and not bb:Has("command.issuedMs")

	_ClearSelection()
	_ClearAgentCommand(bb, nil)

	local pass = retreatOk and rallyOff and focusOk and enemyOverridden and expiredOk
		and retreatIntentOk and focusIntentOk and lifecycleCleared
	print("[Sandbox19CommandSelfTest] " .. (pass and "PASS" or "FAIL") ..
		" retreat=" .. tostring(retreatOk) ..
		" rallyExclusive=" .. tostring(rallyOff) ..
		" focus=" .. tostring(focusOk) ..
		" enemyOverridden=" .. tostring(enemyOverridden) ..
		" ttlExpired=" .. tostring(expiredOk))
	print("[Sandbox19IntentSelfTest] " .. (pass and "PASS" or "FAIL") ..
		" retreatIntent=" .. tostring(retreatIntentOk) ..
		" focusIntent=" .. tostring(focusIntentOk) ..
		" lifecycleCleared=" .. tostring(lifecycleCleared))
end

local function _ClearCommandState()
	_observer:Release(_FindAgentById)
	_profiles = {}
	_ClearSelection()
	_dragging = false
	_lastPickedEnemyId = 0
	_commandHint = ""
	_intentSnapshot = {}
	_intentCounts = { focus = 0, retreat = 0, rally = 0 }
	if TeamBlackboard ~= nil and TeamBlackboard.Reset ~= nil then
		TeamBlackboard:Reset()
	end
end

local function _SelectAllLivingAllies()
	_ClearSelection()
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		local agent = agents[i]
		if _IsSelectableAlly(agent) then
			_selection[agent:GetObjId()] = true
		end
	end
end

local function _ClearSquadCommands(reason)
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		local agent = agents[i]
		if _IsSelectableAlly(agent) then
			_ClearAgentCommand(_GetBlackboard(agent), reason)
		end
	end
	if TeamBlackboard ~= nil and TeamBlackboard.Reset ~= nil then
		TeamBlackboard:Reset()
	end
end

local function _RecoverSquad()
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		local agent = agents[i]
		if _IsSelectableAlly(agent) then
			agent:SetHealth(math.min(100, agent:GetHealth() + 25))
			local weapon = agent:GetWeaponComponent()
			if weapon ~= nil then weapon:RestoreAmmo() end
		end
	end
end

local function _SpawnWaveEnemy(waveIndex, slotIndex)
	local spawnPattern = MATCH.waveSpawnIndices[waveIndex] or MATCH.waveSpawnIndices[#MATCH.waveSpawnIndices]
	local spawnIndex = spawnPattern[((slotIndex - 1) % #spawnPattern) + 1]
	local aiScript = "res/scripts/agent/BehaviorSoldierAgent.lua"
	local enemy = Create_SoldierWithProfile(aiScript, Soldier.AppearanceTypes.DARK, 0, "ai_soldier")
	if not enemy:HasComponent("ai") or enemy:HasComponent("player") then
		error("[Sandbox19] wave enemy must contain ai and exclude player")
	end
	_profiles[enemy:GetObjId()] = "ai_soldier"
	table.insert(_agents, enemy)
	ConfigManager:PlaceAgentOnPresetSpawn(enemy, _sampleName, spawnIndex, "default")
	enemy:SetForward(Vector3(0, 0, -1))
	return enemy
end

local function _FindNearestOpponent(agent)
	local nearest, nearestDistSq = nil, math.huge
	local position = agent:GetPosition()
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		local candidate = agents[i]
		if candidate ~= nil and candidate ~= agent and candidate:GetHealth() > 0
			and candidate:GetTeamId() ~= agent:GetTeamId() then
			local delta = candidate:GetPosition() - position
			local distSq = delta:squaredLength()
			if distSq < nearestDistSq then
				nearest, nearestDistSq = candidate, distSq
			end
		end
	end
	return nearest
end

local function _IsFinitePosition(position)
	local function finite(value)
		return value == value and value > -100000 and value < 100000
	end
	return position ~= nil and finite(position.x) and finite(position.y) and finite(position.z)
end

local function _BuildContactPosition(opponent, slotIndex)
	local opponentPosition = opponent:GetPosition()
	if not _IsFinitePosition(opponentPosition) then
		local fallback = _player:GetPosition()
		return Vector3(fallback.x, fallback.y, fallback.z)
	end

	-- 失联单位只在第二次超时后被拉回最近对手周围；固定有限偏移避免导航路径
	-- 的异常点直接进入 Bullet/Ogre transform，也避免多个敌人叠在同一点。
	local side = slotIndex % 2 == 0 and -1 or 1
	local ring = math.floor(slotIndex / 2)
	return Vector3(
		opponentPosition.x + side * (3.0 + ring * 1.5),
		opponentPosition.y,
		opponentPosition.z + (slotIndex % 3 - 1) * 1.5)
end

local function _ForceWaveContact()
	local moved = 0
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		local agent = agents[i]
		if agent ~= nil and agent:GetHealth() > 0
			and agent:GetTeamId() ~= _player:GetTeamId() then
			local opponent = _FindNearestOpponent(agent)
			if opponent ~= nil then
				local contactPosition = _BuildContactPosition(opponent, moved + 1)
				if _IsFinitePosition(contactPosition) then
					agent:SetVelocity(Vector3(0, 0, 0))
					agent:setPosition(contactPosition)
					local bb = _GetBlackboard(agent)
					if bb ~= nil then bb:SetVec3("movePos", opponent:GetPosition()) end
					moved = moved + 1
				end
			end
		end
	end
	return moved
end

local function _ConvergeStalledWave(nowMs, enemyAlive)
	if enemyAlive < _waveLastEnemyAlive then
		_waveLastEnemyAlive = enemyAlive
		_waveLastProgressMs = nowMs
		_waveConvergeCount = 0
		return
	end
	if nowMs - _waveLastProgressMs < MATCH.stalemateMs then return end

	_waveConvergeCount = _waveConvergeCount + 1
	if _waveConvergeCount >= 2 then
		local moved = _ForceWaveContact()
		_waveLastProgressMs = nowMs
		_SetHint("Enemies regrouped nearby")
		print("[Sandbox19Match] phase=FORCE_CONTACT wave=" .. tostring(_waveIndex) ..
			" enemies=" .. tostring(enemyAlive) ..
			" moved=" .. tostring(moved) ..
			" elapsedMs=" .. tostring(nowMs - _matchStartedMs))
		return
	end

	local redirected = 0
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		local agent = agents[i]
		if agent ~= nil and agent ~= _player and agent:GetHealth() > 0 then
			local bb = _GetBlackboard(agent)
			local opponent = _FindNearestOpponent(agent)
			if bb ~= nil and opponent ~= nil and not bb:Has("command.issuedMs") then
				bb:SetVec3("movePos", opponent:GetPosition())
				redirected = redirected + 1
			end
		end
	end

	_waveLastProgressMs = nowMs
	_SetHint("contact lost - squads converging")
	print("[Sandbox19Match] phase=CONVERGE wave=" .. tostring(_waveIndex) ..
		" enemies=" .. tostring(enemyAlive) ..
		" redirected=" .. tostring(redirected) ..
		" elapsedMs=" .. tostring(nowMs - _matchStartedMs))
end

local function _StartNextWave(nowMs)
	_waveIndex = _waveIndex + 1
	local enemyCount = MATCH.waveEnemyCounts[_waveIndex] or 0
	_matchState = "WAVE"
	_phaseDeadlineMs = 0
	for slot = 1, enemyCount do
		_SpawnWaveEnemy(_waveIndex, slot)
	end
	_waveLastEnemyAlive = enemyCount
	_waveLastProgressMs = nowMs
	_waveConvergeCount = 0
	_SelectAllLivingAllies()
	_SetHint("wave " .. tostring(_waveIndex) .. " incoming - squad selected")
	print("[Sandbox19Match] phase=WAVE wave=" .. tostring(_waveIndex) ..
		" enemies=" .. tostring(enemyCount) ..
		" elapsedMs=" .. tostring(nowMs - _matchStartedMs))
end

local function _EnterIntermission(nowMs)
	_matchState = "INTERMISSION"
	_phaseDeadlineMs = nowMs + MATCH.intermissionMs
	_ClearSquadCommands("wave-complete")
	_RecoverSquad()
	_SelectAllLivingAllies()
	_SetHint("Wave clear - squad healed")
	print("[Sandbox19Match] phase=INTERMISSION completedWave=" .. tostring(_waveIndex) ..
		" nextInMs=" .. tostring(MATCH.intermissionMs))
end

local function _SetTerminalMatchState(state, nowMs)
	_matchEndReason = state == "VICTORY" and "All waves cleared - well done!"
		or (_player:GetHealth() <= 0 and "Defeat: commander down" or "Defeat: squad eliminated")
	_matchEndedMs = nowMs
	_matchState = state
	_phaseDeadlineMs = 0
	_ClearSquadCommands("match-" .. string.lower(state))
	_SetHint(state .. " - press Enter to restart")
	print("[Sandbox19Match] phase=" .. state ..
		" wave=" .. tostring(_waveIndex) ..
		" elapsedMs=" .. tostring(nowMs - _matchStartedMs))
end

local function _UpdateMatchFlow(nowMs)
	if _player == nil or _matchState == "VICTORY" or _matchState == "DEFEAT" then return end
	local allyAlive, enemyAlive = _CountAliveAgents()
	if _player:GetHealth() <= 0 or allyAlive <= 0 then
		_SetTerminalMatchState("DEFEAT", nowMs)
		return
	end
	if _matchState == "PREPARE" then
		if nowMs >= _phaseDeadlineMs then _StartNextWave(nowMs) end
	elseif _matchState == "WAVE" then
		if enemyAlive <= 0 then
			if _waveIndex >= #MATCH.waveEnemyCounts then
				_SetTerminalMatchState("VICTORY", nowMs)
			else
				_EnterIntermission(nowMs)
			end
		else
			_ConvergeStalledWave(nowMs, enemyAlive)
		end
	elseif _matchState == "INTERMISSION" and nowMs >= _phaseDeadlineMs then
		_StartNextWave(nowMs)
	end
end

local function _RunMatchSelfTest()
	local _, enemyAlive = _CountAliveAgents()
	local preset = ConfigManager:GetSamplePreset(_sampleName)
	local commanderUnarmed = _player ~= nil and _player:GetWeaponComponent() == nil
	local waveStarted = _matchState == "WAVE" and _waveIndex == 1
	local enemyCountOk = enemyAlive == MATCH.waveEnemyCounts[1]
	local fixedSeed = preset ~= nil and tonumber(preset.seed) == 20260710
	local contactPositionFinite = _player ~= nil and _IsFinitePosition(_BuildContactPosition(_player, 1))
	local pass = commanderUnarmed and waveStarted and enemyCountOk and fixedSeed
		and contactPositionFinite and #MATCH.waveEnemyCounts >= 3
	print("[Sandbox19MatchSelfTest] " .. (pass and "PASS" or "FAIL") ..
		" commanderUnarmed=" .. tostring(commanderUnarmed) ..
		" waveStarted=" .. tostring(waveStarted) ..
		" enemyCount=" .. tostring(enemyAlive) ..
		" fixedSeed=" .. tostring(fixedSeed) ..
		" contactPositionFinite=" .. tostring(contactPositionFinite) ..
		" waves=" .. tostring(#MATCH.waveEnemyCounts))
end

local function _SpawnEncounter()
	_matchEndedMs = nil
	_matchEndReason = ""
	_agents = {}
	_matchState = "PREPARE"
	_restartRequested = false
	_waveIndex = 0
	_matchStartedMs = GameManager:getTimeInMillis()
	_phaseDeadlineMs = _matchStartedMs + MATCH.prepareMs
	_waveLastProgressMs = _matchStartedMs
	_waveLastEnemyAlive = 0
	_waveConvergeCount = 0

	local humanScript = "res/scripts/agent/HumanSoldierAgent.lua"
	_player = Create_SoldierWithProfile(humanScript, Soldier.AppearanceTypes.LIGHT, 1, "commander_soldier")
	if not _player:HasComponent("player") or _player:HasComponent("ai") or _player:HasComponent("weapon") then
		error("[Sandbox19] commander_soldier must contain player and exclude ai/weapon")
	end
	_profiles[_player:GetObjId()] = "commander_soldier"
	table.insert(_agents, _player)
	ConfigManager:PlaceAgentOnPresetSpawn(_player, _sampleName, 1, "default")

	local aiScript = "res/scripts/agent/BehaviorSoldierAgent.lua"
	for i = 1, MATCH.allyCount do
		local agent = Create_SoldierWithProfile(aiScript, Soldier.AppearanceTypes.LIGHT, 1, "ai_soldier")
		if not agent:HasComponent("ai") or agent:HasComponent("player") then
			error("[Sandbox19] ai_soldier profile must contain ai and exclude player")
		end
		_profiles[agent:GetObjId()] = "ai_soldier"
		table.insert(_agents, agent)
		ConfigManager:PlaceAgentOnPresetSpawn(agent, _sampleName, i + 1, "default")
	end
	_SelectAllLivingAllies()
	_SetHint("deploy - squad selected")

	print("[Sandbox19] ready playerId=" .. tostring(_player:GetObjId()) ..
		" role=commander components=" .. tostring(_player:BuildComponentDebugString()))
	-- blip 池在 _CreateRadar 已建好并复用；_UpdateRadar 每帧按存活单位重定位/隐藏，无需重建。

	if _G.HELLO_SANDBOX_SMOKE_MODE == true or _observationTest ~= nil then
		_StartNextWave(_matchStartedMs)
		_RunCommandSelfTest()
		if TeamBlackboard ~= nil and TeamBlackboard.Reset ~= nil then TeamBlackboard:Reset() end
		_SelectAllLivingAllies()
		_RunMatchSelfTest()
	end
end

local function _RestartEncounter()
	_player = nil
	_agents = {}
	_ClearCommandState()
	ObjectManager:clearAllObjects(MGR_OBJ_AGENT)
	_SpawnEncounter()
end

function EventHandle_Keyboard(keycode, pressed)
	GUI_HandleKeyEvent(keycode, pressed)
	if not pressed then return end

	if keycode == OIS.KC_RETURN then
		_restartRequested = true
	elseif keycode == OIS.KC_TAB then
		_SelectAllLivingAllies()
		_SetHint(_SelectionCount() .. " allies selected | F / T / G")
	elseif keycode == OIS.KC_I then
		_observer.enabled = not _observer.enabled
	elseif keycode == OIS.KC_O then
		_observer:Dump()
	elseif keycode == OIS.KC_F then
		_IssueFocus()
	elseif keycode == OIS.KC_T then
		-- 撤退="到我这来"。用 T 而非 R：R 已是 PlayerController 的换弹键。
		if _player ~= nil then
			_IssueCommand("retreat", -1, _player:GetPosition())
		end
	elseif keycode == OIS.KC_G then
		if _player ~= nil then
			_IssueCommand("rally", -1, _player:GetPosition())
		end
	end
end

-- ctype: 0=move / 1=down / 2=up；button 沿用 OIS 数值（左 0 / 右 1）。
-- 只处理右键选择；commander_soldier 没有 WeaponComponent，左键不会产生射击。
function EventHandle_Mouse(ctype, x, y, button)
	if _inputDiagEnabled and ctype ~= 0 then
		print(string.format("[MouseDiag] ctype=%s x=%s y=%s button=%s",
			tostring(ctype), tostring(x), tostring(y), tostring(button)))
	end
	if ctype == 0 then
		if _dragging then
			_dragNow.x, _dragNow.y = x, y
		end
		return
	end

	if button ~= 1 then return end

	if ctype == 1 then
		_dragging = true
		_dragStart.x, _dragStart.y = x, y
		_dragNow.x, _dragNow.y = x, y
	elseif ctype == 2 and _dragging then
		_dragging = false
		local dx, dy = x - _dragStart.x, y - _dragStart.y
		if math.sqrt(dx * dx + dy * dy) >= COMMAND.dragThresholdPx then
			_BoxSelect(_dragStart.x, _dragStart.y, x, y)
		else
			_PickAt(x, y)
		end
	end
end

function EventHandle_WindowResized(width, height)
	GUI_WindowResized(width, height)
	_LayoutCrosshair(width, height)
end

-- 此场地只服务指挥切片；其它 AI 章节保留原来的高台/坡道关卡。
local function _CreateCommandArena()
	_G.SandboxLevelBoxes = {}
	-- 沿用已验证的立方体生成路径，拼成无高差平台。
	-- 长方体在当前 procedural 渲染路径会出现竖直薄片，不在此处扩改第三方生成器。
	for _, x in ipairs({ -16, 0, 16 }) do
		for _, z in ipairs({ -16, 0, 16, 32 }) do
			CreateLevelBox(16, Vector3(x, -8, z), Vector3(0, 0, 0))
		end
	end
	-- 中央 x=-6..6 保持连通与可见，掩体留在两翼。
	for _, x in ipairs({ -10, 10 }) do
		for _, z in ipairs({ 8, 10, 22, 24 }) do
			CreateLevelBox(2, Vector3(x, 1, z), Vector3(0, 0, 0))
		end
	end
end

local function _RunArenaSelfTest()
	if _G.HELLO_SANDBOX_SMOKE_MODE ~= true then return end
	local points = ConfigManager:GetSamplePreset(_sampleName).spawnPoints
	local start = Vector3(points[1][1], points[1][2], points[1][3])
	-- 只验收本遭遇战引用的槽位；配置数组替换语义另由 test_config_presets 覆盖。
	local used = {}
	for index = 1, MATCH.allyCount + 1 do used[index] = true end
	for _, wave in ipairs(MATCH.waveSpawnIndices) do
		for _, index in ipairs(wave) do used[index] = true end
	end
	local pass = true
	for index = 1, #points do
		if used[index] then
			local point = points[index]
			local intended = Vector3(point[1], point[2], point[3])
			local projected = SandboxNav:FindClosestPoint("default", intended)
			local path = std.vector_Ogre__Vector3_()
			local found = SandboxNav:FindPath("default", start, intended, path)
			local nearSpawn = _IsFinitePosition(projected) and (projected - intended):squaredLength() < 1
			local reachesSpawn = found and path:size() > 0
				and (path[path:size() - 1] - intended):squaredLength() < 1
			local ok = nearSpawn and reachesSpawn
			pass = pass and ok
			print("[Sandbox19ArenaSelfTest] " .. (ok and "PASS" or "FAIL") .. " spawn=" .. index)
		end
	end
	print("[Sandbox19ArenaSelfTest] " .. (pass and "PASS" or "FAIL") .. " all-spawns-connected")
end

function Sandbox_Initialize()
	GUI_CreateCameraAndProfileInfo()
	GUI_CreateSandboxText(infoText, { w = 430, h = 210 })
	_LoadMatchConfig()
	_CreateHud()
	_CreateRadar()
	_LayoutCrosshair(GameManager:getScreenWidth(), GameManager:getScreenHeight())

	SandboxAgentConfig:SetUseCppFsmFlag(true)
	local camera = SandboxCamera:GetCamera()
	camera:setPosition(Vector3(-5, 4, 13))
	camera:setOrientation(Quaternion(-90, 0, -180))

	SandboxScene:SetSkyBox("ThickCloudsWaterSkyBox", Vector3(0, 180, 0))
	local plane = SandboxObjects:CreatePlane(200, 200)
	plane:setPosition(Vector3(0, -10, 0))
	plane:setMaterial("Ground2")

	SandboxScene:SetAmbientLight(Vector3(0.3))
	local directLight = SandboxScene:CreateDirectionalLight(Vector3(1, -1, 1))
	directLight:setDiffuseColour(ColourValue(1.8, 1.4, 0.9))
	directLight:setSpecularColour(ColourValue(1.8, 1.4, 0.9))

	_CreateCommandArena()
	SandboxScene:UpdateSceneGraph()

	local navMeshConfig = rcConfig()
	SandboxNav:DefaultConfig(navMeshConfig)
	SandboxNav:ApplySettingConfig(navMeshConfig, 0.0, 0.4, 0.2)
	navMeshConfig.minRegionArea = math.pow(250, 2)
	navMeshConfig.walkableSlopeAngle = 45
	SandboxNav:CreateNavigationMesh(navMeshConfig, "default")
	_RunArenaSelfTest()

	print(ConfigManager:BuildDebugSummary(_sampleName))
	_SpawnEncounter()
	if os.getenv("HELLO_LOCOMOTION_SELF_TEST") == "1" then
		require("res.scripts.samples.locomotion_selftest.lua").Run(_agents[2], _agents[3])
	end
end

local function _UpdateObservationTest(nowMs)
	if _observationTest == nil or _observationTest.done then return end
	_observationTest:Step({
		find = _FindAgentById,
		snapshot = function() return _observer.snapshot end,
		select = function(id) _ClearSelection(); _selection[id] = true end,
		pickPair = function()
			local allyId, enemyId = nil, nil
			local agents = ObjectManager:getAllAgents()
			for i = 0, agents:size() - 1 do
				local a = agents[i]
				if _IsSelectableAlly(a) and allyId == nil then allyId = a:GetObjId() end
				if a:GetTeamId() ~= _player:GetTeamId() and enemyId == nil then enemyId = a:GetObjId() end
			end
			return allyId, enemyId
		end,
		focus = function(id, targetId)
			_ClearSelection(); _selection[id] = true
			_IssueCommand("focus", targetId, nil)
		end,
		retreat = function(id)
			_ClearSelection(); _selection[id] = true
			_IssueCommand("retreat", -1, _player:GetPosition())
		end,
		hideEnemies = function()
			local agents = ObjectManager:getAllAgents()
			for i = 0, agents:size() - 1 do
				local a = agents[i]
				if a:GetTeamId() ~= _player:GetTeamId() then
					a:setPosition(Vector3(300 + i * 3, 0.8, 300))
					a:SetVelocity(Vector3(0, 0, 0))
				end
			end
		end,
		restart = _RestartEncounter,
	}, nowMs)
end

local function _UpdateRetreatPolicy(nowMs)
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		local agent = agents[i]
		if _profiles[agent:GetObjId()] == "ai_soldier" then
			local bb = _GetBlackboard(agent)
			if bb ~= nil then RetreatPolicy.Update(agent, bb, nowMs, MATCH.criticalRetreatMs) end
		end
	end
end

function Sandbox_Update(deltaTimeInMillis)
	-- The client can tick Lua before scene initialization has created the UI.
	if _observerPanel == nil or _player == nil then return end
	GUI_UpdateCameraInfo()
	GUI_UpdateProfileInfo()

	if _restartRequested then
		_RestartEncounter()
	end

	local nowMs = GameManager:getTimeInMillis()
	_UpdateMatchFlow(nowMs)
	_UpdateRetreatPolicy(nowMs)
	_MaintainCommands(nowMs)
	_RefreshIntentSnapshot(nowMs)
	_UpdateObserver(nowMs)

	_UpdateHud()
	_UpdateRadar()
	_UpdateCommandUi()
	_UpdateIntentVisuals()
	_UpdateObservationTest(nowMs)
	if _stabilityTest ~= nil then
		_stabilityTest:Step({
			playerId = _player:GetObjId(), state = _matchState, wave = _waveIndex,
			isCombatAgent = function(agent) return _profiles[agent:GetObjId()] == "ai_soldier" end,
			restart = function() _restartRequested = true end,
			clearWave = function()
				local agents = ObjectManager:getAllAgents()
				for i = 0, agents:size() - 1 do
					local agent = agents[i]
					if _profiles[agent:GetObjId()] ~= nil and agent:GetTeamId() ~= _player:GetTeamId()
						and agent:GetHealth() > 0 then agent:SetHealth(0) end
				end
			end,
		}, nowMs)
	end
end
