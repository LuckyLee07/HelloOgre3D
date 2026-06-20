-- ShootAction.lua
-- 朝 blackboard.enemy 转身、播射击动画、消耗 1 弹药、TERMINATED。
-- 持续 ~600ms 给动画时间，然后终止以便 DT 重新评估（敌人也许已死/已远）。

require("res.scripts.ai.decision.ActionStatus.lua")
require("res.scripts.agent.AgentUtils.lua")
local ActionIntent = require("res.scripts.ai.decision.ActionIntent.lua")
local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

local _elapsedMs = 0
local _durationMs = 600
local _hasFired = false
local _idleRequested = false

local function _IsShootBodyStateCurrent(owner)
    local asm = AgentComponents.GetBodyAsm(owner)
    if asm == nil then
        return false
    end

    if asm:IsTransitioning() then
        return false
    end

    local stateName = asm:GetCurrStateName()
    return stateName == "fire" or stateName == "crouch_fire"
end

function OnInitialize(owner, bb)
    _elapsedMs = 0
    _hasFired = false
    _idleRequested = false
    if owner then
        AgentComponents.EnterShootAnim(owner)
        ActionIntent.Record(owner, bb, {
            action = "shoot",
            phase = "initialize",
            movement = "stopForShoot",
            animation = "shoot",
            enemy = bb ~= nil and bb:GetAgent("enemy") or nil,
            elapsedMs = _elapsedMs,
            durationMs = _durationMs,
            reason = "enterShoot",
        })
    end
end

function OnUpdate(deltaMs, owner, bb)
    _elapsedMs = _elapsedMs + deltaMs
    if not owner or owner:GetHealth() <= 0 then
        ActionIntent.Record(owner, bb, {
            action = "shoot",
            phase = "terminate",
            movement = "none",
            animation = "shoot",
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
        if AgentComponents.HasAmmo(owner) then
            AgentComponents.ConsumeAmmo(owner, 1)
            AgentComponents.ShootBullet(owner)
        end
    end

    if _idleRequested then
        ActionIntent.Record(owner, bb, {
            action = "shoot",
            phase = "terminate",
            movement = movementIntent,
            animation = "idle",
            enemy = enemy,
            elapsedMs = _elapsedMs,
            durationMs = _durationMs,
            reason = "idleRequested",
        })
        return ActionStatus.TERMINATED
    end

    if _hasFired and _IsShootBodyStateCurrent(owner) then
        _idleRequested = true
        AgentComponents.EnterIdleAnim(owner)
        ActionIntent.Record(owner, bb, {
            action = "shoot",
            phase = "update",
            movement = movementIntent,
            animation = "idle",
            enemy = enemy,
            elapsedMs = _elapsedMs,
            durationMs = _durationMs,
            reason = "fireStateReached",
        })
        return ActionStatus.RUNNING
    end

    if _elapsedMs >= _durationMs then
        _idleRequested = true
        AgentComponents.EnterIdleAnim(owner)
        ActionIntent.Record(owner, bb, {
            action = "shoot",
            phase = "terminate",
            movement = movementIntent,
            animation = "idle",
            enemy = enemy,
            elapsedMs = _elapsedMs,
            durationMs = _durationMs,
            reason = "durationExpired",
        })
        return ActionStatus.TERMINATED
    end
    ActionIntent.Record(owner, bb, {
        action = "shoot",
        phase = "update",
        movement = movementIntent,
        animation = "shoot",
        enemy = enemy,
        elapsedMs = _elapsedMs,
        durationMs = _durationMs,
        reason = _hasFired and "fired" or "aiming",
    })
    return ActionStatus.RUNNING
end

function OnCleanUp(owner, bb)
    if owner and owner:GetHealth() > 0 then
        AgentComponents.EnterIdleAnim(owner)
    end
    ActionIntent.Record(owner, bb, {
        action = "shoot",
        phase = "cleanup",
        movement = "hold",
        animation = "idle",
        elapsedMs = _elapsedMs,
        durationMs = _durationMs,
        reason = "cleanupIdle",
    })
    _elapsedMs = 0
    _hasFired = false
    _idleRequested = false
end
