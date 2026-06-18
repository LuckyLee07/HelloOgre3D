-- Sandbox11.lua
-- Chapter 8 Perception sample: multi-agent senses, team communication and shared enemy memory.

require("res.scripts.agent.SoldierAgent.lua")
require("res.scripts.agent.BehaviorSoldierAgent.lua")
local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

local textSize = {w = 300, h = 185}
local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
        "[Sandbox11 - Chapter 8 Sensory Perception]" .. GUI.MarkupNewline ..
        "W/A/S/D: to move" .. GUI.MarkupNewline ..
        "Hold Shift: to accelerate movement" .. GUI.MarkupNewline ..
        "Hold RMB: to look" .. GUI.MarkupNewline ..
        GUI.MarkupNewline ..
        "F1: to reset the camera" .. GUI.MarkupNewline ..
        "F2: toggle the menu" .. GUI.MarkupNewline ..
        "F3: toggle the navigation mesh" .. GUI.MarkupNewline ..
        "F5: toggle performance information" .. GUI.MarkupNewline ..
        "F6: toggle camera information" .. GUI.MarkupNewline ..
        "F7: toggle physics debug";

local _agents = {}
local _agentsById = {}
local _safeSpawnById = {}
local _spawnNavPositions = {}
local _demoPanel = nil
local _demoPanelSize = {w = 530, h = 270}
local _navMesh = nil
local _drawNavMesh = true
local _colors = {
    team0 = ColourValue(1.0, 0.45, 0.15),
    team1 = ColourValue(0.95, 0.9, 0.1),
    comm = ColourValue(0.0, 0.85, 1.0),
    shared = ColourValue(0.7, 0.55, 1.0),
}

local _chapter8 = {
    enabled = false,
    elapsedMs = 0,
    directSightings = {},
    sightLines = {},
    teamMemory = {},
    recentEvents = {},
    lastBroadcastAt = {},
    lastSharedAt = {},
    totalSightings = 0,
    totalBroadcasts = 0,
    totalSharedMoves = 0,
    activeDirectSightings = 0,
    activeTeamMemories = 0,
    activeSharedReceivers = 0,
    config = {},
}

local function _AxisValue(pos, axis)
    if axis == "x" then return pos.x end
    if axis == "y" then return pos.y end
    return pos.z
end

local function _PointInsideAabb(point, minPos, maxPos)
    return point.x >= minPos.x and point.x <= maxPos.x
        and point.y >= minPos.y and point.y <= maxPos.y
        and point.z >= minPos.z and point.z <= maxPos.z
end

local function _SegmentIntersectsAabb(startPos, endPos, box)
    if startPos == nil or endPos == nil or box == nil or box.position == nil or box.size == nil then
        return false
    end

    local halfSize = (tonumber(box.size) or 0.0) * 0.5
    if halfSize <= 0.0 then
        return false
    end

    local minPos = Vector3(box.position.x - halfSize, box.position.y - halfSize, box.position.z - halfSize)
    local maxPos = Vector3(box.position.x + halfSize, box.position.y + halfSize, box.position.z + halfSize)
    if _PointInsideAabb(startPos, minPos, maxPos) or _PointInsideAabb(endPos, minPos, maxPos) then
        return false
    end

    local direction = endPos - startPos
    local tMin = 0.0
    local tMax = 1.0
    local axes = {"x", "y", "z"}
    for _, axis in ipairs(axes) do
        local startValue = _AxisValue(startPos, axis)
        local directionValue = _AxisValue(direction, axis)
        local minValue = _AxisValue(minPos, axis)
        local maxValue = _AxisValue(maxPos, axis)

        if math.abs(directionValue) < 0.0001 then
            if startValue < minValue or startValue > maxValue then
                return false
            end
        else
            local invDirection = 1.0 / directionValue
            local t1 = (minValue - startValue) * invDirection
            local t2 = (maxValue - startValue) * invDirection
            if t1 > t2 then
                t1, t2 = t2, t1
            end
            if t1 > tMin then tMin = t1 end
            if t2 < tMax then tMax = t2 end
            if tMin > tMax then
                return false
            end
        end
    end

    return tMax >= 0.0 and tMin <= 1.0
end

local function _IsBlockedByLevel(startPos, endPos)
    local boxes = _G.SandboxLevelBoxes or {}
    for _, box in ipairs(boxes) do
        if _SegmentIntersectsAabb(startPos, endPos, box) then
            return true
        end
    end
    return false
end

local function _GetAgentHeight(agent)
    if agent ~= nil and agent.GetHeight ~= nil then
        local height = tonumber(agent:GetHeight()) or 0.0
        if height > 0.0 then
            return height
        end
    end
    return 1.6
end

local function _GetSightHeightRatio(key, defaultValue)
    local config = _chapter8.config or {}
    local value = tonumber(config[key])
    if value == nil then
        return defaultValue
    end
    return value
end

local function _GetSightAnchor(agent, heightRatio)
    if agent == nil then
        return nil
    end
    return agent:GetPosition() + Vector3(0, _GetAgentHeight(agent) * heightRatio, 0)
end

local function _PassesOriginalFov(agent, target)
    if agent == nil or target == nil then
        return false
    end

    local toTarget = target:GetPosition() - agent:GetPosition()
    toTarget.y = 0
    if toTarget:squaredLength() <= 0.0001 then
        return true
    end

    local forward = agent:GetForward()
    forward.y = 0
    if forward:squaredLength() <= 0.0001 then
        return true
    end

    toTarget = Vector.Normalize(toTarget)
    forward = Vector.Normalize(forward)
    return forward:dotProduct(toTarget) >= 0.707
end

local function _BuildSightLine(agent, target)
    if agent == nil or target == nil or agent == target then
        return nil
    end
    if not _PassesOriginalFov(agent, target) then
        return nil
    end

    local startPos = _GetSightAnchor(agent, _GetSightHeightRatio("sightOriginHeightRatio", 0.30))
    local endPos = _GetSightAnchor(target, _GetSightHeightRatio("sightTargetHeightRatio", 0.0))
    return {
        startPos = startPos,
        endPos = endPos,
        spotter = agent,
        target = target,
        blocked = _IsBlockedByLevel(startPos, endPos),
    }
end

local function _NewTeamMemory()
    return {
        visibleEnemies = {},
        lastEvent = nil,
    }
end

local function _EnsureTeamMemory(teamId)
    teamId = tonumber(teamId) or 0
    if _chapter8.teamMemory[teamId] == nil then
        _chapter8.teamMemory[teamId] = _NewTeamMemory()
    end
    return _chapter8.teamMemory[teamId]
end

local function _GetBlackboard(agent)
    if agent == nil then return nil end
    local ai = AgentComponents.GetAI(agent)
    return ai ~= nil and ai:GetBlackboard() or nil
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

local function _FormatVec3(pos)
    if pos == nil then return "-" end
    return string.format("%.1f, %.1f, %.1f", pos.x, pos.y, pos.z)
end

local function _Chapter8DebugLog(...)
    local config = _chapter8.config or {}
    if config.debugLog == true then
        print(...)
    end
end

local function _ApplyChapter8Camera()
    local camera = SandboxCamera:GetCamera()
    local preset = ConfigManager:GetSamplePreset("Sandbox11")
    local config = preset ~= nil and preset.chapter8Comms or nil
    local position = config ~= nil and config.cameraPosition or nil
    local orientation = config ~= nil and config.cameraOrientation or nil

    if position == nil then position = { -30, 18, -17 } end
    if orientation == nil then orientation = { -146, -40, -157 } end

    camera:setPosition(Vector3(position[1], position[2], position[3]))
    camera:setOrientation(Quaternion(orientation[1], orientation[2], orientation[3]))
end

local function _GetAgentId(agent)
    return agent ~= nil and agent:GetObjId() or -1
end

local function _GetTeamColor(teamId)
    return tonumber(teamId) == 1 and _colors.team1 or _colors.team0
end

local function _IsAlive(agent)
    return agent ~= nil and agent:GetHealth() > 0
end

local function _IsInsideDemoStage(pos)
    local bounds = _chapter8.config and _chapter8.config.stageBounds or nil
    if bounds == nil or pos == nil then
        return true
    end
    return pos.x >= (tonumber(bounds.minX) or -100000.0)
        and pos.x <= (tonumber(bounds.maxX) or 100000.0)
        and pos.y >= (tonumber(bounds.minY) or -100000.0)
        and pos.y <= (tonumber(bounds.maxY) or 100000.0)
        and pos.z >= (tonumber(bounds.minZ) or -100000.0)
        and pos.z <= (tonumber(bounds.maxZ) or 100000.0)
end

local function _ClearAgentDemoOrders(agent)
    if agent == nil then return end
    agent:SetVelocity(Vector3(0, 0, 0))
    agent:SetTarget(agent:GetPosition())
    if agent.ClearMovePosition ~= nil then
        agent:ClearMovePosition()
    end

    local bb = _GetBlackboard(agent)
    if bb ~= nil then
        bb:Remove("enemy")
        bb:Remove("movePos")
        bb:Remove("chapter8.sharedTargetId")
        bb:Remove("chapter8.sharedTargetPos")
        bb:Remove("chapter8.receivedEvent")
    end
end

local function _KeepAgentsInDemoStage()
    if _chapter8.config == nil or _chapter8.config.keepAgentsInDemoStage ~= true then
        return
    end

    for _, agent in ipairs(_agents) do
        if agent ~= nil and _IsAlive(agent) then
            local pos = agent:GetPosition()
            if not _IsInsideDemoStage(pos) then
                local safePos = _safeSpawnById[_GetAgentId(agent)]
                if safePos ~= nil then
                    agent:setPosition(safePos)
                    _ClearAgentDemoOrders(agent)
                    _Chapter8DebugLog("[Chapter8Comms] stage guard", "id=", _GetAgentId(agent), "from=", _FormatVec3(pos), "to=", _FormatVec3(safePos))
                end
            end
        end
    end
end

local function _CreateDemoPanel()
    if _chapter8.config == nil or _chapter8.config.showDemoPanel ~= true then
        return
    end

    local panel = SandboxUI:CreateUIFrame()
    panel:setPosition(Vector2(20, 20))
    panel:setDimension(Vector2(_demoPanelSize.w, _demoPanelSize.h))
    panel:setTextMargin(12, 10)
    panel:setGradientColor(Gorilla.Gradient_NorthSouth, ColourValue(0.04, 0.07, 0.09, 0.78), ColourValue(0.0, 0.0, 0.0, 0.78))
    _demoPanel = panel
end

local function _ReadDirectSighting(agent)
    local bb = _GetBlackboard(agent)
    if bb == nil or not bb:GetBool("perception.hasTarget", false) then
        return nil
    end
    if not bb:Has("perception.targetId") or not bb:Has("perception.targetPos") then
        return nil
    end

    local targetId = bb:GetObjectId("perception.targetId", -1)
    if targetId < 0 then
        return nil
    end

    return {
        eventType = "EnemySighted",
        teamId = agent:GetTeamId(),
        spotter = agent,
        spotterId = _GetAgentId(agent),
        targetId = targetId,
        targetPos = bb:GetVec3("perception.targetPos"),
        distance = _GetFloatValue(bb, "perception.targetDistance", -1.0),
        confidence = _GetFloatValue(bb, "memory.snapshot.lastKnownEnemyConfidence", 1.0),
        lastSeenMs = _chapter8.elapsedMs,
    }
end

local function _BuildOriginalSighting(agent, target, line)
    if agent == nil or target == nil or line == nil or line.blocked then
        return nil
    end
    if target:GetTeamId() == agent:GetTeamId() or not _IsAlive(target) then
        return nil
    end

    local toTarget = target:GetPosition() - agent:GetPosition()
    return {
        eventType = "EnemySighted",
        teamId = agent:GetTeamId(),
        spotter = agent,
        spotterId = _GetAgentId(agent),
        targetId = _GetAgentId(target),
        targetPos = target:GetPosition(),
        distance = toTarget:length(),
        confidence = 1.0,
        lastSeenMs = _chapter8.elapsedMs,
    }
end

local function _PushRecentEvent(event)
    table.insert(_chapter8.recentEvents, event)
    while #_chapter8.recentEvents > 16 do
        table.remove(_chapter8.recentEvents, 1)
    end
end

local function _ShouldBroadcast(sighting)
    local intervalMs = tonumber(_chapter8.config.broadcastIntervalMs) or 500
    local key = tostring(sighting.teamId) .. ":" .. tostring(sighting.spotterId) .. ":" .. tostring(sighting.targetId)
    local lastAt = _chapter8.lastBroadcastAt[key]
    if lastAt ~= nil and (_chapter8.elapsedMs - lastAt) < intervalMs then
        return false
    end
    _chapter8.lastBroadcastAt[key] = _chapter8.elapsedMs
    return true
end

local function _MarkSharedMove(agent, sighting)
    local intervalMs = tonumber(_chapter8.config.broadcastIntervalMs) or 500
    local receiverId = _GetAgentId(agent)
    local key = tostring(receiverId) .. ":" .. tostring(sighting.targetId)
    local lastAt = _chapter8.lastSharedAt[key]
    if lastAt ~= nil and (_chapter8.elapsedMs - lastAt) < intervalMs then
        return
    end

    _chapter8.lastSharedAt[key] = _chapter8.elapsedMs
    _chapter8.totalSharedMoves = _chapter8.totalSharedMoves + 1
    _Chapter8DebugLog("[Chapter8Comms] SharedEnemy", "team=", agent:GetTeamId(), "receiver=", receiverId, "from=", sighting.spotterId, "target=", sighting.targetId, "pos=", _FormatVec3(sighting.targetPos))
end

local function _PublishEnemySighted(sighting)
    local memory = _EnsureTeamMemory(sighting.teamId)
    local existing = memory.visibleEnemies[sighting.targetId]
    local isNew = existing == nil or (_chapter8.elapsedMs - existing.lastSeenMs) > 250

    memory.visibleEnemies[sighting.targetId] = sighting
    if isNew then
        _chapter8.totalSightings = _chapter8.totalSightings + 1
    end

    if not _ShouldBroadcast(sighting) then
        return
    end

    local event = {
        eventType = "EnemySighted",
        teamId = sighting.teamId,
        senderId = sighting.spotterId,
        targetId = sighting.targetId,
        targetPos = sighting.targetPos,
        timeMs = _chapter8.elapsedMs,
    }
    memory.lastEvent = event
    _chapter8.totalBroadcasts = _chapter8.totalBroadcasts + 1
    _PushRecentEvent(event)
    _Chapter8DebugLog("[Chapter8Comms] EnemySighted", "team=", sighting.teamId, "sender=", sighting.spotterId, "target=", sighting.targetId, "pos=", _FormatVec3(sighting.targetPos))
end

local function _ExpireTeamMemory()
    local ttlMs = tonumber(_chapter8.config.teamMemoryTtlMs) or 2500
    _chapter8.activeTeamMemories = 0
    for _, memory in pairs(_chapter8.teamMemory) do
        for targetId, sighting in pairs(memory.visibleEnemies) do
            if (_chapter8.elapsedMs - sighting.lastSeenMs) > ttlMs then
                memory.visibleEnemies[targetId] = nil
            else
                _chapter8.activeTeamMemories = _chapter8.activeTeamMemories + 1
            end
        end
    end
end

local function _PruneRecentEvents()
    local ttlMs = tonumber(_chapter8.config.eventTtlMs) or 1400
    local kept = {}
    for _, event in ipairs(_chapter8.recentEvents) do
        if (_chapter8.elapsedMs - event.timeMs) <= ttlMs then
            table.insert(kept, event)
        end
    end
    _chapter8.recentEvents = kept
end

local function _SelectTeamMemoryForAgent(agent)
    local memory = _EnsureTeamMemory(agent:GetTeamId())
    local receiverId = _GetAgentId(agent)
    local best = nil
    local bestDistanceSq = nil
    for targetId, sighting in pairs(memory.visibleEnemies) do
        local target = _agentsById[targetId]
        if sighting.spotterId ~= receiverId and _IsAlive(target) and target:GetTeamId() ~= agent:GetTeamId() then
            local distanceSq = (agent:GetPosition() - sighting.targetPos):squaredLength()
            if best == nil or distanceSq < bestDistanceSq then
                best = sighting
                bestDistanceSq = distanceSq
            end
        end
    end
    return best
end

local function _HasDirectTarget(agent)
    local bb = _GetBlackboard(agent)
    return bb ~= nil and bb:GetBool("perception.hasTarget", false)
end

local function _ApplyTeamMemoryToAgent(agent)
    if agent == nil or _HasDirectTarget(agent) then
        return false
    end

    local sighting = _SelectTeamMemoryForAgent(agent)
    local bb = _GetBlackboard(agent)
    local target = sighting ~= nil and _agentsById[sighting.targetId] or nil
    if sighting == nil or bb == nil then
        if bb ~= nil then
            bb:Remove("chapter8.sharedTargetId")
            bb:Remove("chapter8.sharedTargetPos")
            bb:Remove("chapter8.receivedEvent")
        end
        return false
    end

    if not _IsAlive(target) then
        return false
    end

    bb:SetAgent("enemy", target)
    bb:SetString("chapter8.receivedEvent", "EnemySighted")
    bb:SetObjectId("chapter8.sharedTargetId", sighting.targetId)
    bb:SetVec3("chapter8.sharedTargetPos", sighting.targetPos)
    bb:SetInt("chapter8.sharedFromAgentId", sighting.spotterId)
    bb:SetInt("chapter8.sharedSeenAtMs", sighting.lastSeenMs)
    _MarkSharedMove(agent, sighting)
    return true
end

local function _UpdateChapter8Comms(deltaTimeInMillis)
    if not _chapter8.enabled then return end

    _chapter8.elapsedMs = _chapter8.elapsedMs + math.max(0, deltaTimeInMillis)
    _chapter8.directSightings = {}
    _chapter8.sightLines = {}
    _chapter8.activeDirectSightings = 0
    _chapter8.activeSharedReceivers = 0

    for _, spotter in ipairs(_agents) do
        if _IsAlive(spotter) then
            for _, target in ipairs(_agents) do
                if _IsAlive(target) and target ~= spotter then
                    local line = _BuildSightLine(spotter, target)
                    if line ~= nil then
                        table.insert(_chapter8.sightLines, line)
                        local sighting = _BuildOriginalSighting(spotter, target, line)
                        if sighting ~= nil then
                            table.insert(_chapter8.directSightings, sighting)
                            _chapter8.activeDirectSightings = _chapter8.activeDirectSightings + 1
                            _PublishEnemySighted(sighting)
                        end
                    end
                end
            end
        end
    end

    _ExpireTeamMemory()
    _PruneRecentEvents()

    for _, agent in ipairs(_agents) do
        if _IsAlive(agent) and _ApplyTeamMemoryToAgent(agent) then
            _chapter8.activeSharedReceivers = _chapter8.activeSharedReceivers + 1
        end
    end
end

local function _DrawAgentMarker(agent)
    if agent == nil then return end
    local pos = agent:GetPosition()
    local color = _GetTeamColor(agent:GetTeamId())
    DebugDrawer:drawCircle(pos + Vector3(0, 0.15, 0), 1.15, 24, color, false)
    DebugDrawer:drawLine(pos + Vector3(0, 0.2, 0), pos + Vector3(0, 3.0, 0), color)
end

local function _DrawMemoryMarker(pos, color)
    if pos == nil then return end
    local base = Vector3(pos.x, pos.y, pos.z)
    DebugDrawer:drawCircle(base + Vector3(0, 0.2, 0), 1.4, 32, color, false)
    DebugDrawer:drawLine(base + Vector3(-1.2, 0.35, 0), base + Vector3(1.2, 0.35, 0), color)
    DebugDrawer:drawLine(base + Vector3(0, 0.35, -1.2), base + Vector3(0, 0.35, 1.2), color)
    DebugDrawer:drawLine(base + Vector3(0, 0.2, 0), base + Vector3(0, 4.2, 0), color)
end

local function _DrawDirectSightings()
    if _chapter8.config == nil or _chapter8.config.drawDirectSightLines ~= true then
        return
    end

    for _, line in ipairs(_chapter8.sightLines or {}) do
        if line.startPos ~= nil and line.endPos ~= nil then
            local color = line.blocked and UtilColors.Red or UtilColors.Green
            DebugDrawer:drawLine(line.startPos, line.endPos, color)
        end
    end
end

local function _DrawTeamBroadcasts()
    if _chapter8.config == nil or _chapter8.config.drawTeamBroadcasts ~= true then
        return
    end

    for _, event in ipairs(_chapter8.recentEvents) do
        local sender = _agentsById[event.senderId]
        if sender ~= nil then
            for _, agent in ipairs(_agents) do
                if agent ~= sender and agent:GetTeamId() == event.teamId then
                    DebugDrawer:drawLine(sender:GetPosition() + Vector3(0, 2.5, 0), agent:GetPosition() + Vector3(0, 2.5, 0), _colors.comm)
                end
            end
        end
    end
end

local function _DrawTeamMemories()
    if _chapter8.config == nil or _chapter8.config.drawTeamMemories ~= true then
        return
    end

    for teamId, memory in pairs(_chapter8.teamMemory) do
        local color = _GetTeamColor(teamId)
        for _, sighting in pairs(memory.visibleEnemies) do
            _DrawMemoryMarker(sighting.targetPos, color)
        end
    end

    for _, agent in ipairs(_agents) do
        local bb = _GetBlackboard(agent)
        if bb ~= nil and bb:Has("chapter8.sharedTargetPos") then
            local pos = bb:GetVec3("chapter8.sharedTargetPos")
            DebugDrawer:drawLine(agent:GetPosition() + Vector3(0, 1.1, 0), pos + Vector3(0, 1.1, 0), _colors.shared)
        end
    end
end

local function _DrawDemoGuides()
    for _, agent in ipairs(_agents) do
        if _chapter8.config ~= nil and _chapter8.config.drawAgentMarkers == true then
            _DrawAgentMarker(agent)
        end
        if _chapter8.config ~= nil and _chapter8.config.drawVisionRange == true then
            local bb = _GetBlackboard(agent)
            local range = _GetFloatValue(bb, "perception.visionRange", 16.0)
            if range > 0.0 then
                DebugDrawer:drawCircle(agent:GetPosition() + Vector3(0, 0.08, 0), range, 48, UtilColors.Blue, false)
            end
        end
    end

    _DrawDirectSightings()
    _DrawTeamBroadcasts()
    _DrawTeamMemories()
end

local function _TeamSummary(teamId)
    local memory = _EnsureTeamMemory(teamId)
    local count = 0
    local last = "-"
    for targetId, sighting in pairs(memory.visibleEnemies) do
        count = count + 1
        last = "#" .. tostring(targetId) .. " by #" .. tostring(sighting.spotterId)
    end
    return count, last
end

local function _LastEventSummary()
    local event = _chapter8.recentEvents[#_chapter8.recentEvents]
    if event == nil then
        return "-"
    end
    return string.format("%s team=%d sender=#%d target=#%d", event.eventType, event.teamId, event.senderId, event.targetId)
end

local function _BuildDemoPanelText()
    local team0Count, team0Last = _TeamSummary(0)
    local team1Count, team1Last = _TeamSummary(1)

    return GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
        "[Sandbox11 - Chapter 8 Perception]" .. GUI.MarkupNewline ..
        GUI.MarkupColor.Green .. "Stage: CHAPTER 8 ORIGINAL FLAVOR" .. GUI.MarkupColor.White .. GUI.MarkupNewline ..
        "Goal: 6 soldiers roam, see, communicate and fight" .. GUI.MarkupNewline ..
        GUI.MarkupNewline ..
        "Vision: green line + blue range ring" .. GUI.MarkupNewline ..
        "TeamComms: cyan lines broadcast EnemySighted" .. GUI.MarkupNewline ..
        "TeamMemory: shared EnemySighted becomes teammate enemy" .. GUI.MarkupNewline ..
        GUI.MarkupNewline ..
        "Direct sightings: " .. tostring(_chapter8.activeDirectSightings) ..
            "  team memories: " .. tostring(_chapter8.activeTeamMemories) ..
            "  receivers: " .. tostring(_chapter8.activeSharedReceivers) .. GUI.MarkupNewline ..
        "Team 0 memory: " .. tostring(team0Count) .. "  " .. team0Last .. GUI.MarkupNewline ..
        "Team 1 memory: " .. tostring(team1Count) .. "  " .. team1Last .. GUI.MarkupNewline ..
        "Last event: " .. _LastEventSummary() .. GUI.MarkupNewline ..
        "Totals: sightings=" .. tostring(_chapter8.totalSightings) ..
            " broadcasts=" .. tostring(_chapter8.totalBroadcasts) ..
            " sharedEnemies=" .. tostring(_chapter8.totalSharedMoves)
end

local function _UpdateDemoPanel()
    if _demoPanel == nil then return end
    _demoPanel:setMarkupText(_BuildDemoPanelText())
end

local function _InitializeChapter8Comms(sampleName)
    local preset = ConfigManager:GetSamplePreset(sampleName)
    local config = preset.chapter8Comms or {}
    _chapter8.enabled = config.enabled == true
    _chapter8.elapsedMs = 0
    _chapter8.directSightings = {}
    _chapter8.teamMemory = {
        [0] = _NewTeamMemory(),
        [1] = _NewTeamMemory(),
    }
    _chapter8.recentEvents = {}
    _chapter8.lastBroadcastAt = {}
    _chapter8.lastSharedAt = {}
    _chapter8.totalSightings = 0
    _chapter8.totalBroadcasts = 0
    _chapter8.totalSharedMoves = 0
    _chapter8.config = config

    if _chapter8.enabled then
        _Chapter8DebugLog("[Chapter8Comms] armed", "agents=", #_agents, "teamMemoryTtlMs=", tonumber(config.teamMemoryTtlMs) or 2500)
    end
end

local function _IsFarEnoughFromPlacedAgents(position, minDistance)
    local minDistanceSq = minDistance * minDistance
    for _, placedPosition in ipairs(_spawnNavPositions) do
        if (position - placedPosition):squaredLength() < minDistanceSq then
            return false
        end
    end
    return true
end

local function _FindOriginalChapter8Spawn(config)
    local minDistance = tonumber(config.minSpawnDistance) or 18.0
    local minFallbackDistance = tonumber(config.minFallbackSpawnDistance) or math.min(12.0, minDistance)
    local minSpawnY = tonumber(config.minSpawnY) or -1.0
    local maxAttempts = tonumber(config.maxSpawnAttempts) or 256
    local distanceSteps = {
        minDistance,
        math.max(minFallbackDistance, minDistance * 0.85),
        math.max(minFallbackDistance, minDistance * 0.70),
        minFallbackDistance,
    }

    for _, distance in ipairs(distanceSteps) do
        for _ = 1, maxAttempts do
            local position = Sandbox:RandomPoint("default")
            if position ~= nil
                and position.y >= minSpawnY
                and _IsFarEnoughFromPlacedAgents(position, distance) then
                return position
            end
        end
    end
    return nil
end

local function _PlaceChapter8Agent(agent, sampleName, index, config)
    if config.useOriginalRandomSpawn ~= false then
        local position = _FindOriginalChapter8Spawn(config)
        if position ~= nil then
            table.insert(_spawnNavPositions, Vector3(position.x, position.y, position.z))
            position.y = position.y + agent:GetHeight() * 0.5
            agent:setPosition(position)
            agent:SetTarget(position)
            agent:SetTargetRadius(1)
            return position
        end
        _Chapter8DebugLog("[Chapter8Comms] warning random spawn fallback", "index=", index)
    end

    local position = ConfigManager:PlaceAgentOnPresetSpawn(agent, sampleName, index, "default")
    if position ~= nil then
        table.insert(_spawnNavPositions, Vector3(position.x, position.y - agent:GetHeight() * 0.5, position.z))
    end
    return position
end

function EventHandle_Keyboard(keycode, pressed)
    GUI_HandleKeyEvent(keycode, pressed)

    if not pressed then return end
    if (keycode == OIS.KC_F1) then
        _ApplyChapter8Camera()
    elseif (keycode == OIS.KC_F3) then
        _drawNavMesh = not _drawNavMesh
        if _navMesh ~= nil then
            _navMesh:SetDebugVisible(_drawNavMesh)
        end
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
    _agents = {}
    _agentsById = {}
    _safeSpawnById = {}
    _spawnNavPositions = {}
    _navMesh = nil
    _drawNavMesh = true

    GUI_CreateCameraAndProfileInfo()
    GUI_CreateSandboxText(infoText, textSize)

    Sandbox:SetUseCppFsmFlag(true)

    _ApplyChapter8Camera()

    Sandbox:SetSkyBox("ThickCloudsWaterSkyBox", Vector3(0, 180, 0));

    local plane = SandboxObjects:CreatePlane(200, 200);
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

    _navMesh = Sandbox:CreateNavigationMesh(navMeshConfig, 'default')
    _drawNavMesh = true
    if _navMesh ~= nil then _navMesh:SetDebugVisible(_drawNavMesh) end

    local sampleName = "Sandbox11"
    local agentCount = ConfigManager:GetAgentCount(sampleName, 6)
    local preset = ConfigManager:GetSamplePreset(sampleName)
    local chapter8Config = preset.chapter8Comms or {}
    _chapter8.config = chapter8Config
    _CreateDemoPanel()
    _Chapter8DebugLog(ConfigManager:BuildDebugSummary(sampleName))

    local agentLuafile = "res/scripts/agent/BehaviorSoldierAgent.lua"
    for i = 1, agentCount do
        local teamId = ConfigManager:GetAgentTeamId(sampleName, i)
        if chapter8Config.alternateTeams == true then
            teamId = (i % 2 == 1) and 1 or 0
        end
        local agentType = teamId == 0 and Soldier.AppearanceTypes.DARK or Soldier.AppearanceTypes.LIGHT
        local agent = Create_Soldier(agentLuafile, agentType, teamId)
        if chapter8Config.agentMaxSpeed ~= nil then
            agent:SetMaxSpeed(tonumber(chapter8Config.agentMaxSpeed) or SOLDIER_STAND_SPEED)
        end
        local spawnPoint = _PlaceChapter8Agent(agent, sampleName, i, chapter8Config)
        table.insert(_agents, agent)
        _agentsById[_GetAgentId(agent)] = agent
        _safeSpawnById[_GetAgentId(agent)] = spawnPoint

        _Chapter8DebugLog("[Chapter8Comms] spawn", "index=", i, "id=", _GetAgentId(agent), "team=", teamId, "pos=", _FormatVec3(spawnPoint))
    end

    _InitializeChapter8Comms(sampleName)
end

function Sandbox_Update(deltaTimeInMillis)
    GUI_UpdateCameraInfo()
    GUI_UpdateProfileInfo()
    _KeepAgentsInDemoStage()
    _UpdateChapter8Comms(deltaTimeInMillis)
    _DrawDemoGuides()
    _UpdateDemoPanel()
end
