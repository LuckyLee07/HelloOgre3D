-- Sample-local orders and guard policy; shared chapter conditions stay unchanged.
require("res.scripts.ai.behavior.SoldierConditions.lua")
Sandbox19CommandConditions = {}
setmetatable(Sandbox19CommandConditions, {__index = SoldierConditions})
local C = Sandbox19CommandConditions

local function find(id)
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		if agents[i]:GetObjId() == id then return agents[i] end
	end
	return nil
end
local function active(agent, bb, kind)
	return agent:GetHealth() > 0 and bb:Has("command.issuedMs") and bb:GetString("command.kind") == kind
end
local function executing(agent, bb)
	if bb:GetString("command.status") ~= "executing" then
		bb:SetString("command.status", "executing")
		local simulationMs = GameManager:getTimeInMillis()
		local issuedMs = bb:GetInt("command.issuedMs", simulationMs)
		print("[Sandbox19Order] agent=" .. agent:GetObjId() .. " kind=" .. bb:GetString("command.kind")
			.. " result=executing source=bt-condition simulationMs=" .. simulationMs
			.. " issuedMs=" .. issuedMs .. " latencyMs=" .. math.max(0, simulationMs - issuedMs))
	end
	return true
end
function C.HasCommandFocus(agent, bb)
	if not active(agent, bb, "focus") then return false end
	local id = bb:GetObjectId("command.focusTargetId", -1)
	if not agent:GetAIComponent():CanSeeEnemy(id) then return false end
	local enemy = find(id)
	if enemy == nil or enemy:GetHealth() <= 0 then return false end
	bb:SetAgent("enemy", enemy)
	return executing(agent, bb)
end
function C.HasCommandRetreat(agent, bb)
	return active(agent, bb, "retreat") and bb:Has("movePos") and executing(agent, bb)
end
function C.HasCommandRally(agent, bb)
	return active(agent, bb, "rally") and bb:Has("movePos") and executing(agent, bb)
end
function C.IsCriticalHealth(agent, bb)
	return SoldierConditions.IsCriticalHealth(agent, bb) and not bb:GetBool("sandbox19.retreatExhausted", false)
end
function C.CanShootEnemy(agent, bb)
	local enemy = bb:GetAgent("enemy")
	if enemy == nil then return false end
	-- Resolve the raw BB pointer before dereferencing, including after restarts.
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		if agents[i] == enemy then
			local d = enemy:GetPosition() - agent:GetPosition()
			d.y = 0
			return d:squaredLength() <= 100 and agent:GetAIComponent():CanSeeEnemy(enemy:GetObjId())
		end
	end
	return false
end
function C.HasEnemy(agent, bb)
	if not agent:GetAIComponent():HasEnemy("default") then bb:Remove("enemy"); return false end
	local enemy = agent:GetAIComponent():GetEnemy()
	if enemy == nil then return false end
	local anchor = bb:Has("sandbox19.anchorPos") and bb:GetVec3("sandbox19.anchorPos") or agent:GetPosition()
	local d = enemy:GetPosition() - anchor
	local radius = bb:Has("sandbox19.holdPos") and 6 or 14
	if d:squaredLength() > radius * radius then bb:Remove("enemy"); return false end
	bb:SetAgent("enemy", enemy)
	return true
end
function C.ShouldReturnToPost(agent, bb)
	if not bb:Has("sandbox19.anchorPos") then return false end
	local target = bb:GetVec3("sandbox19.anchorPos")
	local d = target - agent:GetPosition()
	d.y = 0
	if d:squaredLength() <= 4 then return false end
	bb:SetVec3("movePos", target)
	return true
end
