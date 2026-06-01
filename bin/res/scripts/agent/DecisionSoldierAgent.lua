-- DecisionSoldierAgent.lua
-- DT 驱动的士兵 agent 入口。
--
-- 流程：
--   1. C++ 创建 SoldierObject 后绑定本脚本到该对象的 plugin env
--   2. C++ 调用 Agent_Initialize(agent)
--   3. agent:GetAI():SetDriverByType("dt") 拆掉 FSM controller、建出 DecisionTreeDriver + Blackboard
--   4. 用 SoldierDecisionTreeBuilder + driver 工厂方法拼装 DT
--   5. driver 持有所有节点的所有权，agent 销毁时统一释放（无需 Lua 端保活）
--   6. C++ 每帧 tick driver，driver 走 tree.Tick → resolve action → action lua 回调

require("res.scripts.ai.decision.SoldierDecisionTree.lua")

function Agent_Initialize(agent)
    if agent == nil then return end

    agent:SetMaxSpeed(SOLDIER_STAND_SPEED)

    local ai = agent:GetAI()
    if ai == nil then
        print("Error: AIController not available")
        return
    end

    ai:SetDriverByType("dt")
    local driver = ai:GetDecisionTreeDriver()
    if driver == nil then
        print("Error: DecisionTreeDriver not available after SetDriverByType(dt)")
        return
    end

    local bb = driver:GetBlackboard()
    bb:SetFloat("maxHealth", agent:GetMaxHealth())
    local sampleName = _G.HELLO_SANDBOX_SAMPLE_NAME or "Sandbox7"
    if ConfigManager ~= nil and ConfigManager.ApplyAiBlackboardDefaults ~= nil then
        ConfigManager:ApplyAiBlackboardDefaults(bb, sampleName)
    end

    local tree = SoldierDecisionTreeBuilder.Build(agent, driver, bb)
    driver:SetTree(tree)
end

function Agent_Update(agent, deltaTimeInMillis)
    -- C++ 侧 SoldierObject::Update 已经自动 tick 了 DecisionTreeDriver；
    -- 这里留作日后插入 per-frame Lua 检查（如重置 enemy、清理超时数据）的钩子。
end
