-- SoldierConditions.lua
-- BT 用的布尔条件函数。每个返回 true（→ SUCCESS）或 false（→ FAILURE）。
-- 与 DT 的 SoldierEvaluators 平行：DT 返回 1/2 索引，BT 返回 bool。
-- 副作用许可（HasEnemy 顺手把 enemy 写到 blackboard，与 chapter_6 一致）。

require("res.scripts.base.global.lua")
local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

SoldierConditions = {}

local function _IsValidEnemy(agent, enemy)
    return agent ~= nil
        and enemy ~= nil
        and enemy ~= agent
        and enemy:GetHealth() > 0
        and enemy:GetTeamId() ~= agent:GetTeamId()
end

local function _GetClosestEnemy(agent)
    if not agent:HasEnemy("default") then return nil end
    return agent:GetEnemy()
end

function SoldierConditions.IsAlive(agent, bb)
    return agent:GetHealth() > 0
end

function SoldierConditions.IsCriticalHealth(agent, bb)
    return agent:GetHealth() < (bb:GetFloat("maxHealth", 100.0) * 0.2)
end

function SoldierConditions.HasMovePosition(agent, bb)
    return agent:HasMovePosition(1.5)
end

function SoldierConditions.HasDangerThreat(agent, bb, cfg)
    if bb == nil or not bb:Has("sense.dangerEscapePos") then
        return false
    end
    local minDanger = cfg and tonumber(cfg.minDanger) or 0.1
    return bb:GetFloat("sense.dangerLevel", 0.0) >= minDanger
end

function SoldierConditions.HasHeardSound(agent, bb, cfg)
    if bb == nil or not bb:Has("sense.heardSoundPos") then
        return false
    end
    if cfg == nil or cfg.requireNoVisual ~= false then
        if bb:GetBool("perception.hasTarget", false) then
            return false
        end
    end
    local minConfidence = cfg and tonumber(cfg.minConfidence) or 0.05
    return bb:GetFloat("sense.heardSoundConfidence", 0.0) >= minConfidence
end

function SoldierConditions.HasFormationSlot(agent, bb, cfg)
    if bb == nil or not bb:GetBool("formation.enabled", false) then
        return false
    end
    return bb:Has("formation.slotPos")
end

function SoldierConditions.ShouldCallForBackup(agent, bb, cfg)
    return bb ~= nil and bb:GetBool("team.shouldCallForBackup", false)
end

function SoldierConditions.ShouldWaitForSquadMate(agent, bb, cfg)
    if bb == nil or not bb:GetBool("formation.waitForSquadMate", false) then
        return false
    end
    return bb:GetInt("formation.readyCount", 0) < bb:GetInt("formation.minReadyCount", 2)
end

function SoldierConditions.HasEnemy(agent, bb)
    local eventEnemy = bb:GetAgent("enemy")
    if _IsValidEnemy(agent, eventEnemy) then
        return true
    end

    local enemy = _GetClosestEnemy(agent)
    if enemy then
        bb:SetAgent("enemy", enemy)
        return true
    end
    bb:Remove("enemy")
    return false
end

local function _GetLastKnownEnemySnapshot(bb)
    if bb == nil or bb.GetBool == nil then
        return nil
    end
    if not bb:GetBool("memory.snapshot.hasLastKnownEnemy", false) then
        return nil
    end

    local enemyId = bb:GetObjectId("memory.snapshot.lastKnownEnemyId", -1)
    if enemyId < 0 then
        return nil
    end

    return {
        enemyId = enemyId,
        observedAtMs = bb:GetInt("memory.snapshot.lastKnownEnemyObservedAtMs", -1),
        confidence = bb:GetFloat("memory.snapshot.lastKnownEnemyConfidence", 0.0),
    }
end

function SoldierConditions.HasLastKnownEnemyMemory(agent, bb, cfg)
    local memory = _GetLastKnownEnemySnapshot(bb)
    if memory == nil then
        return false
    end

    if cfg == nil or cfg.skipCompleted ~= false then
        local completedId = bb:GetObjectId("memory.search.completedEnemyId", -1)
        local completedObservedAtMs = bb:GetInt("memory.search.completedObservedAtMs", -1)
        if completedId == memory.enemyId and completedObservedAtMs == memory.observedAtMs then
            return false
        end
    end

    local minConfidence = cfg and tonumber(cfg.minConfidence) or 0.05
    return memory.confidence >= minConfidence
end

function SoldierConditions.HasAmmo(agent, bb)
    return AgentComponents.HasAmmo(agent)
end

function SoldierConditions.HasObjectId(key)
    return function(agent, bb)
        if bb == nil or bb.GetObjectId == nil then
            return false
        end
        return bb:GetObjectId(tostring(key), -1) >= 0
    end
end

function SoldierConditions.ArrayCountAtLeast(values, minCount)
    return function()
        if type(values) ~= "table" then
            return false
        end
        return #values >= (tonumber(minCount) or 1)
    end
end

local function _CanShootEnemy(agent, bb, maxDistanceSq)
    maxDistanceSq = tonumber(maxDistanceSq) or 9.0
    local eventEnemy = bb:GetAgent("enemy")
    if _IsValidEnemy(agent, eventEnemy) then
        local toEnemy = eventEnemy:GetPosition() - agent:GetPosition()
        toEnemy.y = 0
        return toEnemy:squaredLength() < maxDistanceSq
    end
    return agent:CanShootEnemy("default", math.sqrt(maxDistanceSq))
end

function SoldierConditions.CanShootEnemy(agentOrMaxDistanceSq, bb)
    if type(agentOrMaxDistanceSq) == "number" then
        local maxDistanceSq = agentOrMaxDistanceSq
        return function(agent, blackboard)
            return _CanShootEnemy(agent, blackboard, maxDistanceSq)
        end
    end
    return _CanShootEnemy(agentOrMaxDistanceSq, bb, 9.0)
end

function SoldierConditions.RandomChance(threshold)
    -- 工厂：返回一个无参 evaluator 闭包。
    return function(agent, bb)
        return math.random() >= (threshold or 0.5)
    end
end
