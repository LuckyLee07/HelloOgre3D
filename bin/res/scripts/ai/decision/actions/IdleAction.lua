-- IdleAction.lua
-- 站立等待 2 秒，然后 TERMINATED。

require("res.scripts.ai.decision.ActionStatus.lua")
require("res.scripts.agent.AgentUtils.lua")
local ActionIntent = require("res.scripts.ai.decision.ActionIntent.lua")
local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

local _elapsedMs = 0
local _durationMs = 2000

function OnInitialize(owner, bb)
    _elapsedMs = 0
    if owner then
        AgentComponents.EnterIdleAnim(owner)
        ActionIntent.Record(owner, bb, {
            action = "idle",
            phase = "initialize",
            movement = "idle",
            animation = "idle",
            elapsedMs = _elapsedMs,
            durationMs = _durationMs,
            reason = "enterIdle",
        })
    end
end

function OnUpdate(deltaMs, owner, bb)
    _elapsedMs = _elapsedMs + deltaMs
    local movementIntent = "idle"
    if owner and owner:IsMoving() then
        movementIntent = "brake"
        Soldier_SlowMovement(owner, deltaMs)
    end
    if _elapsedMs >= _durationMs then
        ActionIntent.Record(owner, bb, {
            action = "idle",
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
        action = "idle",
        phase = "update",
        movement = movementIntent,
        animation = "idle",
        elapsedMs = _elapsedMs,
        durationMs = _durationMs,
        reason = "running",
    })
    return ActionStatus.RUNNING
end

function OnCleanUp(owner, bb)
    ActionIntent.Record(owner, bb, {
        action = "idle",
        phase = "cleanup",
        movement = "idle",
        animation = "idle",
        elapsedMs = _elapsedMs,
        durationMs = _durationMs,
        reason = "cleanup",
    })
    _elapsedMs = 0
end
