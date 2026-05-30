-- BehaviorTreeLoader.lua
-- Builds the existing C++ BehaviorTree runtime from a Lua table config.
--
-- Supported node types in this first pass:
--   Sequence  -> driver:NewSequence()
--   Selector  -> driver:NewSelector()
--   Condition -> driver:NewCondition() + SetEvaluator(function)
--   Action    -> driver:NewLuaAction(name) + BindToScript(script)
--   Event     -> driver:NewCondition() + BehaviorEventRuntime blackboard event check
--   Wait      -> driver:NewWait(waitMs). Use waitMs/ms for milliseconds, wait/seconds for seconds.
--   Inverter / ForceSuccess / ForceFailure -> one-child decorators
-- Optional config flags:
--   debugTrace      -> driver:SetDebugTraceEnabled(bool)
--   debugTracePrint -> driver:SetDebugTracePrintEnabled(bool), also enables trace when debugTrace is omitted

BehaviorTreeLoader = {}

local _DEFAULT_ACTION_DIR = "res/scripts/ai/decision/actions/"
local _unpack = unpack or table.unpack
local _behaviorEventRuntime = nil

local function _PrintError(message)
    print("BehaviorTreeLoader error: " .. tostring(message))
end

local function _GetBehaviorEventRuntime()
    if _behaviorEventRuntime == nil then
        _behaviorEventRuntime = require("res.scripts.runtime.BehaviorEventRuntime")
    end
    return _behaviorEventRuntime
end

local function _GetNodeType(cfg)
    return cfg and (cfg.node or cfg.type)
end

local function _BuildDebugName(cfg, nodeType)
    if cfg.debugName ~= nil then return tostring(cfg.debugName) end

    if nodeType == "Action" then
        return "Action:" .. tostring(cfg.action or cfg.name or cfg.script)
    end
    if nodeType == "Condition" then
        return "Condition:" .. tostring(cfg.condition or cfg.name)
    end
    if nodeType == "Wait" then
        return "Wait"
    end

    if cfg.name ~= nil then
        return tostring(nodeType) .. ":" .. tostring(cfg.name)
    end
    return tostring(nodeType)
end

local function _SetDebugName(node, cfg, context, nodeType)
    if node ~= nil and context.driver.SetNodeDebugName ~= nil then
        context.driver:SetNodeDebugName(node, _BuildDebugName(cfg, nodeType))
    end
    return node
end

local function _BindChildren(parent, children, context)
    if not children then return true end

    for _, childCfg in ipairs(children) do
        local child = BehaviorTreeLoader.BuildNode(childCfg, context)
        if child == nil then
            return false
        end
        parent:AddChild(child)
    end
    return true
end

local function _GetSingleChildConfig(cfg)
    if cfg.child ~= nil then return cfg.child end
    if cfg.children ~= nil then return cfg.children[1] end
    return nil
end

local function _BuildSingleChild(cfg, context)
    local childCfg = _GetSingleChildConfig(cfg)
    if childCfg == nil then
        _PrintError(tostring(_GetNodeType(cfg)) .. " node missing child")
        return nil
    end
    return BehaviorTreeLoader.BuildNode(childCfg, context)
end

local function _CreateAction(cfg, context)
    local key = cfg.action or cfg.name or cfg.script
    if key == nil then
        _PrintError("Action node missing action/name/script")
        return nil
    end

    local reuse = cfg.reuse
    if reuse == nil then reuse = true end

    if reuse and context.actionCache[key] ~= nil then
        return context.actionCache[key]
    end

    local scriptName = cfg.script or context.actions[key]
    if scriptName == nil then
        _PrintError("Action script not found: " .. tostring(key))
        return nil
    end

    local actionName = cfg.name or cfg.action or scriptName
    local action = context.driver:NewLuaAction(actionName)
    action:BindToScript((cfg.actionDir or context.actionDir) .. scriptName)
    _SetDebugName(action, cfg, context, "Action")

    if reuse then
        context.actionCache[key] = action
    end
    return action
end

local function _CreateConditionEvaluator(cfg, context)
    local condition = cfg.condition or cfg.name

    if type(condition) == "function" then
        return function()
            return condition(context.agent, context.blackboard, cfg)
        end
    end

    if type(condition) ~= "string" then
        _PrintError("Condition node missing condition/name")
        return nil
    end

    local fn = context.conditions and context.conditions[condition]
    if type(fn) ~= "function" then
        _PrintError("Condition function not found: " .. tostring(condition))
        return nil
    end

    if cfg.params ~= nil then
        local evaluator = fn(_unpack(cfg.params))
        if type(evaluator) == "function" then
            return function()
                return evaluator(context.agent, context.blackboard, cfg)
            end
        end
        return function()
            return evaluator and true or false
        end
    end

    return function()
        return fn(context.agent, context.blackboard, cfg)
    end
end

local function _CreateCondition(cfg, context)
    local evaluator = _CreateConditionEvaluator(cfg, context)
    if evaluator == nil then return nil end

    local node = context.driver:NewCondition()
    node:SetEvaluator(evaluator)
    _SetDebugName(node, cfg, context, "Condition")
    return node
end

local function _CreateEventCondition(cfg, context)
    local eventName = cfg.event or cfg.name
    if eventName == nil then
        _PrintError("Event node missing event/name")
        return nil
    end

    local node = context.driver:NewCondition()
    node:SetEvaluator(function()
        return _GetBehaviorEventRuntime().Test(context.agent, context.blackboard, eventName, cfg)
    end)
    _SetDebugName(node, cfg, context, "Event")
    return node
end

function BehaviorTreeLoader.BuildNode(cfg, context)
    if type(cfg) ~= "table" then
        _PrintError("node config must be a table")
        return nil
    end

    local nodeType = _GetNodeType(cfg)

    if nodeType == "Sequence" then
        local node = context.driver:NewSequence()
        _SetDebugName(node, cfg, context, nodeType)
        if not _BindChildren(node, cfg.children, context) then return nil end
        return node
    end

    if nodeType == "Selector" then
        local node = context.driver:NewSelector()
        _SetDebugName(node, cfg, context, nodeType)
        if not _BindChildren(node, cfg.children, context) then return nil end
        return node
    end

    if nodeType == "Condition" then
        return _CreateCondition(cfg, context)
    end

    if nodeType == "Event" or nodeType == "BehaviorEvent" then
        return _CreateEventCondition(cfg, context)
    end

    if nodeType == "Action" then
        return _CreateAction(cfg, context)
    end

    if nodeType == "Wait" then
        local waitMs = cfg.waitMs or cfg.ms
        if waitMs == nil and cfg.wait ~= nil then waitMs = cfg.wait * 1000.0 end
        if cfg.seconds ~= nil then waitMs = cfg.seconds * 1000.0 end
        if waitMs == nil then waitMs = 0.0 end
        local node = context.driver:NewWait(waitMs)
        return _SetDebugName(node, cfg, context, nodeType)
    end

    if nodeType == "Inverter" or nodeType == "Invert" then
        local child = _BuildSingleChild(cfg, context)
        if child == nil then return nil end
        local node = context.driver:NewInverter(child)
        return _SetDebugName(node, cfg, context, nodeType)
    end

    if nodeType == "ForceSuccess" or nodeType == "Success" or nodeType == "Succeeder" then
        local child = _BuildSingleChild(cfg, context)
        if child == nil then return nil end
        local node = context.driver:NewForceSuccess(child)
        return _SetDebugName(node, cfg, context, nodeType)
    end

    if nodeType == "ForceFailure" or nodeType == "Failure" or nodeType == "Failer" then
        local child = _BuildSingleChild(cfg, context)
        if child == nil then return nil end
        local node = context.driver:NewForceFailure(child)
        return _SetDebugName(node, cfg, context, nodeType)
    end

    _PrintError("unsupported node type: " .. tostring(nodeType))
    return nil
end

function BehaviorTreeLoader.Build(config, agent, driver, blackboard, conditions)
    if type(config) ~= "table" then
        _PrintError("tree config must be a table")
        return nil
    end
    if driver == nil then
        _PrintError("driver is nil")
        return nil
    end

    local context = {
        agent = agent,
        driver = driver,
        blackboard = blackboard,
        conditions = conditions or {},
        actions = config.actions or {},
        actionDir = config.actionDir or _DEFAULT_ACTION_DIR,
        actionCache = {},
    }

    if config.debugTrace ~= nil and driver.SetDebugTraceEnabled ~= nil then
        driver:SetDebugTraceEnabled(config.debugTrace and true or false)
    end
    if config.debugTracePrint ~= nil and driver.SetDebugTracePrintEnabled ~= nil then
        driver:SetDebugTracePrintEnabled(config.debugTracePrint and true or false)
        if config.debugTrace == nil and config.debugTracePrint then
            driver:SetDebugTraceEnabled(true)
        end
    end

    local rootConfig = config.tree or config.root or config
    local root = BehaviorTreeLoader.BuildNode(rootConfig, context)
    if root == nil then return nil end

    local tree = driver:NewTree()
    tree:SetRoot(root)
    return tree
end
