local Chapter9Profile = {}

local PROFILE_DEFINITIONS = {
	luaTeaching = {
		displayName = "Chapter9 Lua teaching",
		role = "teaching",
		implementation = "lua",
		expectedSample = "Sandbox17",
		expectedDriver = "dt",
		parityTarget = "Chapter9 semantics",
	},
	legacyParity = {
		displayName = "Chapter9 legacy parity",
		role = "legacyParity",
		implementation = "lua",
		expectedSample = "Sandbox17",
		expectedDriver = "legacy",
		parityTarget = "HelloOgre3DX chapter_9_tactics",
	},
	cppProduction = {
		displayName = "Chapter9 C++ production",
		role = "production",
		implementation = "cpp",
		expectedSample = "Sandbox18",
		expectedDriver = "dt",
		parityTarget = "Chapter9 semantics",
	},
}

local function _CloneTable(value)
	if type(value) ~= "table" then
		return value
	end
	local result = {}
	for key, item in pairs(value) do
		result[key] = _CloneTable(item)
	end
	return result
end

local function _MergeTable(base, override)
	local result = _CloneTable(base or {})
	for key, item in pairs(override or {}) do
		if type(item) == "table" and type(result[key]) == "table" then
			result[key] = _MergeTable(result[key], item)
		else
			result[key] = _CloneTable(item)
		end
	end
	return result
end

local function _BoolString(value)
	return value == true and "true" or "false"
end

local function _ReadBool(value, defaultValue)
	if value == nil then
		return defaultValue
	end
	return value == true or value == "1" or value == "true" or value == "TRUE" or value == "True" or value == "yes" or value == "YES"
end

local function _ReadNumber(value, defaultValue)
	local numberValue = tonumber(value)
	if numberValue == nil then
		return defaultValue
	end
	return numberValue
end

local function _GetPath(root, path)
	local value = root
	for token in string.gmatch(path, "[^%.]+") do
		if type(value) ~= "table" then
			return nil
		end
		value = value[token]
	end
	return value
end

local function _Add(list, message)
	list[#list + 1] = message
end

local function _RequireType(issues, root, path, typeName)
	local value = _GetPath(root, path)
	if type(value) ~= typeName then
		_Add(issues.errors, path .. " must be " .. typeName .. ", got " .. type(value))
	end
	return value
end

local function _RequireNumber(issues, root, path)
	local value = _GetPath(root, path)
	if tonumber(value) == nil then
		_Add(issues.errors, path .. " must be number, got " .. type(value))
	end
	return value
end

function Chapter9Profile.Make(name, overrides)
	local definition = PROFILE_DEFINITIONS[name] or {}
	local profile = _MergeTable(definition, { name = name })
	return _MergeTable(profile, overrides or {})
end

function Chapter9Profile.InferDriver(agentScript)
	agentScript = tostring(agentScript or "")
	if string.find(agentScript, "Chapter9LegacySoldierAgent", 1, true) ~= nil then
		return "legacy"
	end
	if string.find(agentScript, "Chapter9SoldierAgent", 1, true) ~= nil then
		return "bt"
	end
	if string.find(agentScript, "DecisionSoldierAgent", 1, true) ~= nil then
		return "dt"
	end
	if string.find(agentScript, "BehaviorSoldierAgent", 1, true) ~= nil then
		return "bt"
	end
	return "unknown"
end

function Chapter9Profile.ValidatePreset(sampleName, preset)
	local issues = {
		errors = {},
		warnings = {},
	}
	if type(preset) ~= "table" then
		_Add(issues.errors, "preset must be table")
		return issues
	end

	local profile = preset.chapter9Profile or {}
	local tactics = preset.chapter9Tactics or {}
	local blackboard = preset.aiBlackboard or {}
	local floats = blackboard.floats or {}
	local ints = blackboard.ints or {}
	local bools = blackboard.bools or {}
	local mapConfig = tactics.influenceMap or {}

	if type(profile) ~= "table" then
		_Add(issues.errors, "chapter9Profile must be table")
		profile = {}
	end
	if type(tactics) ~= "table" then
		_Add(issues.errors, "chapter9Tactics must be table")
		tactics = {}
	end
	if type(blackboard) ~= "table" then
		_Add(issues.errors, "aiBlackboard must be table")
	end
	if type(mapConfig) ~= "table" then
		_Add(issues.errors, "chapter9Tactics.influenceMap must be table")
		mapConfig = {}
	end

	_RequireType(issues, preset, "chapter9Tactics.agentScript", "string")
	_RequireNumber(issues, preset, "seed")
	_RequireNumber(issues, preset, "agentCount")
	_RequireNumber(issues, preset, "lightTeamCount")
	_RequireNumber(issues, preset, "chapter9Tactics.eventTtlMs")
	_RequireNumber(issues, preset, "chapter9Tactics.dangerUpdateIntervalMs")
	_RequireNumber(issues, preset, "chapter9Tactics.teamUpdateIntervalMs")
	_RequireNumber(issues, preset, "chapter9Tactics.drawThreshold")
	_RequireNumber(issues, preset, "chapter9Tactics.teamDrawYOffset")
	_RequireNumber(issues, preset, "chapter9Tactics.influenceMap.cellSize")
	if tonumber(floats["perception.visionRange"]) == nil then
		_Add(issues.errors, "aiBlackboard.floats[perception.visionRange] must be number")
	end
	if tonumber(floats["perception.fieldOfViewDegrees"]) == nil then
		_Add(issues.errors, "aiBlackboard.floats[perception.fieldOfViewDegrees] must be number")
	end
	if tonumber(ints["perception.visionIntervalMs"]) == nil then
		_Add(issues.errors, "aiBlackboard.ints[perception.visionIntervalMs] must be number")
	end

	if profile.name == nil then
		_Add(issues.warnings, "chapter9Profile.name is missing")
	end

	local driver = Chapter9Profile.InferDriver(tactics.agentScript)
	if profile.expectedDriver ~= nil and profile.expectedDriver ~= driver then
		_Add(issues.warnings, "expected driver " .. tostring(profile.expectedDriver) .. ", inferred " .. tostring(driver) .. " from " .. tostring(tactics.agentScript))
	end
	if profile.expectedSample ~= nil and sampleName ~= nil and profile.expectedSample ~= sampleName then
		_Add(issues.warnings, "profile expected sample " .. tostring(profile.expectedSample) .. ", running " .. tostring(sampleName))
	end

	if profile.role == "legacyParity" then
		if preset.spawnMode ~= "fixed" then
			_Add(issues.warnings, "legacy parity should use fixed spawnMode for reproducible comparison")
		end
		if _ReadBool(tactics.useLegacyAgentEvents, false) ~= true then
			_Add(issues.warnings, "legacy parity should enable useLegacyAgentEvents")
		end
		if type(preset.parityTrace) ~= "table" then
			_Add(issues.warnings, "legacy parity should define parityTrace")
		end
	end

	if profile.implementation == "cpp" then
		if _ReadBool(tactics.cppInfluenceMap, false) ~= true then
			_Add(issues.warnings, "cpp profile should enable cppInfluenceMap")
		end
		if _ReadBool(tactics.tacticalQueryEnabled, false) ~= true then
			_Add(issues.warnings, "cpp profile should enable tacticalQueryEnabled")
		end
	end

	if _ReadNumber(floats["perception.fieldOfViewDegrees"], -1) <= 0 then
		_Add(issues.warnings, "perception.fieldOfViewDegrees should be positive")
	end
	if _ReadNumber(ints["perception.visionIntervalMs"], -1) <= 0 then
		_Add(issues.warnings, "perception.visionIntervalMs should be positive")
	end
	if bools["perception.requirePath"] == nil then
		_Add(issues.warnings, "perception.requirePath is not specified")
	end
	if _ReadNumber(mapConfig.cellSize, 0) <= 0 then
		_Add(issues.warnings, "influenceMap.cellSize should be positive")
	end

	return issues
end

function Chapter9Profile.BuildTraceMetadata(preset)
	preset = preset or {}
	local profile = preset.chapter9Profile or {}
	local tactics = preset.chapter9Tactics or {}
	local blackboard = preset.aiBlackboard or {}
	local floats = blackboard.floats or {}
	local ints = blackboard.ints or {}
	local bools = blackboard.bools or {}
	local mapConfig = tactics.influenceMap or {}
	return {
		name = profile.name,
		role = profile.role,
		implementation = profile.implementation,
		parityTarget = profile.parityTarget,
		driver = Chapter9Profile.InferDriver(tactics.agentScript),
		agentScript = tactics.agentScript,
		spawnMode = preset.spawnMode,
		agentCount = preset.agentCount,
		lightTeamCount = preset.lightTeamCount,
		fov = floats["perception.fieldOfViewDegrees"],
		visionIntervalMs = ints["perception.visionIntervalMs"],
		staggerScans = bools["perception.staggerScans"],
		cppInfluenceMap = tactics.cppInfluenceMap == true,
		projectInfluenceToNav = tactics.projectInfluenceToNav == true,
		teamDrawYOffset = tactics.teamDrawYOffset,
		cellSize = mapConfig.cellSize,
		cellHeight = mapConfig.cellHeight,
		drawTeamLayer = tactics.drawTeamLayer == true,
		drawDangerLayer = tactics.drawDangerLayer == true,
		drawObjectiveLayer = tactics.drawObjectiveLayer == true,
	}
end

function Chapter9Profile.BuildSummary(sampleName, preset)
	local metadata = Chapter9Profile.BuildTraceMetadata(preset)
	local profileName = metadata.name or "missing"
	local role = metadata.role or "unknown"
	local implementation = metadata.implementation or "unknown"
	local parityTarget = metadata.parityTarget or "none"
	return string.format(
		"[Chapter9Profile] sample=%s preset=%s profile=%s role=%s impl=%s target=%s driver=%s agent=%s seed=%d agents=%d light=%d spawn=%s fov=%.1f visionMs=%d stagger=%s cppMap=%s projectNav=%s cell=%.2f cellH=%.2f teamY=%.3f drawTeam=%s drawDanger=%s drawObjective=%s",
		tostring(sampleName or (preset and preset.sampleName) or "unknown"),
		tostring((preset and preset.name) or "unknown"),
		tostring(profileName),
		tostring(role),
		tostring(implementation),
		tostring(parityTarget),
		tostring(metadata.driver or "unknown"),
		tostring(metadata.agentScript or "unknown"),
		math.floor(tonumber(preset and preset.seed) or 0),
		math.floor(tonumber(metadata.agentCount) or 0),
		math.floor(tonumber(metadata.lightTeamCount) or 0),
		tostring(metadata.spawnMode or "unknown"),
		tonumber(metadata.fov) or 0.0,
		math.floor(tonumber(metadata.visionIntervalMs) or 0),
		_BoolString(metadata.staggerScans == true),
		_BoolString(metadata.cppInfluenceMap == true),
		_BoolString(metadata.projectInfluenceToNav == true),
		tonumber(metadata.cellSize) or 0.0,
		tonumber(metadata.cellHeight) or 0.0,
		tonumber(metadata.teamDrawYOffset) or 0.0,
		_BoolString(metadata.drawTeamLayer == true),
		_BoolString(metadata.drawDangerLayer == true),
		_BoolString(metadata.drawObjectiveLayer == true))
end

function Chapter9Profile.PrintStartupSummary(sampleName, preset)
	local issues = Chapter9Profile.ValidatePreset(sampleName, preset)
	print(Chapter9Profile.BuildSummary(sampleName, preset))
	for _, warning in ipairs(issues.warnings) do
		print("[Chapter9ProfileWarning]", warning)
	end
	if #issues.errors > 0 then
		for _, message in ipairs(issues.errors) do
			print("[Chapter9ProfileError]", message)
		end
		error("[Chapter9Profile] invalid profile for " .. tostring(sampleName))
	end
	return issues
end

return Chapter9Profile
