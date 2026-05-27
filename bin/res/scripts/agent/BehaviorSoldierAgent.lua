-- BehaviorSoldierAgent.lua
-- BT 驱动的士兵 agent 入口（与 DecisionSoldierAgent 对称）。
--
-- 流程：
--   1. C++ 创建 SoldierObject 后绑定本脚本到该对象的 plugin env
--   2. agent:GetAI():SetDriverByType("bt") 拆掉 FSM controller、建出 BehaviorTreeDriver + Blackboard
--   3. BehaviorTreeLoader loads SoldierBT.lua and builds the C++ BT via driver factories
--   4. driver 持有所有节点的所有权，agent 销毁时统一释放
--   5. C++ 每帧 tick driver，driver 走 tree:Tick → 各节点逐级 Tick → action lua 回调

require("res.scripts.ai.behavior.SoldierConditions.lua")
require("res.scripts.ai.behavior.BehaviorTreeLoader.lua")
require("res.scripts.ai.behavior.config.SoldierBT.lua")

function Agent_Initialize(agent)
    if agent == nil then return end

    agent:SetMaxSpeed(SOLDIER_STAND_SPEED)

    local ai = agent:GetAI()
    if ai == nil then
        print("Error: AIController not available")
        return
    end

    ai:SetDriverByType("bt")
    local driver = ai:GetBehaviorTreeDriver()
    if driver == nil then
        print("Error: BehaviorTreeDriver not available after SetDriverByType(bt)")
        return
    end

    local bb = driver:GetBlackboard()
    bb:SetFloat("maxHealth", agent:GetMaxHealth())

    local tree = BehaviorTreeLoader.Build(SoldierBTConfig, agent, driver, bb, SoldierConditions)
    if tree == nil then
        print("Error: failed to build Soldier behavior tree from config")
        return
    end
    driver:SetTree(tree)
end

function Agent_Update(agent, deltaTimeInMillis)
    -- C++ SoldierObject::Update 已经自动 tick 了 BehaviorTreeDriver；
    -- 这里留给 per-frame Lua 钩子。
end
