-- SoldierConditions.lua
-- BT 用的布尔条件函数。每个返回 true（→ SUCCESS）或 false（→ FAILURE）。
-- 与 DT 的 SoldierEvaluators 平行：DT 返回 1/2 索引，BT 返回 bool。
-- 副作用许可（HasEnemy 顺手把 enemy 写到 blackboard，与 chapter_6 一致）。

require("res.scripts.base.global.lua")

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

function SoldierConditions.HasAmmo(agent, bb)
    return agent:HasAmmo()
end

function SoldierConditions.CanShootEnemy(agent, bb)
    local eventEnemy = bb:GetAgent("enemy")
    if _IsValidEnemy(agent, eventEnemy) then
        local toEnemy = eventEnemy:GetPosition() - agent:GetPosition()
        toEnemy.y = 0
        return toEnemy:squaredLength() < 9.0
    end
    return agent:CanShootEnemy("default", 3.0)
end

function SoldierConditions.RandomChance(threshold)
    -- 工厂：返回一个无参 evaluator 闭包。
    return function() return math.random() >= (threshold or 0.5) end
end
