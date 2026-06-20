-- Sandbox10.lua
-- Chapter 8 Perception sample: VisionSensor -> MemoryStore -> BT lastKnown search.

require("res.scripts.agent.SoldierAgent.lua")
require("res.scripts.agent.BehaviorSoldierAgent.lua")
local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

local textSize = {w = 360, h = 260}
local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
        "[Sandbox10 - Chapter 8 Perception]" .. GUI.MarkupNewline ..
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
        "VisionSensor -> MemoryStore -> BT" .. GUI.MarkupNewline ..
        "shows current sight vs remembered target";

local _agents = {}
local _demoPanel = nil
local _demoPanelSize = {w = 470, h = 245}
local _perceptionDemo = {
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
    if agent == nil then return end
    local ai = AgentComponents.GetAI(agent)
    local bb = ai ~= nil and ai:GetBlackboard() or nil
    if bb ~= nil then
        bb:SetString("debug.demoPhase", phase)
    end
end

local function _GetBlackboard(agent)
    if agent == nil then return nil end
    local ai = AgentComponents.GetAI(agent)
    return ai ~= nil and ai:GetBlackboard() or nil
end

local function _GetPursuer()
    return _agents[1]
end

local function _GetDemoTarget()
    local config = _perceptionDemo.config or {}
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

local function _GetIntValue(bb, key, defaultValue)
    if bb == nil or not bb:Has(key) then
        return defaultValue
    end
    return bb:GetInt(key, defaultValue or 0)
end

local function _GetFloatValue(bb, key, defaultValue)
    if bb == nil or not bb:Has(key) then
        return defaultValue
    end
    return bb:GetFloat(key, defaultValue or 0.0)
end

local function _GetVisionRange(bb)
    return _GetFloatValue(bb, "perception.visionRange", 12.0)
end

local function _GetMemoryState(bb)
    if bb == nil then
        return "none", "-", "-"
    end
    if bb:GetBool("memory.snapshot.hasLastKnownEnemy", false) then
        return "active",
            tostring(_GetIntValue(bb, "memory.snapshot.lastKnownEnemyAgeMs", -1)) .. "ms",
            string.format("%.2f", _GetFloatValue(bb, "memory.snapshot.lastKnownEnemyConfidence", 0.0))
    end
    if bb:Has("debug.lastKnownSearchEnemyPos") then
        return "BT search snapshot", "-", "-"
    end
    if _perceptionDemo.relocated then
        return "expired or empty", "-", "-"
    end
    return "waiting for vision", "-", "-"
end

local function _CreateDemoPanel()
    local panel = SandboxUI:CreateUIFrame()
    panel:setPosition(Vector2(20, 20))
    panel:setDimension(Vector2(_demoPanelSize.w, _demoPanelSize.h))
    panel:setTextMargin(12, 10)
    panel:setGradientColor(Gorilla.Gradient_NorthSouth, ColourValue(0.05, 0.08, 0.09, 0.75), ColourValue(0.0, 0.0, 0.0, 0.78))
    _demoPanel = panel
end

local function _SetPhase(phase)
    if _perceptionDemo.phase == phase then return end
    _perceptionDemo.phase = phase
    for _, agent in ipairs(_agents) do
        _SetAgentDemoPhase(agent, phase)
    end
end

local function _InitializePerceptionDemo(sampleName)
    local preset = ConfigManager:GetSamplePreset(sampleName)
    local config = preset.perceptionDemo or preset.lastKnownDemo or {}
    _perceptionDemo.enabled = config.enabled == true
    _perceptionDemo.elapsedMs = 0
    _perceptionDemo.relocated = false
    _perceptionDemo.lastKnownPos = nil
    _perceptionDemo.relocatedPos = nil
    _perceptionDemo.config = config

    if not _perceptionDemo.enabled then return end
    local targetIndex = tonumber(config.targetIndex) or 2
    local target = _agents[targetIndex]
    if target ~= nil and config.freezeTarget == true then
        AgentComponents.SetMaxSpeed(target, 0)
        target:SetVelocity(Vector3(0, 0, 0))
    end
    _SetPhase("see_chase")
    print("[Chapter8Perception] armed", "targetIndex=", targetIndex, "relocateAfterMs=", tonumber(config.relocateAfterMs) or 0)
end

local function _RelocatePerceptionTarget()
    if not _perceptionDemo.enabled or _perceptionDemo.relocated then return end
    local config = _perceptionDemo.config or {}
    local target = _agents[tonumber(config.targetIndex) or 2]
    local relocateTo = _PointToVector3(config.relocateTo)
    if target == nil or relocateTo == nil then return end

    _perceptionDemo.lastKnownPos = target:GetPosition()
    local position = SandboxNav:FindClosestPoint("default", relocateTo)
    position.y = position.y + AgentComponents.GetHeight(target) * 0.5
    target:setPosition(position)
    AgentComponents.SetTarget(target, position)
    target:SetVelocity(Vector3(0, 0, 0))
    if config.freezeTarget == true then
        AgentComponents.SetMaxSpeed(target, 0)
    end

    _perceptionDemo.relocated = true
    _perceptionDemo.relocatedPos = position
    _SetPhase("target_vanished")
    print("[Chapter8Perception] target relocated", "id=", target:GetObjId(), "pos=", position.x, position.y, position.z)
end

local function _ResolveLastKnownPos(bb)
    if bb == nil then
        return _perceptionDemo.lastKnownPos
    end
    if bb:GetBool("memory.snapshot.hasLastKnownEnemy", false) then
        return bb:GetVec3("memory.snapshot.lastKnownEnemyPos")
    end
    if bb:Has("debug.lastKnownSearchEnemyPos") then
        return bb:GetVec3("debug.lastKnownSearchEnemyPos")
    end
    return _perceptionDemo.lastKnownPos
end

local function _RefreshPhaseFromBlackboard(bb)
    if not _perceptionDemo.relocated then
        _SetPhase("see_chase")
        return
    end

    local action = _GetStringValue(bb, "__bt.currentAction", "")
    if action == "moveToLastKnownEnemy" then
        _SetPhase("memory_guides_search")
    elseif bb ~= nil and bb:Has("memory.search.completedEnemyId") then
        _SetPhase("search_done")
    else
        _SetPhase("target_vanished")
    end
end

local function _BuildDemoPanelText()
    local pursuer = _GetPursuer()
    local target = _GetDemoTarget()
    local bb = _GetBlackboard(pursuer)
    _RefreshPhaseFromBlackboard(bb)

    local phaseTitle = {
        setup = "SETUP",
        see_chase = "1 VISION HAS TARGET",
        target_vanished = "2 VISION LOST TARGET",
        memory_guides_search = "3 MEMORY GUIDES SEARCH",
        search_done = "4 MEMORY SEARCH DONE",
    }
    local action = _GetStringValue(bb, "__bt.currentAction", "-")
    local sense = "none"
    if bb ~= nil and bb:Has("perception.targetId") then
        sense = tostring(bb:GetObjectId("perception.targetId", -1))
    end
    local lastKnownPos = _ResolveLastKnownPos(bb)
    local realTargetPos = target ~= nil and target:GetPosition() or nil
    local memoryState, memoryAge, memoryConfidence = _GetMemoryState(bb)
    local visionRange = _GetVisionRange(bb)

    local color = GUI.MarkupColor.White
    if _perceptionDemo.phase == "see_chase" then
        color = GUI.MarkupColor.Green
    elseif _perceptionDemo.phase == "target_vanished" then
        color = GUI.MarkupColor.Orange
    elseif _perceptionDemo.phase == "memory_guides_search" then
        color = GUI.MarkupColor.Yellow
    end

    return GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
        "[Sandbox10 - Chapter 8 Perception]" .. GUI.MarkupNewline ..
        color .. "Stage: " .. tostring(phaseTitle[_perceptionDemo.phase] or _perceptionDemo.phase) .. GUI.MarkupColor.White .. GUI.MarkupNewline ..
        "Goal: separate current vision from remembered evidence" .. GUI.MarkupNewline ..
        GUI.MarkupNewline ..
        "VisionSensor: blue range ring, green line when visible" .. GUI.MarkupNewline ..
        "MemoryStore: yellow last-known ring + pillar" .. GUI.MarkupNewline ..
        "BT: searches memory when vision target is none" .. GUI.MarkupNewline ..
        GUI.MarkupNewline ..
        "Vision target: " .. sense .. "  range: " .. string.format("%.1f", visionRange) .. GUI.MarkupNewline ..
        "Memory: " .. memoryState .. "  age: " .. memoryAge .. "  conf: " .. memoryConfidence .. GUI.MarkupNewline ..
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
        DebugDrawer:drawCircle(pos + Vector3(0, 0.1, 0), _GetVisionRange(bb), 48, UtilColors.Blue, false)
    end
    if pursuer ~= nil and target ~= nil and not _perceptionDemo.relocated then
        DebugDrawer:drawLine(pursuer:GetPosition() + Vector3(0, 1.6, 0), target:GetPosition() + Vector3(0, 1.6, 0), UtilColors.Green)
    end
    if lastKnownPos ~= nil then
        _DrawLastKnownMarker(lastKnownPos)
        if pursuer ~= nil then
            DebugDrawer:drawLine(pursuer:GetPosition() + Vector3(0, 1.2, 0), lastKnownPos + Vector3(0, 1.2, 0), UtilColors.Yellow)
        end
    end
    if _perceptionDemo.relocatedPos ~= nil and lastKnownPos ~= nil then
        DebugDrawer:drawLine(lastKnownPos + Vector3(0, 0.6, 0), _perceptionDemo.relocatedPos + Vector3(0, 0.6, 0), UtilColors.Red)
    end
end

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
    if _demoPanel ~= nil then
        _demoPanel:setPosition(Vector2(20, 20))
        _demoPanel:setDimension(Vector2(_demoPanelSize.w, _demoPanelSize.h))
    end
end

function Sandbox_Initialize()
    GUI_CreateCameraAndProfileInfo()
    GUI_CreateSandboxText(infoText, textSize)
    _CreateDemoPanel()

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

    _InitializePerceptionDemo(sampleName)
end

function Sandbox_Update(deltaTimeInMillis)
    GUI_UpdateCameraInfo()
    GUI_UpdateProfileInfo()
    if _perceptionDemo.enabled and not _perceptionDemo.relocated then
        _perceptionDemo.elapsedMs = _perceptionDemo.elapsedMs + deltaTimeInMillis
        local relocateAfterMs = tonumber(_perceptionDemo.config and _perceptionDemo.config.relocateAfterMs) or 0
        if _perceptionDemo.elapsedMs >= relocateAfterMs then
            _RelocatePerceptionTarget()
        end
    end
    _DrawDemoGuides()
    _UpdateDemoPanel()
end
