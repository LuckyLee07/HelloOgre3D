-- Run from the repository root: lua5.1 tools/test_config_presets.lua
-- Use real ConfigManager/GetSamplePreset; only adapt the engine's require names.
local script = string.gsub(arg[0], "\\", "/")
local root = string.match(script, "^(.*)tools/[^/]+$") or ""
table.insert(package.loaders, 2, function(name)
	name = string.gsub(name, "%.lua$", "")
	local path = root .. "bin/" .. string.gsub(name, "%.", "/") .. ".lua"
	local loader, message = loadfile(path)
	return loader or message
end)

-- Environment overrides are a separate layer; keep this merge contract isolated.
os.getenv = function() return nil end
local presets = require("res.scripts.config.sample_presets")
local manager = require("res.scripts.manager.ConfigManager.lua")
local selectedName = "__merge_selected"
manager.GetSelectedPresetName = function() return selectedName end
local failures = 0
local function check(name, run)
	local ok, reason = pcall(run)
	print("[ConfigPresetTest] " .. (ok and "PASS" or "FAIL") .. " case=" .. name ..
		(ok and "" or " reason=" .. tostring(reason)))
	if not ok then failures = failures + 1 end
end
local function equal(actual, expected, label)
	assert(actual == expected, (label or "value") .. ": expected " .. tostring(expected) .. ", got " .. tostring(actual))
end

check("all-declared-spawn-arrays", function()
	local names = {}
	for name, preset in pairs(presets) do
		if type(preset) == "table" and type(preset.spawnPoints) == "table" then names[#names + 1] = name end
	end
	table.sort(names)
	local mismatches = {}
	for _, name in ipairs(names) do
		selectedName = name
		local actual = manager:GetSamplePreset(name)
		local expected = presets[name].spawnPoints
		print("[ConfigPresetCount] preset=" .. name .. " declared=" .. #expected .. " resolved=" .. #actual.spawnPoints)
		if #actual.spawnPoints ~= #expected then mismatches[#mismatches + 1] = name end
	end
	assert(#mismatches == 0, table.concat(mismatches, ","))
end)

local originalDefault = presets.default
local function resolve(base, sample, selected)
	presets.default = base
	presets.__merge_sample = sample or {}
	presets.__merge_selected = selected or {}
	selectedName = "__merge_selected"
	return manager:GetSamplePreset("__merge_sample")
end
check("short-array-and-coordinate-replace", function()
	local p = resolve({ spawnPoints = {{1, 2, 3}, {4, 5, 6}} }, { spawnPoints = {{7, 8}} })
	equal(#p.spawnPoints, 1, "spawn count")
	equal(#p.spawnPoints[1], 2, "coordinate length")
	equal(p.spawnPoints[1][1], 7)
end)
check("explicit-empty-array", function()
	local p = resolve({ commanderMatch = { waveSpawnIndices = {{1, 2}, {3, 4}} } },
		{ commanderMatch = { waveSpawnIndices = {} } })
	equal(next(p.commanderMatch.waveSpawnIndices), nil)
end)
check("map-inheritance-and-selected-precedence", function()
	local p = resolve({ aiScheduler = { enabled = true, tickMs = 50, maxPerFrame = 8 } },
		{ aiScheduler = { enabled = false, tickMs = 20 } }, { aiScheduler = { tickMs = 10 } })
	equal(p.aiScheduler.enabled, false)
	equal(p.aiScheduler.tickMs, 10)
	equal(p.aiScheduler.maxPerFrame, 8)
end)
check("selected-array-replaces-sample", function()
	local p = resolve({ commanderMatch = {waveEnemyCounts = {1, 2, 3}} },
		{ commanderMatch = {waveEnemyCounts = {4, 5}} }, { commanderMatch = {waveEnemyCounts = {6}} })
	equal(#p.commanderMatch.waveEnemyCounts, 1)
	equal(p.commanderMatch.waveEnemyCounts[1], 6)
end)
check("empty-map-inherits", function()
	local p = resolve({ diagnostics = { maxObjects = 8 } }, { diagnostics = {} })
	equal(p.diagnostics.maxObjects, 8)
end)
check("sparse-numeric-map-inherits", function()
	local p = resolve({ lookup = {[10] = "a", [20] = "b"} }, { lookup = {[10] = "c"} })
	equal(p.lookup[10], "c")
	equal(p.lookup[20], "b")
end)
check("dense-indexed-agent-map-inherits", function()
	local p = resolve({ chapter9Tactics = {legacyForceInitialRandomAgents = {[1] = true, [2] = true}} },
		{ chapter9Tactics = {legacyForceInitialRandomAgents = {[1] = false}} })
	equal(p.chapter9Tactics.legacyForceInitialRandomAgents[1], false)
	equal(p.chapter9Tactics.legacyForceInitialRandomAgents[2], true)
end)
check("nested-wave-lists-replace", function()
	local p = resolve({ commanderMatch = {waveSpawnIndices = {{1, 2, 3}, {4, 5}}, prepareMs = 6000} },
		{ commanderMatch = {waveSpawnIndices = {{6}}} })
	equal(#p.commanderMatch.waveSpawnIndices, 1)
	equal(#p.commanderMatch.waveSpawnIndices[1], 1)
	equal(p.commanderMatch.waveSpawnIndices[1][1], 6)
	equal(p.commanderMatch.prepareMs, 6000)
end)
check("deep-copy-isolation", function()
	local base, sample, selected = { nested = {keep = 1} }, { spawnPoints = {{2}} }, { nested = {extra = 3} }
	local p = resolve(base, sample, selected)
	p.spawnPoints[1][1], p.nested.keep, p.nested.extra = 20, 10, 30
	local again = manager:GetSamplePreset("__merge_sample")
	equal(sample.spawnPoints[1][1], 2)
	equal(base.nested.keep, 1)
	equal(selected.nested.extra, 3)
	equal(again.spawnPoints[1][1], 2)
	equal(again.nested.keep, 1)
end)
presets.default = originalDefault
presets.__merge_sample, presets.__merge_selected = nil, nil
if failures > 0 then os.exit(1) end
print("[ConfigPresetTest] PASS all")
