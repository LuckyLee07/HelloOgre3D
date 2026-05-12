-- SoldierDecisionTree.lua
-- 用 tolua 暴露的 C++ API 拼装出与 chapter_6 SoldierLogic_DecisionTree 同构的决策树。
--
-- 节点全部由 DecisionTreeDriver 工厂创建（driver:NewBranch / NewLuaAction / NewTree），
-- driver 持有所有权，agent 销毁时统一释放。Lua 这边不需要再用 _treeHandle 强引用保活。

require("res.scripts.ai.decision.SoldierEvaluators.lua")

SoldierDecisionTreeBuilder = {}

local _ACTION_DIR = "res/scripts/ai/decision/actions/"

local function _NewLuaAction(driver, name, scriptName)
    local action = driver:NewLuaAction(name)
    action:BindToScript(_ACTION_DIR .. scriptName)
    return action
end

-- agent: SoldierObject*
-- driver: DecisionTreeDriver*（节点工厂 + 所有者）
-- bb: Blackboard*（取自 driver:GetBlackboard()）
-- 返回： DecisionTree*；调用方把它喂给 driver:SetTree
function SoldierDecisionTreeBuilder.Build(agent, driver, bb)
    -- ===== Action 叶子 =====
    local idleAction       = _NewLuaAction(driver, "idle",       "IdleAction.lua")
    local moveAction       = _NewLuaAction(driver, "move",       "MoveAction.lua")
    local pursueAction     = _NewLuaAction(driver, "pursue",     "PursueAction.lua")
    local shootAction      = _NewLuaAction(driver, "shoot",      "ShootAction.lua")
    local reloadAction     = _NewLuaAction(driver, "reload",     "ReloadAction.lua")
    local fleeAction       = _NewLuaAction(driver, "flee",       "FleeAction.lua")
    local dieAction        = _NewLuaAction(driver, "die",        "DieAction.lua")
    local randomMoveAction = _NewLuaAction(driver, "randomMove", "RandomMoveAction.lua")

    -- ===== Branch 节点 =====
    local isAliveBranch     = driver:NewBranch()
    local criticalBranch    = driver:NewBranch()
    local moveFleeBranch    = driver:NewBranch()
    local enemyBranch       = driver:NewBranch()
    local ammoBranch        = driver:NewBranch()
    local shootBranch       = driver:NewBranch()
    local moveRandomBranch  = driver:NewBranch()
    local randomBranch      = driver:NewBranch()

    -- ===== 装配（顺序对应 evaluator 的 1/2 返回值） =====
    isAliveBranch:AddChild(criticalBranch)              -- 1: alive
    isAliveBranch:AddChild(dieAction)                   -- 2: dead
    isAliveBranch:SetEvaluator(function() return SoldierEvaluators.IsAliveBranch(agent, bb) end)

    criticalBranch:AddChild(moveFleeBranch)             -- 1: critical
    criticalBranch:AddChild(enemyBranch)                -- 2: healthy
    criticalBranch:SetEvaluator(function() return SoldierEvaluators.HasCriticalHealthBranch(agent, bb) end)

    moveFleeBranch:AddChild(moveAction)                 -- 1: has move pos
    moveFleeBranch:AddChild(fleeAction)                 -- 2: no move pos
    moveFleeBranch:SetEvaluator(function() return SoldierEvaluators.HasMovePositionBranch(agent, bb) end)

    enemyBranch:AddChild(ammoBranch)                    -- 1: has enemy
    enemyBranch:AddChild(moveRandomBranch)              -- 2: no enemy
    enemyBranch:SetEvaluator(function() return SoldierEvaluators.HasEnemyBranch(agent, bb) end)

    ammoBranch:AddChild(shootBranch)                    -- 1: has ammo
    ammoBranch:AddChild(reloadAction)                   -- 2: no ammo
    ammoBranch:SetEvaluator(function() return SoldierEvaluators.HasAmmoBranch(agent, bb) end)

    shootBranch:AddChild(shootAction)                   -- 1: can shoot
    shootBranch:AddChild(pursueAction)                  -- 2: out of range
    shootBranch:SetEvaluator(function() return SoldierEvaluators.CanShootEnemyBranch(agent, bb) end)

    moveRandomBranch:AddChild(moveAction)               -- 1: has move pos
    moveRandomBranch:AddChild(randomBranch)             -- 2: no move pos
    moveRandomBranch:SetEvaluator(function() return SoldierEvaluators.HasMovePositionBranch(agent, bb) end)

    randomBranch:AddChild(randomMoveAction)             -- 1: random pick
    randomBranch:AddChild(idleAction)                   -- 2: idle
    randomBranch:SetEvaluator(function() return SoldierEvaluators.RandomBranch(agent, bb) end)

    -- ===== 树容器 =====
    local tree = driver:NewTree()
    tree:SetRoot(isAliveBranch)

    return tree
end
