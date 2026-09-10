-- Sandbox19-only recovery window. Persistent state belongs to each agent's
-- Blackboard, so restart/deletion cannot leave a Lua userdata cache behind.
require("res.scripts.ai.behavior.SoldierConditions.lua")
local Policy = {}
local START = "sandbox19.retreatStartedMs"
local EXHAUSTED = "sandbox19.retreatExhausted"

function Policy.Update(agent, bb, nowMs, durationMs)
	if agent:GetHealth() <= 0 or not SoldierConditions.IsCriticalHealth(agent, bb) then
		if bb:Has(START) then
			bb:Remove(START)
			bb:Remove(EXHAUSTED)
			print("[Sandbox19Retreat] id=" .. agent:GetObjId() .. " phase=RESET")
		end
		return
	end
	if not bb:Has(START) then
		bb:SetInt(START, nowMs)
		bb:SetBool(EXHAUSTED, false)
		print("[Sandbox19Retreat] id=" .. agent:GetObjId() .. " phase=BEGIN durationMs=" .. durationMs)
	end
	if not bb:GetBool(EXHAUSTED, false) and nowMs - bb:GetInt(START, 0) >= durationMs then
		bb:SetBool(EXHAUSTED, true)
		print("[Sandbox19Retreat] id=" .. agent:GetObjId() .. " phase=EXHAUSTED")
	end
end

return Policy
