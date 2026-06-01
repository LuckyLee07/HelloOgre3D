-- Sandbox16.lua
-- AI perception pressure sample for the C++ spatial query path.

require("res.scripts.agent.SoldierAgent.lua")
require("res.scripts.agent.PerceptionPressureAgent.lua")

local _sampleName = "Sandbox16"
local _agents = {}
local _panel = nil
local _panelSize = { w = 620, h = 250 }
local _summary = ""
local _elapsedMs = 0
local _summaryIntervalMs = 250
local _avgFrameMs = 0
local _frameCount = 0
local _runtimeSummaryPrinted = false

local textSize = { w = 390, h = 210 }
local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
	"[Sandbox16 - AI Perception Pressure]" .. GUI.MarkupNewline ..
	"Each soldier runs VisionSensor only" .. GUI.MarkupNewline ..
	"Use HELLO_SAMPLE_AGENT_COUNT to scale" .. GUI.MarkupNewline ..
	"Use HELLO_AI_SPATIAL_INDEX_ENABLE=0 for linear fallback"

local function _GetEnvNumber(name, defaultValue)
	local value = os.getenv and tonumber(os.getenv(name)) or nil
	if value == nil then
		return defaultValue
	end
	return value
end

local function _GetPressureConfig()
	local preset = ConfigManager:GetSamplePreset(_sampleName)
	return preset.perceptionPressure or {}
end

local function _CreatePanel()
	_panel = Sandbox:CreateUIFrame()
	_panel:setPosition(Vector2(20, 20))
	_panel:setDimension(Vector2(_panelSize.w, _panelSize.h))
	_panel:setTextMargin(12, 10)
	_panel:setGradientColor(Gorilla.Gradient_NorthSouth, ColourValue(0.05, 0.07, 0.09, 0.75), ColourValue(0.0, 0.0, 0.0, 0.78))
end

local function _FormatSummaryText()
	local spatialEnabled = os.getenv and os.getenv("HELLO_AI_SPATIAL_INDEX_ENABLE") or nil
	if spatialEnabled == nil or spatialEnabled == "" then
		spatialEnabled = "default-on"
	end

	return GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
		"[Sandbox16 - AI Perception Pressure]" .. GUI.MarkupNewline ..
		"agents=" .. tostring(#_agents) ..
		" avgFrameMs=" .. string.format("%.2f", _avgFrameMs) ..
		" spatialEnv=" .. tostring(spatialEnabled) .. GUI.MarkupNewline ..
		"cellSizeEnv=" .. tostring(os.getenv and os.getenv("HELLO_AI_SPATIAL_INDEX_CELL_SIZE") or "default") .. GUI.MarkupNewline ..
		GUI.MarkupNewline ..
		tostring(_summary)
end

local function _RefreshSummary(force)
	if ObjectManager == nil or ObjectManager.buildAiDebugSummary == nil then
		return
	end
	if not force and _elapsedMs < _summaryIntervalMs then
		return
	end
	_elapsedMs = 0
	_summary = ObjectManager:buildAiDebugSummary(0)
	if _panel ~= nil then
		_panel:setMarkupText(_FormatSummaryText())
	end
end

local function _PrintSummary(prefix)
	if _summary == "" then
		return
	end
	for line in string.gmatch(tostring(_summary), "[^\r\n]+") do
		print(prefix .. " " .. line)
	end
end

local function _DrawDebugGuides()
	local maxDraw = math.min(#_agents, 48)
	for i = 1, maxDraw do
		local agent = _agents[i]
		if agent ~= nil then
			local pos = agent:GetPosition()
			local color = agent:GetTeamId() == 1 and UtilColors.Green or UtilColors.Red
			DebugDrawer:drawCircle(pos + Vector3(0, 0.15, 0), 1.0, 16, color, false)
		end
	end
end

local function _PlaceAgent(agent, index, count, config)
	local spacing = tonumber(config.spacing) or _GetEnvNumber("HELLO_PRESSURE_SPACING", 6.0)
	local columns = math.max(1, math.ceil(math.sqrt(count)))
	local row = math.floor((index - 1) / columns)
	local column = (index - 1) % columns
	local originX = -((columns - 1) * spacing) * 0.5
	local rows = math.ceil(count / columns)
	local originZ = -((rows - 1) * spacing) * 0.5
	local position = Vector3(originX + column * spacing, 0, originZ + row * spacing)
	position.y = position.y + agent:GetHeight() * 0.5

	agent:setPosition(position)
	agent:SetTarget(position)
	agent:SetTargetRadius(1)
	agent:SetVelocity(Vector3(0, 0, 0))
	agent:SetMaxSpeed(0)
	return position
end

local function _CreateAgents()
	local preset = ConfigManager:GetSamplePreset(_sampleName)
	local config = _GetPressureConfig()
	local count = ConfigManager:GetAgentCount(_sampleName, tonumber(config.defaultAgentCount) or 120)
	local lightTeamCount = math.floor(count * 0.5)
	local agentLuafile = "res/scripts/agent/PerceptionPressureAgent.lua"

	for i = 1, count do
		local teamId = i <= lightTeamCount and 1 or 0
		local appearance = teamId == 1 and Soldier.AppearanceTypes.LIGHT or Soldier.AppearanceTypes.DARK
		local agent = Create_Soldier(agentLuafile, appearance, teamId)
		_PlaceAgent(agent, i, count, config)
		table.insert(_agents, agent)
	end

	print(ConfigManager:BuildDebugSummary(_sampleName))
	print("[Sandbox16] perception pressure agents=", count, "preset=", tostring(preset.name))
end

function EventHandle_Keyboard(keycode, pressed)
	GUI_HandleKeyEvent(keycode, pressed)

	if not pressed then return end
	if keycode == OIS.KC_F1 then
		local camera = Sandbox:GetCamera()
		camera:setPosition(Vector3(0, 120, 120))
		camera:setOrientation(Quaternion(-45, 0, 180))
	end
end

function EventHandle_Mouse(ctype)
end

function EventHandle_WindowResized(width, height)
	GUI_WindowResized(width, height)
	if _panel ~= nil then
		_panel:setPosition(Vector2(20, 20))
		_panel:setDimension(Vector2(_panelSize.w, _panelSize.h))
	end
end

function Sandbox_Initialize()
	GUI_CreateCameraAndProfileInfo()
	GUI_CreateSandboxText(infoText, textSize)
	_CreatePanel()

	Sandbox:SetUseCppFsmFlag(false)

	local camera = Sandbox:GetCamera()
	camera:setPosition(Vector3(0, 120, 120))
	camera:setOrientation(Quaternion(-45, 0, 180))

	Sandbox:SetSkyBox("ThickCloudsWaterSkyBox", Vector3(0, 180, 0))

	local config = _GetPressureConfig()
	local planeSize = tonumber(config.planeSize) or 260
	local plane = Sandbox:CreatePlane(planeSize, planeSize)
	plane:setPosition(Vector3(0, -10, 0))
	plane:setMaterial("Ground2")

	Sandbox:SetAmbientLight(Vector3(0.45))
	local directLight = Sandbox:CreateDirectionalLight(Vector3(1, -1, 1))
	directLight:setDiffuseColour(ColourValue(1.6, 1.4, 1.1))
	directLight:setSpecularColour(ColourValue(1.4, 1.3, 1.0))

	_CreateAgents()
	_RefreshSummary(true)
	_PrintSummary("[Sandbox16InitSummary]")
end

function Sandbox_Update(deltaTimeInMillis)
	GUI_UpdateCameraInfo()
	GUI_UpdateProfileInfo()

	_elapsedMs = _elapsedMs + deltaTimeInMillis
	_frameCount = _frameCount + 1
	local alpha = _frameCount < 30 and (1.0 / _frameCount) or 0.05
	_avgFrameMs = _avgFrameMs * (1.0 - alpha) + deltaTimeInMillis * alpha

	_DrawDebugGuides()
	_RefreshSummary(false)

	if not _runtimeSummaryPrinted and _frameCount >= 3 then
		_RefreshSummary(true)
		_PrintSummary("[Sandbox16RuntimeSummary]")
		_runtimeSummaryPrinted = true
	end
end
