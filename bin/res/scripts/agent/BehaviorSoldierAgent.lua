-- BehaviorSoldierAgent.lua
-- BT 驱动的士兵 agent 入口（与 DecisionSoldierAgent 对称）。
--
-- 流程：
--   1. C++ 创建 SoldierObject 后绑定本脚本到该对象的 plugin env
--   2. agent:UseBehaviorTreeDriver() 拆掉 FSM controller、建出 BehaviorTreeDriver + Blackboard
--   3. 用 SoldierBehaviorTreeBuilder + driver 工厂方法拼装 BT
--   4. driver 持有所有节点的所有权，agent 销毁时统一释放
--   5. C++ 每帧 tick driver，driver 走 tree:Tick → 各节点逐级 Tick → action lua 回调

require("res.scripts.ai.behavior.SoldierBehaviorTree.lua")

function Agent_Initialize(agent)
    if agent == nil then return end

    agent:SetMaxSpeed(SOLDIER_STAND_SPEED)

    agent:UseBehaviorTreeDriver()
    local driver = agent:GetBehaviorTreeDriver()
    if driver == nil then
        print("Error: BehaviorTreeDriver not available after UseBehaviorTreeDriver()")
        return
    end

    local bb = driver:GetBlackboard()
    bb:SetFloat("maxHealth", agent:GetMaxHealth())

    local tree = SoldierBehaviorTreeBuilder.Build(agent, driver, bb)
    driver:SetTree(tree)
end

function Agent_Update(agent, deltaTimeInMillis)
    -- C++ SoldierObject::Update 已经自动 tick 了 BehaviorTreeDriver；
    -- 这里留给 per-frame Lua 钩子。
end
