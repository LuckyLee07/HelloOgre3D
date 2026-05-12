-- RandomMoveAction.lua
-- 选一个 navmesh 上的随机点存到 blackboard.movePos，TERMINATED。
-- 真正的移动由后续 MoveAction 完成。

require("res.scripts.ai.decision.ActionStatus.lua")

function OnInitialize(owner, bb)
    if not owner then return end
    local p = Sandbox:RandomPoint("default")
    bb:SetVec3("movePos", p)
    owner:SetMovePosition(p)
end

function OnUpdate(deltaMs, owner, bb)
    return ActionStatus.TERMINATED
end

function OnCleanUp(owner, bb)
end
