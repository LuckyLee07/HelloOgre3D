-- BehaviorTreeLoader.lua
-- Builds the existing C++ BehaviorTree runtime from a Lua table config.
--
-- Supported node types in this first pass:
--   Sequence  -> driver:NewSequence(reevaluateMs)
--   Selector  -> driver:NewSelector(reevaluateMs)
--   Parallel  -> driver:NewParallel(successPolicy, failurePolicy)
--   Random    -> driver:NewRandomSelector()
--   Condition -> driver:NewCondition() + SetEvaluator(function)
--   Action    -> driver:NewLuaAction(name) + BindToScript(script)
--   Wait      -> driver:NewWait(waitMs). Use waitMs/ms for milliseconds, wait/seconds for seconds.
--   Inverter / ForceSuccess / ForceFailure -> one-child decorators
-- Runtime params:
--   params = { { blackboard = "key", type = "float", default = 0 } } reads from Blackboard per tick.
--   type supports scalar values plus object-id / int-array / float-array / string-array / object-id-array.
-- Condition cache:
--   cache=true or cacheMs/resultCacheMs enables LuaCondition result cache.
--   cacheKeys/dependsOn/dirtyKeys narrows invalidation to Blackboard key revisions.
-- Optional config flags:
--   reactive / reevaluateMs -> Sequence/Selector can restart from first child to interrupt RUNNING branch
--   debugTrace      -> driver:SetDebugTraceEnabled(bool)
--   debugTracePrint -> driver:SetDebugTracePrintEnabled(bool), also enables trace when debugTrace is omitted

BehaviorTreeLoader = {}

local _DEFAULT_ACTION_DIR = "res/scripts/ai/decision/actions/"
local _unpack = unpack or table.unpack

local function _GetNodeType(cfg)
    return cfg and (cfg.node or cfg.type)
end

local function _PathToString(path)
    if path == nil or path == "" then
        return "root"
    end
    return tostring(path)
end

local function _NodeLabel(cfg)
    local nodeType = _GetNodeType(cfg)
    if nodeType == nil then
        return "unknown"
    end
    return tostring(nodeType)
end

local function _Issue(context, cfg, path, field, message, fallback, level)
    level = level or "error"
    local prefix = level == "warning" and "BehaviorTreeLoader warning: " or "BehaviorTreeLoader error: "
    local text = prefix ..
        "path=" .. _PathToString(path) ..
        " node=" .. _NodeLabel(cfg) ..
        " field=" .. tostring(field or "node") ..
        " message=" .. tostring(message)
    if fallback ~= nil and fallback ~= "" then
        text = text .. " fallback=" .. tostring(fallback)
    end
    print(text)

    if context ~= nil then
        if level == "warning" then
            context.warningCount = (context.warningCount or 0) + 1
        else
            context.errorCount = (context.errorCount or 0) + 1
        end
    end
end

local function _IssueError(context, cfg, path, field, message, fallback)
    _Issue(context, cfg, path, field, message, fallback or "tree build fails; node is not attached", "error")
end

local function _IssueWarning(context, cfg, path, field, message, fallback)
    _Issue(context, cfg, path, field, message, fallback, "warning")
end

local function _ChildPath(path, index)
    return _PathToString(path) .. ".children[" .. tostring(index) .. "]"
end

local function _FieldPath(path, field)
    return _PathToString(path) .. "." .. tostring(field)
end

local function _ListKeys(values)
    if type(values) ~= "table" then
        return "-"
    end

    local keys = {}
    for key, _ in pairs(values) do
        table.insert(keys, tostring(key))
    end
    table.sort(keys)
    if #keys == 0 then
        return "-"
    end
    return table.concat(keys, ",")
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

local function _NormalizeValueType(valueType, defaultValue)
    if valueType ~= nil then
        return string.lower(tostring(valueType))
    end
    if type(defaultValue) == "boolean" then return "bool" end
    if type(defaultValue) == "string" then return "string" end
    return "float"
end

local function _IsValueType(valueType, ...)
    for i = 1, select("#", ...) do
        if valueType == select(i, ...) then
            return true
        end
    end
    return false
end

local function _ReadArrayValue(spec, bb, key, valueType, defaultValue)
    local fallback = type(defaultValue) == "table" and defaultValue or {}
    if not bb:Has(key) then return fallback end

    if _IsValueType(valueType, "int-array", "integer-array", "int[]", "integer[]", "ints", "integers") then
        if bb.GetIntArrayCount == nil then return fallback end
        local values = {}
        local defaultElement = tonumber(spec.defaultElement or spec.elementDefault) or 0
        for i = 1, bb:GetIntArrayCount(key) do
            values[i] = bb:GetIntArrayValue(key, i, defaultElement)
        end
        return values
    end

    if _IsValueType(valueType, "float-array", "number-array", "float[]", "number[]", "floats", "numbers") then
        if bb.GetFloatArrayCount == nil then return fallback end
        local values = {}
        local defaultElement = tonumber(spec.defaultElement or spec.elementDefault) or 0.0
        for i = 1, bb:GetFloatArrayCount(key) do
            values[i] = bb:GetFloatArrayValue(key, i, defaultElement)
        end
        return values
    end

    if _IsValueType(valueType, "string-array", "string[]", "strings") then
        if bb.GetStringArrayCount == nil then return fallback end
        local values = {}
        for i = 1, bb:GetStringArrayCount(key) do
            values[i] = bb:GetStringArrayValue(key, i)
        end
        return values
    end

    if _IsValueType(valueType, "object-id-array", "objectid-array", "object-id[]", "objectid[]", "objectids", "object-ids") then
        if bb.GetObjectIdArrayCount == nil then return fallback end
        local values = {}
        local defaultElement = tonumber(spec.defaultElement or spec.elementDefault) or -1
        for i = 1, bb:GetObjectIdArrayCount(key) do
            values[i] = bb:GetObjectIdArrayValue(key, i, defaultElement)
        end
        return values
    end

    return fallback
end

local function _IsBlackboardValueSpec(value)
    if type(value) ~= "table" then return false end
    if value.blackboard ~= nil or value.bb ~= nil then return true end
    if value.source == "blackboard" or value.from == "blackboard" then return value.key ~= nil end
    return false
end

local function _ReadBlackboardValue(spec, context)
    local bb = context.blackboard
    if bb == nil then return spec.default end

    local key = spec.blackboard or spec.bb or spec.key
    if key == nil then return spec.default end
    key = tostring(key)

    local defaultValue = spec.default
    local valueType = _NormalizeValueType(spec.valueType or spec.type, defaultValue)

    if _IsValueType(valueType,
        "int-array", "integer-array", "int[]", "integer[]", "ints", "integers",
        "float-array", "number-array", "float[]", "number[]", "floats", "numbers",
        "string-array", "string[]", "strings",
        "object-id-array", "objectid-array", "object-id[]", "objectid[]", "objectids", "object-ids") then
        return _ReadArrayValue(spec, bb, key, valueType, defaultValue)
    end
    if valueType == "bool" or valueType == "boolean" then
        local boolValue = bb:GetBool(key, defaultValue and true or false)
        return boolValue
    end
    if valueType == "object-id" or valueType == "objectid" then
        if bb.GetObjectId ~= nil then
            local objectId = bb:GetObjectId(key, tonumber(defaultValue) or -1)
            return objectId
        end
        local intId = bb:GetInt(key, tonumber(defaultValue) or -1)
        return intId
    end
    if valueType == "int" or valueType == "integer" then
        local intValue = bb:GetInt(key, tonumber(defaultValue) or 0)
        return intValue
    end
    if valueType == "string" then
        if not bb:Has(key) then return defaultValue end
        local stringValue = bb:GetString(key)
        return stringValue
    end
    if valueType == "agent" or valueType == "object" then
        local agentValue = bb:GetAgent(key)
        return agentValue or defaultValue
    end
    if valueType == "vec3" or valueType == "vector3" then
        if not bb:Has(key) then return defaultValue end
        local vec3Value = bb:GetVec3(key)
        return vec3Value
    end
    local floatValue = bb:GetFloat(key, tonumber(defaultValue) or 0.0)
    return floatValue
end

local function _ResolveValue(value, context)
    if _IsBlackboardValueSpec(value) then
        return _ReadBlackboardValue(value, context)
    end
    if type(value) == "table" and value.value ~= nil then
        return value.value
    end
    return value
end

local function _ResolveParams(params, context)
    local values = {}
    for i, value in ipairs(params) do
        values[i] = _ResolveValue(value, context)
    end
    return values
end

local function _PolicyToInt(value, defaultValue, context, cfg, path, field, fallbackLabel)
    if value == nil then return defaultValue end
    if type(value) == "number" then return value end

    local text = string.lower(tostring(value))
    if text == "one" or text == "any" or text == "first" then
        return 1
    end
    if text == "all" then
        return 2
    end
    _IssueWarning(context, cfg, path, field, "unsupported policy '" .. tostring(value) .. "'", fallbackLabel)
    return defaultValue
end

local function _GetReevaluateMs(cfg, context, path)
	local value = cfg.reevaluateMs or cfg.recheckMs or cfg.reactiveMs
    if value == nil and cfg.reevaluateSeconds ~= nil then
        value = cfg.reevaluateSeconds * 1000.0
    end
    if value == nil and cfg.recheckSeconds ~= nil then
        value = cfg.recheckSeconds * 1000.0
    end
    if value == nil and cfg.reactive == true then
        value = 0.0
    end
    if value == nil and cfg.reevaluate == true then
        value = 0.0
    end
    if value == nil then
        return -1.0
    end
    value = _ResolveValue(value, context)
    if type(value) == "boolean" then
        return value and 0.0 or -1.0
    end
    local numericValue = tonumber(value)
    if numericValue == nil then
        _IssueWarning(context, cfg, path, "reevaluateMs", "invalid reevaluate value '" .. tostring(value) .. "'", "reactive reevaluation disabled")
        return -1.0
    end
	return numericValue
end

local function _AddConditionCacheKey(node, key)
    if key == nil or node.AddResultCacheDependencyKey == nil then return false end
    local text = tostring(key)
    if text == "" then return false end
    node:AddResultCacheDependencyKey(text)
    return true
end

local function _ConfigureConditionCache(node, cfg, context, path)
    if node == nil or node.SetResultCacheEnabled == nil then return end

    local cacheMs = cfg.cacheMs or cfg.resultCacheMs or cfg.cacheTtlMs
    if cacheMs == nil and cfg.cacheSeconds ~= nil then
        cacheMs = cfg.cacheSeconds * 1000.0
    end
    if cacheMs ~= nil and node.SetResultCacheTtlMs ~= nil then
        local resolvedCacheMs = _ResolveValue(cacheMs, context)
        local numericCacheMs = tonumber(resolvedCacheMs)
        if numericCacheMs == nil then
            _IssueWarning(context, cfg, path, "cacheMs", "invalid cache duration '" .. tostring(resolvedCacheMs) .. "'", "condition result cache uses default ttl")
        else
            node:SetResultCacheTtlMs(numericCacheMs)
        end
    end

    local cacheKeys = cfg.cacheKeys or cfg.dependsOn or cfg.dirtyKeys or cfg.dependencyKeys
    if cacheKeys ~= nil and node.AddResultCacheDependencyKey ~= nil then
        if node.ClearResultCacheDependencyKeys ~= nil then
            node:ClearResultCacheDependencyKeys()
        end
        if type(cacheKeys) == "table" then
            local added = 0
            for _, key in ipairs(cacheKeys) do
                if _AddConditionCacheKey(node, key) then
                    added = added + 1
                end
            end
            if added == 0 then
                for key, enabled in pairs(cacheKeys) do
                    if enabled == true then
                        _AddConditionCacheKey(node, key)
                    end
                end
            end
        else
            _AddConditionCacheKey(node, cacheKeys)
        end
    end

    local cacheEnabled = cfg.cache == true
        or cfg.resultCache == true
        or cacheMs ~= nil
        or cacheKeys ~= nil
    if cfg.cache == false or cfg.resultCache == false then
        cacheEnabled = false
    end
    node:SetResultCacheEnabled(cacheEnabled and true or false)
end

local function _SetDebugName(node, cfg, context, nodeType)
    if node ~= nil and context.driver.SetNodeDebugName ~= nil then
        context.driver:SetNodeDebugName(node, _BuildDebugName(cfg, nodeType))
    end
    return node
end

local function _BindChildren(parent, children, context, path, cfg)
    if children == nil then
        _IssueWarning(context, cfg, path, "children", "children list is missing", "node is created with no children")
        return true
    end
    if type(children) ~= "table" then
        _IssueWarning(context, cfg, path, "children", "children must be a table", "children ignored; node is created with no children")
        return true
    end
    if #children == 0 then
        _IssueWarning(context, cfg, path, "children", "children list is empty", "node is created with no children")
        return true
    end

    for index, childCfg in ipairs(children) do
        local child = BehaviorTreeLoader.BuildNode(childCfg, context, _ChildPath(path, index))
        if child == nil then
            return false
        end
        parent:AddChild(child)
    end
    return true
end

local function _GetSingleChildConfig(cfg)
    if cfg.child ~= nil then return cfg.child end
    if type(cfg.children) == "table" then return cfg.children[1] end
    return nil
end

local function _BuildSingleChild(cfg, context, path)
    local childCfg = _GetSingleChildConfig(cfg)
    if childCfg == nil then
        _IssueError(context, cfg, path, "child", "decorator node missing child", "tree build fails; decorator is not created")
        return nil
    end
    local childPath = cfg.child ~= nil and _FieldPath(path, "child") or _ChildPath(path, 1)
    return BehaviorTreeLoader.BuildNode(childCfg, context, childPath)
end

local function _CreateAction(cfg, context, path)
    local key = cfg.action or cfg.name or cfg.script
    if key == nil then
        _IssueError(context, cfg, path, "action", "Action node missing action/name/script")
        return nil
    end

    local reuse = cfg.reuse
    if reuse == nil then reuse = true end

    if reuse and context.actionCache[key] ~= nil then
        return context.actionCache[key]
    end

    local scriptName = cfg.script or context.actions[key]
    if scriptName == nil then
        _IssueError(context, cfg, path, "script", "Action script not found for key '" .. tostring(key) .. "'; known actions=" .. _ListKeys(context.actions))
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

local function _CreateConditionEvaluator(cfg, context, path)
    local condition = cfg.condition or cfg.name

    if type(condition) == "function" then
        return function()
            return condition(context.agent, context.blackboard, cfg)
        end
    end

    if type(condition) ~= "string" then
        _IssueError(context, cfg, path, "condition", "Condition node missing condition/name")
        return nil
    end

    local fn = context.conditions and context.conditions[condition]
    if type(fn) ~= "function" then
        _IssueError(context, cfg, path, "condition", "Condition function not found '" .. tostring(condition) .. "'; known conditions=" .. _ListKeys(context.conditions))
        return nil
    end

    if cfg.params ~= nil then
        if type(cfg.params) ~= "table" then
            _IssueWarning(context, cfg, path, "params", "params must be a table", "condition is called without params")
            return function()
                return fn(context.agent, context.blackboard, cfg)
            end
        end
        return function()
            local evaluator = fn(_unpack(_ResolveParams(cfg.params, context)))
            if type(evaluator) == "function" then
                return evaluator(context.agent, context.blackboard, cfg)
            end
            return evaluator and true or false
        end
    end

    return function()
        return fn(context.agent, context.blackboard, cfg)
    end
end

local function _CreateCondition(cfg, context, path)
    local evaluator = _CreateConditionEvaluator(cfg, context, path)
    if evaluator == nil then return nil end

    local node = context.driver:NewCondition()
    node:SetEvaluator(evaluator)
    _ConfigureConditionCache(node, cfg, context, path)
    _SetDebugName(node, cfg, context, "Condition")
    return node
end

function BehaviorTreeLoader.BuildNode(cfg, context, path)
    path = _PathToString(path)
    if type(cfg) ~= "table" then
        _IssueError(context, { node = "unknown" }, path, "node", "node config must be a table")
        return nil
    end

    local nodeType = _GetNodeType(cfg)
    if nodeType == nil then
        _IssueError(context, cfg, path, "node", "node type is missing")
        return nil
    end

    if nodeType == "Sequence" then
        local node = context.driver:NewSequence(_GetReevaluateMs(cfg, context, path))
        _SetDebugName(node, cfg, context, nodeType)
        if not _BindChildren(node, cfg.children, context, path, cfg) then return nil end
        return node
    end

    if nodeType == "Selector" then
        local node = context.driver:NewSelector(_GetReevaluateMs(cfg, context, path))
        _SetDebugName(node, cfg, context, nodeType)
        if not _BindChildren(node, cfg.children, context, path, cfg) then return nil end
        return node
    end

    if nodeType == "Parallel" then
        local successPolicy = _PolicyToInt(cfg.successPolicy or cfg.success, 2, context, cfg, path, "successPolicy", "uses default successPolicy=all")
        local failurePolicy = _PolicyToInt(cfg.failurePolicy or cfg.failure, 1, context, cfg, path, "failurePolicy", "uses default failurePolicy=one")
        local node = context.driver:NewParallel(successPolicy, failurePolicy)
        _SetDebugName(node, cfg, context, nodeType)
        if not _BindChildren(node, cfg.children, context, path, cfg) then return nil end
        return node
    end

    if nodeType == "Random" or nodeType == "RandomSelector" then
        local node = context.driver:NewRandomSelector()
        _SetDebugName(node, cfg, context, "Random")
        if not _BindChildren(node, cfg.children, context, path, cfg) then return nil end
        return node
    end

    if nodeType == "Condition" then
        return _CreateCondition(cfg, context, path)
    end

    if nodeType == "Action" then
        return _CreateAction(cfg, context, path)
    end

    if nodeType == "Wait" then
        local waitMs = cfg.waitMs or cfg.ms
        if waitMs == nil and cfg.wait ~= nil then waitMs = cfg.wait * 1000.0 end
        if cfg.seconds ~= nil then waitMs = cfg.seconds * 1000.0 end
        waitMs = _ResolveValue(waitMs, context)
        local numericWaitMs = tonumber(waitMs)
        if numericWaitMs == nil then
            _IssueWarning(context, cfg, path, "waitMs", "missing or invalid wait duration '" .. tostring(waitMs) .. "'", "uses waitMs=0")
            numericWaitMs = 0.0
        end
        local node = context.driver:NewWait(numericWaitMs)
        return _SetDebugName(node, cfg, context, nodeType)
    end

    if nodeType == "Inverter" or nodeType == "Invert" then
        local child = _BuildSingleChild(cfg, context, path)
        if child == nil then return nil end
        local node = context.driver:NewInverter(child)
        return _SetDebugName(node, cfg, context, nodeType)
    end

    if nodeType == "ForceSuccess" or nodeType == "Success" or nodeType == "Succeeder" then
        local child = _BuildSingleChild(cfg, context, path)
        if child == nil then return nil end
        local node = context.driver:NewForceSuccess(child)
        return _SetDebugName(node, cfg, context, nodeType)
    end

    if nodeType == "ForceFailure" or nodeType == "Failure" or nodeType == "Failer" then
        local child = _BuildSingleChild(cfg, context, path)
        if child == nil then return nil end
        local node = context.driver:NewForceFailure(child)
        return _SetDebugName(node, cfg, context, nodeType)
    end

    _IssueError(context, cfg, path, "node", "unsupported node type '" .. tostring(nodeType) .. "'")
    return nil
end

function BehaviorTreeLoader.Build(config, agent, driver, blackboard, conditions)
    if type(config) ~= "table" then
        _IssueError(nil, { node = "Tree" }, "root", "config", "tree config must be a table")
        return nil
    end
    if driver == nil then
        _IssueError(nil, { node = "Tree" }, "root", "driver", "driver is nil")
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
        errorCount = 0,
        warningCount = 0,
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
    local root = BehaviorTreeLoader.BuildNode(rootConfig, context, "root")
    if root == nil then
        print("BehaviorTreeLoader build failed: errors=" .. tostring(context.errorCount) .. " warnings=" .. tostring(context.warningCount))
        return nil
    end

    local tree = driver:NewTree()
    tree:SetRoot(root)
    if context.warningCount > 0 then
        print("BehaviorTreeLoader build completed with warnings: warnings=" .. tostring(context.warningCount))
    end
    return tree
end
