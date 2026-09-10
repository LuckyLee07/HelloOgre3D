-- Semantic order lifecycle. Only this module owns command.*; the BT executes it.
local Commands = {}
Commands.__index = Commands

local function flatDistance(a, b)
	local dx, dz = a.x - b.x, a.z - b.z
	return math.sqrt(dx * dx + dz * dz)
end

function Commands.New(host)
	return setmetatable({host = host, active = {}, serial = 0,
		stats = {issued = 0, completed = 0, failed = 0, replaced = 0, cancelled = 0, active = 0}}, Commands)
end

function Commands:Finish(id, result, reason)
	local order = self.active[id]
	if order == nil then return end
	local agent = self.host.find(id)
	local bb = agent ~= nil and agent:GetAIComponent():GetBlackboard() or nil
	if bb ~= nil then
		-- Clear only the movement request installed by this order.
		if bb:Has("command.targetPos") then
			agent:GetAIComponent():ClearMovePosition()
			bb:Remove("movePos")
			agent:SetVelocity(Vector3(0, 0, 0))
		end
		for _, key in ipairs({"command.kind", "command.issuedMs", "command.focusTargetId",
			"command.targetPos", "command.status", "command.semantic"}) do bb:Remove(key) end
		bb:SetString("__debug.commandClearReason", reason)
		bb:SetString("sandbox19.lastOrderResult", result)
	end
	self.active[id] = nil
	self.stats[result] = self.stats[result] + 1
	self.stats.active = self.stats.active - 1
	print(string.format("[Sandbox19Order] serial=%d agent=%d kind=%s result=%s reason=%s",
		order.serial, id, order.kind, result, reason))
	if result == "failed" then self.host.hint("Order failed: " .. reason, "failed") end
end

function Commands:Issue(agent, kind, targetId, position, nowMs)
	local id, ai = agent:GetObjId(), agent:GetAIComponent()
	local bb = ai:GetBlackboard()
	if kind == "focus" and not ai:CanSeeEnemy(targetId) then return false, "target not visible to this ally" end
	if kind ~= "focus" then
		local path = std.vector_Ogre__Vector3_()
		if not SandboxNav:FindPath("default", agent:GetPosition(), position, path) or path:size() == 0
			or flatDistance(path[path:size() - 1], position) > 1.5 then
			return false, "destination unreachable"
		end
	end
	self:Finish(id, "replaced", "new-order")
	bb:Remove("sandbox19.holdPos")
	bb:SetString("command.kind", kind)
	bb:SetString("command.status", "accepted")
	bb:SetBool("command.semantic", true)
	bb:SetInt("command.issuedMs", nowMs)
	bb:SetString("__debug.navStatus", "pending")
	if kind == "focus" then
		bb:SetObjectId("command.focusTargetId", targetId)
		bb:Remove("movePos")
	else
		bb:SetVec3("command.targetPos", position)
		bb:SetVec3("movePos", position)
		bb:SetVec3("sandbox19.anchorPos", position)
	end
	self.serial = self.serial + 1
	self.active[id] = {kind = kind, targetId = targetId, position = position, serial = self.serial,
		issuedMs = nowMs, progressMs = nowMs, bestDistance = position and flatDistance(agent:GetPosition(), position) or 0}
	self.stats.issued = self.stats.issued + 1
	self.stats.active = self.stats.active + 1
	print(string.format("[Sandbox19Order] serial=%d agent=%d kind=%s result=accepted target=%d",
		self.serial, id, kind, targetId or -1))
	return true
end

function Commands:Update(nowMs)
	local ids = {}
	for id in pairs(self.active) do ids[#ids + 1] = id end
	for _, id in ipairs(ids) do
		local order, agent = self.active[id], self.host.find(id)
		if agent == nil or agent:GetHealth() <= 0 then
			self:Finish(id, "failed", "owner-down")
		else
			local ai = agent:GetAIComponent()
			local bb = ai:GetBlackboard()
			if order.kind == "focus" then
				local target = self.host.find(order.targetId)
				if target == nil or target:GetHealth() <= 0 then
					self:Finish(id, "completed", "target-down")
				elseif not ai:CanSeeEnemy(order.targetId) then
					self:Finish(id, "failed", "target-lost")
				end
			else
				local distance = flatDistance(agent:GetPosition(), order.position)
				if distance <= 1.6 then
					if order.kind == "retreat" then bb:SetVec3("sandbox19.holdPos", order.position) end
					self:Finish(id, "completed", order.kind == "retreat" and "arrived-holding" or "arrived")
				elseif bb:GetString("__debug.navStatus") == "failure"
					and bb:GetInt("__debug.navTimeMs", -1) >= order.issuedMs then
					self:Finish(id, "failed", "path-failed")
				elseif distance < order.bestDistance - 0.35 then
					order.bestDistance, order.progressMs = distance, nowMs
				elseif nowMs - order.progressMs > 10000 then
					self:Finish(id, "failed", "no-progress")
				end
			end
		end
	end
end

function Commands:Clear(result, reason)
	local ids = {}
	for id in pairs(self.active) do ids[#ids + 1] = id end
	for _, id in ipairs(ids) do self:Finish(id, result or "cancelled", reason or "cancelled") end
end

return Commands
