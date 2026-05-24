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

SamplePresets.Sandbox6 = SamplePresets.default
SamplePresets.Sandbox7 = SamplePresets.default
SamplePresets.Sandbox8 = SamplePresets.default

return SamplePresets
