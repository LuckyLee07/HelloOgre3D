-- Sandbox10.lua
-- Sandbox8-derived BT scenario focused on lastKnown memory demonstration.

require("res.scripts.agent.SoldierAgent.lua")
require("res.scripts.agent.BehaviorSoldierAgent.lua")

local textSize = {w = 330, h = 260}
local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
        "[Sandbox10 - LastKnown Memory]" .. GUI.MarkupNewline ..
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
        "BT perception -> memory -> lastKnown search" .. GUI.MarkupNewline ..
        "(fixed two-agent demo preset)";

local _agents = {}
local _demoPanel = nil
local _demoPanelSize = {w = 430, h = 210}
local _lastKnownDemo = {
    enabled = false,
    elapsedMs = 0,
    relocated = false,
    phase = "setup",
    lastKnownPos = nil,
    relocatedPos = nil,
}

local function _PointToVector3(point)
    if point == nil then return nil end
    return Vector3(point[1] or 0, point[2] or 0, point[3] or 0)
end

local function _SetAgentDemoPhase(agent, phase)
    if agent == nil or agent.GetAI == nil then return end
    local ai = agent:GetAI()
    local bb = ai ~= nil and ai:GetBlackboard() or nil
    if bb ~= nil then
        bb:SetString("debug.demoPhase", phase)
    end
end

local function _GetBlackboard(agent)
    if agent == nil or agent.GetAI == nil then return nil end
    local ai = agent:GetAI()
    return ai ~= nil and ai:GetBlackboard() or nil
end

local function _GetPursuer()
    return _agents[1]
end

local function _GetDemoTarget()
    local config = _lastKnownDemo.config or {}
    return _agents[tonumber(config.targetIndex) or 2]
end

local function _FormatVec3(pos)
    if pos == nil then return "-" end
    return string.format("%.1f, %.1f, %.1f", pos.x, pos.y, pos.z)
end

local function _GetStringValue(bb, key, defaultValue)
    if bb == nil or not bb:Has(key) then
        return defaultValue
    end
    return bb:GetString(key)
end

local function _CreateDemoPanel()
    local panel = Sandbox:CreateUIFrame()
    panel:setPosition(Vector2(20, 20))
    panel:setDimension(Vector2(_demoPanelSize.w, _demoPanelSize.h))
    panel:setTextMargin(12, 10)
    panel:setGradientColor(Gorilla.Gradient_NorthSouth, ColourValue(0.05, 0.08, 0.09, 0.75), ColourValue(0.0, 0.0, 0.0, 0.78))
    _demoPanel = panel
end

local function _SetPhase(phase)
    _lastKnownDemo.phase = phase
    for _, agent in ipairs(_agents) do
        _SetAgentDemoPhase(agent, phase)
    end
end

local function _InitializeLastKnownDemo(sampleName)
    local preset = ConfigManager:GetSamplePreset(sampleName)
    local config = preset.lastKnownDemo or {}
    _lastKnownDemo.enabled = config.enabled == true
    _lastKnownDemo.elapsedMs = 0
    _lastKnownDemo.relocated = false
    _lastKnownDemo.lastKnownPos = nil
    _lastKnownDemo.relocatedPos = nil
    _lastKnownDemo.config = config

    if not _lastKnownDemo.enabled then return end
    local targetIndex = tonumber(config.targetIndex) or 2
    local target = _agents[targetIndex]
    if target ~= nil and config.freezeTarget == true then
        target:SetMaxSpeed(0)
        target:SetVelocity(Vector3(0, 0, 0))
    end
    _SetPhase("see_chase")
    print("[LastKnownDemo] armed", "targetIndex=", targetIndex, "relocateAfterMs=", tonumber(config.relocateAfterMs) or 0)
end

local function _RelocateLastKnownTarget()
    if not _lastKnownDemo.enabled or _lastKnownDemo.relocated then return end
    local config = _lastKnownDemo.config or {}
    local target = _agents[tonumber(config.targetIndex) or 2]
    local relocateTo = _PointToVector3(config.relocateTo)
    if target == nil or relocateTo == nil then return end

    _lastKnownDemo.lastKnownPos = target:GetPosition()
    local position = Sandbox:FindClosestPoint("default", relocateTo)
    position.y = position.y + target:GetHeight() * 0.5
    target:setPosition(position)
    target:SetTarget(position)
    target:SetVelocity(Vector3(0, 0, 0))
    if config.freezeTarget == true then
        target:SetMaxSpeed(0)
    end

    _lastKnownDemo.relocated = true
    _lastKnownDemo.relocatedPos = position
    _SetPhase("target_vanished")
    print("[LastKnownDemo] target relocated", "id=", target:GetObjId(), "pos=", position.x, position.y, position.z)
end

local function _ResolveLastKnownPos(bb)
    if bb == nil then
        return _lastKnownDemo.lastKnownPos
    end
    if bb:GetBool("memory.snapshot.hasLastKnownEnemy", false) then
        return bb:GetVec3("memory.snapshot.lastKnownEnemyPos")
    end
    if bb:Has("debug.lastKnownSearchEnemyPos") then
        return bb:GetVec3("debug.lastKnownSearchEnemyPos")
    end
    return _lastKnownDemo.lastKnownPos
end

local function _RefreshPhaseFromBlackboard(bb)
    if not _lastKnownDemo.relocated then
        _lastKnownDemo.phase = "see_chase"
        return
    end

    local action = _GetStringValue(bb, "__bt.currentAction", "")
    if action == "moveToLastKnownEnemy" then
        _lastKnownDemo.phase = "search_last_known"
    elseif bb ~= nil and bb:Has("memory.search.completedEnemyId") then
        _lastKnownDemo.phase = "search_done"
    else
        _lastKnownDemo.phase = "target_vanished"
    end
end

local function _BuildDemoPanelText()
    local pursuer = _GetPursuer()
    local target = _GetDemoTarget()
    local bb = _GetBlackboard(pursuer)
    _RefreshPhaseFromBlackboard(bb)

    local phaseTitle = {
        setup = "SETUP",
        see_chase = "1 SEE ENEMY -> CHASE",
        target_vanished = "2 TARGET VANISHES",
        search_last_known = "3 SEARCH LAST KNOWN",
        search_done = "4 SEARCH DONE",
    }
    local action = _GetStringValue(bb, "__bt.currentAction", "-")
    local sense = "none"
    if bb ~= nil and bb:Has("perception.targetId") then
        sense = tostring(bb:GetObjectId("perception.targetId", -1))
    end
    local lastKnownPos = _ResolveLastKnownPos(bb)
    local realTargetPos = target ~= nil and target:GetPosition() or nil

    local color = GUI.MarkupColor.White
    if _lastKnownDemo.phase == "see_chase" then
        color = GUI.MarkupColor.Green
    elseif _lastKnownDemo.phase == "target_vanished" then
        color = GUI.MarkupColor.Orange
    elseif _lastKnownDemo.phase == "search_last_known" then
        color = GUI.MarkupColor.Yellow
    end

    return GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
        "[Sandbox10 readable AI demo]" .. GUI.MarkupNewline ..
        color .. "Stage: " .. tostring(phaseTitle[_lastKnownDemo.phase] or _lastKnownDemo.phase) .. GUI.MarkupColor.White .. GUI.MarkupNewline ..
        "Meaning: current sight can be empty, memory still guides BT" .. GUI.MarkupNewline ..
        GUI.MarkupNewline ..
        "Pursuer: green marker" .. GUI.MarkupNewline ..
        "Real target: red marker after vanish" .. GUI.MarkupNewline ..
        "Last known: yellow ring + pillar" .. GUI.MarkupNewline ..
        GUI.MarkupNewline ..
        "Sense target: " .. sense .. GUI.MarkupNewline ..
        "Last known: " .. _FormatVec3(lastKnownPos) .. GUI.MarkupNewline ..
        "Real target: " .. _FormatVec3(realTargetPos) .. GUI.MarkupNewline ..
        "BT action: " .. tostring(action)
end

local function _UpdateDemoPanel()
    if _demoPanel == nil then return end
    _demoPanel:setMarkupText(_BuildDemoPanelText())
end

local function _DrawAgentMarker(agent, color, radius)
    if agent == nil then return end
    local pos = agent:GetPosition()
    DebugDrawer:drawCircle(pos + Vector3(0, 0.15, 0), radius or 1.3, 24, color, false)
    DebugDrawer:drawLine(pos + Vector3(0, 0.2, 0), pos + Vector3(0, 3.2, 0), color)
end

local function _DrawLastKnownMarker(pos)
    if pos == nil then return end
    local base = Vector3(pos.x, pos.y, pos.z)
    DebugDrawer:drawCircle(base + Vector3(0, 0.18, 0), 1.6, 32, UtilColors.Yellow, false)
    DebugDrawer:drawCircle(base + Vector3(0, 0.22, 0), 0.6, 24, UtilColors.Yellow, false)
    DebugDrawer:drawLine(base + Vector3(-1.6, 0.35, 0), base + Vector3(1.6, 0.35, 0), UtilColors.Yellow)
    DebugDrawer:drawLine(base + Vector3(0, 0.35, -1.6), base + Vector3(0, 0.35, 1.6), UtilColors.Yellow)
    DebugDrawer:drawLine(base + Vector3(0, 0.2, 0), base + Vector3(0, 5.0, 0), UtilColors.Yellow)
end

local function _DrawDemoGuides()
    local pursuer = _GetPursuer()
    local target = _GetDemoTarget()
    local bb = _GetBlackboard(pursuer)
    local lastKnownPos = _ResolveLastKnownPos(bb)

    _DrawAgentMarker(pursuer, UtilColors.Green, 1.2)
    _DrawAgentMarker(target, UtilColors.Red, 1.2)

    if pursuer ~= nil then
        local pos = pursuer:GetPosition()
        DebugDrawer:drawCircle(pos + Vector3(0, 0.1, 0), 12.0, 48, UtilColors.Blue, false)
    end
    if pursuer ~= nil and target ~= nil and not _lastKnownDemo.relocated then
        DebugDrawer:drawLine(pursuer:GetPosition() + Vector3(0, 1.6, 0), target:GetPosition() + Vector3(0, 1.6, 0), UtilColors.Green)
    end
    if lastKnownPos ~= nil then
        _DrawLastKnownMarker(lastKnownPos)
        if pursuer ~= nil then
            DebugDrawer:drawLine(pursuer:GetPosition() + Vector3(0, 1.2, 0), lastKnownPos + Vector3(0, 1.2, 0), UtilColors.Yellow)
        end
    end
    if _lastKnownDemo.relocatedPos ~= nil and lastKnownPos ~= nil then
        DebugDrawer:drawLine(lastKnownPos + Vector3(0, 0.6, 0), _lastKnownDemo.relocatedPos + Vector3(0, 0.6, 0), UtilColors.Red)
    end
end

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
    if _demoPanel ~= nil then
        _demoPanel:setPosition(Vector2(20, 20))
        _demoPanel:setDimension(Vector2(_demoPanelSize.w, _demoPanelSize.h))
    end
end

function Sandbox_Initialize()
    GUI_CreateCameraAndProfileInfo()
    GUI_CreateSandboxText(infoText, textSize)
    _CreateDemoPanel()

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

    local sampleName = "Sandbox10"
    local agentCount = ConfigManager:GetAgentCount(sampleName, 2)
    print(ConfigManager:BuildDebugSummary(sampleName))

    local agentLuafile = "res/scripts/agent/BehaviorSoldierAgent.lua"
    for i = 1, agentCount do
        local teamId = ConfigManager:GetAgentTeamId(sampleName, i)
        local agentType = ConfigManager:GetAgentAppearance(sampleName, i, Soldier.AppearanceTypes)
        local agent = Create_Soldier(agentLuafile, agentType, teamId)
        table.insert(_agents, agent)

        ConfigManager:PlaceAgentOnPresetSpawn(agent, sampleName, i, "default")
    end

    _InitializeLastKnownDemo(sampleName)
end

function Sandbox_Update(deltaTimeInMillis)
    GUI_UpdateCameraInfo()
    GUI_UpdateProfileInfo()
    if _lastKnownDemo.enabled and not _lastKnownDemo.relocated then
        _lastKnownDemo.elapsedMs = _lastKnownDemo.elapsedMs + deltaTimeInMillis
        local relocateAfterMs = tonumber(_lastKnownDemo.config and _lastKnownDemo.config.relocateAfterMs) or 0
        if _lastKnownDemo.elapsedMs >= relocateAfterMs then
            _RelocateLastKnownTarget()
        end
    end
    _DrawDemoGuides()
    _UpdateDemoPanel()
end
