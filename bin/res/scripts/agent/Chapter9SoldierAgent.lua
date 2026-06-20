-- Chapter9SoldierAgent.lua
-- Chapter 9 tactics agent entrypoint. It intentionally mirrors the original
-- chapter_9_tactics behavior tree shape instead of using later sample branches.

require("res.scripts.ai.behavior.SoldierConditions.lua")

local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

local _ACTION_DIR = "res/scripts/ai/decision/actions/"

local function _NewLuaAction(driver, name, scriptName)
    local action = driver:NewLuaAction(name)
    action:BindToScript(_ACTION_DIR .. scriptName)
    return action
end

local function _Cond(driver, fn)
    local cond = driver:NewCondition()
    cond:SetEvaluator(fn)
    return cond
end

local function _Seq(driver, ...)
    local seq = driver:NewSequence()
    for _, child in ipairs({...}) do seq:AddChild(child) end
    return seq
end

local function _Sel(driver, ...)
    local sel = driver:NewSelector()
    for _, child in ipairs({...}) do sel:AddChild(child) end
    return sel
end

local function _BuildChapter9Tree(agent, driver, bb)
    local idleAction       = _NewLuaAction(driver, "idle",       "IdleAction.lua")
    local moveAction       = _NewLuaAction(driver, "move",       "MoveAction.lua")
    local pursueAction     = _NewLuaAction(driver, "pursue",     "PursueAction.lua")
    local shootAction      = _NewLuaAction(driver, "shoot",      "ShootAction.lua")
    local reloadAction     = _NewLuaAction(driver, "reload",     "ReloadAction.lua")
    local fleeAction       = _NewLuaAction(driver, "flee",       "FleeAction.lua")
    local dieAction        = _NewLuaAction(driver, "die",        "DieAction.lua")
    local randomMoveAction = _NewLuaAction(driver, "randomMove", "RandomMoveAction.lua")

    local criticalChoice = _Seq(driver,
        _Cond(driver, function() return SoldierConditions.IsCriticalHealth(agent, bb) end),
        _Sel(driver,
            _Seq(driver,
                _Cond(driver, function() return SoldierConditions.HasMovePosition(agent, bb) end),
                moveAction),
            fleeAction))

    local combatChoice = _Seq(driver,
        _Cond(driver, function() return SoldierConditions.HasEnemy(agent, bb) end),
        _Sel(driver,
            _Seq(driver,
                _Cond(driver, function() return not SoldierConditions.HasAmmo(agent, bb) end),
                reloadAction),
            _Seq(driver,
                _Cond(driver, function() return SoldierConditions.CanShootEnemy(agent, bb) end),
                shootAction),
            pursueAction))

    local moveChoice = _Seq(driver,
        _Cond(driver, function() return SoldierConditions.HasMovePosition(agent, bb) end),
        moveAction)

    local randomChoice = _Seq(driver,
        _Cond(driver, function() return math.random() >= 0.5 end),
        randomMoveAction)

    local actionChoice = _Sel(driver,
        criticalChoice,
        combatChoice,
        moveChoice,
        randomChoice,
        idleAction)

    local aliveChoice = _Seq(driver,
        _Cond(driver, function() return SoldierConditions.IsAlive(agent, bb) end),
        actionChoice)

    local root = _Sel(driver, aliveChoice, dieAction)
    local tree = driver:NewTree()
    tree:SetRoot(root)
    return tree
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
    local sampleName = _G.HELLO_SANDBOX_SAMPLE_NAME or "Sandbox17"
    if ConfigManager ~= nil and ConfigManager.ApplyAiBlackboardDefaults ~= nil then
        ConfigManager:ApplyAiBlackboardDefaults(bb, sampleName)
    end
    if ConfigManager ~= nil and ConfigManager.ConfigureBehaviorTreeDriver ~= nil then
        ConfigManager:ConfigureBehaviorTreeDriver(driver, sampleName)
    end

    driver:SetTree(_BuildChapter9Tree(agent, driver, bb))
end

function Agent_Update(agent, deltaTimeInMillis)
end

function Agent_EventHandle(agent, keycode)
end
