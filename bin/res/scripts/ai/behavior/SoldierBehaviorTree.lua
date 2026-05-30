-- SoldierBehaviorTree.lua
-- 用 Sequence / Selector / LuaCondition / LuaBehaviorAction 拼装出一棵
-- 与 DT 同结构的行为树，验证 BT 也能产出等价的士兵决策。
--
-- DT 的"二选一 branch"在 BT 里对应：Selector { Sequence{ Cond, IfTrue }, IfFalse }
-- 即"先尝试条件成立的分支，失败则走 fallback"。
--
-- 拓扑（与 DT topology 等价）：
--
-- Selector(root)
--   Sequence(alive)
--     Cond: IsAlive
--     Selector(action)
--       Sequence(critical) [IsCritical → 决定 flee 还是 move]
--         Cond: IsCritical
--         Selector(criticalChoice)
--           Sequence(moveIfHasPos) [HasMovePos → MoveAction]
--             Cond: HasMovePos
--             MoveAction
--           FleeAction
--       Sequence(combat) [HasEnemy → 战斗子树]
--         Cond: HasEnemy            -- 副作用：bb.enemy
--         Selector(combatChoice)
--           Sequence(ammoOk) [HasAmmo → 射 / 追]
--             Cond: HasAmmo
--             Selector(ammoChoice)
--               Sequence(shootIfCan) [CanShoot → ShootAction]
--                 Cond: CanShoot
--                 ShootAction
--               PursueAction
--           ReloadAction
--       Sequence(memory) [失去当前目标但有记忆 → 去最后已知位置]
--         Cond: HasLastKnownEnemyMemory
--         MoveToLastKnownEnemyAction
--       Sequence(idle) [无敌人、非 critical → 巡逻或 idle]
--         Selector(idleChoice)
--           Sequence(moveIfHasPos2)
--             Cond: HasMovePos
--             MoveAction
--           Selector(roam)
--             Sequence(maybeRandom)
--               Cond: random > 0.5
--               RandomMoveAction
--             IdleAction
--   DieAction

require("res.scripts.ai.behavior.SoldierConditions.lua")

SoldierBehaviorTreeBuilder = {}

-- DT 的 action lua 文件可直接复用：
--   - OnUpdate 返回值 1=RUNNING / 2=终结，BT 这边 1=RUNNING / 2=SUCCESS，对得上
--   - 这些 action 永远不会失败（只会持续 RUNNING 或最终 SUCCESS），所以 BT 语义没问题
local _ACTION_DIR = "res/scripts/ai/decision/actions/"

local function _NewLuaAction(driver, name, scriptName)
    local action = driver:NewLuaAction(name)
    action:BindToScript(_ACTION_DIR .. scriptName)
    return action
end

local function _NewCond(driver, condFn)
    local c = driver:NewCondition()
    c:SetEvaluator(condFn)
    return c
end

local function _Seq(driver, ...)
    local s = driver:NewSequence()
    for _, child in ipairs({...}) do s:AddChild(child) end
    return s
end

local function _Sel(driver, ...)
    local s = driver:NewSelector()
    for _, child in ipairs({...}) do s:AddChild(child) end
    return s
end

local function _ReactiveSel(driver, reevaluateMs, ...)
    local s = driver:NewSelector(reevaluateMs or 0)
    for _, child in ipairs({...}) do s:AddChild(child) end
    return s
end

-- agent: SoldierObject*
-- driver: BehaviorTreeDriver*
-- bb: Blackboard*
-- 返回： BehaviorTree*
function SoldierBehaviorTreeBuilder.Build(agent, driver, bb)
    -- ===== Action 叶子（每个动作只创建一份实例） =====
    local idleAction       = _NewLuaAction(driver, "idle",       "IdleAction.lua")
    local moveAction       = _NewLuaAction(driver, "move",       "MoveAction.lua")
    local pursueAction     = _NewLuaAction(driver, "pursue",     "PursueAction.lua")
    local shootAction      = _NewLuaAction(driver, "shoot",      "ShootAction.lua")
    local reloadAction     = _NewLuaAction(driver, "reload",     "ReloadAction.lua")
    local fleeAction       = _NewLuaAction(driver, "flee",       "FleeAction.lua")
    local dieAction        = _NewLuaAction(driver, "die",        "DieAction.lua")
    local randomMoveAction = _NewLuaAction(driver, "randomMove", "RandomMoveAction.lua")
    local memoryMoveAction = _NewLuaAction(driver, "moveToLastKnownEnemy", "MoveToLastKnownEnemyAction.lua")

    -- ===== 条件叶子（闭包捕获 agent + bb） =====
    local condAlive    = _NewCond(driver, function() return SoldierConditions.IsAlive(agent, bb) end)
    local condCritical = _NewCond(driver, function() return SoldierConditions.IsCriticalHealth(agent, bb) end)
    local condHasPos   = _NewCond(driver, function() return SoldierConditions.HasMovePosition(agent, bb) end)
    local condHasPos2  = _NewCond(driver, function() return SoldierConditions.HasMovePosition(agent, bb) end)
    local condHasEnemy = _NewCond(driver, function() return SoldierConditions.HasEnemy(agent, bb) end)
    local condHasMemory = _NewCond(driver, function() return SoldierConditions.HasLastKnownEnemyMemory(agent, bb) end)
    local condHasAmmo  = _NewCond(driver, function() return SoldierConditions.HasAmmo(agent, bb) end)
    local condCanShoot = _NewCond(driver, function() return SoldierConditions.CanShootEnemy(agent, bb) end)
    local condRandom   = _NewCond(driver, function() return math.random() >= 0.5 end)

    -- ===== 装配 =====
    local moveIfHasPos = _Seq(driver, condHasPos, moveAction)         -- HasMovePos → Move，否则 fail
    local critFallback = _Sel(driver, moveIfHasPos, fleeAction)       -- 先 Move，失败走 Flee
    local criticalSeq  = _Seq(driver, condCritical, critFallback)     -- 仅 critical 时进

    local shootIfCan   = _Seq(driver, condCanShoot, shootAction)
    local ammoChoice   = _Sel(driver, shootIfCan, pursueAction)
    local ammoOkSeq    = _Seq(driver, condHasAmmo, ammoChoice)
    local combatChoice = _Sel(driver, ammoOkSeq, reloadAction)
    local combatSeq    = _Seq(driver, condHasEnemy, combatChoice)

    local memorySeq = _Seq(driver, condHasMemory, memoryMoveAction)

    local moveIfHasPos2 = _Seq(driver, condHasPos2, moveAction)
    local maybeRandom   = _Seq(driver, condRandom, randomMoveAction)
    local roamChoice    = _Sel(driver, maybeRandom, idleAction)
    local idleChoice    = _Sel(driver, moveIfHasPos2, roamChoice)
    local idleSeq       = idleChoice  -- 无前置条件，直接走 selector

    local actionSel = _ReactiveSel(driver, 200, criticalSeq, combatSeq, memorySeq, idleSeq)
    local aliveSeq  = _Seq(driver, condAlive, actionSel)

    local root = _Sel(driver, aliveSeq, dieAction)

    local tree = driver:NewTree()
    tree:SetRoot(root)
    return tree
end
