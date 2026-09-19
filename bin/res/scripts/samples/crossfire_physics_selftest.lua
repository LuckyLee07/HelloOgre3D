-- Synthetic fixture only: exercises real WeaponComponent / Bullet collision paths.
-- New({ids = {droneA, droneB, sentinel}, find = findAgent}) returns an instance.
-- Keep simulation unpaused, call instance:Update(deltaMs), and skip natural match
-- scoring while it runs. The main sample must honor crossfire.fixture in both BT
-- actions. Restart the sample after completion; fixture actors deliberately stay idle.
local Test = {}
Test.__index = Test

local function bb(actor) return actor:GetAIComponent():GetBlackboard() end
local function blockIds()
 local result = {}
 local blocks = ObjectManager:getAllBlocks()
 for i=0,blocks:size()-1 do result[blocks[i]:GetObjId()] = true end
 return result
end
local function countTracked(ids)
 local present, count = blockIds(), 0
 for _,id in ipairs(ids) do if present[id] then count=count+1 end end
 return count
end
local function near(a,b) return math.abs(a-b)<0.001 end
local function place(actor,x,z,fx,fz,sentinel)
 actor:SetVelocity(Vector3(0,0,0))
 actor:setPosition(Vector3(x,sentinel and .82 or .72,z))
 actor:SetForward(Vector3(fx,0,fz))
 local board=bb(actor)
 board:SetBool("crossfire.fixture",true)
 board:Remove("movePos")
 board:SetInt("crossfire.target",-1)
 board:SetFloat("crossfire.charge",0)
 actor:GetAIComponent():ClearMovePosition()
 actor:GetLocomotionComponent():SetPath(std.vector_Ogre__Vector3_(),false)
end

function Test:Record(name,ok,detail)
 if ok then self.passed=self.passed+1 else self.failed=self.failed+1 end
 print("[CrossfirePhysics] case="..name.." result="..(ok and "PASS" or "FAIL")
  .." synthetic=true "..(detail or ""))
end
function Test:Finish()
 self.done=true
 SandboxObjects:ClearProjectiles()
 print(string.format("[CrossfirePhysics] result=%s synthetic=true passed=%d failed=%d elapsedMs=%d",
  self.failed==0 and "PASS" or "FAIL",self.passed,self.failed,self.elapsed))
end
function Test.New(ctx)
 local self=setmetatable({ctx=ctx,actors={},passed=0,failed=0,elapsed=0,
  caseIndex=0,phase="next",done=false},Test)
 for i=1,3 do
  local actor=ctx.find(ctx.ids[i])
  if not actor or actor:GetHealth()<=20 or not actor:GetWeaponComponent() then
   self:Record("fixture_ready",false,"reason=missing_or_unhealthy_actor slot="..i)
   self:Finish(); return self
  end
  self.actors[i]=actor
  bb(actor):SetBool("crossfire.fixture",true)
  bb(actor):SetFloat("crossfire.damage",10)
 end
 SandboxObjects:ClearProjectiles()
 print("[CrossfirePhysics] begin synthetic=true cases=6 simulation=real_bullet bt=fixture_idle")
 return self
end

function Test:Prepare()
 self.caseIndex=self.caseIndex+1
 if self.caseIndex>6 then self:Finish();return end
 SandboxObjects:ClearProjectiles()
 local a,b,s=self.actors[1],self.actors[2],self.actors[3]
 a:SetTeamId(1);b:SetTeamId(1);s:SetTeamId(2)
 bb(a):SetBool("crossfire.shield",false)
 bb(b):SetBool("crossfire.shield",false)
 bb(s):SetBool("crossfire.shield",true)
 bb(s):SetFloat("crossfire.shieldCos",.5)
 place(a,-2,-7,1,0,false)
 place(b,-5,-10,0,1,false)
 place(s,2,-7,-1,0,true)
 self.target=s
 if self.caseIndex==1 then
  -- This 4 m shot must be consumed at the friendly drone before the far wall.
  place(b,2,-7,-1,0,false);place(s,-5,-10,0,1,true)
  self.target=b;self.name="friendly_no_damage"
 elseif self.caseIndex==2 then self.name="front_shield"
 elseif self.caseIndex==3 then
  s:SetForward(Vector3(1,0,0));self.name="rear_single_damage"
 elseif self.caseIndex==4 then
  -- The authored central core is a real static collider between these actors.
  place(a,0,-4,0,1,false);place(s,0,4,0,1,true)
  self.name="core_blocks_projectile"
 elseif self.caseIndex==5 then self.name="clear_projectiles"
 else self.name="projectile_ttl" end
 self.phase="settle";self.phaseAt=self.elapsed
 self.shotIds={};self.firstGoneMs=nil;self.aliveAt3500=false
end

function Test:Fire()
 local targetBoard=bb(self.target)
 self.healthBefore=self.target:GetHealth()
 self.hitsBefore=targetBoard:GetInt("crossfire.hitCount",0)
 self.blocksBefore=targetBoard:GetInt("crossfire.blockedCount",0)
 local before=blockIds()
 local shooter=self.actors[1]
 local target=self.target:GetPosition()
 if self.caseIndex>=5 then
  -- Upward shot avoids all actors and courtyard walls for cleanup/TTL checks.
  target=shooter:GetPosition()+Vector3(0,100,0)
 end
 shooter:GetWeaponComponent():ShootBulletAt(target)
 for id in pairs(blockIds()) do
  if not before[id] then self.shotIds[#self.shotIds+1]=id end
 end
 self.emitted=#self.shotIds
 self.phase="flight";self.phaseAt=self.elapsed
 if self.caseIndex==5 then
  self.clearCount=SandboxObjects:ClearProjectiles()
  self.clearAgain=SandboxObjects:ClearProjectiles()
 end
end

function Test:Evaluate()
 local board=bb(self.target)
 local damage=self.healthBefore-self.target:GetHealth()
 local hits=board:GetInt("crossfire.hitCount",0)-self.hitsBefore
 local blocks=board:GetInt("crossfire.blockedCount",0)-self.blocksBefore
 local remaining=countTracked(self.shotIds)
 local emittedOnce=self.emitted==1
 local ok=false
 if self.caseIndex==1 then
  ok=emittedOnce and near(damage,0) and hits==0 and blocks==0 and remaining==0
   and self.firstGoneMs~=nil and self.firstGoneMs<=170
 elseif self.caseIndex==2 then
  ok=emittedOnce and near(damage,0) and hits==0 and blocks==1 and remaining==0
 elseif self.caseIndex==3 then
  ok=emittedOnce and near(damage,10) and hits==1 and blocks==0 and remaining==0
 elseif self.caseIndex==4 then
  ok=emittedOnce and near(damage,0) and hits==0 and blocks==0 and remaining==0
   and self.firstGoneMs~=nil and self.firstGoneMs<=170
 elseif self.caseIndex==5 then
  ok=emittedOnce and self.clearCount==1 and self.clearAgain==0 and remaining==0
   and near(damage,0) and hits==0 and blocks==0
 else
  ok=emittedOnce and self.aliveAt3500 and remaining==0 and self.firstGoneMs~=nil
   and self.firstGoneMs>=3900 and self.firstGoneMs<=4400 and near(damage,0)
 end
 local detail=string.format("emitted=%d damage=%.1f hits=%d blocks=%d remaining=%d firstGoneMs=%d",
  self.emitted,damage,hits,blocks,remaining,self.firstGoneMs or -1)
 if self.caseIndex==5 then detail=detail.." clearCount="..self.clearCount.." clearAgain="..self.clearAgain end
 if self.caseIndex==6 then detail=detail.." aliveAt3500="..tostring(self.aliveAt3500) end
 self:Record(self.name,ok,detail)
 if self.caseIndex==3 then
  -- Keep the single rear shot under observation for 1.5 seconds: no later contact
  -- manifold or delayed body cleanup may apply damage a second time.
  self:Record("single_consumption",emittedOnce and hits==1 and near(damage,10) and remaining==0,
   "observedMs=1500 "..detail)
 end
 self.phase="next"
end

function Test:Update(dt)
 if self.done then return true end
 self.elapsed=self.elapsed+math.max(0,dt or 0)
 for _,actor in ipairs(self.actors) do
  bb(actor):SetBool("crossfire.fixture",true)
  bb(actor):SetInt("crossfire.clockMs",GameManager:getTimeInMillis())
 end
 if self.phase=="next" then self:Prepare()
 elseif self.phase=="settle" then
  if self.elapsed-self.phaseAt>=300 then self:Fire() end
 elseif self.phase=="flight" then
  local age=self.elapsed-self.phaseAt
  local remaining=countTracked(self.shotIds)
  if remaining==0 and self.firstGoneMs==nil then self.firstGoneMs=age end
  if age>=3500 and remaining==1 then self.aliveAt3500=true end
  local duration=self.caseIndex==6 and 4600 or (self.caseIndex==3 and 1500 or 700)
  if age>=duration then self:Evaluate() end
 end
 return self.done
end
return Test
