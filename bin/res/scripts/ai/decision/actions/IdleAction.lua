-- IdleAction.lua
-- 站立等待 2 秒，然后 TERMINATED。

require("res.scripts.ai.decision.ActionStatus.lua")
require("res.scripts.agent.AgentUtils.lua")

local _elapsedMs = 0
local _durationMs = 2000

function OnInitialize(owner, bb)
    _elapsedMs = 0
    if owner then
        owner:EnterIdleAnim()
    end
end

function OnUpdate(deltaMs, owner, bb)
    _elapsedMs = _elapsedMs + deltaMs
    if owner and owner:IsMoving() then
        Soldier_SlowMovement(owner, deltaMs)
    end
    if _elapsedMs >= _durationMs then
        return ActionStatus.TERMINATED
    end
    return ActionStatus.RUNNING
end

function OnCleanUp(owner, bb)
    _elapsedMs = 0
end
