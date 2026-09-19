-- Pure presentation/command queries; physical entities remain the truth.
local Scene=require("res.scripts.samples.crossfire_scene.lua")
local Feedback={}
Feedback.colors={ColourValue(.24,.88,.78,1),ColourValue(.38,.62,1,1)}
local red=ColourValue(1,.30,.22,1)
local amber=ColourValue(1,.68,.22,1)
local function finite(v) return v.x==v.x and v.y==v.y and v.z==v.z and math.abs(v.x)<10000 and math.abs(v.y)<10000 and math.abs(v.z)<10000 end
function Feedback.Name(id,ids)
 for i,actorId in ipairs(ids) do
  if id==actorId then return i==1 and "VEGA" or (i==2 and "ROOK" or "哨卫 0"..(i-2)) end
 end
 return Scene.SurfaceKind(id) or "设备"
end
function Feedback.Target(source,target,ids,find)
 if not source or source:GetHealth()<=0 or not target or target:GetHealth()<=0 then return nil end
 local from=source:GetWeaponComponent():GetMuzzlePosition()
 local to=target:GetPosition()
 local hit=Vector3(to.x,to.y,to.z)
 local first=SandboxRaycast:TraceProjectile(from,to,source:GetObjId(),hit)
 local r={from=from,to=to,hit=hit,first=first,target=target:GetObjId(),name=Feedback.Name(target:GetObjId(),ids),color=Feedback.colors[1]}
 local delta=to-source:GetPosition();delta.y=0
 if not finite(from) or first<0 then r.state="unknown";r.text="射界暂不可用";r.color=red
 elseif delta:squaredLength()>18*18 then r.state="range";r.text="超出射程 · 靠近后再开火";r.color=amber
 elseif first>0 and first~=target:GetObjId() then
  local blocker=find(first)
  local wreck=blocker and blocker:GetHealth()<=0
  r.state=wreck and "wreck" or "obstructed";r.color=red
  r.text="被"..Feedback.Name(first,ids)..(wreck and "残骸" or "").."挡住 · 向侧面换位"
 elseif not source:GetAIComponent():CanSeeEnemy(target:GetObjId()) then
  r.state="unavailable";r.text="尚不能锁定 · 先移到开阔处";r.color=amber
 else
  local facing=source:GetForward();facing.y=0
  if delta:squaredLength()>0 and facing:squaredLength()>0 and facing:normalisedCopy():dotProduct(delta:normalisedCopy())<.96 then
   r.state="aiming";r.text="瞄准中 · 开火前检查射界";r.color=amber
   return r
  end
  local incoming=from-to;incoming.y=0
  local forward=target:GetForward();forward.y=0
  local shield=target:GetAIComponent():GetBlackboard():GetBool("crossfire.shield",false)
  if shield and incoming:squaredLength()>0 and forward:squaredLength()>0 and incoming:normalisedCopy():dotProduct(forward:normalisedCopy())>=.5 then
   r.state="shield";r.text="正面护盾挡弹 · 绕到侧后方";r.color=amber
  else r.state="clear";r.text="侧面暴露 · 当前射界畅通" end
 end
 return r
end
-- When perception has no target, explain the nearest visible-on-the-board
-- sentry's current physical obstruction. This never selects an AI target.
function Feedback.Reference(source,ids,find)
 if not source or source:GetHealth()<=0 then return nil end
 local nearest,distance=nil,math.huge
 for i=3,#ids do
  local candidate=find(ids[i])
  if candidate and candidate:GetHealth()>0 then
   local d=(candidate:GetPosition()-source:GetPosition()):squaredLength()
   if d<distance then nearest,distance=candidate,d end
  end
 end
 local report=Feedback.Target(source,nearest,ids,find)
 if report then report.reference=true end
 return report
end
function Feedback.Draw(r,color)
 if not r or not finite(r.from) then return end
 color=r.state=="clear" and (color or r.color) or r.color
 local stop=(r.state=="obstructed" or r.state=="wreck") and r.hit or r.to
 if r.state=="clear" or r.state=="aiming" then
  -- A broken preview distinguishes current line of fire from an actual tracer.
  local delta=stop-r.from
  for i=0,14,2 do DebugDrawer:drawLine(r.from+delta*(i/16),r.from+delta*((i+1)/16),color) end
 else DebugDrawer:drawLine(r.from,stop,color) end
 DebugDrawer:drawCircle(stop,.25,20,color,false)
 if r.state=="obstructed" or r.state=="wreck" then
  -- The muted, dashed remainder makes the intended target distinct from impact.
  local d=r.to-stop
  for i=0,8,2 do DebugDrawer:drawLine(stop+d*(i/10),stop+d*((i+1)/10),ColourValue(.50,.53,.59,.8)) end
  DebugDrawer:drawLine(stop+Vector3(-.24,.24,0),stop+Vector3(.24,-.24,0),red)
  DebugDrawer:drawLine(stop+Vector3(-.24,-.24,0),stop+Vector3(.24,.24,0),red)
 end
end
function Feedback.Ground(x,y,source)
 local p=SandboxCamera:ScreenToGroundPoint(x,y,0)
 if not finite(p) then return {ok=false,text="请选择庭院内的地面"} end
 local camera=SandboxCamera:GetCameraPosition()
 local hit=Vector3(p.x,p.y,p.z)
 local first=SandboxRaycast:PickSurface(camera,p+(p-camera)*.1,hit)
 local kind=Scene.SurfaceKind(first)
 if kind~="floor" then return {ok=false,pos=hit,text=kind and ("这里是"..kind.." · 请点旁边的地面") or "请选择边线内的地面"} end
 if not Scene.IsWalkableFloor(hit) then return {ok=false,pos=hit,text="请点亮色边线内的地面"} end
 local projected=SandboxNav:FindClosestPoint("default",hit)
 local offset=projected-hit;offset.y=0
 local path=std.vector_Ogre__Vector3_()
 if not source or offset:length()>=.65 or not SandboxNav:FindPath("default",source:GetPosition(),projected,path) then
  return {ok=false,pos=hit,text="路线不可达 · 请避开设备和围栏"}
 end
 return {ok=true,pos=Vector3(projected.x,projected.y,projected.z),path=path,text="点击移动到这里"}
end
function Feedback.DrawGround(r,color)
 if not r or not r.pos or not finite(r.pos) then return end
 local p=r.pos+Vector3(0,.06,0);color=r.ok and color or red
 DebugDrawer:drawCircle(p,.38,28,color,false)
 if r.ok and r.path then
  for i=1,r.path:size()-1 do local a,b=r.path[i-1],r.path[i];a.y=.08;b.y=.08;DebugDrawer:drawLine(a,b,color) end
 else
  DebugDrawer:drawLine(p+Vector3(-.25,0,-.25),p+Vector3(.25,0,.25),color)
  DebugDrawer:drawLine(p+Vector3(-.25,0,.25),p+Vector3(.25,0,-.25),color)
 end
end
return Feedback
