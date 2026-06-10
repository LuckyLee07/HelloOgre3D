-- parity_trace.lua
-- Structured trace helper for reproducible sample-to-sample comparisons.

local ParityTrace = {}

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

function ParityTrace.AgentSnapshot(agent, index, extra)
	local snapshot = {
		index = index,
		id = _SafeCall(agent, "GetObjId", -1),
		team = _SafeCall(agent, "GetTeamId", -1),
		hp = _Round(_SafeCall(agent, "GetHealth", 0), 10),
		state = tostring(_SafeCall(agent, "GetCurStateName", "")),
		stateId = _SafeCall(agent, "GetCurStateId", -1),
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
	}
	ParityTrace.Emit(state, "start", {
		maxAgents = state.maxAgents,
		maxSamples = state.maxSamples,
		intervalMs = state.intervalMs,
		delayMs = state.delayMs,
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
	print("[ParityTrace] " .. _EncodeJson(record))
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

	local payload = snapshotFn ~= nil and snapshotFn(state) or {}
	state.sampleCount = state.sampleCount + 1
	payload.sampleIndex = state.sampleCount
	ParityTrace.Emit(state, "sample", payload)
	state.nextSampleMs = state.nextSampleMs + state.intervalMs

	if state.sampleCount >= state.maxSamples then
		ParityTrace.Emit(state, "finish", { samples = state.sampleCount })
	end
end

return ParityTrace
