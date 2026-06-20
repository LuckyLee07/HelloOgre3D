-- InvestigateSoundAction.lua
-- Move toward a sound position written by the hearing sample.

require("res.scripts.ai.decision.ActionStatus.lua")
require("res.scripts.ai.decision.MoveHelpers.lua")
local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

local _acc = nil
local _elapsedMs = 0
local _lastTarget = nil

local function _GetTarget(bb)
    if bb == nil then return nil end
    if bb:Has("sense.heardInvestigatePos") then
        return bb:GetVec3("sense.heardInvestigatePos")
    end
    if bb:Has("sense.heardSoundPos") then
        return bb:GetVec3("sense.heardSoundPos")
    end
    return nil
end

function OnInitialize(owner, bb)
    _elapsedMs = 0
    if _acc == nil then _acc = Vector3(0, 0, 0) end
    if owner == nil then return end

    local target = _GetTarget(bb)
    if target == nil then return end

    AgentComponents.EnterMoveAnim(owner)
    AgentComponents.SetMovePosition(owner, target)
    if _lastTarget == nil or ((target - _lastTarget):squaredLength() > 0.25) then
        if MoveHelpers.BuildAndSetPath(owner, owner:GetPosition(), target) then
            _lastTarget = target
        end
    end
end

function OnUpdate(deltaMs, owner, bb)
    _elapsedMs = _elapsedMs + deltaMs
    if owner == nil or owner:GetHealth() <= 0 then
        return ActionStatus.TERMINATED
    end

    local target = _GetTarget(bb)
    if target == nil then
        return ActionStatus.TERMINATED
    end

    MoveHelpers.ApplySteering(owner, _acc, deltaMs)
    MoveHelpers.DrawPath(owner, target, UtilColors.Orange, Vector3(0, 0.12, 0), 1.2)

    if AgentComponents.IsTargetReached(owner, 1.5) or _elapsedMs >= 2600 then
        if bb ~= nil and bb.SetInt ~= nil then
            local sourceId = bb:GetObjectId("sense.heardSoundSourceId", -1)
            bb:SetInt("sense.heard.completedSourceId", sourceId)
        end
        return ActionStatus.TERMINATED
    end
    return ActionStatus.RUNNING
end

function OnCleanUp(owner, bb)
    _elapsedMs = 0
end
