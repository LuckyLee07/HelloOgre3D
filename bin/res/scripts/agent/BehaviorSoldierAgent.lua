-- BehaviorSoldierAgent.lua
-- BT 驱动的士兵 agent 入口（与 DecisionSoldierAgent 对称）。
--
-- 流程：
--   1. C++ 创建 SoldierObject 后绑定本脚本到该对象的 plugin env
--   2. agent:GetAIComponent():SetDriverByType("bt") 拆掉 FSM controller、建出 BehaviorTreeDriver + Blackboard
--   3. BehaviorTreeLoader loads SoldierBT.lua and builds the C++ BT via driver factories
--   4. driver 持有所有节点的所有权，agent 销毁时统一释放
--   5. C++ 每帧 tick driver，driver 走 tree:Tick → 各节点逐级 Tick → action lua 回调

local _SOLDIER_BT_MODULE = "res.scripts.ai.behavior.config.SoldierBT.lua"
local _SOLDIER_BT_GLOBAL = "SoldierBTConfig"

require("res.scripts.ai.behavior.SoldierConditions.lua")
require("res.scripts.ai.behavior.BehaviorTreeLoader.lua")
require(_SOLDIER_BT_MODULE)

local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

local function _GetBehaviorTreePreset(sampleName)
    if ConfigManager == nil or ConfigManager.GetSamplePreset == nil then
        return nil
    end
    local preset = ConfigManager:GetSamplePreset(sampleName)
    return preset ~= nil and preset.behaviorTree or nil
end

local function _ShouldRunBtRebuildSelfTest(sampleName)
    local behaviorTree = _GetBehaviorTreePreset(sampleName)
    return behaviorTree ~= nil and behaviorTree.rebuildSelfTest == true
end

local function _ShouldRunBtHotReloadSelfTest(sampleName)
    local behaviorTree = _GetBehaviorTreePreset(sampleName)
    return behaviorTree ~= nil and behaviorTree.hotReloadSelfTest == true
end

-- 行为树绑定：preset.behaviorTree 可覆盖模块/全局名/条件表，缺省仍是共享的 SoldierBT。
-- 用于 Sandbox19 指挥切片挂自己的树，避免改共享 SoldierBT 影响其它 sample。
local function _GetBtBinding(sampleName)
    local behaviorTree = _GetBehaviorTreePreset(sampleName)
    local moduleName = _SOLDIER_BT_MODULE
    local globalName = _SOLDIER_BT_GLOBAL
    local conditions = SoldierConditions
    if behaviorTree ~= nil then
        if behaviorTree.module ~= nil then moduleName = behaviorTree.module end
        if behaviorTree.global ~= nil then globalName = behaviorTree.global end
        if behaviorTree.conditionsGlobal ~= nil and _G[behaviorTree.conditionsGlobal] ~= nil then
            conditions = _G[behaviorTree.conditionsGlobal]
        end
    end
    return moduleName, globalName, conditions
end

local function _GetAgentId(agent)
    if agent == nil then return -1 end
    if agent.GetObjId ~= nil then return agent:GetObjId() end
    if agent.GetId ~= nil then return agent:GetId() end
    return -1
end

function Agent_Initialize(agent)
    if agent == nil then return end

    AgentComponents.SetMaxSpeed(agent, SOLDIER_STAND_SPEED)

    local ai = AgentComponents.GetAI(agent)
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
    bb:SetFloat("maxHealth", AgentComponents.GetMaxHealth(agent))
    local sampleName = _G.HELLO_SANDBOX_SAMPLE_NAME or "Sandbox8"
    if ConfigManager ~= nil and ConfigManager.ApplyAiBlackboardDefaults ~= nil then
        ConfigManager:ApplyAiBlackboardDefaults(bb, sampleName)
    end
    if ConfigManager ~= nil and ConfigManager.ConfigureBehaviorTreeDriver ~= nil then
        ConfigManager:ConfigureBehaviorTreeDriver(driver, sampleName)
    end

    local btModule, btGlobal, btConditions = _GetBtBinding(sampleName)

    if _ShouldRunBtRebuildSelfTest(sampleName) then
        local warmupTree = BehaviorTreeLoader.Build(_G[btGlobal] or SoldierBTConfig, agent, driver, bb, btConditions)
        if warmupTree ~= nil then
            driver:SetTree(warmupTree)
        end
    end

    local tree = BehaviorTreeLoader.BuildFromModule(btModule, btGlobal, agent, driver, bb, btConditions)
    if tree == nil then
        print("Error: failed to build Soldier behavior tree from config module=" .. tostring(btModule))
        return
    end
    driver:SetTree(tree)

    if _ShouldRunBtHotReloadSelfTest(sampleName) then
        local ok, meta = BehaviorTreeLoader.ReloadModule(btModule, btGlobal, agent, driver, bb, btConditions)
        local context = meta ~= nil and meta.context or nil
        if ok then
            print("[BTHotReloadSelfTest] PASS agent=" .. tostring(_GetAgentId(agent)) ..
                " module=" .. tostring(btModule) ..
                " subtreeBuilds=" .. tostring(context ~= nil and context.subtreeBuildCount or 0) ..
                " warnings=" .. tostring(context ~= nil and context.warningCount or 0))
        else
            print("[BTHotReloadSelfTest] FAIL agent=" .. tostring(_GetAgentId(agent)) ..
                " module=" .. tostring(btModule) ..
                " reason=" .. tostring(meta ~= nil and meta.error or "unknown"))
        end
    end
end

function Agent_Update(agent, deltaTimeInMillis)
    -- C++ SoldierObject::Update 已经自动 tick 了 BehaviorTreeDriver；
    -- 这里留给 per-frame Lua 钩子。
end

function Agent_EventHandle(agent, keycode)
end
