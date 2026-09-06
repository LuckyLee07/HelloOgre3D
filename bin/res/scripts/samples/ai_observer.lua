-- Read-only sampled evidence for Sandbox19. Never call sensing/condition APIs here:
-- HasEnemy / CanShootEnemy can scan and overwrite the decision target.
local Components = require("res.scripts.agent.AgentComponentAccess.lua")
local Observer = {}
Observer.__index = Observer

local function vec(v)
	if v == nil then return "none" end
	return string.format("%.1f,%.1f,%.1f", v.x, v.y, v.z)
end

function Observer.Command(bb, nowMs, ttlMs, findAgent)
	if bb == nil or not bb:Has("command.issuedMs") then return { kind = "none" } end
	local kind = bb:GetString("command.kind")
	local elapsed = nowMs - bb:GetInt("command.issuedMs", nowMs)
	local reason = nil
	if elapsed < 0 or elapsed > ttlMs then
		reason = "ttl-expired"
	elseif kind == "focus" then
		local target = findAgent(bb:GetObjectId("command.focusTargetId", 0))
		if target == nil or target:GetHealth() <= 0 then reason = "focus-target-invalid" end
	elseif (kind ~= "retreat" and kind ~= "rally") or not bb:Has("command.targetPos") then
		reason = "invalid-command"
	end
	return { kind = kind, active = reason == nil, reason = reason,
		remainingMs = math.max(0, ttlMs - elapsed), targetId = bb:GetObjectId("command.focusTargetId", 0) }
end

function Observer.Capture(agent, nowMs, command, profile)
	local ai = Components.GetAI(agent)
	local bb = ai ~= nil and ai:GetBlackboard() or nil
	if bb == nil then return nil end
	local driver = ai:GetBehaviorTreeDriver()
	local trace = driver ~= nil and driver:GetLastDebugTrace() or ""
	local action = bb:GetString("__bt.currentAction")
	local status = bb:GetString("__bt.currentActionStatus")
	local visible = bb:GetBool("perception.hasTarget", false)
	local memory = bb:GetBool("memory.snapshot.hasLastKnownEnemy", false)
	local visionId = bb:GetObjectId("perception.targetId", -1)
	local source = "unknown (no matching evidence)"
	-- Trace is the actual last sampled tree traversal, not a re-evaluation.
	local commandBranch = string.find(trace, "commandFocus=RUNNING", 1, true)
		or string.find(trace, "commandRetreat=RUNNING", 1, true)
		or string.find(trace, "commandRally=RUNNING", 1, true)
	if command.active and commandBranch then
		source = "player branch (sampled trace)"
	elseif action == "moveToLastKnownEnemy" and memory then
		source = "local memory (search action)"
	elseif action == "investigateSound" then
		source = "sound action (see trace)"
	elseif action == "moveToFormationSlot" then
		source = "formation action (see trace)"
	elseif action == "randomMove" or action == "idle" then
		source = "autonomous action (see trace)"
	elseif (action == "shoot" or action == "pursue") and visible then
		source = "combat; vision available"
	end
	-- Blackboard::GetAgent is a raw pointer. Resolve by equality against live actors
	-- before dereferencing, and retain only the id in the snapshot.
	local decisionTargetId = -1
	local rawTarget = bb:GetAgent("enemy")
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		if agents[i] == rawTarget then decisionTargetId = agents[i]:GetObjId(); break end
	end
	local locomotion = Components.GetLocomotion(agent)
	local path = Components.GetPath(agent)
	local asm = Components.GetBodyAsm(agent)
	return {
		id = agent:GetObjId(), timeMs = nowMs, profile = profile or "unknown",
		controller = "AIController", driver = driver ~= nil and "BT" or "unknown",
		action = action ~= "" and action or "pending", status = status ~= "" and status or "pending",
		source = source, decisionTargetId = decisionTargetId, trace = trace, traceFrame = driver ~= nil and driver:GetDebugTraceFrame() or 0,
		visible = visible, visionId = visionId, memory = memory,
		memoryId = bb:GetObjectId("memory.snapshot.lastKnownEnemyId", -1),
		memoryAgeMs = bb:GetInt("memory.snapshot.lastKnownEnemyAgeMs", -1),
		memoryPos = memory and vec(bb:GetVec3("memory.snapshot.lastKnownEnemyPos")) or "none",
		observedAtMs = bb:GetInt("memory.snapshot.lastKnownEnemyObservedAtMs", -1),
		teamId = bb:GetObjectId("team.focusTargetId", -1),
		teamFrom = bb:GetInt("team.supportFromAgentId", -1),
		command = command,
		lastClear = bb:GetString("__debug.commandClearReason"),
		speed = Components.GetSpeed(agent, 0), ammo = Components.GetAmmo(agent, -1),
		movePos = bb:Has("movePos") and vec(bb:GetVec3("movePos")) or "none",
		target = locomotion ~= nil and vec(locomotion:GetTarget()) or "unknown",
		pathPoints = path ~= nil and path:size() or 0,
		navStatus = bb:GetString("__debug.navStatus"),
		navTimeMs = bb:GetInt("__debug.navTimeMs", -1),
		animation = asm ~= nil and asm:GetCurrStateName() or "unknown",
	}
end

function Observer.Lines(s)
	if s == nil then return { "AI OBSERVER | select a living ally" } end
	return {
		string.format("AI OBSERVER #%d | t=%dms | I: hide", s.id, s.timeMs),
		s.profile .. " (spawn) | " .. s.controller .. "/" .. s.driver,
		"Last action: " .. s.action .. " / " .. s.status,
		"Decision target (BB): #" .. s.decisionTargetId,
		"Evidence: " .. s.source,
		"Vision sample: " .. (s.visible and ("#" .. s.visionId) or "none") .. " (not a fresh scan)",
		"Local visual memory: " .. (s.memory and ("#" .. s.memoryId .. " age=" .. s.memoryAgeMs .. "ms") or "none"),
		"Last seen: " .. s.observedAtMs .. "ms | pos=" .. s.memoryPos,
		"Team support fact: #" .. s.teamId .. " from #" .. s.teamFrom,
		"Order: " .. s.command.kind .. " | " .. (s.command.active and (s.command.remainingMs .. "ms left") or "inactive"),
		"Last order clear: " .. (s.lastClear ~= "" and s.lastClear or "none"),
		"Move request: " .. s.movePos,
		string.format("Speed %.2f | ammo %d | path points %d", s.speed, s.ammo, s.pathPoints),
		"Last path: " .. (s.navStatus ~= "" and s.navStatus or "unknown") .. " @" .. s.navTimeMs .. "ms",
		"Animation: " .. s.animation .. " | BT trace #" .. s.traceFrame,
		"O: dump full evidence to Sandbox.log",
	}
end

function Observer.New()
	return setmetatable({ id = 0, enabled = true, nextMs = 0, snapshot = nil }, Observer)
end

function Observer:Release(findAgent)
	local agent = findAgent(self.id)
	local ai = agent ~= nil and Components.GetAI(agent) or nil
	local driver = ai ~= nil and ai:GetBehaviorTreeDriver() or nil
	if driver ~= nil and self.savedTrace ~= nil then
		driver:SetDebugTraceEnabled(self.savedTrace)
		driver:SetDebugTraceSampleInterval(self.savedInterval)
	end
	self.id, self.snapshot, self.savedTrace, self.nextMs = 0, nil, nil, 0
end

function Observer:Update(selection, findAgent, nowMs, ttlMs, profiles)
	local id = 0
	if self.enabled then
		for candidate in pairs(selection) do
			local agent = findAgent(candidate)
			if agent ~= nil and agent:GetHealth() > 0 and (id == 0 or candidate < id) then id = candidate end
		end
	end
	if id ~= self.id then
		self:Release(findAgent)
		self.id = id
		local agent = findAgent(id)
		local ai = agent ~= nil and Components.GetAI(agent) or nil
		local driver = ai ~= nil and ai:GetBehaviorTreeDriver() or nil
		if driver ~= nil then
			self.savedTrace, self.savedInterval = driver:IsDebugTraceEnabled(), driver:GetDebugTraceSampleInterval()
			driver:SetDebugTraceEnabled(true)
			driver:SetDebugTraceSampleInterval(1)
		end
	end
	if id == 0 or nowMs < self.nextMs then return end
	self.nextMs = nowMs + 200
	local agent = findAgent(id)
	local ai = agent ~= nil and Components.GetAI(agent) or nil
	self.snapshot = ai ~= nil and Observer.Capture(agent, nowMs,
		Observer.Command(ai:GetBlackboard(), nowMs, ttlMs, findAgent), profiles[id]) or nil
end

function Observer:Dump()
	for _, line in ipairs(Observer.Lines(self.snapshot)) do print("[AIObservation] " .. line) end
	if self.snapshot ~= nil then print("[AIObservation] " .. self.snapshot.trace) end
end

return Observer
