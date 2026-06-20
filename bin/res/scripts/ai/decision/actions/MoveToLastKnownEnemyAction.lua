-- MoveToLastKnownEnemyAction.lua
-- BT 专用：失去当前视野后，移动到 MemoryStore 同步出来的最后已知敌人位置。

require("res.scripts.ai.behavior.BehaviorStatus.lua")
require("res.scripts.ai.decision.MoveHelpers.lua")
local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

local _REACH_DISTANCE = 1.5
local _SEGMENT_MS = 3500

local _elapsedMs = 0
local _acc = nil
local _target = nil
local _enemyId = -1
local _observedAtMs = -1
local _started = false
local _completed = false

local function _ReadMemoryTarget(bb)
    if bb == nil or not bb:GetBool("memory.snapshot.hasLastKnownEnemy", false) then
        return nil, -1, -1
    end

    local enemyId = bb:GetObjectId("memory.snapshot.lastKnownEnemyId", -1)
    if enemyId < 0 then
        return nil, -1, -1
    end

    return bb:GetVec3("memory.snapshot.lastKnownEnemyPos"),
        enemyId,
        bb:GetInt("memory.snapshot.lastKnownEnemyObservedAtMs", -1)
end

local function _MarkSearchCompleted(bb)
    if bb == nil or _enemyId < 0 then
        return
    end
    bb:SetObjectId("memory.search.completedEnemyId", _enemyId)
    bb:SetInt("memory.search.completedObservedAtMs", _observedAtMs)
end

local function _WriteDebugSearchTarget(bb)
    if bb == nil or _target == nil then
        return
    end
    bb:SetObjectId("debug.lastKnownSearchEnemyId", _enemyId)
    bb:SetVec3("debug.lastKnownSearchEnemyPos", _target)
    bb:SetInt("debug.lastKnownSearchObservedAtMs", _observedAtMs)
end

local function _ClearMove(owner, bb)
    if owner then
        AgentComponents.ClearMovePosition(owner)
        owner:SetVelocity(Vector3(0, 0, 0))
    end
    if bb then
        bb:Remove("movePos")
    end
end

function OnInitialize(owner, bb)
    _elapsedMs = 0
    _started = false
    _completed = false
    _target, _enemyId, _observedAtMs = _ReadMemoryTarget(bb)

    if _acc == nil then _acc = Vector3(0, 0, 0) end
    if not owner or _target == nil then
        return
    end

    _WriteDebugSearchTarget(bb)
    AgentComponents.EnterMoveAnim(owner)
    AgentComponents.SetMovePosition(owner, _target)
    if bb then bb:SetVec3("movePos", _target) end

    _started = MoveHelpers.BuildAndSetPath(owner, owner:GetPosition(), _target)
end

function OnUpdate(deltaMs, owner, bb)
    _elapsedMs = _elapsedMs + deltaMs
    if not owner or owner:GetHealth() <= 0 then
        return BehaviorStatus.FAILURE
    end
    if not _started or _target == nil then
        _MarkSearchCompleted(bb)
        _ClearMove(owner, bb)
        return BehaviorStatus.FAILURE
    end

    MoveHelpers.ApplySteering(owner, _acc, deltaMs)
    MoveHelpers.DrawPath(owner, _target, UtilColors.Yellow, Vector3(0, 0.15, 0), _REACH_DISTANCE)

    if AgentComponents.IsTargetReached(owner, _REACH_DISTANCE) or _elapsedMs >= _SEGMENT_MS then
        _completed = true
        _MarkSearchCompleted(bb)
        _ClearMove(owner, bb)
        return BehaviorStatus.SUCCESS
    end

    return BehaviorStatus.RUNNING
end

function OnCleanUp(owner, bb)
    _elapsedMs = 0
    _ClearMove(owner, bb)
end
