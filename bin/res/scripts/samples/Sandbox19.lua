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

local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
	"[Sandbox19 - Playable Encounter]" .. GUI.MarkupNewline ..
	"W/A/S/D: move Soldier" .. GUI.MarkupNewline ..
	"Shift: sprint" .. GUI.MarkupNewline ..
	"Hold RMB: aim camera" .. GUI.MarkupNewline ..
	"LMB: fire" .. GUI.MarkupNewline ..
	"R: reload" .. GUI.MarkupNewline ..
	"V: FPS / free camera" .. GUI.MarkupNewline ..
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
end
