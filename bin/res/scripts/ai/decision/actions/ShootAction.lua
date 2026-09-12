-- ShootAction.lua
-- A stationary, bounded-turn shot. The same shoot_fire notify drives AI and player timing.
require("res.scripts.ai.decision.ActionStatus.lua")
require("res.scripts.agent.AgentUtils.lua")
local ActionIntent = require("res.scripts.ai.decision.ActionIntent.lua")
local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

local _elapsedMs = 0
local _durationMs = 600
local _shotElapsedMs = 0
local _hasFired = false
local _shotStarted = false
local _previousActionOwnsFire = false
local _ownsFireOverride = false

local function Record(owner, bb, phase, reason)
    ActionIntent.Record(owner, bb, {
        action = "shoot", phase = phase, movement = "stopped",
        animation = _shotStarted and "shoot" or "idle",
        enemy = bb ~= nil and bb:GetAgent("enemy") or nil,
        elapsedMs = _elapsedMs, durationMs = _durationMs, reason = reason,
    })
end

function OnInitialize(owner, bb)
    _elapsedMs, _hasFired, _shotStarted, _shotElapsedMs = 0, false, false, 0
    _ownsFireOverride = false
    if owner then
        Soldier_StopMovement(owner)
        AgentComponents.EnterIdleAnim(owner)
        local anim = AgentComponents.GetAnim(owner)
        if anim ~= nil then anim:ConsumeShootExecution() end
        if bb ~= nil then
            _previousActionOwnsFire = bb:GetBool("weapon.actionOwnsFire", false)
            bb:SetBool("weapon.actionOwnsFire", true)
            _ownsFireOverride = true
        end
        Record(owner, bb, "initialize", "stopAndAim")
    end
end

function OnUpdate(deltaMs, owner, bb)
    _elapsedMs = _elapsedMs + deltaMs
    if _shotStarted then _shotElapsedMs = _shotElapsedMs + deltaMs end
    if not owner or owner:GetHealth() <= 0 then
        Record(owner, bb, "terminate", "deadOrMissingOwner")
        return ActionStatus.TERMINATED
    end
    Soldier_StopMovement(owner)
    local enemy = bb ~= nil and bb:GetAgent("enemy") or nil
    if enemy == nil or enemy:GetHealth() <= 0 then
        AgentComponents.EnterIdleAnim(owner)
        Record(owner, bb, "terminate", "enemyInvalid")
        return ActionStatus.TERMINATED
    end

    local toEnemy = enemy:GetPosition() - owner:GetPosition()
    toEnemy.y = 0
    local facing = Soldier_FaceDirection(owner, toEnemy, deltaMs)
    if not _shotStarted and facing then
        _shotStarted = AgentComponents.EnterShootAnim(owner)
    end

    local anim = AgentComponents.GetAnim(owner)
    if not _hasFired and _shotStarted and anim ~= nil and anim:ConsumeShootExecution() then
        _hasFired = true
        if AgentComponents.HasAmmo(owner) then
            AgentComponents.ConsumeAmmo(owner, 1)
            if bb:GetBool("sandbox19.aimedFire", false) then
                local locomotion = enemy:GetLocomotionComponent()
                local chestOffset = locomotion ~= nil and locomotion:GetHeight() * 0.25 or 0
                owner:GetWeaponComponent():ShootBulletAt(enemy:GetPosition() + Vector3(0, chestOffset, 0))
            else
                AgentComponents.ShootBullet(owner)
            end
        end
        Record(owner, bb, "update", "shootFireNotify")
    end

    local asm = AgentComponents.GetBodyAsm(owner)
    local complete = _hasFired and asm ~= nil and not asm:IsTransitioning()
        and (asm:GetCurrStateName() == "fire" or asm:GetCurrStateName() == "crouch_fire")
        and asm:GetCurrStateProgress() >= 0.9
    if complete or (_shotStarted and _shotElapsedMs >= _durationMs) or (not _shotStarted and _elapsedMs >= 1000) then
        AgentComponents.EnterIdleAnim(owner)
        Record(owner, bb, "terminate", complete and "shootComplete" or "durationExpired")
        return ActionStatus.TERMINATED
    end
    Record(owner, bb, "update", _hasFired and "fired" or "aiming")
    return ActionStatus.RUNNING
end

function OnCleanUp(owner, bb)
    if owner and owner:GetHealth() > 0 then
        AgentComponents.EnterIdleAnim(owner)
    end
    if _ownsFireOverride and bb ~= nil then
        bb:SetBool("weapon.actionOwnsFire", _previousActionOwnsFire)
    end
    _ownsFireOverride = false
    Record(owner, bb, "cleanup", "cleanupIdle")
    _elapsedMs, _hasFired, _shotStarted, _shotElapsedMs = 0, false, false, 0
end
