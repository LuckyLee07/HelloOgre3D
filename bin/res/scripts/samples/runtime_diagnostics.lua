local RuntimeDiagnostics = {}

local function getEnvValue(name)
	return os.getenv and os.getenv(name) or nil
end

local function isTruthy(value)
	return value == true or value == "1" or value == "true" or value == "TRUE" or value == "True" or value == "yes" or value == "YES"
end

local function getDiagnosticLimit(key, envName, defaultValue)
	local envValue = tonumber(getEnvValue(envName))
	if envValue ~= nil then
		return envValue
	end
	if ConfigManager ~= nil and ConfigManager.GetDiagnosticLimit ~= nil then
		return ConfigManager:GetDiagnosticLimit(_G.HELLO_SANDBOX_SAMPLE_NAME, key, defaultValue)
	end
	return defaultValue
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
	local maxObjects = getDiagnosticLimit("maxObjects", "HELLO_RUNTIME_DIAGNOSTIC_MAX_OBJECTS", 8)
	local maxResources = getDiagnosticLimit("maxResources", "HELLO_RUNTIME_DIAGNOSTIC_MAX_RESOURCES", 6)
	local maxEvents = getDiagnosticLimit("maxEvents", "HELLO_RUNTIME_DIAGNOSTIC_MAX_EVENTS", 6)

	print("[RuntimeDiag] self test begin", "sample=", tostring(_G.HELLO_SANDBOX_SAMPLE_NAME), "maxObjects=", maxObjects, "maxResources=", maxResources, "maxEvents=", maxEvents)
	if ConfigManager == nil or ConfigManager.BuildDebugSummary == nil then
		print("[RuntimeDiag] config manager unavailable")
		ok = false
	else
		printLines(ConfigManager:BuildDebugSummary(_G.HELLO_SANDBOX_SAMPLE_NAME))
	end

	if ObjectManager == nil or ObjectManager.buildObjectDebugSummary == nil then
		print("[RuntimeDiag] object inspector unavailable")
		ok = false
	else
		printLines(ObjectManager:buildObjectDebugSummary(maxObjects))
	end

	if ObjectManager == nil or ObjectManager.buildAiDebugSummary == nil then
		print("[RuntimeDiag] AI debug summary unavailable")
		ok = false
	else
		local aiSummary = tostring(ObjectManager:buildAiDebugSummary(maxObjects))
		printLines(aiSummary)
		if isTruthy(getEnvValue("HELLO_AI_BLACKBOARD_SELF_TEST")) and string.find(aiSummary, "[BlackboardSelfTest] result=true", 1, true) == nil then
			ok = false
		end
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

	if isTruthy(getEnvValue("HELLO_AI_EVENT_SELF_TEST")) then
		if ObjectManager == nil or ObjectManager.runAiEventScopeSelfTest == nil then
			print("[RuntimeDiag] AI event selftest unavailable")
			ok = false
		else
			local eventSelfTest = tostring(ObjectManager:runAiEventScopeSelfTest())
			printLines(eventSelfTest)
			if string.find(eventSelfTest, "[AIEventSelfTest] result=true", 1, true) == nil then
				ok = false
			end
		end
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
