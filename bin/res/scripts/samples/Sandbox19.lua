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

-- FairyGUI 矢量雷达（程序化多边形，无美术资源）。screen* 为屏幕左上（HUD 下方避开），
-- cx/cy/radius 为容器内像素，range 为覆盖世界半径。数值按 Sandbox19 尺度，可手感调。
local _radar = { root = nil, blips = {}, ok = false }
local RADAR = {
	screenX = 16, screenY = 152,
	cx = 64, cy = 64, radius = 64,
	range = 60, discSides = 40, blipSize = 8,
}

local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
	"[Sandbox19 - Playable Encounter]" .. GUI.MarkupNewline ..
	"W/A/S/D: move Soldier" .. GUI.MarkupNewline ..
	"Shift: sprint" .. GUI.MarkupNewline ..
	"Hold RMB: rotate view / aim" .. GUI.MarkupNewline ..
	"LMB: fire" .. GUI.MarkupNewline ..
	"R: reload" .. GUI.MarkupNewline ..
	"Enter: restart encounter"

local function _CreateHud()
	_hud = SandboxUI:CreateUIFrame()
	_hud:setPosition(Vector2(20, 20))
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

local function _RadarAvailable()
	return FairyGuiRuntime ~= nil and FairyGuiRuntime:IsAvailable()
end

local function _CreateRadar()
	if not _RadarAvailable() then return end
	local root = FairyGuiRuntime:CreateContainer("sandbox19_radar")
	if root == 0 then return end
	local d = RADAR.radius * 2
	FairyGuiRuntime:SetObjectSize(root, d, d)
	FairyGuiRuntime:AddObjectToRoot(root)
	FairyGuiRuntime:SetObjectPosition(root, RADAR.screenX, RADAR.screenY)
	-- 圆盘底（多边形近似圆）
	local disc = FairyGuiRuntime:CreateGraphRegularPolygon(root, "disc", d, d, RADAR.discSides, 0.05, 0.08, 0.10, 0.72)
	FairyGuiRuntime:SetObjectPosition(disc, 0, 0)
	-- 玩家箭头（三角形，居中静止朝上；player-up 雷达，同 code-master）
	local arrow = FairyGuiRuntime:CreateGraphRegularPolygon(root, "arrow", 14, 16, 3, 0.9, 0.9, 0.95, 1.0)
	FairyGuiRuntime:SetObjectPosition(arrow, RADAR.cx - 7, RADAR.cy - 8)
	_radar.root = root
	_radar.blips = {}
	_radar.ok = true
end

local function _DestroyBlips()
	if not _radar.ok then return end
	for _, b in ipairs(_radar.blips) do
		if b.handle ~= nil and b.handle ~= 0 then
			FairyGuiRuntime:RemoveObject(b.handle)
		end
	end
	_radar.blips = {}
end

local function _CreateBlips()
	if not _radar.ok or _player == nil then return end
	_DestroyBlips()
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		local agent = agents[i]
		if agent ~= nil and agent ~= _player then
			local enemy = agent:GetTeamId() ~= _player:GetTeamId()
			local r = enemy and 0.95 or 0.20
			local g = enemy and 0.20 or 0.90
			local bl = enemy and 0.15 or 0.35
			local h = FairyGuiRuntime:CreateGraphRegularPolygon(_radar.root, "blip", RADAR.blipSize, RADAR.blipSize, 12, r, g, bl, 1.0)
			if h ~= 0 then
				table.insert(_radar.blips, { handle = h, agent = agent })
			end
		end
	end
end

local function _UpdateRadar()
	if not _radar.ok or _player == nil then return end
	local pp = _player:GetPosition()
	local pf = _player:GetForward()
	local flen = math.sqrt(pf.x * pf.x + pf.z * pf.z)
	if flen < 1e-4 then return end
	local fx, fz = pf.x / flen, pf.z / flen   -- 角色前向(XZ)
	local rx, rz = fz, -fx                     -- 右向 = UNIT_Y × forward
	local scale = RADAR.radius / RADAR.range
	local half = RADAR.blipSize * 0.5
	for _, b in ipairs(_radar.blips) do
		local agent = b.agent
		local visible = false
		if agent ~= nil and agent:GetHealth() > 0 then
			local ap = agent:GetPosition()
			local dx, dz = ap.x - pp.x, ap.z - pp.z
			local fwdC = dx * fx + dz * fz         -- 前向分量 → 屏幕上(-y)
			local rgtC = dx * rx + dz * rz         -- 右向分量 → 屏幕右(+x)
			if fwdC * fwdC + rgtC * rgtC <= RADAR.range * RADAR.range then
				FairyGuiRuntime:SetObjectPosition(b.handle, RADAR.cx + rgtC * scale - half, RADAR.cy - fwdC * scale - half)
				visible = true
			end
		end
		FairyGuiRuntime:SetObjectVisible(b.handle, visible)
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
	_CreateBlips()
end

local function _RestartEncounter()
	_player = nil
	_agents = {}
	_DestroyBlips()
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
