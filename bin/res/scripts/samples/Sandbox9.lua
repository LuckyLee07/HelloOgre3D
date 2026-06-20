-- Sandbox9.lua
-- Chapter 7 Knowledge sample：KnowledgeSource -> Blackboard -> DecisionTree。

require("res.scripts.agent.SoldierAgent.lua")
require("res.scripts.agent.KnowledgeSoldierAgent.lua")

local textSize = {w = 330, h = 260}
local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
		"[Sandbox9 - Chapter 7 Knowledge]" .. GUI.MarkupNewline ..
		"W/A/S/D: to move" .. GUI.MarkupNewline ..
		"Hold Shift: to accelerate movement" .. GUI.MarkupNewline ..
		"Hold RMB: to look" .. GUI.MarkupNewline ..
		GUI.MarkupNewline ..
		"F1: to reset the camera" .. GUI.MarkupNewline ..
		"F2: toggle the menu" .. GUI.MarkupNewline ..
		"F5: toggle performance information" .. GUI.MarkupNewline ..
		"F6: toggle camera information" .. GUI.MarkupNewline ..
		"F7: toggle physics debug" .. GUI.MarkupNewline ..
		GUI.MarkupNewline ..
		"3 random soldiers driven by Chapter 7 KnowledgeSource" .. GUI.MarkupNewline ..
		"(Lua source + C++ Blackboard + BT)";

local _agents = {}

function EventHandle_Keyboard(keycode, pressed)
	GUI_HandleKeyEvent(keycode, pressed)

	if not pressed then return end
	if (keycode == OIS.KC_F1) then
		local camera = SandboxCamera:GetCamera();
		camera:setPosition(Vector3(15, 65, 15));
		camera:setOrientation(Quaternion(-90, 0, -180));
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

	SandboxAgentConfig:SetUseCppFsmFlag(true)

	local camera = SandboxCamera:GetCamera();
	camera:setPosition(Vector3(-30, 18, -17));
	camera:setOrientation(Quaternion(-146, -40, -157));

	SandboxScene:SetSkyBox("ThickCloudsWaterSkyBox", Vector3(0, 180, 0));

	local plane = SandboxObjects:CreatePlane(200, 200);
	plane:setPosition(Vector3(0, -10, 0));
	plane:setMaterial("Ground2");

	SandboxScene:SetAmbientLight(Vector3(0.3));
	local directLight = SandboxScene:CreateDirectionalLight(Vector3(1, -1, 1));
	directLight:setDiffuseColour(ColourValue(1.8, 1.4, 0.9));
	directLight:setSpecularColour(ColourValue(1.8, 1.4, 0.9));

	SandboxUtilities_CreateLevel()
	SandboxScene:UpdateSceneGraph()

	local navMeshConfig = rcConfig();
	SandboxNav:DefaultConfig(navMeshConfig)
	SandboxNav:ApplySettingConfig(navMeshConfig, 0.0, 0.4, 0.2)
	navMeshConfig.minRegionArea = math.pow(250, 2)
	navMeshConfig.walkableSlopeAngle = 45

	local navMesh = SandboxNav:CreateNavigationMesh(navMeshConfig, 'default')
	if navMesh ~= nil then navMesh:SetDebugVisible(true) end

	local sampleName = "Sandbox9"
	local agentCount = ConfigManager:GetAgentCount(sampleName, 3)
	print(ConfigManager:BuildDebugSummary(sampleName))

	local agentLuafile = "res/scripts/agent/KnowledgeSoldierAgent.lua"
	for i = 1, agentCount do
		local teamId = ConfigManager:GetAgentTeamId(sampleName, i)
		local agentType = ConfigManager:GetAgentAppearance(sampleName, i, Soldier.AppearanceTypes)
		local agent = Create_Soldier(agentLuafile, agentType, teamId)
		table.insert(_agents, agent)

		ConfigManager:PlaceAgentOnPresetSpawn(agent, sampleName, i, "default")
	end
end

function Sandbox_Update(deltaTimeInMillis)
	GUI_UpdateCameraInfo()
	GUI_UpdateProfileInfo()
end
