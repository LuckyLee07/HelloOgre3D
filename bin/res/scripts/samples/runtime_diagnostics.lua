local RuntimeDiagnostics = {}
local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

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

local function runComponentAccessSelfTest()
	local ok = true
	local detail = {}

	if SandboxObjects == nil or SandboxObjects.CreateAgent == nil then
		print("[ComponentAccessSelfTest] SandboxObjects:CreateAgent unavailable")
		return false
	end

	local agentType = _G.AGENT_OBJ_NONE or 0
	local agent = SandboxObjects:CreateAgent(agentType, "res/scripts/agent/ComponentProbeAgent.lua")
	if agent == nil then
		print("[ComponentAccessSelfTest] failed to create non-soldier agent")
		return false
	end
	if agent.SetRenderVisible ~= nil then
		agent:SetRenderVisible(false)
	end

	local componentCount = agent.GetComponentCount ~= nil and agent:GetComponentCount() or 0
	local hasAi = agent.HasComponent ~= nil and agent:HasComponent("ai") or false
	local hasAttrib = agent.HasComponent ~= nil and agent:HasComponent("attrib") or false
	local ai = AgentComponents.GetAI(agent)
	local attrib = AgentComponents.GetAttrib(agent)
	local debugString = agent.BuildComponentDebugString ~= nil and agent:BuildComponentDebugString() or ""
	local blackboardOwnerMatches = false

	if componentCount < 2 then
		ok = false
		table.insert(detail, "componentCount<2")
	end
	if not hasAi or ai == nil then
		ok = false
		table.insert(detail, "aiMissing")
	end
	if not hasAttrib or attrib == nil then
		ok = false
		table.insert(detail, "attribMissing")
	end
	if ai ~= nil and ai.GetBlackboard ~= nil then
		local blackboard = ai:GetBlackboard()
		local blackboardOwner = blackboard ~= nil and blackboard.GetAgentOwner ~= nil and blackboard:GetAgentOwner() or nil
		blackboardOwnerMatches = blackboardOwner == agent
	end
	if not blackboardOwnerMatches then
		ok = false
		table.insert(detail, "blackboardAgentOwner")
	end

	if attrib ~= nil then
		attrib:SetMaxHealth(123)
		attrib:SetHealth(77)
		local health = AgentComponents.GetHealth(agent, -1)
		local maxHealth = AgentComponents.GetMaxHealth(agent, -1)
		if health ~= 77 or maxHealth ~= 123 then
			ok = false
			table.insert(detail, "attribRoundTrip")
		end
	end

	print("[ComponentAccessSelfTest] result=" .. tostring(ok),
		"componentCount=" .. tostring(componentCount),
		"hasAi=" .. tostring(hasAi),
		"hasAttrib=" .. tostring(hasAttrib),
		"blackboardOwner=" .. tostring(blackboardOwnerMatches),
		"debug=" .. tostring(debugString),
		"detail=" .. table.concat(detail, ","))
	return ok
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

	if not runComponentAccessSelfTest() then
		ok = false
	end

	print("[RuntimeDiag] self test result:", ok)
	return ok
end

_G.RuntimeDiagnostics = RuntimeDiagnostics
return RuntimeDiagnostics
