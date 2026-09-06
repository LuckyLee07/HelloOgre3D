-- Opt-in whole-match/lifecycle regression, separate from M1's synthetic cases.
local Test = {}
Test.__index = Test

function Test.New()
	local rounds = tonumber(os.getenv("HELLO_SANDBOX19_STABILITY_ROUNDS"))
	if rounds == nil or rounds < 1 then return nil end
	if os.getenv("HELLO_SANDBOX_SMOKE_TEST") == "1"
		or os.getenv("HELLO_SANDBOX19_OBSERVATION_SELF_TEST") == "1" then
		error("[Sandbox19Stability] cannot combine with synthetic M1/smoke tests")
	end
	return setmetatable({ rounds = math.floor(rounds), completed = 0, nextCheckMs = 0,
		timeoutMs = (tonumber(os.getenv("HELLO_SANDBOX19_STABILITY_TIMEOUT")) or 120) * 1000,
		scripted = os.getenv("HELLO_SANDBOX19_STABILITY_SCRIPTED_VICTORY") == "1", maxWave = 0 }, Test)
end

function Test:Fail(reason)
	self.done = true
	print("[Sandbox19Stability] FAIL reason=" .. reason)
end

function Test:Step(ctx, nowMs)
	if self.done or nowMs < self.nextCheckMs then return end
	self.nextCheckMs = nowMs + 100
	local agents = ObjectManager:getAllAgents()
	local liveIds = {}
	for i = 0, agents:size() - 1 do
		local agent = agents[i]
		local id, pos, velocity = agent:GetObjId(), agent:GetPosition(), agent:GetVelocity()
		liveIds[id] = true
		for _, value in ipairs({pos.x, pos.y, pos.z, velocity.x, velocity.y, velocity.z}) do
			if value ~= value or math.abs(value) == math.huge then
				self:Fail("non-finite-transform agent=" .. tostring(id))
				return
			end
		end
	end

	if self.playerId ~= ctx.playerId then
		if self.previousIds ~= nil then
			for id in pairs(self.previousIds) do
				if liveIds[id] then self:Fail("stale-agent-after-restart id=" .. id); return end
			end
			print("[Sandbox19Stability] restart-clean=true")
		end
		self.playerId, self.roundStartMs, self.roundMaxWave = ctx.playerId, nowMs, 0
		self.awaitRestart, self.nextSnapshotMs = false, nil
	end
	if self.awaitRestart then
		if nowMs - self.restartRequestedMs > 2000 then self:Fail("restart-timeout") end
		return
	end
	self.roundMaxWave = math.max(self.roundMaxWave, ctx.wave)
	self.maxWave = math.max(self.maxWave, ctx.wave)
	if nowMs - self.roundStartMs >= 60000 and nowMs >= (self.nextSnapshotMs or 0) then
		self.nextSnapshotMs = nowMs + 60000
		print("[Sandbox19Stability] stalled-snapshot elapsedMs=" .. (nowMs - self.roundStartMs))
		for i = 0, agents:size() - 1 do
			local agent = agents[i]
			local pos, weapon = agent:GetPosition(), agent:GetWeaponComponent()
			print("[Sandbox19StabilityAgent] id=" .. agent:GetObjId() .. " team=" .. agent:GetTeamId() ..
				" hp=" .. agent:GetHealth() .. " pos=" .. pos.x .. "," .. pos.y .. "," .. pos.z ..
				" ammo=" .. (weapon ~= nil and weapon:GetAmmo() or -1))
		end
		local summary = ObjectManager:buildAiRuntimeDebugSummary(16)
		for line in string.gmatch(tostring(summary), "[^\r\n]+") do print(line) end
	end
	if nowMs - self.roundStartMs > self.timeoutMs then self:Fail("round-timeout"); return end

	-- This mode only validates all-wave victory/restart transitions. Its result
	-- must not be reported as a naturally won match or combat-quality evidence.
	if self.scripted and ctx.state == "WAVE" then ctx.clearWave() end
	if ctx.state ~= "VICTORY" and ctx.state ~= "DEFEAT" then return end
	if self.roundMaxWave < 1 then self:Fail("terminal-before-first-wave"); return end
	if self.scripted and ctx.state ~= "VICTORY" then self:Fail("scripted-victory-not-reached"); return end
	self.completed = self.completed + 1
	print("[Sandbox19Stability] round=" .. self.completed .. " result=" .. ctx.state ..
		" maxWave=" .. self.roundMaxWave .. " elapsedMs=" .. (nowMs - self.roundStartMs) ..
		" scripted=" .. tostring(self.scripted))
	if self.completed >= self.rounds then
		self.done = true
		print("[Sandbox19Stability] PASS rounds=" .. self.completed .. " maxWave=" .. self.maxWave ..
			" scripted=" .. tostring(self.scripted))
	else
		self.previousIds, self.awaitRestart, self.restartRequestedMs = liveIds, true, nowMs
		ctx.restart()
	end
end

return Test
