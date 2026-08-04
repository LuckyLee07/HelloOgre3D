-- Sandbox19.lua
-- Playable tactical encounter: one player-controlled SoldierObject and six AI soldiers.

require("res.scripts.agent.SoldierAgent.lua")
require("res.scripts.agent.BehaviorSoldierAgent.lua")

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

local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
	"[Sandbox19 - Playable Encounter]" .. GUI.MarkupNewline ..
	"W/S: move forward/back" .. GUI.MarkupNewline ..
	"A/D: turn left/right" .. GUI.MarkupNewline ..
	"Shift: sprint" .. GUI.MarkupNewline ..
	"LMB: fire    R: reload" .. GUI.MarkupNewline ..
	"Enter: restart encounter"

local function _CreateHud()
	_hud = SandboxUI:CreateUIFrame()
	_hud:setPosition(Vector2(20, 188))   -- 下移避开左上角雷达
	_hud:setDimension(Vector2(330, 125))
	_hud:setTextMargin(12, 10)
	_hud:setGradientColor(Gorilla.Gradient_NorthSouth,
		ColourValue(0.05, 0.08, 0.09, 0.82),
		ColourValue(0.0, 0.0, 0.0, 0.84))

	_crosshair = SandboxUI:CreateUIFrame()
	_crosshair:setDimension(Vector2(32, 32))
	_crosshair:setTextMargin(4, 8)
	_crosshair:setText("+")
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
	local text = string.format(
		"HP: %d / 100\nAmmo: %d / %d\nAllies: %d    Enemies: %d\n%s",
		math.max(0, math.floor(_player:GetHealth())),
		ammo,
		maxAmmo,
		friendlyAlive,
		enemyAlive,
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
end

local function _RestartEncounter()
	_player = nil
	_agents = {}
	ObjectManager:clearAllObjects(MGR_OBJ_AGENT)
	_SpawnEncounter()
end

function EventHandle_Keyboard(keycode, pressed)
	GUI_HandleKeyEvent(keycode, pressed)
	if pressed and keycode == OIS.KC_RETURN then
		_restartRequested = true
	end
end

function EventHandle_Mouse(ctype)
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
end
