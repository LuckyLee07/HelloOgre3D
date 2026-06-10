-- MoveAction.lua
-- 经 navmesh + OpenSteer 转向走到 blackboard.movePos。
-- 到达 1.5 范围或超时 (~2s) 则 TERMINATED；与 chapter_6 一致每段切片让 DT 重评估。
-- 切片重入时不刹车、不重置 accumulator、相同目标也不重建 path，避免视觉卡顿。

require("res.scripts.ai.decision.ActionStatus.lua")
require("res.scripts.ai.decision.MoveHelpers.lua")

local _SAME_TARGET_SQ = 0.25  -- 上次 movePos 与本次差距 < 0.5m 就不重建 path

local _elapsedMs = 0
local _segmentMs = 2000
local _acc = nil           -- steering accumulator；跨切片持续，平滑加速度
local _lastTarget = nil    -- 上一次实际建过 path 的目标点

function OnInitialize(owner, bb)
    _elapsedMs = 0
    _segmentMs = bb and bb:GetFloat("move.segmentMs", 2000.0) or 2000
    if _segmentMs <= 0 then _segmentMs = 2000 end
    if _acc == nil then _acc = Vector3(0, 0, 0) end
    if not owner then return end

    owner:EnterMoveAnim()

    local target = bb:GetVec3("movePos")
    owner:SetMovePosition(target)

    local needRebuild = (_lastTarget == nil)
        or ((target - _lastTarget):squaredLength() > _SAME_TARGET_SQ)
    if needRebuild then
        if MoveHelpers.BuildAndSetPath(owner, owner:GetPosition(), target) then
            _lastTarget = target
        end
    end
end

function OnUpdate(deltaMs, owner, bb)
    _elapsedMs = _elapsedMs + deltaMs
    if not owner or owner:GetHealth() <= 0 then
        return ActionStatus.TERMINATED
    end

    MoveHelpers.ApplySteering(owner, _acc, deltaMs)
    -- 仿 Sandbox6 C++ MoveState：橙色折线 + 末端圆圈，仅 Move 期间画。
    -- target 来自 RandomPoint，本就在 navmesh 上，可直接当圆心。
    MoveHelpers.DrawPath(owner, owner:GetTarget(), UtilColors.Orange, Vector3(0, 0.05, 0), 1.5)

    if owner:IsTargetReached(1.5) then
        return ActionStatus.TERMINATED
    end
    if _elapsedMs >= _segmentMs then
        return ActionStatus.TERMINATED
    end
    return ActionStatus.RUNNING
end

function OnCleanUp(owner, bb)
    -- 不刹车：velocity 留给下一段 / 下一个 action 自己接管。
    -- 真正想停下来的 action（Idle / Shoot / Reload）会自己 SetVelocity(0)。
    _elapsedMs = 0
end
