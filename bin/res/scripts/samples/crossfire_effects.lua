-- Fixed-capacity event sprites. No collision, trajectory, random event or clock ownership.
-- Call Release BEFORE clearing MGR_OBJ_BLOCK; it deliberately never touches objects.
-- Reset reuses the same pool on a same-scene retry. Every position/direction input
-- is copied into ordinary numbers, never retained as an actor or Vector3 userdata.
local Effects={}
Effects.__index=Effects
local order={"muzzle","shield","hit","destroy"}
local config={
 muzzle={count=8,width=.65,height=.95,life=90,material="Crossfire/FxMuzzle"},
 shield={count=8,width=1.15,height=1.15,life=220,material="Crossfire/FxShield"},
 hit={count=8,width=.72,height=.72,life=150,material="Crossfire/FxHit"},
 destroy={count=4,width=2.35,height=2.35,life=620,material="Crossfire/FxBurst"},
}
local CAPACITY=28
local function finite(v)
 return type(v)=="number" and v==v and v~=math.huge and v~=-math.huge
end
local function copyPoint(v)
 if type(v)~="table" and type(v)~="userdata" then return nil end
 local ok,x,y,z=pcall(function() return v.x,v.y,v.z end)
 if not ok or not finite(x) or not finite(y) or not finite(z) then return nil end
 return {x=x,y=y,z=z}
end
local function multiply(a,b)
 return {w=a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z,
  x=a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,
  y=a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,
  z=a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w}
end
local yaw,pitch=math.rad(178),math.rad(-42)
local camera={w=math.cos(yaw/2)*math.cos(pitch/2),x=math.cos(yaw/2)*math.sin(pitch/2),
 y=math.sin(yaw/2)*math.cos(pitch/2),z=-math.sin(yaw/2)*math.sin(pitch/2)}
local right={x=math.cos(yaw),y=0,z=-math.sin(yaw)}
local up={x=math.sin(yaw)*math.sin(pitch),y=math.cos(pitch),z=math.cos(yaw)*math.sin(pitch)}
local back={x=math.sin(yaw)*math.cos(pitch),y=-math.sin(pitch),z=math.cos(yaw)*math.cos(pitch)}
-- The vendored PlaneGenerator has normal +Y, UV U=+Z and V=+X.
-- This 120-degree axis permutation maps them to camera +Z, +X and +Y.
local uvBasis={w=.5,x=.5,y=.5,z=.5}
local facing=multiply(camera,uvBasis)
local function orientation(kind,forward)
 if kind~="muzzle" or not forward then return facing end
 local x=forward.x*right.x+forward.y*right.y+forward.z*right.z
 local y=forward.x*up.x+forward.y*up.y+forward.z*up.z
 if not finite(x) or not finite(y) or x*x+y*y<.000001 then return facing end
 -- The muzzle's long texture V axis follows the shot's screen projection.
 local angle=math.atan2(-x,y)
 local roll={w=math.cos(angle/2),x=0,y=0,z=math.sin(angle/2)}
 return multiply(multiply(camera,roll),uvBasis)
end
local function engineQuaternion(q)
 -- Global Quaternion is an Euler helper, so its three arguments are explicit.
 local value=Quaternion(0,0,0)
 value.w=q.w;value.x=q.x;value.y=q.y;value.z=q.z
 return value
end
local function hide(self,slot)
 if not slot.active then return end
 slot.object:setPosition(Vector3(0,-50,0))
 slot.active=false;slot.pos=nil;slot.forward=nil;slot.rotation=nil;slot.dirty=false
 self._stats.active=self._stats.active-1
end
local function render(slot,nowMs)
 if slot.dirty then
  local p=slot.pos
  -- A small view-normal bias avoids contact-surface z-fighting. It never moves
  -- down the firing line and cannot imply a projectile passed through a blocker.
  slot.object:setPosition(Vector3(p.x+back.x*.025,p.y+back.y*.025,p.z+back.z*.025))
  slot.object:setOrientation(engineQuaternion(slot.rotation))
  slot.dirty=false
 end
 local stage=math.min(4,1+math.floor(math.max(0,nowMs-slot.born)/slot.spec.life*4))
 if stage~=slot.stage then
  slot.object:setMaterial(slot.spec.material..stage)
  slot.stage=stage
 end
end
function Effects.New()
 local self=setmetatable({_groups={},_slots={},_lastNow=nil,_released=false,
  _stats={capacity=CAPACITY,allocated=0,active=0,peak=0,emitted=0,reused=0,evicted=0,
   expired=0,dropped=0,resetCount=0,rewinds=0}},Effects)
 for _,kind in ipairs(order) do
  local spec=config[kind]
  local group={};self._groups[kind]=group
  for i=1,spec.count do
   local object=SandboxObjects:CreateVisualPlane(spec.width,spec.height)
   if object then
    object:setPosition(Vector3(0,-50,0))
    object:setOrientation(engineQuaternion(facing))
    object:setMaterial(spec.material..4)
    local slot={object=object,kind=kind,spec=spec,active=false,used=false,stage=4}
    group[#group+1]=slot;self._slots[#self._slots+1]=slot
    self._stats.allocated=self._stats.allocated+1
   end
  end
 end
 return self
end
function Effects:Update(nowMs)
 if self._released or not finite(nowMs) or nowMs<0 then return false end
 if self._lastNow and nowMs<self._lastNow then
  -- Restarted clocks must never resurrect an old event.
  self:Reset();self._stats.rewinds=self._stats.rewinds+1
 end
 self._lastNow=nowMs
 for _,slot in ipairs(self._slots) do
  if slot.active then
   if nowMs-slot.born>=slot.spec.life then
    hide(self,slot);self._stats.expired=self._stats.expired+1
   else render(slot,nowMs) end
  end
 end
 return true
end
function Effects:Emit(kind,pos,forward,nowMs)
 local position=copyPoint(pos)
 if self._released or not config[kind] or not position or not finite(nowMs) or nowMs<0
  or (self._lastNow and nowMs<self._lastNow) then
  self._stats.dropped=self._stats.dropped+1;return false
 end
 local direction=copyPoint(forward)
 self:Update(nowMs)
 local group=self._groups[kind]
 local chosen=nil
 for _,slot in ipairs(group) do
  if not slot.active then chosen=slot;break end
  if not chosen or slot.serial<chosen.serial then chosen=slot end
 end
 if not chosen then self._stats.dropped=self._stats.dropped+1;return false end
 if chosen.active then
  hide(self,chosen);self._stats.evicted=self._stats.evicted+1
 end
 if chosen.used then self._stats.reused=self._stats.reused+1 end
 local stats=self._stats
 stats.emitted=stats.emitted+1;stats.active=stats.active+1;stats.peak=math.max(stats.peak,stats.active)
 chosen.used=true;chosen.active=true;chosen.pos=position;chosen.forward=direction
 chosen.born=nowMs;chosen.serial=stats.emitted;chosen.rotation=orientation(kind,direction);chosen.dirty=true
 render(chosen,nowMs)
 return true
end
function Effects:Reset()
 if self._released then return false end
 for _,slot in ipairs(self._slots) do hide(self,slot) end
 self._lastNow=nil;self._stats.resetCount=self._stats.resetCount+1
 return true
end
function Effects:Release()
 if self._released then return end
 -- The caller is about to clear all blocks. Do not hide or dereference any plane:
 -- this also makes repeated Release and late callbacks safe after scene teardown.
 self._slots={};self._groups={};self._lastNow=nil;self._released=true
 self._stats.active=0;self._stats.allocated=0
end
function Effects:Stats()
 local result={released=self._released,byKind={}}
 for key,value in pairs(self._stats) do result[key]=value end
 for _,kind in ipairs(order) do
  local active,allocated=0,0
  for _,slot in ipairs(self._groups[kind] or {}) do
   allocated=allocated+1;if slot.active then active=active+1 end
  end
  result.byKind[kind]={capacity=config[kind].count,allocated=allocated,active=active}
 end
 return result
end
return Effects
