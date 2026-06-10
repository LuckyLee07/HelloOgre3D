-- ReloadAction.lua
-- 播放换弹动画、等待 ~1.5s、补满弹药、TERMINATED。

require("res.scripts.ai.decision.ActionStatus.lua")
require("res.scripts.agent.AgentUtils.lua")

local _elapsedMs = 0
local _durationMs = 1500

function OnInitialize(owner, bb)
    _elapsedMs = 0
    if owner then
        owner:EnterReloadAnim()
    end
end

function OnUpdate(deltaMs, owner, bb)
    _elapsedMs = _elapsedMs + deltaMs
    if not owner or owner:GetHealth() <= 0 then
        return ActionStatus.TERMINATED
    end

    if owner:IsMoving() then
        Soldier_SlowMovement(owner, deltaMs)
    end

    if _elapsedMs >= _durationMs then
        owner:RestoreAmmo()
        owner:EnterIdleAnim()
        return ActionStatus.TERMINATED
    end
    return ActionStatus.RUNNING
end

function OnCleanUp(owner, bb)
    if owner and owner:GetHealth() > 0 then
        owner:EnterIdleAnim()
    end
    _elapsedMs = 0
end
