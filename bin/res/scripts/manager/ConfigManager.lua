local ConfigManager = {}

local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

local function getAiSchedulerService()
	if SandboxAIScheduler ~= nil and SandboxAIScheduler.configureAiScheduler ~= nil then
		return SandboxAIScheduler
	end
	return nil
end

local SamplePresets = require("res.scripts.config.sample_presets")

local function getEnvValue(name)
	return os.getenv and os.getenv(name) or nil
end

local function isTruthy(value)
	return value == true or value == "1" or value == "true" or value == "TRUE" or value == "True" or value == "yes" or value == "YES"
end

local function getEnvNumber(name, defaultValue)
	local value = tonumber(getEnvValue(name))
	if value == nil then
		return defaultValue
	end
	return value
end

local function cloneTable(value)
	if type(value) ~= "table" then
		return value
	end
	local result = {}
	for k, v in pairs(value) do
		result[k] = cloneTable(v)
	end
	return result
end

local function mergeTable(base, override)
	local result = cloneTable(base or {})
	for k, v in pairs(override or {}) do
		if type(v) == "table" and type(result[k]) == "table" then
			result[k] = mergeTable(result[k], v)
		else
			result[k] = cloneTable(v)
		end
	end
	return result
end

local function toVector3(point)
	if point == nil then
		return nil
	end
	return Vector3(point[1] or 0, point[2] or 0, point[3] or 0)
end

function ConfigManager:GetSelectedPresetName()
	local presetName = getEnvValue("HELLO_SAMPLE_PRESET")
	if presetName == nil or presetName == "" then
		return _G.HELLO_SANDBOX_SAMPLE_NAME or "default"
	end
	return presetName
end

function ConfigManager:GetSamplePreset(sampleName)
	local selectedName = self:GetSelectedPresetName()
	local base = SamplePresets.default or {}
	local samplePreset = SamplePresets[sampleName or ""] or {}
	local selectedPreset = SamplePresets[selectedName] or {}
	local preset = mergeTable(mergeTable(base, samplePreset), selectedPreset)
	preset.name = selectedName
	preset.sampleName = sampleName or _G.HELLO_SANDBOX_SAMPLE_NAME or "default"

	preset.seed = getEnvNumber("HELLO_SAMPLE_SEED", preset.seed or 20260524)

	local agentCountEnv = getEnvValue("HELLO_SAMPLE_AGENT_COUNT")
	local lightCountEnv = getEnvValue("HELLO_SAMPLE_LIGHT_COUNT")
	preset.agentCount = getEnvNumber("HELLO_SAMPLE_AGENT_COUNT", preset.agentCount or 7)
	if lightCountEnv ~= nil and lightCountEnv ~= "" then
		preset.lightTeamCount = getEnvNumber("HELLO_SAMPLE_LIGHT_COUNT", preset.lightTeamCount or math.floor((preset.agentCount or 7) / 2))
	elseif agentCountEnv ~= nil and agentCountEnv ~= "" then
		preset.lightTeamCount = math.floor((preset.agentCount or 7) / 2)
	else
		preset.lightTeamCount = preset.lightTeamCount or math.floor((preset.agentCount or 7) / 2)
	end
	preset.spawnMode = getEnvValue("HELLO_SAMPLE_SPAWN_MODE") or preset.spawnMode or "fixed"

	preset.aiScheduler = preset.aiScheduler or {}
	local schedulerEnv = getEnvValue("HELLO_SAMPLE_AI_SCHEDULER")
	if schedulerEnv ~= nil and schedulerEnv ~= "" then
		preset.aiScheduler.enabled = isTruthy(schedulerEnv)
	end
	preset.aiScheduler.tickMs = getEnvNumber("HELLO_SAMPLE_AI_TICK_MS", preset.aiScheduler.tickMs or 50)
	preset.aiScheduler.maxPerFrame = getEnvNumber("HELLO_SAMPLE_AI_MAX_PER_FRAME", preset.aiScheduler.maxPerFrame or 8)
	return preset
end

function ConfigManager:ApplyStartupSeed(sampleName)
	local preset = self:GetSamplePreset(sampleName)
	local seed = math.floor(tonumber(preset.seed) or 0)
	math.randomseed(seed)
	_G.HELLO_SAMPLE_SEED = seed
	print("[ConfigManager] preset=", tostring(preset.name), "sample=", tostring(preset.sampleName), "seed=", seed, "agents=", preset.agentCount, "spawnMode=", tostring(preset.spawnMode), "deterministic=", true)
	return preset
end

function ConfigManager:ConfigureAiScheduler(objectManager, sampleName)
	local schedulerService = getAiSchedulerService()
	if schedulerService == nil then
		return false
	end

	local preset = self:GetSamplePreset(sampleName)
	local scheduler = preset.aiScheduler or {}
	local enabled = scheduler.enabled == true
	if getEnvValue("HELLO_AI_SCHEDULER_ENABLE") ~= nil then
		enabled = isTruthy(getEnvValue("HELLO_AI_SCHEDULER_ENABLE"))
	end
	local tickMs = getEnvNumber("HELLO_AI_SCHEDULER_TICK_MS", scheduler.tickMs or 50)
	local maxPerFrame = getEnvNumber("HELLO_AI_SCHEDULER_MAX_PER_FRAME", scheduler.maxPerFrame or 8)
	schedulerService:configureAiScheduler(enabled, tickMs, maxPerFrame)
	print("[AIScheduler] configured", "enabled=", tostring(enabled), "tickMs=", tickMs, "maxPerFrame=", maxPerFrame, "preset=", tostring(preset.name))
	return enabled
end

function ConfigManager:GetAgentCount(sampleName, defaultValue)
	local preset = self:GetSamplePreset(sampleName)
	return math.max(0, tonumber(preset.agentCount) or defaultValue or 0)
end

function ConfigManager:GetDiagnosticLimit(sampleName, key, defaultValue)
	local preset = self:GetSamplePreset(sampleName)
	local diagnostics = preset.diagnostics or {}
	return tonumber(diagnostics[key]) or defaultValue
end

function ConfigManager:IsSelectedPreset(presetName)
	return self:GetSelectedPresetName() == presetName
end

function ConfigManager:ApplyAiBlackboardDefaults(blackboard, sampleName)
	if blackboard == nil then
		return false
	end

	local preset = self:GetSamplePreset(sampleName)
	local config = preset.aiBlackboard or {}
	for key, value in pairs(config.floats or {}) do
		blackboard:SetFloat(tostring(key), tonumber(value) or 0.0)
	end
	for key, value in pairs(config.ints or {}) do
		blackboard:SetInt(tostring(key), tonumber(value) or 0)
	end
	for key, value in pairs(config.bools or {}) do
		blackboard:SetBool(tostring(key), isTruthy(value))
	end
	for key, value in pairs(config.strings or {}) do
		blackboard:SetString(tostring(key), tostring(value))
	end
	return next(config) ~= nil
end

function ConfigManager:ConfigureBehaviorTreeDriver(driver, sampleName)
	if driver == nil then
		return false
	end

	local preset = self:GetSamplePreset(sampleName)
	local config = preset.behaviorTree or {}
	if config.debugTrace ~= nil and driver.SetDebugTraceEnabled ~= nil then
		driver:SetDebugTraceEnabled(isTruthy(config.debugTrace))
	end
	if config.debugTracePrint ~= nil and driver.SetDebugTracePrintEnabled ~= nil then
		driver:SetDebugTracePrintEnabled(isTruthy(config.debugTracePrint))
	end
	return next(config) ~= nil
end

function ConfigManager:GetAgentTeamId(sampleName, index)
	local preset = self:GetSamplePreset(sampleName)
	local lightCount = tonumber(preset.lightTeamCount) or 0
	return index > lightCount and 0 or 1
end

function ConfigManager:GetAgentAppearance(sampleName, index, appearanceTypes)
	local teamId = self:GetAgentTeamId(sampleName, index)
	if teamId == 0 then
		return appearanceTypes.DARK
	end
	return appearanceTypes.LIGHT
end

function ConfigManager:PlaceAgentOnPresetSpawn(agent, sampleName, index, navMeshName)
	if agent == nil then
		return nil
	end
	local preset = self:GetSamplePreset(sampleName)
	local spawnPoint = nil
	if preset.spawnMode == "fixed" and preset.spawnPoints ~= nil and #preset.spawnPoints > 0 then
		local spawnIndex = ((index - 1) % #preset.spawnPoints) + 1
		spawnPoint = toVector3(preset.spawnPoints[spawnIndex])
	end
	if spawnPoint == nil then
		spawnPoint = SandboxNav:RandomPoint(navMeshName or "default")
	else
		spawnPoint = SandboxNav:FindClosestPoint(navMeshName or "default", spawnPoint)
	end

	local height = AgentComponents.GetHeight(agent, 1.6)
	spawnPoint.y = spawnPoint.y + height * 0.5
	agent:setPosition(spawnPoint)

	local navPosition = SandboxNav:FindClosestPoint(navMeshName or "default", agent:GetPosition())
	AgentComponents.SetTarget(agent, navPosition)
	AgentComponents.SetTargetRadius(agent, preset.targetRadius or 1)
	return spawnPoint
end

function ConfigManager:BuildDebugSummary(sampleName)
	local preset = self:GetSamplePreset(sampleName)
	local scheduler = preset.aiScheduler or {}
	return string.format("[ConfigManager] preset=%s sample=%s seed=%d agents=%d light=%d spawnMode=%s aiScheduler=%s tickMs=%d maxPerFrame=%d",
		tostring(preset.name),
		tostring(preset.sampleName),
		tonumber(preset.seed) or 0,
		tonumber(preset.agentCount) or 0,
		tonumber(preset.lightTeamCount) or 0,
		tostring(preset.spawnMode),
		tostring(scheduler.enabled == true),
		tonumber(scheduler.tickMs) or 0,
		tonumber(scheduler.maxPerFrame) or 0)
end

_G.ConfigManager = ConfigManager
return ConfigManager
