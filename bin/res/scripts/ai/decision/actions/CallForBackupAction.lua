-- CallForBackupAction.lua
-- Publish a lightweight backup request into TeamBlackboard for Chapter 9 samples.

require("res.scripts.ai.decision.ActionStatus.lua")
local TeamBlackboard = require("res.scripts.ai.team.TeamBlackboard.lua")

function OnInitialize(owner, bb)
    if owner == nil or bb == nil then return end
    if not bb:GetBool("team.shouldCallForBackup", false) then return end

    local perceptionTargetId = bb:GetObjectId("perception.targetId", -1)
    local targetId = bb:GetObjectId("team.focusTargetId", perceptionTargetId)
    local targetPos = nil
    if bb:Has("team.lastKnownEnemyPos") then
        targetPos = bb:GetVec3("team.lastKnownEnemyPos")
    elseif bb:Has("perception.targetPos") then
        targetPos = bb:GetVec3("perception.targetPos")
    end

    local request = {
        requesterId = owner:GetObjId(),
        teamId = owner:GetTeamId(),
        targetId = targetId,
        targetPos = targetPos,
        timeMs = bb:GetInt("team.requestTimeMs", 0),
    }
    TeamBlackboard:SetValue(owner:GetTeamId(), "backupRequest", request)
    bb:SetBool("team.shouldCallForBackup", false)
    bb:SetInt("team.backupCallCount", bb:GetInt("team.backupCallCount", 0) + 1)
    bb:SetObjectId("team.lastBackupCallTargetId", targetId)
end

function OnUpdate(deltaMs, owner, bb)
    return ActionStatus.TERMINATED
end

function OnCleanUp(owner, bb)
end
