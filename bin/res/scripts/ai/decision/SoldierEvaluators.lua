-- SoldierEvaluators.lua
-- 决策树分支用到的求值函数。每个函数读取 agent / blackboard 状态，
-- 返回 1-based 的 child 索引（约定 1=true 分支, 2=false 分支）。
--
-- 设计原则：求值器应是无副作用的纯查询，唯一例外是 HasEnemy 会把
-- 找到的敌人塞进 blackboard 供后续 action 使用（与 chapter_6 一致）。

require("res.scripts.base.global.lua")

SoldierEvaluators = {}

local function _GetClosestEnemy(agent)
    -- 直接调用 SoldierObject::GetEnemy() / HasEnemy()，这俩是 C++ 侧已经维护好的最近敌人查找。
    -- 这样比 chapter_6 在 Lua 里遍历 GetAgents 高效得多。
    if not agent:HasEnemy("default") then return nil end
    return agent:GetEnemy()
end

-- ============= 二选一分支求值器（返回 1 或 2） =============

function SoldierEvaluators.IsAliveBranch(agent, bb)
    if agent:GetHealth() <= 0 then return 2 end
    return 1
end

function SoldierEvaluators.HasCriticalHealthBranch(agent, bb)
    if agent:GetHealth() < (bb:GetFloat("maxHealth", 100.0) * 0.2) then
        return 1
    end
    return 2
end

function SoldierEvaluators.HasMovePositionBranch(agent, bb)
    if agent:HasMovePosition(1.5) then return 1 end
    return 2
end

function SoldierEvaluators.HasEnemyBranch(agent, bb)
    local enemy = _GetClosestEnemy(agent)
    if enemy ~= nil then
        bb:SetAgent("enemy", enemy)
        return 1
    end
    bb:Remove("enemy")
    return 2
end

function SoldierEvaluators.HasAmmoBranch(agent, bb)
    if agent:HasAmmo() then return 1 end
    return 2
end

function SoldierEvaluators.CanShootEnemyBranch(agent, bb)
    if agent:CanShootEnemy("default", 3.0) then return 1 end
    return 2
end

function SoldierEvaluators.RandomBranch(agent, bb)
    if math.random() >= 0.5 then return 1 end
    return 2
end
