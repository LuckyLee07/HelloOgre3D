-- WaitForSquadMateAction.lua
-- Hold briefly until enough squad members reach their formation slots.

require("res.scripts.ai.decision.ActionStatus.lua")

local _elapsedMs = 0

function OnInitialize(owner, bb)
    _elapsedMs = 0
    if owner ~= nil then
        owner:SetVelocity(Vector3(0, 0, 0))
        owner:EnterIdleAnim()
    end
end

function OnUpdate(deltaMs, owner, bb)
    _elapsedMs = _elapsedMs + deltaMs
    if owner == nil or owner:GetHealth() <= 0 or bb == nil then
        return ActionStatus.TERMINATED
    end

    local readyCount = bb:GetInt("formation.readyCount", 0)
    local minReady = bb:GetInt("formation.minReadyCount", 2)
    if readyCount >= minReady then
        bb:SetBool("formation.waitForSquadMate", false)
        return ActionStatus.TERMINATED
    end

    local timeoutMs = bb:GetInt("formation.waitTimeoutMs", 1800)
    if _elapsedMs >= timeoutMs then
        bb:SetBool("formation.waitForSquadMate", false)
        return ActionStatus.TERMINATED
    end

    owner:SetVelocity(Vector3(0, 0, 0))
    return ActionStatus.RUNNING
end

function OnCleanUp(owner, bb)
    _elapsedMs = 0
end
