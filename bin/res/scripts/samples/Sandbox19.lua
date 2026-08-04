-- Sandbox19.lua
-- Playable tactical encounter: one player-controlled SoldierObject and six AI soldiers.

require("res.scripts.agent.SoldierAgent.lua")
require("res.scripts.agent.BehaviorSoldierAgent.lua")
-- 指令条件表必须在 agent 建树前进入 _G：BehaviorSoldierAgent 按 preset 的
-- conditionsGlobal 名字查 _G，查不到会静默回落到 SoldierConditions。
require("res.scripts.ai.behavior.Sandbox19CommandConditions.lua")

local _sampleName = "Sandbox19"
local _agents = {}
local _player = nil
local _hud = nil
local _crosshair = nil
local _matchState = "FIGHT"
local _restartRequested = false

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

-- 指挥层：玩家是小队长，LMB 仍是射击，RMB 用于选择友军（点选/框选），F/T/G 下令。
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

local _selection = {}          -- objId -> true
local _dragging = false
local _dragStart = { x = 0, y = 0 }
local _dragNow = { x = 0, y = 0 }
local _lastPickedEnemyId = 0
local _commandHint = ""

local SELECT_MARK_POOL = 4       -- Sandbox19 友方 AI 只有 2 个，留一倍余量
local SELECT_MARK_SIZE = 18
local _dragRect = nil
local _selectMarks = {}

local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
	"[Sandbox19 - Playable Encounter]" .. GUI.MarkupNewline ..
	"W/S: move forward/back" .. GUI.MarkupNewline ..
	"A/D: turn left/right" .. GUI.MarkupNewline ..
	"Shift: sprint" .. GUI.MarkupNewline ..
	"LMB: fire    R: reload" .. GUI.MarkupNewline ..
	"RMB: select ally (click / drag box)" .. GUI.MarkupNewline ..
	"F: focus fire   T: fall back   G: rally" .. GUI.MarkupNewline ..
	"Enter: restart encounter"

local function _CreateHud()
	_hud = SandboxUI:CreateUIFrame()
	_hud:setPosition(Vector2(20, 188))   -- 下移避开左上角雷达
	_hud:setDimension(Vector2(330, 158))
	_hud:setTextMargin(12, 10)
	_hud:setGradientColor(Gorilla.Gradient_NorthSouth,
		ColourValue(0.05, 0.08, 0.09, 0.82),
		ColourValue(0.0, 0.0, 0.0, 0.84))

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
end

local function _LayoutCrosshair(width, height)
	if _crosshair == nil then return end
	_crosshair:setPosition(Vector2(width * 0.5 - 16, height * 0.5 - 16))
end

local function _CountAliveAgents()
	local friendlyAlive = 0
	local enemyAlive = 0
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		local agent = agents[i]
		if agent ~= nil and agent:GetHealth() > 0 then
			if agent:GetTeamId() == 1 then
				friendlyAlive = friendlyAlive + 1
			else
				enemyAlive = enemyAlive + 1
			end
		end
	end
	return friendlyAlive, enemyAlive
end

local function _UpdateHud()
	if _hud == nil or _player == nil then return end
	local weapon = _player:GetWeaponComponent()
	local ammo = weapon ~= nil and weapon:GetAmmo() or 0
	local maxAmmo = weapon ~= nil and weapon:GetMaxAmmo() or 0
	local friendlyAlive, enemyAlive = _CountAliveAgents()
	local stateText = _matchState
	if _matchState ~= "FIGHT" then
		stateText = _matchState .. " - press Enter to restart"
	end
	local selCount = 0
	for _ in pairs(_selection) do selCount = selCount + 1 end
	local text = string.format(
		"HP: %d / 100\nAmmo: %d / %d\nAllies: %d    Enemies: %d\nSelected: %d    %s\n%s",
		math.max(0, math.floor(_player:GetHealth())),
		ammo,
		maxAmmo,
		friendlyAlive,
		enemyAlive,
		selCount,
		_commandHint,
		stateText)
	_hud:setText(text)
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
				if enemy then
					b:setBackgroundColor(ColourValue(0.92, 0.20, 0.15, 1.0))  -- 敌红
				else
					b:setBackgroundColor(ColourValue(0.20, 0.80, 0.30, 1.0))  -- 友绿
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
local function _ScreenPosOf(agent)
	local sp = SandboxCamera:WorldToScreen(agent:GetPosition())
	if sp.x < 0 and sp.y < 0 then return nil end
	return sp
end

local function _ClearSelection()
	_selection = {}
end

local function _PickAt(x, y)
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
		_commandHint = "target marked"
		return
	end

	_ClearSelection()
	if bestId > 0 then
		_selection[bestId] = true
		_commandHint = "1 ally selected"
	else
		_commandHint = "selection cleared"
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
	_commandHint = count .. " allies selected"
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

-- 三个指令互斥：写入前清掉另两个键，避免旧指令残留把条件误判成活跃。
local function _WriteCommand(bb, kind, focusTargetId, targetPos, nowMs)
	bb:SetString("command.kind", kind)
	bb:SetInt("command.issuedMs", nowMs)
	if kind == "focus" then
		bb:SetObjectId("command.focusTargetId", focusTargetId)
	else
		bb:Remove("command.focusTargetId")
		bb:SetVec3("movePos", targetPos)
	end
end

local function _IssueCommand(kind, focusTargetId, basePos)
	if _player == nil then return end
	if _SelectionCount() == 0 then
		_commandHint = "no ally selected"
		return
	end

	local nowMs = GameManager:getTimeInMillis()
	local teamId = _player:GetTeamId()
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
						targetPos = _player:GetPosition(),
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

	_commandHint = kind .. " -> " .. issued .. " ally"
	print("[Sandbox19Command] kind=" .. kind .. " issued=" .. issued ..
		" focusTargetId=" .. tostring(focusTargetId))
end

local function _IssueFocus()
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
		_commandHint = "no target in view"
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

	-- 3) TTL：把 issuedMs 推到超出 TTL 的过去，条件应全部落空
	bb:SetInt("command.issuedMs", GameManager:getTimeInMillis() - COMMAND.ttlMs - 1000)
	local expiredOk = conds.HasCommandFocus(ally, bb) == false
		and conds.HasCommandRetreat(ally, bb) == false
		and conds.HasCommandRally(ally, bb) == false

	_ClearSelection()
	bb:Remove("command.kind")
	bb:Remove("command.issuedMs")
	bb:Remove("command.focusTargetId")

	local pass = retreatOk and rallyOff and focusOk and enemyOverridden and expiredOk
	print("[Sandbox19CommandSelfTest] " .. (pass and "PASS" or "FAIL") ..
		" retreat=" .. tostring(retreatOk) ..
		" rallyExclusive=" .. tostring(rallyOff) ..
		" focus=" .. tostring(focusOk) ..
		" enemyOverridden=" .. tostring(enemyOverridden) ..
		" ttlExpired=" .. tostring(expiredOk))
end

local function _ClearCommandState()
	_ClearSelection()
	_dragging = false
	_lastPickedEnemyId = 0
	_commandHint = ""
	if TeamBlackboard ~= nil and TeamBlackboard.Reset ~= nil then
		TeamBlackboard:Reset()
	end
end

local function _SpawnEncounter()
	_agents = {}
	_matchState = "FIGHT"
	_restartRequested = false

	local humanScript = "res/scripts/agent/HumanSoldierAgent.lua"
	_player = Create_SoldierWithProfile(humanScript, Soldier.AppearanceTypes.LIGHT, 1, "player_soldier")
	if not _player:HasComponent("player") or _player:HasComponent("ai") then
		error("[Sandbox19] player_soldier profile must contain player and exclude ai")
	end
	table.insert(_agents, _player)
	ConfigManager:PlaceAgentOnPresetSpawn(_player, _sampleName, 1, "default")

	local agentCount = ConfigManager:GetAgentCount(_sampleName, 7)
	local aiScript = "res/scripts/agent/BehaviorSoldierAgent.lua"
	for i = 2, agentCount do
		local teamId = ConfigManager:GetAgentTeamId(_sampleName, i)
		local appearance = ConfigManager:GetAgentAppearance(_sampleName, i, Soldier.AppearanceTypes)
		local agent = Create_SoldierWithProfile(aiScript, appearance, teamId, "ai_soldier")
		if not agent:HasComponent("ai") or agent:HasComponent("player") then
			error("[Sandbox19] ai_soldier profile must contain ai and exclude player")
		end
		table.insert(_agents, agent)
		ConfigManager:PlaceAgentOnPresetSpawn(agent, _sampleName, i, "default")
	end

	print("[Sandbox19] ready playerId=" .. tostring(_player:GetObjId()) ..
		" components=" .. tostring(_player:BuildComponentDebugString()))
	-- blip 池在 _CreateRadar 已建好并复用；_UpdateRadar 每帧按存活单位重定位/隐藏，无需重建。

	if _G.HELLO_SANDBOX_SMOKE_MODE == true then
		_RunCommandSelfTest()
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
-- 只消费右键：左键仍归 PlayerController 射击。
function EventHandle_Mouse(ctype, x, y, button)
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

function Sandbox_Initialize()
	GUI_CreateCameraAndProfileInfo()
	GUI_CreateSandboxText(infoText, { w = 320, h = 220 })
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

	SandboxUtilities_CreateLevel()
	SandboxScene:UpdateSceneGraph()

	local navMeshConfig = rcConfig()
	SandboxNav:DefaultConfig(navMeshConfig)
	SandboxNav:ApplySettingConfig(navMeshConfig, 0.0, 0.4, 0.2)
	navMeshConfig.minRegionArea = math.pow(250, 2)
	navMeshConfig.walkableSlopeAngle = 45
	SandboxNav:CreateNavigationMesh(navMeshConfig, "default")

	print(ConfigManager:BuildDebugSummary(_sampleName))
	_SpawnEncounter()
end

function Sandbox_Update(deltaTimeInMillis)
	GUI_UpdateCameraInfo()
	GUI_UpdateProfileInfo()

	if _restartRequested then
		_RestartEncounter()
	end

	if _player ~= nil and _matchState == "FIGHT" then
		local friendlyAlive, enemyAlive = _CountAliveAgents()
		if _player:GetHealth() <= 0 then
			_matchState = "DEFEAT"
		elseif enemyAlive <= 0 then
			_matchState = "VICTORY"
		elseif friendlyAlive <= 0 then
			_matchState = "DEFEAT"
		end
	end

	_UpdateHud()
	_UpdateRadar()
	_UpdateCommandUi()
end
