
require("res.scripts.agent.SoldierAgent.lua")
require("res.scripts.agent.IndirectSoldierAgent.lua")

local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

local textSize = {w = 300, h = 260}
local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
        "W/A/S/D: to move" .. GUI.MarkupNewline ..
        "Hold Shift: to accelerate movement" .. GUI.MarkupNewline ..
        "Hold RMB: to look" .. GUI.MarkupNewline ..
        GUI.MarkupNewline ..
        "F1: to reset the camera" .. GUI.MarkupNewline ..
        "F2: toggle the menu" .. GUI.MarkupNewline ..
        "F3: toggle the navigation mesh" .. GUI.MarkupNewline ..
        "F5: toggle performance information" .. GUI.MarkupNewline ..
        "F6: toggle camera information" .. GUI.MarkupNewline ..
        "F7: toggle physics debug" .. GUI.MarkupNewline ..
        GUI.MarkupNewline ..
        "Switch Agents to:" .. GUI.MarkupNewline ..
        "  Num 1: idle state" .. GUI.MarkupNewline ..
        "  Num 2: shooting state" .. GUI.MarkupNewline ..
        "  Num 3: moving state" .. GUI.MarkupNewline ..
        "  Num 4: death state" .. GUI.MarkupNewline ..
        "  Num 5: stand/crouch stance" .. GUI.MarkupNewline;


local _agents = {}

local function _GetFilePath(luafile)
    return "res/scripts/agent/".. luafile;
end

local function _DrawPaths()
    for index, agent in pairs(_agents) do
        -- Draw the agent's cyclic path, offset slightly above the level
        -- geometry.
        DebugDrawer:drawPath(AgentComponents.GetPath(agent), UtilColors.Red, false, Vector3(0.0, 0.02, 0.0))
        DebugDrawer:drawSquare(AgentComponents.GetTarget(agent), 0.1, UtilColors.Red, true);
    end
end

function EventHandle_Keyboard(keycode, pressed)
    -- OIS.KC_F2 Event 已处理
    GUI_HandleKeyEvent(keycode, pressed)

    if not pressed then return end
    if (keycode == OIS.KC_F1) then
        local camera = SandboxCamera:GetCamera();
        camera:setPosition(Vector3(15, 65, 15));
        camera:setOrientation(Quaternion(-90, 0, -180));
    elseif (keycode == OIS.KC_F3) then
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

    -- Initialize the camera position to focus on the soldier.
    local camera = SandboxCamera:GetCamera();
    camera:setPosition(Vector3(-30, 18, -17));
    camera:setOrientation(Quaternion(-146, -40, -157));

    -- Create The Sky.
    SandboxScene:SetSkyBox("ThickCloudsWaterSkyBox", Vector3(0, 180, 0));

    -- Create a plane in the physics world
    local plane = SandboxObjects:CreatePlane(200, 200);
    plane:setPosition(Vector3(0, -10, 0));
    plane:setMaterial("Ground2");

    -- Create Lighting.
    SandboxScene:SetAmbientLight(Vector3(0.3));
    -- Create a directional light for the sun.
    local directLight = SandboxScene:CreateDirectionalLight(Vector3(1, -1, 1));
    directLight:setDiffuseColour(ColourValue(1.8, 1.4, 0.9));
    directLight:setSpecularColour(ColourValue(1.8, 1.4, 0.9));

	-- Create the sandbox level
    SandboxUtilities_CreateLevel()
    
    -- 强制刷新下场景图 防止getWorldAABB()时拿不到有效包围盒
    SandboxScene:UpdateSceneGraph()

    -- Create default navigation mesh
    local navMeshConfig = rcConfig();
    SandboxNav:DefaultConfig(navMeshConfig)
    SandboxNav:ApplySettingConfig(navMeshConfig, 0.0, 0.4, 0.2)
    navMeshConfig.minRegionArea = math.pow(250, 2)
    navMeshConfig.walkableSlopeAngle = 45

    local navMesh = SandboxNav:CreateNavigationMesh(navMeshConfig, 'default')
    if navMesh ~= nil then navMesh:SetDebugVisible(true) end

    local sampleName = "Sandbox6"
    local agentCount = ConfigManager:GetAgentCount(sampleName, 7)
    print(ConfigManager:BuildDebugSummary(sampleName))

    -- Create agents and place them through the selected sample preset.
    local agentLuafile = _GetFilePath("IndirectSoldierAgent.lua")
    for i=1, agentCount do
        local teamId = ConfigManager:GetAgentTeamId(sampleName, i)
        local agentType = ConfigManager:GetAgentAppearance(sampleName, i, Soldier.AppearanceTypes)
        local agent = Create_Soldier(agentLuafile, agentType, teamId)
        table.insert(_agents, agent);

        ConfigManager:PlaceAgentOnPresetSpawn(agent, sampleName, i, "default")
    end
end

function Sandbox_Update(deltaTimeInMillis)
    GUI_UpdateCameraInfo()
    GUI_UpdateProfileInfo()

    --_DrawPaths()
end
