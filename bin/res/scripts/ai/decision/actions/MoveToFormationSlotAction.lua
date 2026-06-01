-- MoveToFormationSlotAction.lua
-- Move toward the slot assigned by the Chapter 9 formation sample.

require("res.scripts.ai.decision.ActionStatus.lua")
require("res.scripts.ai.decision.MoveHelpers.lua")

local _acc = nil
local _elapsedMs = 0
local _lastTarget = nil

local function _GetSlotPos(bb)
    if bb ~= nil and bb:Has("formation.slotPos") then
        return bb:GetVec3("formation.slotPos")
    end
    return nil
end

function OnInitialize(owner, bb)
    _elapsedMs = 0
    if _acc == nil then _acc = Vector3(0, 0, 0) end
    if owner == nil then return end

    local target = _GetSlotPos(bb)
    if target == nil then return end

    owner:EnterMoveAnim()
    owner:SetMovePosition(target)
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

    local target = _GetSlotPos(bb)
    if target == nil then
        return ActionStatus.TERMINATED
    end

    MoveHelpers.ApplySteering(owner, _acc, deltaMs)
    MoveHelpers.DrawPath(owner, target, ColourValue(0.75, 0.45, 1.0), Vector3(0, 0.2, 0), 1.0)

    if owner:IsTargetReached(1.2) then
        if bb ~= nil then
            bb:SetBool("formation.inSlot", true)
        end
        return ActionStatus.TERMINATED
    end
    if _elapsedMs >= 3000 then
        return ActionStatus.TERMINATED
    end
    return ActionStatus.RUNNING
end

function OnCleanUp(owner, bb)
    _elapsedMs = 0
end
