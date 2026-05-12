-- FleeAction.lua
-- 选一个远离当前敌人 (或随机) 的目标点，存到 blackboard.movePos，TERMINATED。
-- 实际移动由后续触发的 MoveAction 完成（这与 chapter_6 的"先选点再走"一致）。

require("res.scripts.ai.decision.ActionStatus.lua")

local _maxAttempts = 10

function OnInitialize(owner, bb)
    if not owner then return end
    local enemy = bb:GetAgent("enemy")
    if enemy then
        local enemyPos = enemy:GetPosition()
        local fleePos = nil
        for i = 1, _maxAttempts do
            local p = Sandbox:RandomPoint("default")
            if (p - enemyPos):squaredLength() >= 16.0 then
                fleePos = p
                break
            end
        end
        if fleePos == nil then fleePos = Sandbox:RandomPoint("default") end
        bb:SetVec3("movePos", fleePos)
        owner:SetMovePosition(fleePos)
    else
        local p = Sandbox:RandomPoint("default")
        bb:SetVec3("movePos", p)
        owner:SetMovePosition(p)
    end
end

function OnUpdate(deltaMs, owner, bb)
    -- Flee 在 Initialize 阶段已完成"选点"，立即终止。
    return ActionStatus.TERMINATED
end

function OnCleanUp(owner, bb)
end
