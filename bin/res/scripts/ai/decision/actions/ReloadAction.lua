-- ReloadAction.lua
-- 播放换弹动画、等待 ~1.5s、补满弹药、TERMINATED。

require("res.scripts.ai.decision.ActionStatus.lua")
require("res.scripts.agent.AgentUtils.lua")
local ActionIntent = require("res.scripts.ai.decision.ActionIntent.lua")
local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

local _elapsedMs = 0
local _durationMs = 1500

function OnInitialize(owner, bb)
    _elapsedMs = 0
    if owner then
        owner:EnterReloadAnim()
        ActionIntent.Record(owner, bb, {
            action = "reload",
            phase = "initialize",
            movement = "stopForReload",
            animation = "reload",
            elapsedMs = _elapsedMs,
            durationMs = _durationMs,
            reason = "enterReload",
        })
    end
end

function OnUpdate(deltaMs, owner, bb)
    _elapsedMs = _elapsedMs + deltaMs
    if not owner or owner:GetHealth() <= 0 then
        ActionIntent.Record(owner, bb, {
            action = "reload",
            phase = "terminate",
            movement = "none",
            animation = "reload",
            elapsedMs = _elapsedMs,
            durationMs = _durationMs,
            reason = "deadOrMissingOwner",
        })
        return ActionStatus.TERMINATED
    end

    local movementIntent = "hold"
    if owner:IsMoving() then
        movementIntent = "brake"
        Soldier_SlowMovement(owner, deltaMs)
    end

    if _elapsedMs >= _durationMs then
        AgentComponents.RestoreAmmo(owner)
        owner:EnterIdleAnim()
        ActionIntent.Record(owner, bb, {
            action = "reload",
            phase = "terminate",
            movement = movementIntent,
            animation = "idle",
            elapsedMs = _elapsedMs,
            durationMs = _durationMs,
            reason = "durationExpired",
        })
        return ActionStatus.TERMINATED
    end
    ActionIntent.Record(owner, bb, {
        action = "reload",
        phase = "update",
        movement = movementIntent,
        animation = "reload",
        elapsedMs = _elapsedMs,
        durationMs = _durationMs,
        reason = "running",
    })
    return ActionStatus.RUNNING
end

function OnCleanUp(owner, bb)
    if owner and owner:GetHealth() > 0 then
        owner:EnterIdleAnim()
    end
    ActionIntent.Record(owner, bb, {
        action = "reload",
        phase = "cleanup",
        movement = "hold",
        animation = "idle",
        elapsedMs = _elapsedMs,
        durationMs = _durationMs,
        reason = "cleanupIdle",
    })
    _elapsedMs = 0
end
