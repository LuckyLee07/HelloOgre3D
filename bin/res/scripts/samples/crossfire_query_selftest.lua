-- Synthetic query fixture for a fresh Sandbox20 first encounter.
-- New({ids={droneA,droneB,sentinel,...},find=findAgent}) / instance:Update(dtMs).
-- Keep physics running and suppress ordinary scoring. The fixture parks all given
-- actors via crossfire.fixture, kills droneB to check its real corpse collider,
-- and leaves the actors parked. Run in a fresh dedicated query-gate process and
-- quit after completion; do not restart or reuse this fixture for natural scoring.
-- The temporary mutable box is parked at (0,-100,0) after its queries and retained
-- by ObjectManager until normal process shutdown (Lua has no single-block delete).
local Test={}
Test.__index=Test
local function bb(a) return a:GetAIComponent():GetBlackboard() end
local function near(a,b) return math.abs(a-b)<.04 end
local function place(a,x,z,sentry)
 a:SetVelocity(Vector3(0,0,0))
 a:setPosition(Vector3(x,sentry and .82 or .72,z))
 a:SetForward(Vector3(1,0,0))
 a:GetAIComponent():ClearMovePosition()
 a:GetLocomotionComponent():SetPath(std.vector_Ogre__Vector3_(),false)
 local board=bb(a)
 board:SetBool("crossfire.fixture",true)
 board:SetBool("crossfire.shield",false)
 board:SetFloat("crossfire.damage",10)
 board:Remove("movePos")
 board:SetInt("crossfire.target",-1)
end
local function blocks()
 local ids={}
 local all=ObjectManager:getAllBlocks()
 for i=0,all:size()-1 do ids[all[i]:GetObjId()]=true end
 return ids
end
function Test:Record(name,ok,detail)
 if ok then self.passed=self.passed+1 else self.failed=self.failed+1 end
 print("[CrossfireQueries] case="..name.." result="..(ok and "PASS" or "FAIL")
  .." synthetic=true "..(detail or ""))
end
function Test:Finish()
 self.done=true
 SandboxObjects:ClearProjectiles()
 print(string.format("[CrossfireQueries] result=%s synthetic=true passed=%d failed=%d elapsedMs=%d",
  self.failed==0 and "PASS" or "FAIL",self.passed,self.failed,self.elapsed))
end
function Test.New(ctx)
 local self=setmetatable({ctx=ctx,actors={},passed=0,failed=0,elapsed=0,index=0,phase="next"},Test)
 for _,id in ipairs(ctx.ids) do
  local a=ctx.find(id)
  if a then bb(a):SetBool("crossfire.fixture",true);a:SetVelocity(Vector3(0,0,0)) end
 end
 for i=1,3 do
  local a=ctx.find(ctx.ids[i])
  if not a or a:GetHealth()<=30 or not a:GetWeaponComponent() then
   self:Record("ready",false,"slot="..i);self:Finish();return self
  end
  self.actors[i]=a
 end
 print("[CrossfireQueries] begin synthetic=true physics=real_bullet fixture=first_encounter")
 return self
end
function Test:Prepare()
 self.index=self.index+1
 if self.index>4 then self:Finish();return end
 SandboxObjects:ClearProjectiles()
 local a,b,s=self.actors[1],self.actors[2],self.actors[3]
 a:SetTeamId(1);b:SetTeamId(1);s:SetTeamId(2)
 place(a,-6,-7,false);place(b,0,-7,false);place(s,6,-7,true)
 self.target=s;self.expected=b
 if self.index==1 then
  place(s,0,-7,true);place(b,6,-7,false)
  self.target=b;self.expected=s;self.name="near_sentry_first"
 elseif self.index==2 then self.name="friendly_first"
 elseif self.index==3 then b:SetHealth(0);self.name="corpse_first"
 else place(b,0,-9,false);self.expected=s;self.name="clear_side_target" end
 self.phase="settle";self.phaseAt=self.elapsed
end
function Test:QueryAndFire()
 local a,b,s=self.actors[1],self.actors[2],self.actors[3]
 local muzzle=a:GetWeaponComponent():GetMuzzlePosition()
 local target=self.target:GetPosition()
 local point=Vector3(0,0,0)
 local first=SandboxRaycast:TraceProjectile(muzzle,target,a:GetObjId(),point)
 self:Record(self.name,first==self.expected:GetObjId(),
  string.format("first=%d expected=%d hit=(%.3f,%.3f,%.3f)",first,self.expected:GetObjId(),point.x,point.y,point.z))
 if self.index==1 then
  -- Start inside a real sentry: the overlap query must not skip its collider.
  local id=SandboxRaycast:TraceProjectile(s:GetPosition(),target,a:GetObjId(),point)
  self:Record("initial_overlap",id==s:GetObjId(),"first="..id)
  -- No Update or physics step occurs between teleport and either query. This
  -- mirrors initial planning/level changes while the game is paused.
  place(b,-3,-7,false)
  id=SandboxRaycast:TraceProjectile(muzzle,target,a:GetObjId(),point)
  self:Record("teleport_trace_without_step",id==b:GetObjId(),"first="..id)
  place(b,6,-7,false)
  -- Factory returns a mutable BlockObject; getAllBlocks() intentionally exposes
  -- const objects and must never be used to reposition authored scene equipment.
  local device=SandboxObjects:CreateBlockBox(1,2,1,1,1)
  self.fixtureBlock=device
  if device then
   local surface=device:GetObjId()
   device:SetMass(0)
   device:setPosition(Vector3(8,1,-7))
   local initial=SandboxRaycast:PickSurface(Vector3(8,14,-7),Vector3(8,-2,-7),point)
   self:Record("created_surface_without_step",initial==surface and near(point.y,2),
    "first="..initial.." y="..point.y)
   device:setPosition(Vector3(8,7,-7))
   local moved=SandboxRaycast:PickSurface(Vector3(8,14,-7),Vector3(8,-2,-7),point)
   self:Record("teleport_surface_without_step",moved==surface and near(point.y,8),
    "first="..moved.." y="..point.y)
   device:setPosition(Vector3(8,1,-7))
   local restored=SandboxRaycast:PickSurface(Vector3(8,14,-7),Vector3(8,-2,-7),point)
   self:Record("surface_restore_without_step",restored==surface and near(point.y,2),
    "first="..restored.." y="..point.y)
   device:setPosition(Vector3(0,-100,0))
   local cleared=SandboxRaycast:PickSurface(Vector3(8,14,-7),Vector3(8,-2,-7),point)
   self:Record("temporary_surface_parked",cleared>0 and cleared~=surface and near(point.y,0),
    "first="..cleared.." y="..point.y)
   print("[CrossfireQueries] temporaryBlock="..surface.." synthetic=true retainedUntilExit=true position=(0,-100,0)")
  else self:Record("teleport_surface_without_step",false,"reason=fixture_box_creation_failed") end
 elseif self.index==3 then
  local report=require("res.scripts.samples.crossfire_feedback.lua").Target(a,s,self.ctx.ids,self.ctx.find)
  self:Record("readable_wreck_reason",report and report.state=="wreck" and report.first==b:GetObjId(),"first="..tostring(report and report.first))
  local id=SandboxRaycast:PickSurface(Vector3(0,5,-7),Vector3(0,-2,-7),point)
  self:Record("pick_ignores_corpse",id>0 and id~=b:GetObjId() and near(point.y,0),"first="..id.." y="..point.y)
 elseif self.index==4 then
  local id=SandboxRaycast:TraceProjectile(a:GetPosition(),target,a:GetObjId(),point)
  self:Record("trace_ignores_self",id==s:GetObjId(),"first="..id)
  id=SandboxRaycast:PickSurface(Vector3(6,5,-7),Vector3(6,-2,-7),point)
  self:Record("pick_ignores_live_agent",id>0 and id~=s:GetObjId() and point.y>=-.12 and point.y<=.15,"first="..id.." y="..point.y)
  id=SandboxRaycast:PickSurface(Vector3(0,6,0),Vector3(0,-2,0),point)
  self:Record("pick_device_surface",id>0 and point.y>.5,"first="..id.." y="..point.y)
  local finish=Vector3(0,20,-3)
  id=SandboxRaycast:TraceProjectile(Vector3(0,20,-7),finish,a:GetObjId(),finish)
  self:Record("clear_output_alias",id==0 and near(finish.z,-3) and near(finish.y,20),"first="..id)
  id=SandboxRaycast:TraceProjectile(muzzle,muzzle,a:GetObjId(),point)
  self:Record("invalid_zero_length",id==-1,"first="..id)
 end
 self.hpB=b:GetHealth();self.hpS=s:GetHealth()
 local before=blocks()
 a:GetWeaponComponent():ShootBulletAt(target)
 self.shots={}
 for id in pairs(blocks()) do if not before[id] then self.shots[#self.shots+1]=id end end
 self.phase="flight";self.phaseAt=self.elapsed;self.checkedFlying=false
end
function Test:Evaluate()
 local b,s=self.actors[2],self.actors[3]
 local damageB=self.hpB-b:GetHealth()
 local damageS=self.hpS-s:GetHealth()
 local count=0
 local present=blocks()
 for _,id in ipairs(self.shots) do if present[id] then count=count+1 end end
 local expectedDamage=(self.index==1 or self.index==4) and 10 or 0
 self:Record(self.name.."_real_impact",#self.shots==1 and count==0 and near(damageB,0) and near(damageS,expectedDamage),
  string.format("shots=%d remaining=%d damageB=%.1f damageS=%.1f",#self.shots,count,damageB,damageS))
 if self.index==4 and not self.checkedFlying then
  self:Record("ignore_flying_projectile",false,"reason=no_flight_frame_observed")
 end
 self.phase="next"
end
function Test:Update(dt)
 if self.done then return true end
 self.elapsed=self.elapsed+math.max(0,dt or 0)
 for _,a in ipairs(self.actors) do
  bb(a):SetBool("crossfire.fixture",true)
  bb(a):SetInt("crossfire.clockMs",GameManager:getTimeInMillis())
 end
 if self.phase=="next" then self:Prepare()
 elseif self.phase=="settle" then
  if self.elapsed-self.phaseAt>=300 then self:QueryAndFire() end
 elseif self.phase=="flight" then
  local age=self.elapsed-self.phaseAt
  if self.index==4 and not self.checkedFlying and age>0 then
   local present=blocks()
   for _,id in ipairs(self.shots) do
    if present[id] then
     local a,s=self.actors[1],self.actors[3]
     local point=Vector3(0,0,0)
     local first=SandboxRaycast:TraceProjectile(a:GetWeaponComponent():GetMuzzlePosition(),s:GetPosition(),a:GetObjId(),point)
     self:Record("ignore_flying_projectile",first==s:GetObjId(),"first="..first.." projectile="..id)
     local all=ObjectManager:getAllBlocks()
     for i=0,all:size()-1 do
      if all[i]:GetObjId()==id then
       local p=all[i]:GetPosition()
       local surface=SandboxRaycast:PickSurface(p+Vector3(0,5,0),Vector3(p.x,-2,p.z),point)
       self:Record("pick_ignores_projectile",surface>0 and surface~=id and near(point.y,0),
        "first="..surface.." projectile="..id.." y="..point.y)
       break
      end
     end
     self.checkedFlying=true
     break
    end
   end
  end
  if age>=700 then self:Evaluate() end
 end
 return self.done
end
return Test
