-- DieAction.lua
-- 播放死亡动画并立即 TERMINATED。
-- DT 后续每帧 IsAliveBranch 都会评估到死亡分支，但 DieAction 只 Initialize 一次（动画自然延续）。

require("res.scripts.ai.decision.ActionStatus.lua")

local ActionIntent = require("res.scripts.ai.decision.ActionIntent.lua")

local _played = false

function OnInitialize(owner, bb)
    if owner and not _played then
        owner:SetVelocity(Vector3(0, 0, 0))
        owner:EnterDeathAnim()
        ActionIntent.Record(owner, bb, {
            action = "die",
            phase = "initialize",
            movement = "stop",
            animation = "death",
            reason = "enterDeath",
        })
        _played = true
    end
end

function OnUpdate(deltaMs, owner, bb)
    ActionIntent.Record(owner, bb, {
        action = "die",
        phase = "terminate",
        movement = "stop",
        animation = "death",
        reason = "deathAction",
    })
    return ActionStatus.TERMINATED
end

function OnCleanUp(owner, bb)
    ActionIntent.Record(owner, bb, {
        action = "die",
        phase = "cleanup",
        movement = "stop",
        animation = "death",
        reason = "deathCleanup",
    })
    -- 不重置 _played，避免反复播放死亡动画。
end
