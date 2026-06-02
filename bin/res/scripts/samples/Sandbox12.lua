-- Sandbox12.lua
-- TeamBlackboard sample: shared team memory built on top of the Chapter 8 communication battlefield.

require("res.scripts.agent.SoldierAgent.lua")
require("res.scripts.agent.BehaviorSoldierAgent.lua")
local TeamBlackboard = require("res.scripts.ai.team.TeamBlackboard.lua")

local textSize = {w = 300, h = 185}
local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
        "[Sandbox12 - Team Blackboard]" .. GUI.MarkupNewline ..
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
local _demoPanelSize = {w = 400, h = 280}
local _navMesh = nil
local _drawNavMesh = true
local _colors = {
    team0 = ColourValue(1.0, 0.45, 0.15),
    team1 = ColourValue(0.95, 0.9, 0.1),
    comm = ColourValue(0.0, 0.85, 1.0),
    shared = ColourValue(0.7, 0.55, 1.0),
    support = ColourValue(0.95, 0.25, 1.0),
}

local _TEAM_INTENT_SUPPORT = "support_shared_enemy"

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
    totalSupportResponses = 0,
    activeDirectSightings = 0,
    activeTeamMemories = 0,
    activeSharedReceivers = 0,
    activeSupportResponses = 0,
    sightScanElapsedMs = 0,
    sightScanIntervalMs = 0,
    sightScanCursor = 0,
    sightScanRunCount = 0,
    sightScanSkipCount = 0,
    sightPairChecks = 0,
    teamApplyElapsedMs = 0,
    teamApplyIntervalMs = 0,
    teamApplyCursor = 1,
    teamApplyRunCount = 0,
    teamApplySkipCount = 0,
    teamApplyAgentChecks = 0,
    cppFactApplyCount = 0,
    teamPruneElapsedMs = 0,
    teamPruneIntervalMs = 0,
    teamPruneRunCount = 0,
    teamPruneSkipCount = 0,
    openingSightingPublished = false,
    supportSmokePrinted = false,
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

local function _GetPositiveConfigNumber(key, defaultValue)
    local config = _chapter8.config or {}
    local value = tonumber(config[key])
    if value == nil or value <= 0 then
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

local function _EnsureTeamMemory(teamId)
    teamId = tonumber(teamId) or 0
    if _chapter8.teamMemory == nil then
        _chapter8.teamMemory = {}
    end
    local memory = TeamBlackboard:GetTeam(teamId)
    _chapter8.teamMemory[teamId] = memory
    return memory
end

local function _GetBlackboard(agent)
    if agent == nil or agent.GetAI == nil then return nil end
    local ai = agent:GetAI()
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

local function _ClearSupportBlackboard(bb)
    if bb == nil then
        return
    end
    local intent = _GetStringValue(bb, "team.intent", "")
    if intent == _TEAM_INTENT_SUPPORT then
        bb:Remove("movePos")
    end
    bb:Remove("chapter8.sharedTargetId")
    bb:Remove("chapter8.sharedTargetPos")
    bb:Remove("chapter8.receivedEvent")
    bb:Remove("team.intent")
    bb:Remove("team.focusTargetId")
    bb:Remove("team.lastKnownEnemyPos")
    bb:Remove("team.supportTargetPos")
    bb:Remove("team.supportFromAgentId")
    bb:Remove("team.supportSeenAtMs")
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
    local camera = Sandbox:GetCamera()
    local preset = ConfigManager:GetSamplePreset("Sandbox12")
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
        _ClearSupportBlackboard(bb)
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

    local panel = Sandbox:CreateUIFrame()
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
        eventType = TeamBlackboard.EventTypes.EnemySighted,
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
        eventType = TeamBlackboard.EventTypes.EnemySighted,
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

local function _BuildSupportPosition(agent, sighting)
    if sighting == nil or sighting.targetPos == nil then
        return nil
    end

    local radius = tonumber(_chapter8.config.supportOffsetDistance) or 3.0
    if radius <= 0.0 then
        return sighting.targetPos
    end

    local responderId = _GetAgentId(agent)
    local angle = (responderId % 8) * 0.785398163
    local candidate = sighting.targetPos + Vector3(math.cos(angle) * radius, 0, math.sin(angle) * radius)
    local navPos = Sandbox:FindClosestPoint("default", candidate)
    return navPos ~= nil and navPos or candidate
end

local function _MarkSupportResponse(agent, sighting, supportPos)
    local response, isNew = TeamBlackboard:RememberSupportResponse(agent:GetTeamId(), {
        teamId = agent:GetTeamId(),
        responderId = _GetAgentId(agent),
        fromAgentId = sighting.spotterId,
        targetId = sighting.targetId,
        targetPos = sighting.targetPos,
        supportPos = supportPos,
        timeMs = _chapter8.elapsedMs,
    })
    if isNew then
        _chapter8.totalSupportResponses = _chapter8.totalSupportResponses + 1
    end
    return response
end

local function _PublishEnemySighted(sighting)
    local memory = _EnsureTeamMemory(sighting.teamId)
    local existing = memory.visibleEnemies[sighting.targetId]
    local isNew = existing == nil or (_chapter8.elapsedMs - existing.lastSeenMs) > 250

    sighting = TeamBlackboard:RememberVisibleEnemy(sighting.teamId, sighting) or sighting
    if isNew then
        _chapter8.totalSightings = _chapter8.totalSightings + 1
    end

    if not _ShouldBroadcast(sighting) then
        return
    end

    local event = TeamBlackboard:BuildEnemySightedEvent(sighting, _chapter8.elapsedMs)
    if event == nil then
        return
    end
    TeamBlackboard:SetLastEvent(sighting.teamId, event)
    _chapter8.totalBroadcasts = _chapter8.totalBroadcasts + 1
    _PushRecentEvent(event)
    _Chapter8DebugLog("[Chapter8Comms] EnemySighted", "team=", sighting.teamId, "sender=", sighting.spotterId, "target=", sighting.targetId, "pos=", _FormatVec3(sighting.targetPos))
end

local function _ExpireTeamMemory()
    local ttlMs = tonumber(_chapter8.config.teamMemoryTtlMs) or 2500
    local supportTtlMs = tonumber(_chapter8.config.supportResponseTtlMs) or ttlMs
    _chapter8.activeTeamMemories = TeamBlackboard:PruneVisibleEnemies(_chapter8.elapsedMs, ttlMs)
    _chapter8.activeSupportResponses = TeamBlackboard:PruneSupportResponses(_chapter8.elapsedMs, supportTtlMs)
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

local function _PublishScriptedOpeningSighting()
    local config = _chapter8.config.scriptedOpeningSighting
    if _chapter8.openingSightingPublished or config == nil or config.enabled ~= true then
        return
    end

    local delayMs = tonumber(config.delayMs) or 500
    if _chapter8.elapsedMs < delayMs then
        return
    end

    local spotter = _agents[tonumber(config.spotterIndex) or 1]
    local target = _agents[tonumber(config.targetIndex) or 2]
    if not _IsAlive(spotter) or not _IsAlive(target) or spotter:GetTeamId() == target:GetTeamId() then
        return
    end

    local toTarget = target:GetPosition() - spotter:GetPosition()
    _PublishEnemySighted({
        eventType = TeamBlackboard.EventTypes.EnemySighted,
        teamId = spotter:GetTeamId(),
        spotter = spotter,
        spotterId = _GetAgentId(spotter),
        targetId = _GetAgentId(target),
        targetPos = target:GetPosition(),
        distance = toTarget:length(),
        confidence = 1.0,
        lastSeenMs = _chapter8.elapsedMs,
    })
    _chapter8.openingSightingPublished = true
    print("[TeamBlackboardDemo] opening EnemySighted", "team=", spotter:GetTeamId(), "sender=", _GetAgentId(spotter), "target=", _GetAgentId(target))
end

local function _MaybePrintTeamBlackboardSmoke()
    if _chapter8.supportSmokePrinted or _G.HELLO_SANDBOX_SMOKE_MODE ~= true then
        return
    end
    if _chapter8.totalBroadcasts > 0
        and _chapter8.totalSupportResponses > 0
        and _chapter8.totalSharedMoves > 0
        and TeamBlackboard:GetCppFactCount() > 0
        and _chapter8.cppFactApplyCount > 0
        and _chapter8.sightScanRunCount > 0
        and _chapter8.sightScanSkipCount > 0
        and _chapter8.teamApplyRunCount > 0
        and _chapter8.teamApplySkipCount > 0 then
        _chapter8.supportSmokePrinted = true
        print("[TeamBlackboardSmoke] PASS",
            "broadcasts=", _chapter8.totalBroadcasts,
            "supportResponses=", _chapter8.totalSupportResponses,
            "sharedMoves=", _chapter8.totalSharedMoves,
            "scanRuns=", _chapter8.sightScanRunCount,
            "scanSkips=", _chapter8.sightScanSkipCount,
            "pairChecks=", _chapter8.sightPairChecks,
            "applyRuns=", _chapter8.teamApplyRunCount,
            "applySkips=", _chapter8.teamApplySkipCount,
            "agentChecks=", _chapter8.teamApplyAgentChecks,
            "cppFacts=", TeamBlackboard:GetCppFactCount(),
            "cppReports=", TeamBlackboard:GetCppReportCount(),
            "cppApplies=", _chapter8.cppFactApplyCount)
    end
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
    if agent == nil then
        return false
    end

    if TeamBlackboard:WriteBestCppEnemyToBlackboard(agent, "team.cpp", false) then
        _chapter8.cppFactApplyCount = _chapter8.cppFactApplyCount + 1
    end

    local sighting = _SelectTeamMemoryForAgent(agent)
    local bb = _GetBlackboard(agent)
    local target = sighting ~= nil and _agentsById[sighting.targetId] or nil
    if _HasDirectTarget(agent) then
        _ClearSupportBlackboard(bb)
        return false
    end
    if sighting == nil or bb == nil then
        if bb ~= nil then
            _ClearSupportBlackboard(bb)
        end
        return false
    end

    if not _IsAlive(target) then
        _ClearSupportBlackboard(bb)
        return false
    end

    local supportPos = _BuildSupportPosition(agent, sighting)
    if supportPos == nil then
        return false
    end

    bb:Remove("enemy")
    bb:SetString("team.intent", _TEAM_INTENT_SUPPORT)
    bb:SetObjectId("team.focusTargetId", sighting.targetId)
    bb:SetVec3("team.lastKnownEnemyPos", sighting.targetPos)
    bb:SetVec3("team.supportTargetPos", supportPos)
    bb:SetInt("team.supportFromAgentId", sighting.spotterId)
    bb:SetInt("team.supportSeenAtMs", sighting.lastSeenMs)
    bb:SetString("chapter8.receivedEvent", TeamBlackboard.EventTypes.EnemySighted)
    bb:SetObjectId("chapter8.sharedTargetId", sighting.targetId)
    bb:SetVec3("chapter8.sharedTargetPos", sighting.targetPos)
    bb:SetInt("chapter8.sharedFromAgentId", sighting.spotterId)
    bb:SetInt("chapter8.sharedSeenAtMs", sighting.lastSeenMs)
    bb:SetVec3("movePos", supportPos)
    agent:SetMovePosition(supportPos)
    _MarkSupportResponse(agent, sighting, supportPos)
    _MarkSharedMove(agent, sighting)
    return true
end

local function _GetSightPairFromCursor(cursor)
    local count = #_agents
    if count <= 1 then
        return nil, nil
    end

    local pairsPerSpotter = count - 1
    local pairIndex = cursor % (count * pairsPerSpotter)
    local spotterIndex = math.floor(pairIndex / pairsPerSpotter) + 1
    local targetSlot = (pairIndex % pairsPerSpotter) + 1
    local targetIndex = targetSlot >= spotterIndex and targetSlot + 1 or targetSlot
    return _agents[spotterIndex], _agents[targetIndex]
end

local function _UpdateScheduledSightScan(deltaTimeInMillis)
    local totalPairs = #_agents * math.max(0, #_agents - 1)
    if totalPairs <= 0 then
        return
    end

    local intervalMs = math.max(1, _GetPositiveConfigNumber("sightScanIntervalMs", 120))
    local pairsPerTick = math.max(1, math.min(totalPairs, _GetPositiveConfigNumber("sightPairsPerTick", 10)))
    _chapter8.sightScanIntervalMs = intervalMs
    _chapter8.sightScanElapsedMs = _chapter8.sightScanElapsedMs + math.max(0, tonumber(deltaTimeInMillis) or 0)

    local shouldForceFirst = _chapter8.sightScanRunCount == 0
    if not shouldForceFirst and _chapter8.sightScanElapsedMs < intervalMs then
        _chapter8.sightScanSkipCount = _chapter8.sightScanSkipCount + 1
        return
    end

    _chapter8.sightScanElapsedMs = 0
    _chapter8.sightScanRunCount = _chapter8.sightScanRunCount + 1
    _chapter8.directSightings = {}
    _chapter8.sightLines = {}
    _chapter8.activeDirectSightings = 0

    for _ = 1, pairsPerTick do
        local spotter, target = _GetSightPairFromCursor(_chapter8.sightScanCursor)
        _chapter8.sightScanCursor = (_chapter8.sightScanCursor + 1) % totalPairs
        _chapter8.sightPairChecks = _chapter8.sightPairChecks + 1
        if _IsAlive(spotter) and _IsAlive(target) and spotter ~= target then
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

local function _UpdateScheduledTeamPrune(deltaTimeInMillis)
    local intervalMs = math.max(1, _GetPositiveConfigNumber("teamPruneIntervalMs", 250))
    _chapter8.teamPruneIntervalMs = intervalMs
    _chapter8.teamPruneElapsedMs = _chapter8.teamPruneElapsedMs + math.max(0, tonumber(deltaTimeInMillis) or 0)
    local shouldForceFirst = _chapter8.teamPruneRunCount == 0
    if not shouldForceFirst and _chapter8.teamPruneElapsedMs < intervalMs then
        _chapter8.teamPruneSkipCount = _chapter8.teamPruneSkipCount + 1
        return
    end

    _chapter8.teamPruneElapsedMs = 0
    _chapter8.teamPruneRunCount = _chapter8.teamPruneRunCount + 1
    _ExpireTeamMemory()
end

local function _UpdateScheduledTeamApply(deltaTimeInMillis)
    if #_agents <= 0 then
        return
    end

    local intervalMs = math.max(1, _GetPositiveConfigNumber("teamApplyIntervalMs", 160))
    local agentsPerTick = math.max(1, math.min(#_agents, _GetPositiveConfigNumber("teamAgentsPerTick", 3)))
    _chapter8.teamApplyIntervalMs = intervalMs
    _chapter8.teamApplyElapsedMs = _chapter8.teamApplyElapsedMs + math.max(0, tonumber(deltaTimeInMillis) or 0)

    local shouldForceFirst = _chapter8.teamApplyRunCount == 0
    if not shouldForceFirst and _chapter8.teamApplyElapsedMs < intervalMs then
        _chapter8.teamApplySkipCount = _chapter8.teamApplySkipCount + 1
        return
    end

    _chapter8.teamApplyElapsedMs = 0
    _chapter8.teamApplyRunCount = _chapter8.teamApplyRunCount + 1
    _chapter8.activeSharedReceivers = 0

    for _ = 1, agentsPerTick do
        local index = ((_chapter8.teamApplyCursor - 1) % #_agents) + 1
        _chapter8.teamApplyCursor = index + 1
        if _chapter8.teamApplyCursor > #_agents then
            _chapter8.teamApplyCursor = 1
        end
        local agent = _agents[index]
        _chapter8.teamApplyAgentChecks = _chapter8.teamApplyAgentChecks + 1
        if _IsAlive(agent) and _ApplyTeamMemoryToAgent(agent) then
            _chapter8.activeSharedReceivers = _chapter8.activeSharedReceivers + 1
        end
    end
end

local function _UpdateChapter8Comms(deltaTimeInMillis)
    if not _chapter8.enabled then return end

    _chapter8.elapsedMs = _chapter8.elapsedMs + math.max(0, deltaTimeInMillis)

    _UpdateScheduledSightScan(deltaTimeInMillis)
    _PublishScriptedOpeningSighting()
    _UpdateScheduledTeamPrune(deltaTimeInMillis)
    _PruneRecentEvents()
    _UpdateScheduledTeamApply(deltaTimeInMillis)
    _MaybePrintTeamBlackboardSmoke()
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
        for _, response in pairs(TeamBlackboard:GetSupportResponses(teamId)) do
            local responder = _agentsById[response.responderId]
            if responder ~= nil and response.supportPos ~= nil then
                DebugDrawer:drawLine(responder:GetPosition() + Vector3(0, 1.4, 0), response.supportPos + Vector3(0, 1.4, 0), _colors.support)
                DebugDrawer:drawCircle(response.supportPos + Vector3(0, 0.18, 0), 1.0, 24, _colors.support, false)
            end
        end
    end

    for _, agent in ipairs(_agents) do
        local bb = _GetBlackboard(agent)
        if bb ~= nil and bb:Has("chapter8.sharedTargetPos") then
            local pos = bb:GetVec3("chapter8.sharedTargetPos")
            DebugDrawer:drawLine(agent:GetPosition() + Vector3(0, 1.1, 0), pos + Vector3(0, 1.1, 0), _colors.shared)
        end
        if bb ~= nil and bb:Has("team.supportTargetPos") then
            local pos = bb:GetVec3("team.supportTargetPos")
            DebugDrawer:drawLine(agent:GetPosition() + Vector3(0, 1.7, 0), pos + Vector3(0, 1.7, 0), _colors.support)
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

local function _SupportSummary(teamId)
    local count = 0
    local last = "-"
    for responderId, response in pairs(TeamBlackboard:GetSupportResponses(teamId)) do
        count = count + 1
        last = "#" .. tostring(responderId) .. " -> #" .. tostring(response.targetId)
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
    local team0SupportCount, team0SupportLast = _SupportSummary(0)
    local team1SupportCount, team1SupportLast = _SupportSummary(1)

    return GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
        "[Sandbox12 - Team Blackboard]" .. GUI.MarkupNewline ..
        GUI.MarkupColor.Green .. "Stage: TEAM BLACKBOARD" .. GUI.MarkupColor.White .. GUI.MarkupNewline ..
        "Goal: teammates reuse shared enemy sightings" .. GUI.MarkupNewline ..
        GUI.MarkupNewline ..
        "Vision: green line + blue range ring" .. GUI.MarkupNewline ..
        "TeamComms: cyan lines broadcast EnemySighted" .. GUI.MarkupNewline ..
        "TeamBlackboard: EnemySighted is stored by teamId" .. GUI.MarkupNewline ..
        GUI.MarkupNewline ..
        "Direct sightings: " .. tostring(_chapter8.activeDirectSightings) ..
            "  shared sightings: " .. tostring(_chapter8.activeTeamMemories) ..
            "  receivers: " .. tostring(_chapter8.activeSharedReceivers) .. GUI.MarkupNewline ..
        "Schedule: scan " .. tostring(_chapter8.sightScanIntervalMs) .. "ms" ..
            " runs=" .. tostring(_chapter8.sightScanRunCount) ..
            " skips=" .. tostring(_chapter8.sightScanSkipCount) ..
            " pairs=" .. tostring(_chapter8.sightPairChecks) .. GUI.MarkupNewline ..
        "Team apply: " .. tostring(_chapter8.teamApplyIntervalMs) .. "ms" ..
            " runs=" .. tostring(_chapter8.teamApplyRunCount) ..
            " skips=" .. tostring(_chapter8.teamApplySkipCount) ..
            " agents=" .. tostring(_chapter8.teamApplyAgentChecks) .. GUI.MarkupNewline ..
        "C++ service: facts=" .. tostring(TeamBlackboard:GetCppFactCount()) ..
            " reports=" .. tostring(TeamBlackboard:GetCppReportCount()) ..
            " writes=" .. tostring(_chapter8.cppFactApplyCount) .. GUI.MarkupNewline ..
        "Team 0 blackboard: " .. tostring(team0Count) .. "  " .. team0Last .. GUI.MarkupNewline ..
        "Team 1 blackboard: " .. tostring(team1Count) .. "  " .. team1Last .. GUI.MarkupNewline ..
        "Support 0: " .. tostring(team0SupportCount) .. "  " .. team0SupportLast .. GUI.MarkupNewline ..
        "Support 1: " .. tostring(team1SupportCount) .. "  " .. team1SupportLast .. GUI.MarkupNewline ..
        "Last event: " .. _LastEventSummary() .. GUI.MarkupNewline ..
        "Totals: sightings=" .. tostring(_chapter8.totalSightings) ..
            " broadcasts=" .. tostring(_chapter8.totalBroadcasts) ..
            " sharedMoves=" .. tostring(_chapter8.totalSharedMoves) ..
            " support=" .. tostring(_chapter8.totalSupportResponses)
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
    TeamBlackboard:Reset()
    TeamBlackboard:ConfigureCppService(tonumber(config.teamMemoryTtlMs) or 2500)
    _chapter8.teamMemory = {}
    _EnsureTeamMemory(0)
    _EnsureTeamMemory(1)
    _chapter8.recentEvents = {}
    _chapter8.lastBroadcastAt = {}
    _chapter8.lastSharedAt = {}
    _chapter8.totalSightings = 0
    _chapter8.totalBroadcasts = 0
    _chapter8.totalSharedMoves = 0
    _chapter8.totalSupportResponses = 0
    _chapter8.activeSupportResponses = 0
    _chapter8.activeDirectSightings = 0
    _chapter8.activeSharedReceivers = 0
    _chapter8.activeTeamMemories = 0
    _chapter8.sightScanElapsedMs = 0
    _chapter8.sightScanIntervalMs = tonumber(config.sightScanIntervalMs) or 120
    _chapter8.sightScanCursor = 0
    _chapter8.sightScanRunCount = 0
    _chapter8.sightScanSkipCount = 0
    _chapter8.sightPairChecks = 0
    _chapter8.teamApplyElapsedMs = 0
    _chapter8.teamApplyIntervalMs = tonumber(config.teamApplyIntervalMs) or 160
    _chapter8.teamApplyCursor = 1
    _chapter8.teamApplyRunCount = 0
    _chapter8.teamApplySkipCount = 0
    _chapter8.teamApplyAgentChecks = 0
    _chapter8.cppFactApplyCount = 0
    _chapter8.teamPruneElapsedMs = 0
    _chapter8.teamPruneIntervalMs = tonumber(config.teamPruneIntervalMs) or 250
    _chapter8.teamPruneRunCount = 0
    _chapter8.teamPruneSkipCount = 0
    _chapter8.openingSightingPublished = false
    _chapter8.supportSmokePrinted = false
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

    _navMesh = Sandbox:CreateNavigationMesh(navMeshConfig, 'default')
    _drawNavMesh = true
    if _navMesh ~= nil then _navMesh:SetDebugVisible(_drawNavMesh) end

    local sampleName = "Sandbox12"
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
