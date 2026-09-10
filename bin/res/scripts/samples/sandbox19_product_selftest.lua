-- Opt-in integration fixture, never a normal-play or visual acceptance claim.
local Observer = require("res.scripts.samples.ai_observer")
local Scene = require("res.scripts.samples.sandbox19_scene.lua")
local Test = {}
Test.__index = Test
local COUNTERS = {"issued", "completed", "failed", "replaced", "cancelled", "active"}
local COMMAND_KEYS = {"command.kind", "command.issuedMs", "command.focusTargetId", "command.targetPos", "command.status", "command.semantic"}

local function snapshot(commands)
	local result = {}
	for _, name in ipairs(COUNTERS) do result[name] = commands.stats[name] end
	return result
end
local function deltaMatches(commands, before, change)
	local stats = commands.stats
	for _, name in ipairs(COUNTERS) do
		if stats[name] ~= before[name] + (change[name] or 0) then return false end
	end
	return stats.issued == stats.completed + stats.failed + stats.replaced + stats.cancelled + stats.active
end
local function commandCleared(bb)
	if bb == nil then return true end
	for _, key in ipairs(COMMAND_KEYS) do if bb:Has(key) then return false end end
	return true
end
local function blackboard(agent)
	local ai = agent and agent:GetAIComponent()
	return ai and ai:GetBlackboard()
end
local function protectFixtureActor(agent)
	if agent == nil or agent:GetHealth() <= 0 then return end
	agent:GetAttribComponent():SetMaxHealth(10000)
	agent:SetHealth(10000)
	local bb = blackboard(agent)
	if bb ~= nil then bb:SetFloat("maxHealth", 10000) end
end

function Test.New() return setmetatable({stage = "prepare", frames = 0, done = false}, Test) end
function Test:Check(ok, name)
	print("[Sandbox19ProductSelfTest] " .. (ok and "PASS" or "FAIL") .. " case=" .. name .. " synthetic=true")
	if not ok then self.done = true end
	return ok
end
function Test:Next(stage, nowMs)
	self.stage, self.since, self.frames = stage, nowMs, 0
end
function Test:WaitForExecution(h, actor, kind, nowMs, name)
	local bb = blackboard(actor)
	local order = actor and h.commands.active[actor:GetObjId()]
	if actor == nil or actor:GetHealth() <= 0 or order == nil or order.kind ~= kind then
		self:Check(false, name .. "-order-ended-before-bt")
		return false
	end
	local action = bb:GetString("__bt.currentAction")
	local actionMatches = kind == "focus" and (action == "shoot" or action == "pursue" or action == "reload")
		or kind ~= "focus" and action == "move"
	-- Only the real BT conditions write executing. The fixture never calls them
	-- and never writes this status; also require the action chosen by that tick.
	if bb:GetString("command.status") == "executing" and actionMatches then
		return self:Check(true, name)
	end
	if nowMs - self.since > 3000 then self:Check(false, name .. "-timeout") end
	return false
end
function Test:PlaceFocusTarget(h, ally, nowMs, nextStage)
	local enemy = h.find(self.enemyId)
	if not self:Check(enemy ~= nil and enemy:GetHealth() > 0, "focus-fixture-target-alive") then return end
	enemy:setPosition(ally:GetPosition() + Vector3(3, 0, 6))
	enemy:SetVelocity(Vector3(0, 0, 0))
	ally:SetForward(Vector3(0, 0, 1))
	self:Next(nextStage, nowMs)
end
function Test:IssueVisibleFocus(h, ally, nowMs, nextStage)
	if nowMs - self.since < 250 then return end
	if not ally:GetAIComponent():CanSeeEnemy(self.enemyId) then
		if nowMs - self.since > 3000 then self:Check(false, "focus-fixture-not-visible") end
		return
	end
	local ok = h.commands:Issue(ally, "focus", self.enemyId, nil, nowMs)
	if not self:Check(ok and blackboard(ally):GetString("command.status") == "accepted", "focus-accepted-before-bt") then return end
	self.before = snapshot(h.commands)
	self:Next(nextStage, nowMs)
end
function Test:Step(h, nowMs)
	if self.done then return end
	self.frames = self.frames + 1
	local ally = h.find(h.allies[1])
	local bb = blackboard(ally)
	if h.state == "DEFEAT" and self.stage ~= "defeat" then self:Check(false, "unexpected-defeat-stage-" .. self.stage); return end
	if ally == nil or bb == nil then self:Check(false, "missing-fixture-ally-stage-" .. self.stage); return end
	if self.stage == "prepare" then
		if self.frames == 1 then self.frozenTime, self.frozenPos = nowMs, h.player:GetPosition() end
		if self.frames < 30 then return end
		if not self:Check(h.state == "PREPARE" and #h.enemies == 0 and nowMs == self.frozenTime
			and (h.player:GetPosition() - self.frozenPos):squaredLength() < 0.001, "briefing-freezes-simulation") then return end
		h.start()
		self:Next("move", nowMs)
	elseif self.stage == "move" then
		if nowMs - self.since < 300 then return end
		if not self:Check(Scene.ValidateCollision(), "real-wall-and-floor-collision") then return end
		if not self:Check(h.wave == 1 and #h.enemies == 2 and h.player:GetWeaponComponent() == nil, "unarmed-two-guard-start") then return end
		h.selectAll()
		h.issue("rally", -1, Vector3(0, 0, -10))
		if not self:Check(h.commands.stats.issued == 2, "rally-accepted-two-actors") then return end
		self.executedAllies = {}
		self:Next("arrival", nowMs)
	elseif self.stage == "arrival" then
		if nowMs - self.since > 16000 then self:Check(false, "real-rally-arrival-timeout"); return end
		for _, id in ipairs(h.allies) do
			local b = blackboard(h.find(id))
			if b and b:GetString("command.status") == "executing" and b:GetString("__bt.currentAction") == "move" then self.executedAllies[id] = true end
		end
		if h.commands.stats.active ~= 0 then return end
		if not self:Check(h.commands.stats.completed == 2 and self.executedAllies[h.allies[1]] and self.executedAllies[h.allies[2]], "real-navigation-arrival-completes") then return end
		h.issue("retreat", -1, h.anchors.fallback)
		self.executedAllies = {}
		self:Next("hold", nowMs)
	elseif self.stage == "hold" then
		if nowMs - self.since > 16000 then self:Check(false, "real-retreat-arrival-timeout"); return end
		for _, id in ipairs(h.allies) do
			local b = blackboard(h.find(id))
			if b and b:GetString("command.status") == "executing" and b:GetString("__bt.currentAction") == "move" then self.executedAllies[id] = true end
		end
		if h.commands.stats.active ~= 0 then return end
		local other = blackboard(h.find(h.allies[2]))
		if not self:Check(bb:Has("sandbox19.holdPos") and other and other:Has("sandbox19.holdPos") and h.commands.stats.completed == 4
			and self.executedAllies[h.allies[1]] and self.executedAllies[h.allies[2]], "retreat-arrived-holding") then return end
		h.pause(true)
		self.frozenTime, self.frozenPos, self.frozenHp = nowMs, ally:GetPosition(), ally:GetHealth()
		self:Next("paused", nowMs)
	elseif self.stage == "paused" then
		if self.frames < 45 then return end
		if not self:Check(nowMs == self.frozenTime and (ally:GetPosition() - self.frozenPos):squaredLength() < 0.001
			and ally:GetHealth() == self.frozenHp, "pause-freezes-clock-physics-health") then return end
		h.pause(false)
		local revision = bb:GetRevision()
		Observer.Capture(ally, nowMs, Observer.Command(bb, nowMs, 8000, h.find), "ai_soldier")
		if not self:Check(revision == bb:GetRevision(), "observer-read-only") then return end
		-- Close contact must not race natural damage/death while lifecycle cases
		-- wait for real BT ticks. This health increase is fixture-only and explicit.
		protectFixtureActor(h.player)
		for _, id in ipairs(h.allies) do protectFixtureActor(h.find(id)) end
		for _, id in ipairs(h.enemies) do protectFixtureActor(h.find(id)) end
		print("[Sandbox19ProductSelfTest] SETUP command-lifecycle-health=10000 synthetic=true")
		self.enemyId = h.enemies[1]
		self:PlaceFocusTarget(h, ally, nowMs, "focus-ready")
	elseif self.stage == "focus-ready" then
		self:IssueVisibleFocus(h, ally, nowMs, "focus-executing")
	elseif self.stage == "focus-executing" then
		if not self:WaitForExecution(h, ally, "focus", nowMs, "focus-executed-by-real-bt") then return end
		local order = h.commands.active[ally:GetObjId()]
		self.focusSerial = order.serial
		order.issuedMs = nowMs - 9000
		bb:SetInt("command.issuedMs", order.issuedMs)
		self:Next("aged-focus", nowMs)
	elseif self.stage == "aged-focus" then
		if nowMs - self.since < 350 then return end
		local order = h.commands.active[ally:GetObjId()]
		if not self:Check(order and order.serial == self.focusSerial and bb:GetString("command.status") == "executing"
			and deltaMatches(h.commands, self.before, {}), "no-blanket-eight-second-expiry") then return end
		self.before = snapshot(h.commands)
		local ok = h.commands:Issue(ally, "rally", -1, h.anchors.fallback + Vector3(0, 0, 8), nowMs)
		if not self:Check(ok and deltaMatches(h.commands, self.before, {issued = 1, replaced = 1})
			and h.commands.active[ally:GetObjId()].serial ~= self.focusSerial, "replacement-recorded-once") then return end
		self:Next("replacement-executing", nowMs)
	elseif self.stage == "replacement-executing" then
		if not self:WaitForExecution(h, ally, "rally", nowMs, "replacement-executed-by-real-bt") then return end
		self.before = snapshot(h.commands)
		h.commands:Finish(ally:GetObjId(), "cancelled", "fixture-cancel")
		if not self:Check(deltaMatches(h.commands, self.before, {cancelled = 1, active = -1})
			and commandCleared(bb) and not bb:Has("movePos"), "cancel-clears-owned-movement") then return end
		h.commands:Finish(ally:GetObjId(), "cancelled", "fixture-cancel-again")
		if not self:Check(deltaMatches(h.commands, self.before, {cancelled = 1, active = -1}), "cancel-terminal-is-idempotent") then return end
		self:PlaceFocusTarget(h, ally, nowMs, "lost-focus-ready")
	elseif self.stage == "lost-focus-ready" then
		self:IssueVisibleFocus(h, ally, nowMs, "lost-focus-executing")
	elseif self.stage == "lost-focus-executing" then
		if not self:WaitForExecution(h, ally, "focus", nowMs, "lost-target-order-executed-by-real-bt") then return end
		-- Force a real visibility loss beyond perception range, not a health change
		-- or direct condition invocation. Commands.Update must supply the terminal.
		local enemy = h.find(self.enemyId)
		enemy:setPosition(Vector3(20, 0, 34))
		enemy:SetVelocity(Vector3(0, 0, 0))
		self:Next("target-lost", nowMs)
	elseif self.stage == "target-lost" then
		if h.commands.active[ally:GetObjId()] ~= nil then
			if nowMs - self.since > 2000 then self:Check(false, "target-loss-cleanup-timeout") end
			return
		end
		if not self:Check(deltaMatches(h.commands, self.before, {failed = 1, active = -1}) and commandCleared(bb)
			and bb:GetString("__debug.commandClearReason") == "target-lost", "focus-target-loss-fails-once") then return end
		self:PlaceFocusTarget(h, ally, nowMs, "death-focus-ready")
	elseif self.stage == "death-focus-ready" then
		self:IssueVisibleFocus(h, ally, nowMs, "death-focus-executing")
	elseif self.stage == "death-focus-executing" then
		if not self:WaitForExecution(h, ally, "focus", nowMs, "death-target-order-executed-by-real-bt") then return end
		h.find(self.enemyId):SetHealth(0)
		self:Next("target-dead", nowMs)
	elseif self.stage == "target-dead" then
		if not self:Check(h.commands.active[ally:GetObjId()] == nil and commandCleared(bb)
			and deltaMatches(h.commands, self.before, {completed = 1, active = -1})
			and bb:GetString("__debug.commandClearReason") == "target-down", "focus-target-death-completes") then return end
		self.ownerId = h.allies[2]
		local owner = h.find(self.ownerId)
		if not self:Check(owner ~= nil and h.commands:Issue(owner, "rally", -1, h.anchors.fallback + Vector3(0, 0, 8), nowMs), "owner-death-order-accepted") then return end
		self.before = snapshot(h.commands)
		self:Next("owner-executing", nowMs)
	elseif self.stage == "owner-executing" then
		local owner = h.find(self.ownerId)
		if not self:WaitForExecution(h, owner, "rally", nowMs, "owner-order-executed-by-real-bt") then return end
		owner:SetHealth(0)
		self:Next("owner-dead", nowMs)
	elseif self.stage == "owner-dead" then
		local ownerBb = blackboard(h.find(self.ownerId))
		if not self:Check(h.commands.active[self.ownerId] == nil and commandCleared(ownerBb)
			and deltaMatches(h.commands, self.before, {failed = 1, active = -1})
			and (ownerBb == nil or ownerBb:GetString("__debug.commandClearReason") == "owner-down"), "owner-death-fails-active-order") then return end
		for _, id in ipairs(h.enemies) do local e = h.find(id); if e then e:SetHealth(0) end end
		self:Next("advance", nowMs)
	elseif self.stage == "advance" then
		if not self:Check(h.state == "ADVANCE" and h.wave == 1, "clear-alone-does-not-spawn-next-stage") then return end
		h.player:setPosition(h.anchors.trigger.center)
		self:Next("second", nowMs)
	elseif self.stage == "second" then
		if not self:Check(h.wave == 2 and #h.enemies == 4, "advance-triggers-front-guards") then return end
		for _, id in ipairs(h.enemies) do local e = h.find(id); if e then e:SetHealth(0) end end
		self:Next("regroup", nowMs)
	elseif self.stage == "regroup" then
		if not self:Check(h.state == "REGROUP", "clear-is-not-victory-without-regroup") then return end
		h.player:setPosition(h.anchors.goal)
		ally:setPosition(h.anchors.goal + Vector3(2, 0, 0))
		self:Next("victory", nowMs)
	elseif self.stage == "victory" then
		if not self:Check(h.state == "VICTORY" and GameManager:IsSimulationPaused(), "regroup-victory-freezes") then return end
		self.oldId = h.player:GetObjId()
		h.restart(false)
		self:Next("restart", nowMs)
	elseif self.stage == "restart" then
		if not self:Check(h.player:GetObjId() ~= self.oldId and h.find(self.oldId) == nil and h.state == "PREPARE"
			and #h.enemies == 0 and h.commands.stats.issued == 0 and next(h.commands.active) == nil
			and commandCleared(bb) and not bb:Has("sandbox19.holdPos"), "restart-new-identities-clean-orders") then return end
		h.start()
		if not self:Check(h.commands:Issue(ally, "rally", -1, Vector3(0, 0, -8), nowMs), "live-restart-order-accepted") then return end
		self:Next("restart-executing", nowMs)
	elseif self.stage == "restart-executing" then
		if not self:WaitForExecution(h, ally, "rally", nowMs, "live-restart-order-executed-by-real-bt") then return end
		self.oldId, self.oldOwnerId = h.player:GetObjId(), ally:GetObjId()
		h.restart(false)
		self:Next("restart-active", nowMs)
	elseif self.stage == "restart-active" then
		local countersClear = true
		for _, name in ipairs(COUNTERS) do countersClear = countersClear and h.commands.stats[name] == 0 end
		if not self:Check(h.state == "PREPARE" and h.find(self.oldId) == nil and h.find(self.oldOwnerId) == nil
			and countersClear and next(h.commands.active) == nil and commandCleared(bb), "restart-clears-live-order-and-identities") then return end
		h.start()
		h.player:SetHealth(0)
		self:Next("defeat", nowMs)
	elseif self.stage == "defeat" then
		if not self:Check(h.state == "DEFEAT", "commander-down-defeat") then return end
		self.done = true
		print("[Sandbox19ProductSelfTest] PASS all=true synthetic=true")
	end
end
return Test
