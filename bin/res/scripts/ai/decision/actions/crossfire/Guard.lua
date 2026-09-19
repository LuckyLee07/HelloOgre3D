require("res.scripts.ai.decision.ActionStatus.lua")
local commandSerial=-1
local phase, elapsed, lockId, aim, bursts = "ready", 0, -1, nil, 0
local function find(id)
 local agents=ObjectManager:getAllAgents()
 for i=0,agents:size()-1 do local a=agents[i]; if a:GetObjId()==id then return a end end
end
local function reset(bb)
 phase,elapsed,lockId,aim,bursts="ready",0,-1,nil,0
 bb:SetFloat("crossfire.charge",0)
 bb:SetInt("crossfire.lockId",-1)
end
function OnInitialize(owner,bb) reset(bb) end
function OnUpdate(dt,owner,bb)
 if bb:GetBool("crossfire.fixture",false) then owner:SetVelocity(Vector3(0,owner:GetVelocity().y,0)); return ActionStatus.RUNNING end
 owner:SetVelocity(Vector3(0,owner:GetVelocity().y,0))
 if owner:GetHealth()<=0 then bb:SetString("crossfire.state","OFFLINE"); return ActionStatus.RUNNING end
 local command=bb:GetInt("command.serial",0)
 if command~=commandSerial then reset(bb); commandSerial=command end
 local ai=owner:GetAIComponent()
 local sentinel=bb:GetBool("crossfire.sentinel",false)
 elapsed=elapsed+dt
 if phase=="cooldown" then
  bb:SetString("crossfire.state","COOLING")
  if elapsed >= (sentinel and bb:GetFloat("crossfire.cooldownMs",1400) or 330) then reset(bb) end
  return ActionStatus.RUNNING
 end
 local target=lockId>0 and find(lockId) or nil
 if target==nil then
  local requested=bb:GetInt("crossfire.target",-1)
  target=requested>0 and find(requested) or ai:GetEnemy()
 end
 if target==nil or target:GetHealth()<=0 or not ai:CanSeeEnemy(target:GetObjId()) then
  reset(bb); bb:SetString("crossfire.state","COVERING")
  if bb:GetInt("crossfire.target",-1)>0 then
   local completed=target~=nil and target:GetHealth()<=0
   bb:SetInt("crossfire.target",-1)
   bb:SetString("command.status",completed and "completed" or "failed")
  end
  return ActionStatus.RUNNING
 end
 local direction=target:GetPosition()-owner:GetPosition(); direction.y=0
 if direction:squaredLength()>18*18 then
  reset(bb); bb:SetString("crossfire.state","OUT OF RANGE")
  if bb:GetInt("crossfire.target",-1)>0 then
   bb:SetInt("crossfire.target",-1); bb:SetString("command.status","failed")
  end
  return ActionStatus.RUNNING
 end
 if phase=="ready" then
  bb:SetString("crossfire.state","TRACKING")
  if owner:GetLocomotionComponent():FaceDirection(direction,dt*(sentinel and 0.16 or 1)) then
   phase,elapsed,lockId="charging",0,target:GetObjId()
   aim=Vector3(target:GetPosition().x,target:GetPosition().y,target:GetPosition().z)
   bb:SetVec3("crossfire.aim",aim); bb:SetInt("crossfire.lockId",lockId)
  end
 elseif phase=="charging" then
  local delay=sentinel and bb:GetFloat("crossfire.chargeMs",1100) or 180
  bb:SetString("crossfire.state","LOCKING")
  bb:SetFloat("crossfire.charge",math.min(1,elapsed/delay))
  if not sentinel then
   owner:GetLocomotionComponent():FaceDirection(direction,dt)
   aim=target:GetPosition()
  end
  if elapsed>=delay then phase,elapsed,bursts="burst",0,0 end
 elseif phase=="burst" then
  if elapsed>=bursts*140 then
   local weapon=owner:GetWeaponComponent()
   if not weapon:HasAmmo() then weapon:RestoreAmmo() end
   weapon:ConsumeAmmo(1)
   weapon:ShootBulletAt(aim)
   bb:SetInt("crossfire.shotCount",bb:GetInt("crossfire.shotCount",0)+1)
   bb:SetInt("crossfire.lastShotMs",GameManager:getTimeInMillis())
   bb:SetString("crossfire.state","FIRING")
   bursts=bursts+1
   if bursts>=3 then phase,elapsed="cooldown",0; bb:SetFloat("crossfire.charge",0) end
  end
 end
 return ActionStatus.RUNNING
end
function OnCleanUp(owner,bb) reset(bb) end
