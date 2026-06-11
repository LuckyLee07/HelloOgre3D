local SamplePresets = {}

local Chapter9Profile = require("res.scripts.config.chapter9_tactics_profile")

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
SamplePresets.chapter9_tactics_lua.chapter9Profile = Chapter9Profile.Make("luaTeaching")
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
	drawTargetRadius = true,
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
	-- team 层网格绘制高度偏移（相对 navmesh 表面）。0.0 = 画在 navmesh 表面（对齐 chapter-9：网格在
	-- navmesh 上、比 block 顶面略高 ~0.05、不共面所以不闪）。负值会把网格压到和 block 共面而 z-fighting，
	-- 想真正零间隙贴地需要给材质加 depth_bias（见对话说明），那是比 chapter-9 更进一步的做法。
	teamDrawYOffset = -0.03,
	drawNeutralCells = true,
	-- 贴合地形：把每个格子投影到 navmesh 表面，使网格贴在 block 上（对齐 chapter-9 表现）。
	-- 逐格 navMesh:FindClosestPoint 结果由 C++ 端 m_tacticalInfluenceDrawProjectionCache 缓存
	-- （key=navmesh+网格尺寸+格坐标，整 sample 不变），只在首次重建逐格算一次，之后全走缓存，
	-- 不构成周期性开销。多层地形必须开启，否则平铺的格子会浮在 block 上方。
	projectInfluenceToNav = true,
	drawNavProjectionMaxDistance = 1.8,
	influenceMap = {
		minX = -32.0,
		maxX = 56.0,
		minZ = -8.0,
		maxZ = 62.0,
		cellSize = 2.0,
		cellHeight = 1.0,
		y = 0.12,
	},
}
SamplePresets.chapter9_tactics_lua.diagnostics = {
	maxObjects = 12,
	maxResources = 6,
	maxEvents = 8,
}

SamplePresets.Sandbox17 = SamplePresets.chapter9_tactics_lua

SamplePresets.chapter9_tactics_legacy_parity = cloneTable(SamplePresets.chapter9_tactics_lua)
SamplePresets.chapter9_tactics_legacy_parity.chapter9Profile = Chapter9Profile.Make("legacyParity")
SamplePresets.chapter9_tactics_legacy_parity.seed = 20260608
SamplePresets.chapter9_tactics_legacy_parity.spawnMode = "fixed"
SamplePresets.chapter9_tactics_legacy_parity.spawnPoints = {
	{ 9.370, 0, 24.598 },
	{ 31.151, 0, -7.671 },
	{ -8.233, 0, -5.396 },
	{ 22.090, 0, 62.469 },
	{ -25.620, 0, 19.110 },
	{ 33.025, 0, 37.661 },
}
SamplePresets.chapter9_tactics_legacy_parity.lightTeamCount = 3
SamplePresets.chapter9_tactics_legacy_parity.aiBlackboard.floats["perception.fieldOfViewDegrees"] = 90.0
SamplePresets.chapter9_tactics_legacy_parity.aiBlackboard.ints["perception.visionIntervalMs"] = 1
SamplePresets.chapter9_tactics_legacy_parity.aiBlackboard.bools["perception.staggerScans"] = false
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.agentScript = "res/scripts/agent/Chapter9LegacySoldierAgent.lua"
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.alternateTeams = false
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.useCppEventSource = false
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.useLegacyAgentEvents = true
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.scriptedEvents = false
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.scriptedEventsInSmoke = false
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.scriptedEventIntervalMs = 1500
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.deadFriendlyDelayMs = 60000
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.dangerPerspectiveTeamId = 1
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.teamPositiveTeamId = 1
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.traceRecentEventLimit = 16
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.movementProfile = {
	enabled = true,
	maxSpeed = 3.0,
	maxForce = 1000.0,
	mass = 90.7,
	predictionTime = 0.5,
	followWeight = 1.5,
	stayWeight = 0.4,
	avoidAgentWeight = 1.0,
	avoidObjectWeight = 2.0,
	speedWeight = 7.0,
	accelerationBlend = 0.4,
	forwardBlend = 0.2,
	forceScale = 1.08,
	slowDamping = 0.91,
	minSteeringLengthSq = 0.1,
	minForwardVelocityLengthSq = 0.1,
}
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.teamUseLegacyPointSpread = true
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.teamFalloff = 0.2
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.teamInertia = 0.5
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.teamSpreadPasses = 20
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.showTacticsPanel = false
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.useLegacyInfoPanel = true
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.drawAgentMarkers = false
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.drawEventMarkers = false
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.drawDangerLayer = false
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.drawTeamLayer = true
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.drawInSmoke = true
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.teamDrawYOffset = 0.0
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.projectInfluenceToNav = false
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.maxDrawCellsPerLayer = 0
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.drawNeutralCells = true
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.targetRingRadius = 4.0
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.targetRingMode = "pursuedAgent"
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.influenceMap.cellSize = 2.0
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.influenceMap.cellHeight = 1.0
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.influenceMap.boundaryMinOffset = { 0.18, 0.0, 0.35 }
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.influenceMap.boundaryMaxOffset = { 0.0, 0.0, 0.0 }
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.legacyFirstAgentId = 115
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.legacyForceInitialRandomAgents = {
	[2] = true,
}
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.legacyRandomMovePoints = {
	[1] = {
		{ 27.230, 0.800, 60.021 },
	},
	[2] = {
		{ 33.025, 0.800, 37.661 },
	},
	[4] = {
		{ 31.966, 0.050, 57.765 },
		{ -6.146, 0.050, -6.155 },
	},
}
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.legacyRandomMoveConsumesRandomPoint = {
	[1] = true,
	[4] = true,
}
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.legacyPreferMoveBeforeEnemyAgents = {
	[4] = true,
}
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.legacyPreferMoveBeforeEnemyMaxRandomCount = {
	[4] = 1,
}
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.legacyForceIdleUntilMs = {
	[4] = 6000,
	[5] = 4000,
	[6] = 6000,
}
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.legacyForceIdleAfterRandomCount = {
	[4] = 1,
}
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.legacyForceRandomAfterMs = {
	[4] = 6000,
}
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.legacyForceRandomAtRandomMoveCount = {
	[4] = 1,
}
SamplePresets.chapter9_tactics_legacy_parity.chapter9Tactics.legacyForceEnemyAfterMs = {
	[5] = { timeMs = 4000, enemyIndex = 3 },
	[6] = { timeMs = 6000, enemyIndex = 2 },
}
SamplePresets.chapter9_tactics_legacy_parity.parityTrace = {
	enabled = true,
	delayMs = 1000,
	intervalMs = 500,
	maxSamples = 16,
	maxAgents = 6,
	includeAiSummary = false,
}

SamplePresets.chapter9_tactics_cpp = cloneTable(SamplePresets.chapter9_tactics_lua)
SamplePresets.chapter9_tactics_cpp.chapter9Profile = Chapter9Profile.Make("cppProduction")
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
-- 性能：错峰 objective 层重建，避免 team(500ms)与 objective 在同一帧重建影响图可视。
SamplePresets.chapter9_tactics_cpp.chapter9Tactics.objectiveUpdateIntervalMs = 650
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
