local VisualTraceGate = {}
local activeState = nil

local function getEnvValue(name)
	return os.getenv and os.getenv(name) or nil
end

local function getEnvNumber(name, defaultValue)
	local value = tonumber(getEnvValue(name))
	if value == nil then
		return defaultValue
	end
	return value
end

local function splitLines(text)
	local lines = {}
	if text == nil then
		return lines
	end
	for line in string.gmatch(tostring(text), "[^\r\n]+") do
		table.insert(lines, line)
	end
	return lines
end

local function parseField(line, key)
	return string.match(line, key .. "=([^%s]+)")
end

local function parseNumber(text)
	if text == nil then
		return nil
	end
	local value = string.match(text, "^%-?%d+%.?%d*")
	return tonumber(value)
end

local function parseVec3(text)
	if text == nil then
		return nil
	end
	local x, y, z = string.match(text, "^([^,]+),([^,]+),([^,]+)$")
	x = tonumber(x)
	y = tonumber(y)
	z = tonumber(z)
	if x == nil or y == nil or z == nil then
		return nil
	end
	return { x = x, y = y, z = z }
end

local function horizontalDistance(a, b)
	if a == nil or b == nil then
		return 0
	end
	local dx = (a.x or 0) - (b.x or 0)
	local dz = (a.z or 0) - (b.z or 0)
	return math.sqrt(dx * dx + dz * dz)
end

local function horizontalSpeed(v)
	if v == nil then
		return 0
	end
	return math.sqrt((v.x or 0) * (v.x or 0) + (v.z or 0) * (v.z or 0))
end

local function formatNumber(value)
	return string.format("%.2f", tonumber(value) or 0)
end

local function parseAgentLine(line)
	if line == nil or string.sub(line, 1, 1) ~= "#" then
		return nil
	end

	local id = tonumber(parseField(line, "id"))
	if id == nil then
		return nil
	end

	local pos = parseVec3(parseField(line, "pos"))
	local vel = parseVec3(parseField(line, "vel"))
	local hp = parseNumber(parseField(line, "hp"))
	local speed = parseNumber(parseField(line, "speed"))
	local team = tonumber(parseField(line, "team"))

	return {
		id = id,
		team = team or 0,
		hp = hp or 0,
		pos = pos,
		vel = vel,
		speed = speed or 0,
		line = line,
	}
end

local function addFailure(state, reason)
	if reason == nil or reason == "" then
		return
	end
	if state.failureSet[reason] then
		return
	end
	state.failureSet[reason] = true
	table.insert(state.failures, reason)
	print("[VisualTraceGate] fail", reason)
end

local function inspectAgent(state, agent, elapsed)
	if agent.pos == nil then
		addFailure(state, "agent#" .. tostring(agent.id) .. " invalid position")
		return
	end
	if agent.vel == nil then
		addFailure(state, "agent#" .. tostring(agent.id) .. " invalid velocity")
		return
	end

	local absMax = math.max(math.abs(agent.pos.x), math.abs(agent.pos.y), math.abs(agent.pos.z))
	if absMax > state.config.maxAbsPosition then
		addFailure(state, "agent#" .. tostring(agent.id) .. " position out of bounds " .. formatNumber(absMax))
	end

	local hSpeed = math.max(horizontalSpeed(agent.vel), agent.speed or 0)
	if hSpeed > state.metrics.maxHorizontalSpeed then
		state.metrics.maxHorizontalSpeed = hSpeed
	end
	if hSpeed > state.config.maxHorizontalSpeed then
		addFailure(state, "agent#" .. tostring(agent.id) .. " horizontal speed " .. formatNumber(hSpeed))
	end

	local previous = state.previous[agent.id]
	if previous ~= nil and previous.pos ~= nil then
		local stepDistance = horizontalDistance(agent.pos, previous.pos)
		if stepDistance > state.metrics.maxStepDistance then
			state.metrics.maxStepDistance = stepDistance
		end
		if stepDistance > state.config.maxStepDistance then
			addFailure(state, "agent#" .. tostring(agent.id) .. " step distance " .. formatNumber(stepDistance))
		end
		if agent.hp < previous.hp then
			state.metrics.healthDrops = state.metrics.healthDrops + 1
			if stepDistance > state.metrics.maxHitStepDistance then
				state.metrics.maxHitStepDistance = stepDistance
			end
			if stepDistance > state.config.maxHitStepDistance then
				addFailure(state, "agent#" .. tostring(agent.id) .. " hit step distance " .. formatNumber(stepDistance))
			end
		end
	end

	state.previous[agent.id] = {
		hp = agent.hp,
		pos = agent.pos,
		elapsed = elapsed,
	}
end

local function sample(state, elapsed)
	if ObjectManager == nil or ObjectManager.buildAiDebugSummary == nil then
		addFailure(state, "ObjectManager.buildAiDebugSummary unavailable")
		return
	end

	local summary = ObjectManager:buildAiDebugSummary(state.config.maxAgents)
	local lines = splitLines(summary)
	local agentCount = 0
	print("[VisualTrace] sample=" .. tostring(state.metrics.samples + 1) .. " t=" .. formatNumber(elapsed))
	for _, line in ipairs(lines) do
		if string.find(line, "AI agents=", 1, true) == 1 then
			print("[VisualTrace] t=" .. formatNumber(elapsed) .. " " .. line)
		elseif string.sub(line, 1, 1) == "#" then
			print("[VisualTrace] t=" .. formatNumber(elapsed) .. " " .. line)
			local lower = string.lower(line)
			if string.find(lower, "nan", 1, true) ~= nil or string.find(lower, "inf", 1, true) ~= nil then
				addFailure(state, "non-finite trace line: " .. line)
			end
			local agent = parseAgentLine(line)
			if agent ~= nil then
				agentCount = agentCount + 1
				inspectAgent(state, agent, elapsed)
			end
		end
	end

	if agentCount <= 0 then
		state.metrics.emptySamples = state.metrics.emptySamples + 1
		print("[VisualTrace] no agents sampled")
	end
	if agentCount > state.metrics.maxAgentsSeen then
		state.metrics.maxAgentsSeen = agentCount
	end
	state.metrics.samples = state.metrics.samples + 1
end

local function finish(state)
	if state.metrics.samples < state.config.minSamples then
		addFailure(state, "samples " .. tostring(state.metrics.samples) .. " < " .. tostring(state.config.minSamples))
	end
	if state.metrics.maxAgentsSeen <= 0 then
		addFailure(state, "no agents sampled")
	end
	if state.metrics.healthDrops < state.config.minHealthDrops then
		addFailure(state, "health drops " .. tostring(state.metrics.healthDrops) .. " < " .. tostring(state.config.minHealthDrops))
	end

	local ok = #state.failures == 0
	print("[VisualTraceGate] result:", ok,
		"samples=", state.metrics.samples,
		"agents=", state.metrics.maxAgentsSeen,
		"maxHorizontalSpeed=", formatNumber(state.metrics.maxHorizontalSpeed),
		"maxStepDistance=", formatNumber(state.metrics.maxStepDistance),
		"healthDrops=", state.metrics.healthDrops,
		"maxHitStepDistance=", formatNumber(state.metrics.maxHitStepDistance))
	return ok
end

function VisualTraceGate.Start()
	local config = {
		delayFrames = math.max(0, math.floor(getEnvNumber("HELLO_VISUAL_TRACE_DELAY_FRAMES", 15))),
		intervalFrames = math.max(1, math.floor(getEnvNumber("HELLO_VISUAL_TRACE_INTERVAL_FRAMES", 15))),
		maxSamples = math.max(1, math.floor(getEnvNumber("HELLO_VISUAL_TRACE_MAX_SAMPLES", 8))),
		maxAgents = math.max(1, math.floor(getEnvNumber("HELLO_VISUAL_TRACE_MAX_AGENTS", 16))),
		minSamples = math.max(1, math.floor(getEnvNumber("HELLO_VISUAL_TRACE_MIN_SAMPLES", 5))),
		minHealthDrops = math.max(0, math.floor(getEnvNumber("HELLO_VISUAL_TRACE_MIN_HEALTH_DROPS", 0))),
		maxHorizontalSpeed = getEnvNumber("HELLO_VISUAL_TRACE_MAX_HORIZONTAL_SPEED", 35.0),
		maxStepDistance = getEnvNumber("HELLO_VISUAL_TRACE_MAX_STEP_DISTANCE", 12.0),
		maxHitStepDistance = getEnvNumber("HELLO_VISUAL_TRACE_MAX_HIT_STEP_DISTANCE", 12.0),
		maxAbsPosition = getEnvNumber("HELLO_VISUAL_TRACE_MAX_ABS_POSITION", 10000.0),
	}

	local state = {
		config = config,
		previous = {},
		failures = {},
		failureSet = {},
		metrics = {
			samples = 0,
			emptySamples = 0,
			frames = 0,
			elapsedMs = 0,
			lastSampleFrame = -1,
			finished = false,
			maxAgentsSeen = 0,
			maxHorizontalSpeed = 0,
			maxStepDistance = 0,
			healthDrops = 0,
			maxHitStepDistance = 0,
		},
	}

	print("[VisualTraceGate] begin",
		"sample=", tostring(_G.HELLO_SANDBOX_SAMPLE_NAME),
		"delayFrames=", config.delayFrames,
		"intervalFrames=", config.intervalFrames,
		"maxSamples=", config.maxSamples,
		"maxSpeed=", formatNumber(config.maxHorizontalSpeed),
		"maxStep=", formatNumber(config.maxStepDistance),
		"maxHitStep=", formatNumber(config.maxHitStepDistance),
		"minHealthDrops=", config.minHealthDrops)

	activeState = state
end

function VisualTraceGate.Update(deltaMs)
	local state = activeState
	if state == nil or state.metrics.finished then
		return
	end

	state.metrics.frames = state.metrics.frames + 1
	state.metrics.elapsedMs = state.metrics.elapsedMs + (tonumber(deltaMs) or 0)

	if state.metrics.frames < state.config.delayFrames then
		return
	end

	local shouldSample = state.metrics.lastSampleFrame < 0
		or (state.metrics.frames - state.metrics.lastSampleFrame) >= state.config.intervalFrames
	if shouldSample then
		sample(state, state.metrics.elapsedMs / 1000.0)
		state.metrics.lastSampleFrame = state.metrics.frames
	end

	if state.metrics.samples >= state.config.maxSamples then
		state.metrics.finished = true
		finish(state)
		activeState = nil
	end
end

_G.VisualTraceGate = VisualTraceGate
return VisualTraceGate
