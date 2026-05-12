-- ShootAction.lua
-- 朝 blackboard.enemy 转身、播射击动画、消耗 1 弹药、TERMINATED。
-- 持续 ~600ms 给动画时间，然后终止以便 DT 重新评估（敌人也许已死/已远）。

require("res.scripts.ai.decision.ActionStatus.lua")

local _elapsedMs = 0
local _durationMs = 600
local _hasFired = false

function OnInitialize(owner, bb)
    _elapsedMs = 0
    _hasFired = false
    if owner then
        owner:SetVelocity(Vector3(0, 0, 0))
        owner:EnterShootAnim()
    end
end

function OnUpdate(deltaMs, owner, bb)
    _elapsedMs = _elapsedMs + deltaMs
    if not owner or owner:GetHealth() <= 0 then
        return ActionStatus.TERMINATED
    end

    local enemy = bb:GetAgent("enemy")
    if enemy and enemy:GetHealth() > 0 then
        -- 朝敌人方向，矫正一次（每帧调一下也可以，这里只在 fire 之前对一次）。
        local toEnemy = enemy:GetPosition() - owner:GetPosition()
        toEnemy.y = 0
        if toEnemy:length() > 0.01 then
            owner:SetForward(toEnemy)
        end
    end

    -- 在动画大约播到一半时真正发射子弹（视觉反馈与子弹生成对齐）。
    if not _hasFired and _elapsedMs >= 200 then
        _hasFired = true
        if owner:HasAmmo() then
            owner:ConsumeAmmo(1)
            owner:ShootBullet()
        end
    end

    if _elapsedMs >= _durationMs then
        return ActionStatus.TERMINATED
    end
    return ActionStatus.RUNNING
end

function OnCleanUp(owner, bb)
    _elapsedMs = 0
    _hasFired = false
end
