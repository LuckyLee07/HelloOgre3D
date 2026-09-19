require("res.scripts.ai.decision.ActionStatus.lua")
require("res.scripts.ai.decision.MoveHelpers.lua")
local serial, acc, elapsed, best = -1, nil, 0, math.huge
local function stop(owner, bb, state)
 owner:GetAIComponent():ClearMovePosition()
 bb:Remove("movePos")
 owner:GetLocomotionComponent():SetPath(std.vector_Ogre__Vector3_(), false)
 owner:SetVelocity(Vector3(0, owner:GetVelocity().y, 0))
 bb:SetString("crossfire.state", state)
 bb:SetString("command.status", state == "BLOCKED" and "failed" or "completed")
end
local function build(owner, bb)
 serial = bb:GetInt("command.serial", 0)
 elapsed, best, acc = 0, math.huge, Vector3(0,0,0)
 local target = bb:GetVec3("movePos")
 if not MoveHelpers.BuildAndSetPath(owner, owner:GetPosition(), target) then
  stop(owner, bb, "BLOCKED"); return false
 end
 bb:SetString("crossfire.state", "MOVING")
 bb:SetString("command.status", "executing")
 print("[CrossfireOrder] agent=" .. owner:GetObjId() .. " serial=" .. serial .. " result=executing")
 return true
end
function OnInitialize(owner, bb) build(owner, bb) end
function OnUpdate(deltaMs, owner, bb)
 if bb:GetBool("crossfire.fixture",false) then owner:SetVelocity(Vector3(0,owner:GetVelocity().y,0)); return ActionStatus.RUNNING end
 if owner:GetHealth() <= 0 or not bb:Has("movePos") then
  stop(owner, bb, owner:GetHealth() <= 0 and "OFFLINE" or "READY")
  return ActionStatus.TERMINATED
 end
 if serial ~= bb:GetInt("command.serial", 0) and not build(owner, bb) then return 3 end
 local d = bb:GetVec3("movePos") - owner:GetPosition(); d.y=0
 local dist = d:length()
 if dist < 0.65 then
  stop(owner, bb, "COVERING")
  print("[CrossfireOrder] agent=" .. owner:GetObjId() .. " result=arrived")
  return ActionStatus.TERMINATED
 end
 if dist < best - 0.15 then best, elapsed = dist, 0 else elapsed = elapsed + deltaMs end
 if elapsed > 6000 then stop(owner, bb, "BLOCKED"); return 3 end
 MoveHelpers.ApplySteering(owner, acc, deltaMs)
 return ActionStatus.RUNNING
end
function OnCleanUp(owner, bb)
 owner:SetVelocity(Vector3(0,owner:GetVelocity().y,0))
 bb:SetFloat("crossfire.charge", 0)
end
