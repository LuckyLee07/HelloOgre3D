-- RandomMoveAction.lua
-- 选一个 navmesh 上的随机点存到 blackboard.movePos，TERMINATED。
-- 真正的移动由后续 MoveAction 完成。

require("res.scripts.ai.decision.ActionStatus.lua")
local ActionIntent = require("res.scripts.ai.decision.ActionIntent.lua")

function OnInitialize(owner, bb)
    if not owner then return end
    local p = Sandbox:RandomPoint("default")
    bb:SetVec3("movePos", p)
    owner:SetMovePosition(p)
    ActionIntent.Record(owner, bb, {
        action = "randomMove",
        phase = "initialize",
        movement = "selectMoveTarget",
        animation = "none",
        target = p,
        reason = "randomPoint",
    })
end

function OnUpdate(deltaMs, owner, bb)
    ActionIntent.Record(owner, bb, {
        action = "randomMove",
        phase = "terminate",
        movement = "selectMoveTarget",
        animation = "none",
        target = bb ~= nil and bb:GetVec3("movePos") or nil,
        reason = "targetSelected",
    })
    return ActionStatus.TERMINATED
end

function OnCleanUp(owner, bb)
end
