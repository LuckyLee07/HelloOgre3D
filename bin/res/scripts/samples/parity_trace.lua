-- parity_trace.lua
-- Structured trace helper for reproducible sample-to-sample comparisons.

local ParityTrace = {}
local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

local function _GetEnv(name)
	return os.getenv and os.getenv(name) or nil
end

local function _Truthy(value)
	return value == true or value == "1" or value == "true" or value == "TRUE" or value == "True" or value == "yes" or value == "YES"
end

local function _Number(value, defaultValue)
	local numberValue = tonumber(value)
	if numberValue == nil then
		return defaultValue
	end
	return numberValue
end

local function _ConfigNumber(config, key, envName, defaultValue)
	local envValue = _GetEnv(envName)
	if envValue ~= nil and envValue ~= "" then
		return _Number(envValue, defaultValue)
	end
	if config ~= nil and config[key] ~= nil then
		return _Number(config[key], defaultValue)
	end
	return defaultValue
end

local function _ConfigBool(config, key, envName, defaultValue)
	local envValue = _GetEnv(envName)
	if envValue ~= nil and envValue ~= "" then
		return _Truthy(envValue)
	end
	if config ~= nil and config[key] ~= nil then
		return _Truthy(config[key])
	end
	return defaultValue
end

local function _ConfigString(config, key, envName, defaultValue)
	local envValue = _GetEnv(envName)
	if envValue ~= nil and envValue ~= "" then
		return tostring(envValue)
	end
	if config ~= nil and config[key] ~= nil then
		return tostring(config[key])
	end
	return defaultValue
end

local function _EscapeString(value)
	value = tostring(value or "")
	value = string.gsub(value, "\\", "\\\\")
	value = string.gsub(value, "\"", "\\\"")
	value = string.gsub(value, "\n", "\\n")
	value = string.gsub(value, "\r", "\\r")
	value = string.gsub(value, "\t", "\\t")
	return value
end

local function _IsArray(value)
	if type(value) ~= "table" then
		return false
	end
	local count = 0
	for key, _ in pairs(value) do
		if type(key) ~= "number" or key < 1 or math.floor(key) ~= key then
			return false
		end
		if key > count then
			count = key
		end
	end
	for index = 1, count do
		if value[index] == nil then
			return false
		end
	end
	return true
end

local function _EncodeJson(value)
	local valueType = type(value)
	if valueType == "nil" then
		return "null"
	elseif valueType == "number" then
		if value ~= value or value == math.huge or value == -math.huge then
			return "null"
		end
		return string.format("%.6g", value)
	elseif valueType == "boolean" then
		return value and "true" or "false"
	elseif valueType == "string" then
		return "\"" .. _EscapeString(value) .. "\""
	elseif valueType == "table" then
		if _IsArray(value) then
			local parts = {}
			for index = 1, #value do
				parts[#parts + 1] = _EncodeJson(value[index])
			end
			return "[" .. table.concat(parts, ",") .. "]"
		end

		local keys = {}
		for key, _ in pairs(value) do
			keys[#keys + 1] = tostring(key)
		end
		table.sort(keys)

		local parts = {}
		for _, key in ipairs(keys) do
			parts[#parts + 1] = "\"" .. _EscapeString(key) .. "\":" .. _EncodeJson(value[key])
		end
		return "{" .. table.concat(parts, ",") .. "}"
	end
	return "\"" .. _EscapeString(value) .. "\""
end

local function _Round(value, scale)
	scale = scale or 1000
	return math.floor((tonumber(value) or 0) * scale + 0.5) / scale
end

function ParityTrace.Vector3(value)
	if value == nil then
		return nil
	end
	return { _Round(value.x), _Round(value.y), _Round(value.z) }
end

local function _SafeCall(object, methodName, defaultValue)
	if object == nil or object[methodName] == nil then
		return defaultValue
	end
	local ok, result = pcall(function()
		return object[methodName](object)
	end)
	if ok then
		return result
	end
	return defaultValue
end

local function _GetAgentBlackboard(agent)
	local ai = AgentComponents.GetAI(agent)
	if ai ~= nil and ai.GetBlackboard ~= nil then
		return ai:GetBlackboard()
	end
	return nil
end

local function _BlackboardHas(bb, key)
	if bb == nil or bb.Has == nil then
		return false
	end
	local ok, result = pcall(function()
		return bb:Has(key)
	end)
	return ok and result == true
end

local function _BlackboardInt(bb, key, defaultValue)
	if bb == nil or bb.GetInt == nil then
		return defaultValue
	end
	local ok, result = pcall(function()
		return bb:GetInt(key, defaultValue)
	end)
	if ok and result ~= nil then
		return result
	end
	return defaultValue
end

local function _BlackboardFloat(bb, key, defaultValue)
	if bb == nil or bb.GetFloat == nil then
		return defaultValue
	end
	local ok, result = pcall(function()
		return bb:GetFloat(key, defaultValue)
	end)
	if ok and result ~= nil then
		return result
	end
	return defaultValue
end

local function _BlackboardBool(bb, key, defaultValue)
	if bb == nil or bb.GetBool == nil then
		return defaultValue
	end
	local ok, result = pcall(function()
		return bb:GetBool(key, defaultValue)
	end)
	if ok and result ~= nil then
		return result
	end
	return defaultValue
end

local function _BlackboardString(bb, key, defaultValue)
	if bb == nil or bb.GetString == nil or not _BlackboardHas(bb, key) then
		return defaultValue
	end
	local ok, result = pcall(function()
		return bb:GetString(key)
	end)
	if ok and result ~= nil then
		return tostring(result)
	end
	return defaultValue
end

local function _BlackboardVec3(bb, key)
	if bb == nil or bb.GetVec3 == nil or not _BlackboardHas(bb, key) then
		return nil
	end
	local ok, result = pcall(function()
		return bb:GetVec3(key)
	end)
	if ok then
		return result
	end
	return nil
end

function ParityTrace.AgentIntentSnapshot(agent)
	local bb = _GetAgentBlackboard(agent)
	local sequence = _BlackboardInt(bb, "intent.sequence", 0)
	if sequence <= 0 then
		return nil
	end

	return {
		sequence = sequence,
		action = _BlackboardString(bb, "intent.action", ""),
		phase = _BlackboardString(bb, "intent.phase", ""),
		movement = _BlackboardString(bb, "intent.movement", ""),
		animation = _BlackboardString(bb, "intent.animation", ""),
		reason = _BlackboardString(bb, "intent.reason", ""),
		elapsedMs = _Round(_BlackboardFloat(bb, "intent.elapsedMs", 0.0), 10),
		durationMs = _Round(_BlackboardFloat(bb, "intent.durationMs", 0.0), 10),
		distance = _Round(_BlackboardFloat(bb, "intent.distance", -1.0), 1000),
		speed = _Round(_BlackboardFloat(bb, "intent.speed", 0.0), 1000),
		maxSpeed = _Round(_BlackboardFloat(bb, "intent.maxSpeed", 0.0), 1000),
		moving = _BlackboardBool(bb, "intent.moving", false),
		position = ParityTrace.Vector3(_BlackboardVec3(bb, "intent.position")),
		target = ParityTrace.Vector3(_BlackboardVec3(bb, "intent.target")),
		velocity = ParityTrace.Vector3(_BlackboardVec3(bb, "intent.velocity")),
		enemyId = _BlackboardInt(bb, "intent.enemyId", -1),
		enemyTeam = _BlackboardInt(bb, "intent.enemyTeam", -1),
		enemyPosition = ParityTrace.Vector3(_BlackboardVec3(bb, "intent.enemyPosition")),
		enemyDistance = _Round(_BlackboardFloat(bb, "intent.enemyDistance", -1.0), 1000),
	}
end

function ParityTrace.AgentSnapshot(agent, index, extra)
	local bodyAsm = AgentComponents.GetBodyAsm(agent)
	local snapshot = {
		index = index,
		id = _SafeCall(agent, "GetObjId", -1),
		team = _SafeCall(agent, "GetTeamId", -1),
		hp = _Round(_SafeCall(agent, "GetHealth", 0), 10),
		state = tostring(_SafeCall(agent, "GetCurStateName", "")),
		stateId = _SafeCall(agent, "GetCurStateId", -1),
		stateNext = tostring(_SafeCall(bodyAsm, "GetNextStateName", "")),
		stateDesired = tostring(_SafeCall(bodyAsm, "GetDesiredStateName", "")),
		stateTransitioning = _SafeCall(bodyAsm, "IsTransitioning", false) == true,
		pos = ParityTrace.Vector3(_SafeCall(agent, "GetPosition", nil)),
		forward = ParityTrace.Vector3(_SafeCall(agent, "GetForward", nil)),
		vel = ParityTrace.Vector3(_SafeCall(agent, "GetVelocity", nil)),
		speed = _Round(_SafeCall(agent, "GetSpeed", 0), 1000),
		target = ParityTrace.Vector3(_SafeCall(agent, "GetTarget", nil)),
	}
	for key, value in pairs(extra or {}) do
		snapshot[key] = value
	end
	return snapshot
end

-- 采集单个 C++ ObjectASM 的动画度量（chapter3 原地动画对比用）。
-- asm 来自 AnimComponent:GetBodyAsm()。extra 可附 deltaTimeMs/role 等。
function ParityTrace.AnimSnapshot(asm, index, extra)
	local snapshot = {
		index = index,
		state = tostring(_SafeCall(asm, "GetCurrStateName", "")),
		stateNext = tostring(_SafeCall(asm, "GetNextStateName", "")),
		stateTransitioning = _SafeCall(asm, "IsTransitioning", false) == true,
		stateTime = _Round(_SafeCall(asm, "GetCurrStateTime", 0), 1000),
		stateLength = _Round(_SafeCall(asm, "GetCurrStateLength", 0), 1000),
		progress = _Round(_SafeCall(asm, "GetCurrStateProgress", 0), 1000),
		weight = nil,
	}
	for key, value in pairs(extra or {}) do
		snapshot[key] = value
	end
	return snapshot
end

function ParityTrace.SplitLines(text, maxLines, maxLength)
	local lines = {}
	if text == nil then
		return lines
	end
	maxLines = math.max(1, math.floor(tonumber(maxLines) or 8))
	maxLength = math.max(1, math.floor(tonumber(maxLength) or 220))
	for line in string.gmatch(tostring(text), "([^\n]+)") do
		line = string.gsub(line, "\r$", "")
		if string.len(line) > maxLength then
			line = string.sub(line, 1, maxLength) .. "..."
		end
		lines[#lines + 1] = line
		if #lines >= maxLines then
			break
		end
	end
	return lines
end

function ParityTrace.Start(options)
	options = options or {}
	local config = options.config or {}
	local enabled = _ConfigBool(config, "enabled", "HELLO_PARITY_TRACE", false)
	if not enabled then
		return nil
	end

	local state = {
		enabled = true,
		sample = tostring(options.sample or "unknown"),
		preset = tostring(options.preset or "default"),
		seed = tonumber(options.seed) or 0,
		elapsedMs = 0,
		frame = 0,
		sampleCount = 0,
		delayMs = math.max(0, _ConfigNumber(config, "delayMs", "HELLO_PARITY_TRACE_DELAY_MS", 1000)),
		intervalMs = math.max(1, _ConfigNumber(config, "intervalMs", "HELLO_PARITY_TRACE_INTERVAL_MS", 500)),
		maxSamples = math.max(1, math.floor(_ConfigNumber(config, "maxSamples", "HELLO_PARITY_TRACE_MAX_SAMPLES", 16))),
		maxAgents = math.max(1, math.floor(_ConfigNumber(config, "maxAgents", "HELLO_PARITY_TRACE_MAX_AGENTS", 8))),
		includeAiSummary = _ConfigBool(config, "includeAiSummary", "HELLO_PARITY_TRACE_AI_SUMMARY", false),
		aiSummaryMaxLines = math.max(1, math.floor(_ConfigNumber(config, "aiSummaryMaxLines", "HELLO_PARITY_TRACE_AI_SUMMARY_MAX_LINES", 8))),
		aiSummaryMaxLineLength = math.max(1, math.floor(_ConfigNumber(config, "aiSummaryMaxLineLength", "HELLO_PARITY_TRACE_AI_SUMMARY_MAX_LINE_LENGTH", 220))),
		nextSampleMs = math.max(0, _ConfigNumber(config, "delayMs", "HELLO_PARITY_TRACE_DELAY_MS", 1000)),
		outputPath = _ConfigString(config, "outputPath", "HELLO_PARITY_TRACE_FILE", nil),
		profile = options.profile,
	}
	state.logToConsole = _ConfigBool(config, "logToConsole", "HELLO_PARITY_TRACE_LOG", state.outputPath == nil)
	if state.outputPath ~= nil and io ~= nil and io.open ~= nil then
		local file = io.open(state.outputPath, "w")
		if file ~= nil then
			file:close()
		end
	end
	ParityTrace.Emit(state, "start", {
		maxAgents = state.maxAgents,
		maxSamples = state.maxSamples,
		intervalMs = state.intervalMs,
		delayMs = state.delayMs,
		profile = state.profile,
	})
	return state
end

function ParityTrace.Emit(state, recordType, payload)
	if state == nil then
		return
	end
	local record = payload or {}
	record.type = recordType
	record.sample = state.sample
	record.preset = state.preset
	record.seed = state.seed
	record.tMs = _Round(state.elapsedMs, 1)
	record.frame = state.frame
	local line = "[ParityTrace] " .. _EncodeJson(record)
	if state.outputPath ~= nil and io ~= nil and io.open ~= nil then
		local file = io.open(state.outputPath, "a")
		if file ~= nil then
			file:write(line .. "\n")
			file:flush()
			file:close()
			if not state.logToConsole then
				return
			end
		end
	end
	if state.logToConsole then
		print(line)
	end
end

function ParityTrace.Tick(state, deltaMs, snapshotFn)
	if state == nil or not state.enabled then
		return
	end
	state.elapsedMs = state.elapsedMs + math.max(0, tonumber(deltaMs) or 0)
	state.frame = state.frame + 1
	if state.sampleCount >= state.maxSamples or state.elapsedMs < state.nextSampleMs then
		return
	end

	local ok, payload = true, {}
	if snapshotFn ~= nil then
		ok, payload = pcall(function()
			return snapshotFn(state)
		end)
	end
	if not ok then
		ParityTrace.Emit(state, "error", {
			message = tostring(payload),
			nextSampleIndex = state.sampleCount + 1,
			nextSampleMs = state.nextSampleMs,
		})
		state.enabled = false
		return
	end
	payload = payload or {}
	state.sampleCount = state.sampleCount + 1
	payload.sampleIndex = state.sampleCount
	ParityTrace.Emit(state, "sample", payload)
	state.nextSampleMs = state.nextSampleMs + state.intervalMs

	if state.sampleCount >= state.maxSamples then
		ParityTrace.Emit(state, "finish", { samples = state.sampleCount })
	end
end

return ParityTrace
