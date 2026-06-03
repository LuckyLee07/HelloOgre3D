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

local chapter6To8AiConfig = cloneTable(SamplePresets.default)
chapter6To8AiConfig.aiBlackboard = {
	floats = {
		["perception.visionRange"] = 40.0,
		["perception.fieldOfViewDegrees"] = 360.0,
	},
	ints = {
		["perception.visionIntervalMs"] = 200,
	},
	bools = {
		["perception.requirePath"] = false,
		["perception.staggerScans"] = true,
	},
}
chapter6To8AiConfig.aiScheduler = {
	enabled = true,
	tickMs = 50,
	maxPerFrame = 8,
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
		enabled = true,
		tickMs = 100,
		maxPerFrame = 1,
	},
	diagnostics = {
		maxObjects = 8,
		maxResources = 6,
		maxEvents = 8,
	},
}

SamplePresets.ai_lastknown_demo = SamplePresets.chapter8_perception

SamplePresets.Sandbox6 = cloneTable(chapter6To8AiConfig)
SamplePresets.Sandbox7 = cloneTable(chapter6To8AiConfig)
SamplePresets.Sandbox8 = cloneTable(chapter6To8AiConfig)
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
SamplePresets.team_blackboard.agentCount = 6
SamplePresets.team_blackboard.lightTeamCount = 3
SamplePresets.team_blackboard.spawnMode = "fixed"
SamplePresets.team_blackboard.aiScheduler.enabled = true
SamplePresets.team_blackboard.aiScheduler.tickMs = 80
SamplePresets.team_blackboard.aiScheduler.maxPerFrame = 3
SamplePresets.team_blackboard.spawnPoints = {
	{ -14.442, 0.080, 21.076 },
	{ 19.732, 0.050, -7.443 },
	{ 20.737, 0.050, 39.859 },
	{ 28.901, 0.068, 66.047 },
	{ -8.847, 0.050, -9.129 },
	{ 30.546, 0.050, 15.425 },
}
SamplePresets.team_blackboard.aiBlackboard.strings["debug.demo"] = "team_blackboard"
SamplePresets.team_blackboard.chapter8Comms.showDemoPanel = true
SamplePresets.team_blackboard.chapter8Comms.drawAgentMarkers = true
SamplePresets.team_blackboard.chapter8Comms.drawTeamBroadcasts = true
SamplePresets.team_blackboard.chapter8Comms.drawTeamMemories = true
SamplePresets.team_blackboard.chapter8Comms.useOriginalRandomSpawn = false
SamplePresets.team_blackboard.chapter8Comms.supportOffsetDistance = 4.0
SamplePresets.team_blackboard.chapter8Comms.supportResponseTtlMs = 3000
SamplePresets.team_blackboard.chapter8Comms.sightScanIntervalMs = 120
SamplePresets.team_blackboard.chapter8Comms.sightPairsPerTick = 10
SamplePresets.team_blackboard.chapter8Comms.teamApplyIntervalMs = 160
SamplePresets.team_blackboard.chapter8Comms.teamAgentsPerTick = 3
SamplePresets.team_blackboard.chapter8Comms.teamPruneIntervalMs = 250
SamplePresets.team_blackboard.chapter8Comms.scriptedOpeningSighting = {
	enabled = true,
	delayMs = 800,
	spotterIndex = 1,
	targetIndex = 2,
}

SamplePresets.Sandbox12 = SamplePresets.team_blackboard

SamplePresets.influence_map = cloneTable(SamplePresets.team_blackboard)
SamplePresets.influence_map.aiBlackboard.strings["debug.demo"] = "influence_map"
SamplePresets.influence_map.aiScheduler.enabled = true
SamplePresets.influence_map.aiScheduler.tickMs = 80
SamplePresets.influence_map.aiScheduler.maxPerFrame = 3
SamplePresets.influence_map.chapter8Comms.showDemoPanel = true
SamplePresets.influence_map.chapter8Comms.drawTeamMemories = true
SamplePresets.influence_map.chapter8Comms.supportOffsetDistance = 8.0
SamplePresets.influence_map.chapter8Comms.influenceMap = {
	enabled = true,
	draw = true,
	updateIntervalMs = 500,
	minX = -30.0,
	maxX = 50.0,
	minZ = -15.0,
	maxZ = 60.0,
	cellSize = 5.0,
	y = 0.12,
	dangerStrength = 1.0,
	dangerRadius = 13.0,
	dangerWeight = 2.8,
	supportStrength = 1.0,
	supportRadius = 5.0,
	supportWeight = 1.2,
	supportOffsetDistance = 8.0,
	supportSamples = 12,
	candidateCount = 16,
	candidateRadii = { 6.0, 8.0, 10.0, 12.0 },
	drawThreshold = 0.08,
}

SamplePresets.Sandbox13 = SamplePresets.influence_map

SamplePresets.hearing_danger = cloneTable(SamplePresets.team_blackboard)
SamplePresets.hearing_danger.aiBlackboard.strings["debug.demo"] = "hearing_danger"
SamplePresets.hearing_danger.aiScheduler.enabled = true
SamplePresets.hearing_danger.aiScheduler.tickMs = 80
SamplePresets.hearing_danger.aiScheduler.maxPerFrame = 3
SamplePresets.hearing_danger.chapter8Comms.sightScanEnabled = false
SamplePresets.hearing_danger.chapter8Comms.teamSupportEnabled = false
SamplePresets.hearing_danger.chapter8Comms.drawDirectSightLines = false
SamplePresets.hearing_danger.chapter8Comms.drawTeamBroadcasts = false
SamplePresets.hearing_danger.chapter8Comms.drawTeamMemories = false
SamplePresets.hearing_danger.chapter8Comms.hearingDanger = {
	enabled = true,
	draw = true,
	scanIntervalMs = 100,
	agentsPerTick = 3,
	eventTtlMs = 2200,
	hearingRadius = 46.0,
	dangerRadius = 14.0,
	investigateStopDistance = 5.0,
	escapeDistance = 9.0,
	responseCooldownMs = 450,
	dangerCooldownMs = 350,
	scriptedGunshot = {
		enabled = true,
		delayMs = 900,
		repeatIntervalMs = 1800,
		shooterIndex = 2,
		targetIndex = 1,
	},
}

SamplePresets.Sandbox14 = SamplePresets.hearing_danger

SamplePresets.formation_tactics = cloneTable(SamplePresets.team_blackboard)
SamplePresets.formation_tactics.aiBlackboard.strings["debug.demo"] = "formation_tactics"
SamplePresets.formation_tactics.aiScheduler.enabled = true
SamplePresets.formation_tactics.aiScheduler.tickMs = 80
SamplePresets.formation_tactics.aiScheduler.maxPerFrame = 3
SamplePresets.formation_tactics.chapter8Comms.sightScanEnabled = false
SamplePresets.formation_tactics.chapter8Comms.teamSupportEnabled = false
SamplePresets.formation_tactics.chapter8Comms.drawTeamBroadcasts = true
SamplePresets.formation_tactics.chapter8Comms.drawTeamMemories = true
SamplePresets.formation_tactics.chapter8Comms.scriptedOpeningSighting = {
	enabled = true,
	delayMs = 700,
	spotterIndex = 1,
	targetIndex = 2,
}
SamplePresets.formation_tactics.chapter8Comms.teamMemoryTtlMs = 6000
SamplePresets.formation_tactics.chapter8Comms.chapter9Formation = {
	enabled = true,
	draw = true,
	teamId = 1,
	updateIntervalMs = 180,
	slotSpacing = 5.0,
	readyDistance = 1.8,
	minReadyCount = 2,
	waitTimeoutMs = 1600,
	callForBackup = true,
	focusPosition = { 19.732, 0.05, -7.443 },
}

SamplePresets.Sandbox15 = SamplePresets.formation_tactics

SamplePresets.chapter9_tactics_lua = cloneTable(SamplePresets.default)
SamplePresets.chapter9_tactics_lua.seed = 20260602
SamplePresets.chapter9_tactics_lua.agentCount = 6
SamplePresets.chapter9_tactics_lua.lightTeamCount = 3
SamplePresets.chapter9_tactics_lua.spawnMode = "random"
SamplePresets.chapter9_tactics_lua.targetRadius = 1
SamplePresets.chapter9_tactics_lua.spawnPoints = {
	{ -18, 0, 10 },
	{ -12, 0, 24 },
	{ -3, 0, 36 },
	{ 22, 0, 10 },
	{ 30, 0, 24 },
	{ 38, 0, 36 },
}
SamplePresets.chapter9_tactics_lua.aiScheduler.enabled = false
SamplePresets.chapter9_tactics_lua.aiBlackboard = {
	floats = {
		["perception.visionRange"] = 60.0,
		["perception.fieldOfViewDegrees"] = 360.0,
	},
	ints = {
		["perception.visionIntervalMs"] = 100,
	},
	bools = {
		["perception.requirePath"] = true,
		["perception.staggerScans"] = true,
	},
	strings = {
		["debug.demo"] = "chapter9_tactics",
	},
}
SamplePresets.chapter9_tactics_lua.chapter9Tactics = {
	agentScript = "res/scripts/agent/DecisionSoldierAgent.lua",
	alternateTeams = true,
	minSpawnDistanceSq = 725.0,
	maxSpawnAttempts = 4096,
	suppressPathDraw = true,
	showTacticsPanel = false,
	drawAgentMarkers = false,
	drawEventMarkers = false,
	useCppEventSource = true,
	scriptedEvents = false,
	scriptedEventsInSmoke = true,
	scriptedEventIntervalMs = 1800,
	eventTtlMs = 1800,
	deadFriendlyDelayMs = 2600,
	dangerUpdateIntervalMs = 500,
	teamUpdateIntervalMs = 500,
	dangerPerspectiveTeamId = 0,
	teamPositiveTeamId = 0,
	dangerStrength = -1.0,
	teamStrength = 1.0,
	bulletShotRadius = 10.0,
	bulletImpactRadius = 8.0,
	deadFriendlyRadius = 12.0,
	enemySightingRadius = 14.0,
	teamInfluenceRadius = 11.0,
	drawThreshold = 0.08,
	drawInSmoke = false,
	maxDrawCellsPerLayer = 0,
	drawDangerLayer = false,
	drawTeamLayer = true,
	drawNeutralCells = true,
	projectInfluenceToNav = true,
	drawNavProjectionMaxDistance = 1.8,
	influenceMap = {
		minX = -32.0,
		maxX = 56.0,
		minZ = -8.0,
		maxZ = 62.0,
		cellSize = 2.0,
		y = 0.12,
	},
}
SamplePresets.chapter9_tactics_lua.diagnostics = {
	maxObjects = 12,
	maxResources = 6,
	maxEvents = 8,
}

SamplePresets.Sandbox17 = SamplePresets.chapter9_tactics_lua

SamplePresets.chapter9_tactics_cpp = cloneTable(SamplePresets.chapter9_tactics_lua)
SamplePresets.chapter9_tactics_cpp.seed = 20260603
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.cppInfluenceMap = true
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.objectiveStrength = 1.0
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.objectiveRadius = 16.0
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.objectiveWeight = 1.0
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.dangerWeight = 1.0
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.teamWeight = 1.0
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.influenceFalloff = 0.2
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.influenceInertia = 0.5
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.influenceSpreadPasses = 2
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.objectiveEnabled = true
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.tacticalQueryEnabled = true
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.drawObjectiveLayer = true
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.drawBestPosition = true
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.tacticalQueryType = "support"
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.tacticalQueryRadius = 24.0
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.tacticalQueryStep = 4.0
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.tacticalDriverTeamId = 0
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.tacticalAgentEnabled = true
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.tacticalAgentMaxSpeed = 2.4
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.tacticalAgentRepathDistance = 1.5
SamplePresets.Sandbox18 = SamplePresets.chapter9_tactics_cpp

SamplePresets.ai_perception_pressure = cloneTable(SamplePresets.default)
SamplePresets.ai_perception_pressure.seed = 20260601
SamplePresets.ai_perception_pressure.agentCount = 120
SamplePresets.ai_perception_pressure.lightTeamCount = 60
SamplePresets.ai_perception_pressure.spawnMode = "grid"
SamplePresets.ai_perception_pressure.aiScheduler.enabled = false
SamplePresets.ai_perception_pressure.aiBlackboard = {
	floats = {
		["perception.visionRange"] = 18.0,
		["perception.fieldOfViewDegrees"] = 360.0,
	},
	ints = {
		["perception.visionIntervalMs"] = 1,
	},
	bools = {
		["perception.requirePath"] = false,
	},
	strings = {
		["debug.demo"] = "ai_perception_pressure",
	},
}
SamplePresets.ai_perception_pressure.perceptionPressure = {
	defaultAgentCount = 120,
	spacing = 6.0,
	planeSize = 260,
	runtimeSummaryFrame = 30,
}
SamplePresets.ai_perception_pressure.diagnostics = {
	maxObjects = 0,
	maxResources = 6,
	maxEvents = 0,
}

SamplePresets.Sandbox16 = SamplePresets.ai_perception_pressure

local function makeAiPerfPreset(agentCount)
	local preset = cloneTable(SamplePresets.ai_perception_pressure)
	preset.seed = 20260601
	preset.agentCount = agentCount
	preset.lightTeamCount = math.floor(agentCount * 0.5)
	preset.spawnMode = "grid"
	preset.aiScheduler.enabled = false
	preset.perceptionPressure.defaultAgentCount = agentCount
	preset.perceptionPressure.profileName = "ai_perf_" .. tostring(agentCount)
	preset.perceptionPressure.runtimeSummaryFrame = 30
	return preset
end

SamplePresets.ai_perf_100 = makeAiPerfPreset(100)
SamplePresets.ai_perf_500 = makeAiPerfPreset(500)
SamplePresets.ai_perf_1000 = makeAiPerfPreset(1000)

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
