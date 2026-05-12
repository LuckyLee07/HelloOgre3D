-- Sandbox8.lua
-- 行为树 (BT) 驱动的士兵 sandbox。
-- 与 Sandbox7 同布局 (7 个士兵 + 默认 navmesh)，区别是每个士兵的 Agent 脚本为
-- BehaviorSoldierAgent.lua，运行时由 C++ BehaviorTreeDriver 驱动 Lua 叶节点。
--
-- 与 Sandbox7 (DT) 行为应等价，可对比验证两套引擎产出一致结果。

require("res.scripts.agent.SoldierAgent.lua")
require("res.scripts.agent.BehaviorSoldierAgent.lua")

local textSize = {w = 300, h = 240}
local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
        "[Sandbox8 - Behavior Tree]" .. GUI.MarkupNewline ..
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
        "All 7 soldiers driven by BehaviorTree" .. GUI.MarkupNewline ..
        "(C++ engine + Lua leaves, Sequence/Selector)";

local _agents = {}

function EventHandle_Keyboard(keycode, pressed)
    GUI_HandleKeyEvent(keycode, pressed)

    if not pressed then return end
    if (keycode == OIS.KC_F1) then
        local camera = Sandbox:GetCamera();
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

    -- 同 Sandbox7：BT 接管决策，C++ FSM flag 仍要保 SoldierAnimController tick。
    Sandbox:SetUseCppFsmFlag(true)

    local camera = Sandbox:GetCamera();
    camera:setPosition(Vector3(-30, 18, -17));
    camera:setOrientation(Quaternion(-146, -40, -157));

    Sandbox:SetSkyBox("ThickCloudsWaterSkyBox", Vector3(0, 180, 0));

    local plane = Sandbox:CreatePlane(200, 200);
    plane:setPosition(Vector3(0, -10, 0));
    plane:setMaterial("Ground2");

    Sandbox:SetAmbientLight(Vector3(0.3));
    local directLight = Sandbox:CreateDirectionalLight(Vector3(1, -1, 1));
    directLight:setDiffuseColour(ColourValue(1.8, 1.4, 0.9));
    directLight:setSpecularColour(ColourValue(1.8, 1.4, 0.9));

    SandboxUtilities_CreateLevel()
    Sandbox:UpdateSceneGraph()

    local navMeshConfig = rcConfig();
    Sandbox:DefaultConfig(navMeshConfig)
    Sandbox:ApplySettingConfig(navMeshConfig, 0.0, 0.4, 0.2)
    navMeshConfig.minRegionArea = math.pow(250, 2)
    navMeshConfig.walkableSlopeAngle = 45

    local navMesh = Sandbox:CreateNavigationMesh(navMeshConfig, 'default')
    if navMesh ~= nil then navMesh:SetDebugVisible(true) end

    -- 7 个 BT 驱动的士兵：3 个 LIGHT (team 1) + 4 个 DARK (team 0)
    local agentLuafile = "res/scripts/agent/BehaviorSoldierAgent.lua"
    for i = 1, 7 do
        local teamId = i > 3 and 0 or 1
        local agentType = (i > 3) and Soldier.AppearanceTypes.DARK
                                   or  Soldier.AppearanceTypes.LIGHT
        local agent = Create_Soldier(agentLuafile, agentType, teamId)
        table.insert(_agents, agent)

        local height = agent:GetHeight()
        local randomPoint = Sandbox:RandomPoint("default")
        randomPoint.y = randomPoint.y + height * 0.5
        agent:setPosition(randomPoint)

        local navPosition = Sandbox:FindClosestPoint("default", agent:GetPosition())
        agent:SetTarget(navPosition)
        agent:SetTargetRadius(1)
    end
end

function Sandbox_Update(deltaTimeInMillis)
    GUI_UpdateCameraInfo()
    GUI_UpdateProfileInfo()
    -- 路径画线已下沉到 MoveAction / PursueAction 的 OnUpdate（与 Sandbox7 完全一样）。
end
