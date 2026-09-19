-- Manager-owned, collision-free weapon signals. States come from the real BT.
-- Release before clearing blocks; no actor/vector userdata is retained.
local Signals={};Signals.__index=Signals
local yaw,pitch=math.rad(178),math.rad(-42)
local cw,cx,cy,cz=math.cos(yaw/2)*math.cos(pitch/2),math.cos(yaw/2)*math.sin(pitch/2),
 math.sin(yaw/2)*math.cos(pitch/2),-math.sin(yaw/2)*math.sin(pitch/2)
local back={x=math.sin(yaw)*math.cos(pitch),y=-math.sin(pitch),z=math.cos(yaw)*math.cos(pitch)}
local function finite(n) return type(n)=="number" and n==n and n~=math.huge and n~=-math.huge end
local function hide(slot)
 if slot.visible then slot.object:setPosition(Vector3(0,-50,0));slot.visible=false end
end
function Signals.New(count)
 local self=setmetatable({slots={},released=false},Signals)
 for i=1,math.max(0,math.min(2,math.floor(tonumber(count) or 0))) do
  local object=SandboxObjects:CreateVisualPlane(.90,.90)
  -- Camera quaternion times PlaneGenerator's +Y/U+Z/V+X UV basis.
  local q=Quaternion(0,0,0)
  q.w=(cw-cx-cy-cz)*.5;q.x=(cw+cx+cy-cz)*.5
  q.y=(cw-cx+cy+cz)*.5;q.z=(cw+cx-cy+cz)*.5
  object:setOrientation(q);object:setPosition(Vector3(0,-50,0))
  object:setMaterial("Crossfire/Charge1")
  self.slots[i]={object=object,visible=false,material="Crossfire/Charge1"}
 end
 print("[CrossfireSignals] allocated="..#self.slots)
 return self
end
function Signals:Set(index,muzzle,state,progress,visible)
 if self.released then return end
 local slot=self.slots[index];if not slot then return end
 if not visible or (state~="LOCKING" and state~="COOLING") or not muzzle
  or not finite(muzzle.x) or not finite(muzzle.y) or not finite(muzzle.z) then hide(slot);return end
 progress=finite(progress) and math.max(0,math.min(1,progress)) or 0
 local material=state=="COOLING" and "Crossfire/Cooling" or ("Crossfire/Charge"..math.min(4,1+math.floor(progress*4)))
 if slot.material~=material then slot.object:setMaterial(material);slot.material=material end
 -- A short view-normal offset keeps the lens legible, never a fake shot path.
 slot.object:setPosition(Vector3(muzzle.x+back.x*.07,muzzle.y+back.y*.07,muzzle.z+back.z*.07))
 slot.visible=true
end
function Signals:HideAll()
 if self.released then return end
 for _,slot in ipairs(self.slots) do hide(slot) end
end
function Signals:Release()
 self.slots={};self.released=true
end
return Signals
