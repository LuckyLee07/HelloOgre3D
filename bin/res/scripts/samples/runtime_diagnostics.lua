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
	local usedAssemblyProfile = SandboxObjects.CreateAgentWithProfile ~= nil
	local agent = nil
	if usedAssemblyProfile then
		agent = SandboxObjects:CreateAgentWithProfile(agentType, "component_probe", "res/scripts/agent/ComponentProbeAgent.lua")
	else
		agent = SandboxObjects:CreateAgent(agentType, "res/scripts/agent/ComponentProbeAgent.lua")
	end
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
	local hasWeapon = agent.HasComponent ~= nil and agent:HasComponent("weapon") or false
	local hasAnim = agent.HasComponent ~= nil and agent:HasComponent("anim") or false
	local hasLocomotion = agent.HasComponent ~= nil and agent:HasComponent("locomotion") or false
	local ai = AgentComponents.GetAI(agent)
	local attrib = AgentComponents.GetAttrib(agent)
	local weapon = AgentComponents.GetWeapon(agent)
	local anim = AgentComponents.GetAnim(agent)
	local locomotion = AgentComponents.GetLocomotion(agent)
	local bodyAsm = AgentComponents.GetBodyAsm(agent)
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
	if not hasWeapon or weapon == nil then
		ok = false
		table.insert(detail, "weaponMissing")
	end
	if not hasAnim or anim == nil or bodyAsm == nil then
		ok = false
		table.insert(detail, "animMissing")
	end
	if not hasLocomotion or locomotion == nil then
		ok = false
		table.insert(detail, "locomotionMissing")
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

	if weapon ~= nil then
		weapon:SetMaxAmmo(3)
		weapon:SetAmmo(2)
		if AgentComponents.GetAmmo(agent, -1) ~= 2 or not AgentComponents.HasAmmo(agent) then
			ok = false
			table.insert(detail, "weaponAmmoRoundTrip")
		end
		AgentComponents.ShootBullet(agent)
	end

	if locomotion ~= nil then
		AgentComponents.SetMaxSpeed(agent, 1.25)
		AgentComponents.SetTargetRadius(agent, 2.5)
		AgentComponents.SetTarget(agent, Vector3(1, 0, 2))
		local target = AgentComponents.GetTarget(agent)
		if AgentComponents.GetMaxSpeed(agent, -1) ~= 1.25 then
			ok = false
			table.insert(detail, "locomotionMaxSpeedRoundTrip")
		end
		if AgentComponents.GetTargetRadius(agent, -1) ~= 2.5 then
			ok = false
			table.insert(detail, "locomotionTargetRadiusRoundTrip")
		end
		if target == nil or target.x ~= 1 or target.z ~= 2 then
			ok = false
			table.insert(detail, "locomotionTargetRoundTrip")
		end
	end

	print("[ComponentAccessSelfTest] result=" .. tostring(ok),
		"componentCount=" .. tostring(componentCount),
		"hasAi=" .. tostring(hasAi),
		"hasAttrib=" .. tostring(hasAttrib),
		"hasWeapon=" .. tostring(hasWeapon),
		"hasAnim=" .. tostring(hasAnim),
		"hasLocomotion=" .. tostring(hasLocomotion),
		"hasBodyAsm=" .. tostring(bodyAsm ~= nil),
		"assemblyProfile=" .. tostring(usedAssemblyProfile),
		"blackboardOwner=" .. tostring(blackboardOwnerMatches),
		"debug=" .. tostring(debugString),
		"detail=" .. table.concat(detail, ","))
	return ok
end

local function runNonSoldierAnimationProfileSelfTest()
	if SandboxObjects == nil or SandboxObjects.CreateAgentWithProfile == nil then
		print("[NonSoldierAnimProfileSelfTest] CreateAgentWithProfile unavailable")
		return false
	end

	local ok = true
	local detail = {}
	local agentType = _G.AGENT_OBJ_NONE or 0
	local agent = SandboxObjects:CreateAgentWithProfile(agentType, "animated_probe", "res/scripts/agent/NonSoldierAnimProbeAgent.lua")
	if agent == nil then
		print("[NonSoldierAnimProfileSelfTest] failed to create animated probe agent")
		return false
	end
	if agent.SetRenderVisible ~= nil then
		agent:SetRenderVisible(false)
	end

	local anim = AgentComponents.GetAnim(agent)
	local bodyAsm = AgentComponents.GetBodyAsm(agent)
	local idleAnim = AgentComponents.GetBodyAnimation(agent, "stand_idle_aim")
	local moveAnim = AgentComponents.GetBodyAnimation(agent, "stand_run_forward_aim")
	if anim == nil then
		ok = false
		table.insert(detail, "animMissing")
	end
	if bodyAsm == nil then
		ok = false
		table.insert(detail, "bodyAsmMissing")
	end
	if idleAnim == nil or moveAnim == nil then
		ok = false
		table.insert(detail, "animationClipMissing")
	end

	local currentState = ""
	local nextState = ""
	local desiredState = ""
	local requestMove = false
	if bodyAsm ~= nil then
		currentState = tostring(bodyAsm:GetCurrStateName())
		requestMove = bodyAsm:RequestState("probe_move") == true
		nextState = tostring(bodyAsm:GetNextStateName())
		desiredState = tostring(bodyAsm:GetDesiredStateName())
	end
	if currentState ~= "probe_idle" then
		ok = false
		table.insert(detail, "initialState")
	end
	if not requestMove or (nextState ~= "probe_move" and desiredState ~= "probe_move") then
		ok = false
		table.insert(detail, "requestMove")
	end

	print("[NonSoldierAnimProfileSelfTest] result=" .. tostring(ok),
		"profile=animated_probe",
		"current=" .. tostring(currentState),
		"next=" .. tostring(nextState),
		"desired=" .. tostring(desiredState),
		"hasIdleClip=" .. tostring(idleAnim ~= nil),
		"hasMoveClip=" .. tostring(moveAnim ~= nil),
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

	local usedUnifiedAiRuntimeSummary = false
	if ObjectManager ~= nil and ObjectManager.buildAiRuntimeDebugSummary ~= nil then
		local aiRuntimeSummary = tostring(ObjectManager:buildAiRuntimeDebugSummary(maxObjects))
		printLines(aiRuntimeSummary)
		usedUnifiedAiRuntimeSummary = true
		if isTruthy(getEnvValue("HELLO_AI_BLACKBOARD_SELF_TEST")) and string.find(aiRuntimeSummary, "[BlackboardSelfTest] result=true", 1, true) == nil then
			ok = false
		end
	elseif ObjectManager == nil or ObjectManager.buildAiDebugSummary == nil then
		print("[RuntimeDiag] AI debug summary unavailable")
		ok = false
	else
		local aiSummary = tostring(ObjectManager:buildAiDebugSummary(maxObjects))
		printLines(aiSummary)
		if isTruthy(getEnvValue("HELLO_AI_BLACKBOARD_SELF_TEST")) and string.find(aiSummary, "[BlackboardSelfTest] result=true", 1, true) == nil then
			ok = false
		end
	end

	if not usedUnifiedAiRuntimeSummary then
		local schedulerService = SandboxAIScheduler
		if schedulerService == nil or schedulerService.buildAiSchedulerDebugSummary == nil then
			schedulerService = ObjectManager
		end
		if schedulerService == nil or schedulerService.buildAiSchedulerDebugSummary == nil then
			print("[RuntimeDiag] AI scheduler diagnostics unavailable")
			ok = false
		else
			printLines(schedulerService:buildAiSchedulerDebugSummary())
		end
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
	if not runNonSoldierAnimationProfileSelfTest() then
		ok = false
	end

	print("[RuntimeDiag] self test result:", ok)
	return ok
end

_G.RuntimeDiagnostics = RuntimeDiagnostics
return RuntimeDiagnostics
