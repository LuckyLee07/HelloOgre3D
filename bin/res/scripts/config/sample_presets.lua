local SamplePresets = {}

local fixedSpawnPoints = {
	{ -5, 0, 13 },
	{ 5, 0, 4 },
	{ 18.5, 0, 4 },
	{ 18.5, 0, 17 },
	{ 15, 0, 21 },
	{ 19, 0, 25 },
	{ 20, 0, 52 },
	{ 24, 4, 36 },
	{ 7, 4, 27 },
	{ -9, 4, 27 },
	{ -26, 0, 25 },
	{ -26, 0, 19 },
	{ 39, 3, 9 },
	{ 39, 3, 17 },
	{ 43, 3, 9 },
	{ 48, 0, 24 },
	{ -13, -0.5, 15 },
	{ -21, -0.5, 19 },
	{ 24, 4, 51 },
	{ 48, 2, 16 },
}

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

SamplePresets.default = {
	seed = 20260524,
	agentCount = 7,
	lightTeamCount = 3,
	spawnMode = "fixed",
	targetRadius = 1,
	spawnPoints = fixedSpawnPoints,
	aiScheduler = {
		enabled = false,
		tickMs = 50,
		maxPerFrame = 8,
	},
	diagnostics = {
		maxObjects = 8,
		maxResources = 6,
		maxEvents = 6,
	},
}

SamplePresets.ai_perf_smoke = {
	seed = 20260524,
	agentCount = 20,
	lightTeamCount = 10,
	spawnMode = "fixed",
	targetRadius = 1,
	spawnPoints = fixedSpawnPoints,
	aiScheduler = {
		enabled = true,
		tickMs = 50,
		maxPerFrame = 8,
	},
	diagnostics = {
		maxObjects = 24,
		maxResources = 6,
		maxEvents = 8,
	},
}

local chapter8PerceptionDemoConfig = {
	enabled = true,
	targetIndex = 2,
	relocateAfterMs = 2200,
	relocateTo = { 48, 0, 24 },
	freezeTarget = true,
}

SamplePresets.chapter8_perception = {
	seed = 20260530,
	agentCount = 2,
	lightTeamCount = 1,
	spawnMode = "fixed",
	targetRadius = 1,
	spawnPoints = {
		{ -5, 0, 13 },
		{ 5, 0, 13 },
	},
	aiBlackboard = {
		floats = {
			["perception.visionRange"] = 12.0,
			["perception.fieldOfViewDegrees"] = 120.0,
		},
		ints = {
			["perception.visionIntervalMs"] = 100,
		},
		bools = {
			["perception.requirePath"] = true,
		},
		strings = {
			["debug.demo"] = "chapter8_perception",
		},
	},
	behaviorTree = {
		debugTrace = true,
		debugTracePrint = false,
	},
	perceptionDemo = chapter8PerceptionDemoConfig,
	lastKnownDemo = chapter8PerceptionDemoConfig,
	aiScheduler = {
		enabled = false,
		tickMs = 50,
		maxPerFrame = 8,
	},
	diagnostics = {
		maxObjects = 8,
		maxResources = 6,
		maxEvents = 8,
	},
}

SamplePresets.ai_lastknown_demo = SamplePresets.chapter8_perception

SamplePresets.Sandbox6 = SamplePresets.default
SamplePresets.Sandbox7 = SamplePresets.default
SamplePresets.Sandbox8 = SamplePresets.default
SamplePresets.Sandbox10 = SamplePresets.chapter8_perception

SamplePresets.chapter8_comms = {
	seed = 20260530,
	agentCount = 6,
	lightTeamCount = 3,
	spawnMode = "random",
	targetRadius = 1,
	spawnPoints = {
		{ -14.442, 0.080, 21.076 },
		{ 19.732, 0.050, -7.443 },
		{ 20.737, 0.050, 39.859 },
		{ 28.901, 0.068, 66.047 },
		{ -8.847, 0.050, -9.129 },
		{ 30.546, 0.050, 15.425 },
	},
	aiBlackboard = {
		floats = {
			["perception.visionRange"] = 0.0,
			["perception.fieldOfViewDegrees"] = 90.0,
		},
		ints = {
			["perception.visionIntervalMs"] = 1,
		},
		bools = {
			["perception.requirePath"] = true,
		},
		strings = {
			["debug.demo"] = "chapter8_comms",
		},
	},
	behaviorTree = {
		debugTrace = true,
		debugTracePrint = false,
	},
	chapter8Comms = {
		enabled = true,
		debugLog = false,
		cameraPosition = { -30, 18, -17 },
		cameraOrientation = { -146, -40, -157 },
		teamMemoryTtlMs = 2500,
		eventTtlMs = 1400,
		broadcastIntervalMs = 500,
		sharedMoveDistance = 2.5,
		sightOriginHeightRatio = 0.5,
		sightTargetHeightRatio = 0.0,
		useOriginalRandomSpawn = true,
		alternateTeams = true,
		minSpawnDistance = 25.0,
		minFallbackSpawnDistance = 14.0,
		minSpawnY = -0.5,
		maxSpawnAttempts = 4096,
		agentMaxSpeed = nil,
		keepAgentsInDemoStage = false,
		showDemoPanel = false,
		drawAgentMarkers = false,
		drawVisionRange = false,
		drawTeamBroadcasts = false,
		drawTeamMemories = false,
		drawDirectSightLines = true,
	},
	aiScheduler = {
		enabled = false,
		tickMs = 50,
		maxPerFrame = 8,
	},
	diagnostics = {
		maxObjects = 10,
		maxResources = 6,
		maxEvents = 10,
	},
}

SamplePresets.Sandbox11 = SamplePresets.chapter8_comms

SamplePresets.team_blackboard = cloneTable(SamplePresets.chapter8_comms)
SamplePresets.team_blackboard.aiBlackboard.strings["debug.demo"] = "team_blackboard"
SamplePresets.team_blackboard.chapter8Comms.showDemoPanel = true
SamplePresets.team_blackboard.chapter8Comms.drawAgentMarkers = true
SamplePresets.team_blackboard.chapter8Comms.drawTeamBroadcasts = true
SamplePresets.team_blackboard.chapter8Comms.drawTeamMemories = true

SamplePresets.Sandbox12 = SamplePresets.team_blackboard

SamplePresets.Sandbox9 = {
	seed = 20260530,
	agentCount = 3,
	lightTeamCount = 3,
	spawnMode = "random",
	targetRadius = 1,
	spawnPoints = fixedSpawnPoints,
	aiScheduler = {
		enabled = false,
		tickMs = 50,
		maxPerFrame = 8,
	},
	diagnostics = {
		maxObjects = 8,
		maxResources = 6,
		maxEvents = 6,
	},
}

return SamplePresets
