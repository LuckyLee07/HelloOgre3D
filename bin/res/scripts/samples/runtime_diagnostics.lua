local RuntimeDiagnostics = {}

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

local function printLines(text)
	if text == nil then
		return
	end
	for line in string.gmatch(tostring(text), "[^\r\n]+") do
		print(line)
	end
end

function RuntimeDiagnostics.RunSelfTest()
	local ok = true
	local maxObjects = getEnvNumber("HELLO_RUNTIME_DIAGNOSTIC_MAX_OBJECTS", 8)
	local maxResources = getEnvNumber("HELLO_RUNTIME_DIAGNOSTIC_MAX_RESOURCES", 6)
	local maxEvents = getEnvNumber("HELLO_RUNTIME_DIAGNOSTIC_MAX_EVENTS", 6)

	print("[RuntimeDiag] self test begin", "sample=", tostring(_G.HELLO_SANDBOX_SAMPLE_NAME), "maxObjects=", maxObjects, "maxResources=", maxResources, "maxEvents=", maxEvents)

	if ObjectManager == nil or ObjectManager.buildObjectDebugSummary == nil then
		print("[RuntimeDiag] object inspector unavailable")
		ok = false
	else
		printLines(ObjectManager:buildObjectDebugSummary(maxObjects))
	end

	if ObjectManager == nil or ObjectManager.buildAiSchedulerDebugSummary == nil then
		print("[RuntimeDiag] AI scheduler diagnostics unavailable")
		ok = false
	else
		printLines(ObjectManager:buildAiSchedulerDebugSummary())
	end

	if ObjectManager == nil or ObjectManager.buildAiEventDebugSummary == nil then
		print("[RuntimeDiag] AI event diagnostics unavailable")
		ok = false
	else
		printLines(ObjectManager:buildAiEventDebugSummary(maxObjects, maxEvents))
	end

	if GameManager == nil or GameManager.buildRuntimeResourceDump == nil then
		print("[RuntimeDiag] resource dump unavailable")
		ok = false
	else
		printLines(GameManager:buildRuntimeResourceDump(maxResources))
	end

	print("[RuntimeDiag] self test result:", ok)
	return ok
end

_G.RuntimeDiagnostics = RuntimeDiagnostics
return RuntimeDiagnostics
