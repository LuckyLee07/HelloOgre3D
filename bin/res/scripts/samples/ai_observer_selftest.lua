-- Explicit opt-in integration test: drives real actors over multiple simulation ticks.
local Observer = require("res.scripts.samples.ai_observer")
local Test = {}
Test.__index = Test

function Test.New()
	return setmetatable({ stage = "start", deadline = 0, done = false }, Test)
end

function Test:Step(host, nowMs)
	if self.done then return end
	local function check(ok, name)
		print("[Sandbox19ObservationSelfTest] " .. (ok and "PASS" or "FAIL") .. " case=" .. name)
		if not ok then self.done = true end
		return ok
	end
	local function nextStage(name, timeout)
		self.stage, self.deadline = name, nowMs + timeout
	end
	if self.stage ~= "start" and nowMs > self.deadline then
		check(false, self.stage .. "-timeout")
		return
	end
	if self.stage == "start" then
		self.allyId, self.enemyId = host.pickPair()
		if not check(self.allyId ~= nil and self.enemyId ~= nil, "actors") then return end
		host.select(self.allyId)
		nextStage("autonomous", 15000)
		return
	end
	local ally = host.find(self.allyId)
	local enemy = host.find(self.enemyId)
	if self.stage == "restart" then
		local s = host.snapshot()
		if s == nil then return end
		if check(host.find(self.allyId) == nil and s.id ~= self.allyId and s.command.kind == "none",
			"restart-new-identity") then
			self.done = true
			print("[Sandbox19ObservationSelfTest] PASS all=true")
		end
		return
	end
	if ally == nil or enemy == nil then check(false, "actors-resolve-" .. self.stage); return end
	local bb = ally:GetAIComponent():GetBlackboard()
	local revision = bb:GetRevision()
	local s = Observer.Capture(ally, nowMs, Observer.Command(bb, nowMs, 8000, host.find), "ai_soldier")
	if revision ~= bb:GetRevision() then check(false, "read-only"); return end
	if self.stage == "autonomous" then
		if not s.visible or (s.action ~= "pursue" and s.action ~= "shoot") then return end
		if not check(s.command.kind == "none" and s.traceFrame > 0, "autonomous-vision-action") then return end
		print("[AIObservationCase] autonomous " .. s.trace)
		host.focus(self.allyId, self.enemyId)
		nextStage("focus", 4000)
	elseif self.stage == "focus" then
		if s.source ~= "player branch (sampled trace)" then return end
		if not check(s.command.active and s.command.targetId == self.enemyId, "focus-actual-branch") then return end
		print("[AIObservationCase] focus " .. s.trace)
		-- Expire through the existing command clock; then remove all visual targets.
		bb:SetInt("command.issuedMs", nowMs - 8001)
		host.hideEnemies()
		nextStage("lost", 5000)
	elseif self.stage == "lost" then
		if s.visible or not s.memory or s.action ~= "moveToLastKnownEnemy" then return end
		if not check(not s.command.active and s.lastClear == "ttl-expired" and s.memoryAgeMs > 0,
			"lost-vision-memory-search") then return end
		print("[AIObservationCase] lost " .. s.trace)
		host.retreat(self.allyId)
		self.issuedMs = nowMs
		nextStage("ttl", 10000)
	elseif self.stage == "ttl" then
		if nowMs - self.issuedMs <= 8000 then return end
		if not check(s.command.kind == "none" and s.lastClear == "ttl-expired", "ttl-natural-expiry") then return end
		host.focus(self.allyId, self.enemyId)
		enemy:SetHealth(0)
		nextStage("target-dead", 2000)
	elseif self.stage == "target-dead" then
		if s.command.kind ~= "none" then return end
		if not check(s.lastClear == "focus-target-invalid", "target-death-clears-order") then return end
		host.retreat(self.allyId)
		ally:SetHealth(0)
		nextStage("owner-dead", 2000)
	elseif self.stage == "owner-dead" then
		local displayed = host.snapshot()
		if not check(s.command.kind == "none" and s.lastClear == "owner-dead"
			and (displayed == nil or displayed.id ~= self.allyId), "owner-death-clears-selection") then return end
		host.restart()
		nextStage("restart", 2000)
	end
end

return Test
