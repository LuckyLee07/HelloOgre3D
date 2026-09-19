-- Authored industrial courts. Static meshes remain the Bullet/navigation source.
local Encounters=require("res.scripts.samples.crossfire_encounters.lua")
local Scene = {}
local keyLight=nil
local moduleCount=0
local surfaceKinds={}
local cameraCache=nil
local statusVisuals={}
local corePositions={}
local coolingPositions={}
local visualOutcome="uninitialised"
local rotorVisuals={}
local restoreElapsed=0
local restoreComplete=false
local equipmentNames={
 tile="floor",tile_service="floor",tile_coolant="floor",tile_interlock="floor",base="floor",deck_marks="floor",deck_marks_08="floor",deck_marks_09="floor",service_trench="floor",
 core="中继核心",cover="掩体",wall="围墙",edge="场地护栏",gate="门厅",
 console="控制台",vent="通风设备",dock="检修台",pipe="管线",service_elbow="管线",
 relay_tower="中继塔",cooling_stack="冷却设备",rear_plinth="维护平台",
 sector_07="区域标牌",sector_08="区域标牌",sector_09="区域标牌",
 court_margin_side="维护边界",court_margin_front="维护边界",court_margin_rear="维护边界",
}
function Scene.SurfaceKind(objectId) return surfaceKinds[objectId] end
function Scene.IsWalkableFloor(point)
 if not point then return false end
 local x,y,z=point.x,point.y,point.z
 return x==x and y==y and z==z and x>-9 and x<9 and z>-11 and z<10.5 and y>=-.12 and y<=.15
end
local function asset(mesh,x,z,yaw,y)
 local a=SandboxObjects:CreateBlockObject("models/crossfire/"..mesh..".mesh")
 a:setPosition(Vector3(x,y or 0,z)); a:setRotation(Vector3(0,yaw or 0,0)); a:SetMass(0)
 moduleCount=moduleCount+1
 surfaceKinds[a:GetObjId()]=equipmentNames[mesh]
 if mesh=="core" then corePositions[#corePositions+1]={x=x,y=y or 0,z=z} end
 if mesh=="cooling_stack" then coolingPositions[#coolingPositions+1]={x=x,y=y or 0,z=z} end
 return a
end
local function floorTile(index,x,z)
 -- Broad service zones break the repeated floor grid. These variants only
 -- remap the material: all vertices, triangle indices and physical bounds match.
 if index==1 and ((x==-4 and (z==-2 or z==2)) or (x==4 and z==6)) then return "tile_service" end
 if index==2 and ((x==-8 and (z==2 or z==6)) or (x==4 and (z==-2 or z==2)) or (x==8 and z==-2)) then return "tile_coolant" end
 if index==3 and (x==-4 or x==4) and (z==-2 or z==2) then return "tile_interlock" end
 return "tile"
end
local function visual(width,depth,x,y,z,yaw,material,pitch)
 local a=SandboxObjects:CreateVisualPlane(width,depth)
 a:setPosition(Vector3(x,y,z));a:setRotation(Vector3(pitch or 0,yaw or 0,0))
 a:setMaterial("Crossfire/"..material)
 return a
end
local function status(width,depth,x,y,z,yaw,family,pitch)
 local a=visual(width,depth,x,y,z,yaw,family.."Idle",pitch)
 -- Energise low front strips first, then the core, gate and rear machinery.
 local delay=math.floor(math.max(0,math.min(1,(z+11.17)/25.12))*900+.5)
 statusVisuals[#statusVisuals+1]={object=a,family=family,delay=delay,state="Idle"}
end
local function setIndicator(entry,suffix)
 if entry.state==suffix then return end
 entry.object:setMaterial("Crossfire/"..entry.family..suffix)
 entry.state=suffix
end
local function resetRotors()
 for _,entry in ipairs(rotorVisuals) do
  entry.angle=entry.startAngle
  entry.object:setRotation(Vector3(0,entry.angle,0))
 end
end
function Scene.SetOutcome(outcome)
 local nextOutcome=(outcome=="VICTORY" or outcome=="DEFEAT") and outcome or nil
 -- A live same-court retry also arrives as nil -> nil; restore the original
 -- visual phase without emitting a duplicate state transition or touching bodies.
 if nextOutcome==nil then resetRotors() end
 if visualOutcome==nextOutcome then return end
 visualOutcome=nextOutcome
 restoreElapsed=0;restoreComplete=false
 for _,entry in ipairs(statusVisuals) do
  local suffix=nextOutcome=="DEFEAT" and "Offline" or "Idle"
  if nextOutcome=="VICTORY" and entry.delay==0 then suffix="Online" end
  setIndicator(entry,suffix)
 end
 print("[CrossfireSceneState] outcome="..(nextOutcome or "idle"))
end
local function finiteDelta(value)
 local delta=tonumber(value) or 0
 if delta~=delta or delta<=0 or delta==math.huge then return 0 end
 return delta
end
function Scene.UpdateVisuals(simDeltaMs,uiDeltaMs,screen,paused)
 -- UI delta is supplied by GameManager. Menus/title deliberately freeze both
 -- clocks; callers pass a non-battle screen while an overlay is open.
 if screen~="battle" or visualOutcome=="DEFEAT" then return end
 local delta=0
 if visualOutcome=="VICTORY" then
  if restoreComplete then return end
  delta=math.min(900-restoreElapsed,finiteDelta(uiDeltaMs))
  restoreElapsed=restoreElapsed+delta
  for _,entry in ipairs(statusVisuals) do
   if entry.delay<=restoreElapsed then setIndicator(entry,"Online") end
  end
  if restoreElapsed>=900 then
   restoreComplete=true
   print("[CrossfireScenePower] elapsedMs=900 lit="..#statusVisuals.." total="..#statusVisuals)
  end
 elseif not paused then
  delta=finiteDelta(simDeltaMs)
 end
 if delta<=0 then return end
 for _,entry in ipairs(rotorVisuals) do
  entry.angle=(entry.angle+delta*entry.speed)%360
  entry.object:setRotation(Vector3(0,entry.angle,0))
 end
end
local function surfaceDetails(index)
 -- Every added surface is a no-body plane, below deck markings or mounted on
 -- existing equipment. No new object contributes to Bullet or navigation.
 if index==1 then
  visual(3.45,5.25,-4,.009,.9,0,"ServicePad")
  visual(3.15,3.4,5.1,.009,6.1,90,"ServicePad")
 elseif index==2 then
  visual(3.25,3.25,-7.4,.009,2.6,0,"CoolantPad")
  visual(3.25,3.25,5.6,.009,-.7,90,"CoolantPad")
 else
  visual(7.8,2.0,0,.009,-2.45,0,"InterlockBus")
  visual(7.8,2.0,0,.009,2.45,180,"InterlockBus")
 end
 -- The circuit remains visible beside a central result panel. It is a physical
 -- indicator strip, not a movement path or an invented emissive light pool.
 for _,x in ipairs({-9.25,9.25}) do
  for z=-9,9,3 do status(1.4,.16,x,.055,z,90,"Power") end
 end
 for _,x in ipairs({-7.5,-4.5,-1.5,1.5,4.5,7.5}) do status(1.4,.16,x,.055,-11.17,0,"Power") end
 for _,x in ipairs({-7.5,-4.5,4.5,7.5}) do status(1.4,.16,x,.055,10.78,0,"Power") end
 for _,x in ipairs({-1.90,1.90}) do status(.16,1.70,x,1.61,10.79,0,"Column",-90) end
 for _,p in ipairs(corePositions) do status(1.90,1.90,p.x,p.y+3.195,p.z,0,"Core") end
 for i,p in ipairs(coolingPositions) do
  status(2.14,2.14,p.x,p.y+2.845,p.z,0,"Core")
  -- The dark circular well covers the old stationary blade paint while its
  -- centre hub and outer ring remain real geometry. Only this thin face rotates.
  local angle=(i*43)%360
  local rotor=visual(1.50,1.50,p.x,p.y+2.837,p.z,angle,"FxRotor")
  rotorVisuals[#rotorVisuals+1]={object=rotor,angle=angle,startAngle=angle,speed=.036}
 end
end
local function commonCourt(index)
 -- All three encounters keep the same single walking layer and deployment area.
 asset("base",0,0)
 for x=-8,8,4 do for z=-10,10,4 do asset(floorTile(index,x,z),x,z) end end
 for x=-8,8,4 do asset("wall",x,12,180); asset("edge",x,-12) end
 for z=-10,10,4 do asset("edge",-10,z,90); asset("edge",10,z,-90) end
 -- Four separate thin solids avoid turning a ring-shaped visual into a convex
 -- hull across the whole board. The inner line exactly matches accepted clicks.
 asset("court_margin_side",9.5,0)
 asset("court_margin_side",-9.5,0,180)
 asset("court_margin_front",0,-11.5)
 asset("court_margin_rear",0,11.25)
 asset("gate",0,11.45,180)
 asset("sector_"..Encounters[index].id,0,10.65,0,2.65)
 -- A real service apron supports the skyline immediately behind the rear wall.
 -- Its furthest point is Z=15.89, keeping Recast bounds close to the playable deck.
 asset("rear_plinth",0,13.95,0,.14)
 asset("dock",-7.8,-9.3); asset("dock",7.8,-9.3)
 local marks=index==1 and "deck_marks" or "deck_marks_"..Encounters[index].id
 asset(marks,0,-8.5,0,.015)
 asset("service_trench",0,-6.35,90)
 asset("service_trench",-8.78,3.8)
 asset("service_trench",8.78,3.8)
end
local function maintenance()
 -- Preserve the first room's authored physics fixture and flanking routes.
 asset("core",0,0)
 asset("cover",-6,-2,20); asset("cover",6,-2,-20)
 asset("deck_marks",-4.8,-4.8,0,.015)
 asset("deck_marks",4.8,-4.8,0,.015)
 asset("console",-6.8,10,165); asset("console",6.8,10,-165)
 asset("vent",-8.3,6.7); asset("vent",8.3,6.7)
 asset("pipe",-9.3,1,90); asset("pipe",9.3,1,-90)
 -- The dish is on camera-left; lower cooling drums balance the other shoulder.
 asset("relay_tower",6.0,14.0)
 asset("cooling_stack",-5.8,13.95); asset("cooling_stack",-8.7,13.95)
 asset("pipe",-2,14.9); asset("pipe",2,14.9)
 asset("service_elbow",8.50,13.30)
end
local function coolantWorks()
 -- Offset machinery gives a sheltered left approach and a different right bend.
 -- Neither cooling stack occupies the authored (-5,6)/(7,4) firing positions.
 asset("core",-2,0)
 asset("cooling_stack",-7.4,2.6)
 asset("cooling_stack",5.6,-.7)
 asset("cover",1.1,-3.4,90)
 asset("cover",-6.2,-2.1,20)
 asset("deck_marks_08",-4.8,-4.8,0,.015)
 asset("deck_marks_08",6.4,-4.8,0,.015)
 asset("service_trench",5.6,3.2)
 asset("console",-6.8,10,165); asset("console",7.1,9.8,-165)
 asset("pipe",-9.3,-.8,90); asset("pipe",9.3,2.2,-90)
 -- A bank of four cooling cylinders gives this court its own rear silhouette.
 for _,x in ipairs({-7.8,-4.7,4.7,7.8}) do asset("cooling_stack",x,13.95) end
 asset("relay_tower",0,14.10,0,-.65)
 asset("pipe",-5.7,14.9); asset("pipe",5.7,14.9)
 asset("service_elbow",8.50,13.30)
end
local function relayInterlock()
 -- The paired central machines split the arena into two open outer approaches.
 asset("core",-3,0); asset("core",3,0)
 asset("cover",0,-4.4)
 asset("cover",0,2.6,90)
 asset("deck_marks_09",-6.4,-4.8,0,.015)
 asset("deck_marks_09",6.4,-4.8,0,.015)
 asset("console",-8.0,9.5,160); asset("console",8.0,9.5,-160)
 asset("vent",-8.3,2.9); asset("vent",8.3,2.9)
 asset("pipe",-9.3,.2,90); asset("pipe",9.3,.2,-90)
 -- The far tower is recessed into its apron to stay clear of the header at the
 -- fixed camera; both are fixed physics transforms, not animated visual offsets.
 asset("relay_tower",-6.5,14.0,0,-.65)
 asset("relay_tower",6.5,14.0)
 for _,x in ipairs({-4,0,4}) do asset("pipe",x,14.7) end
 asset("cooling_stack",-9.0,13.95)
 asset("cooling_stack",9.0,13.95)
end
function Scene.Create(index)
 index=math.max(1,math.min(#Encounters,math.floor(tonumber(index) or 1)))
 moduleCount=0
 surfaceKinds={}
 -- The caller has destroyed the previous court's blocks before Create. Drop
 -- every manager-owned visual reference now; same-court retries use SetOutcome.
 statusVisuals={};corePositions={};coolingPositions={};rotorVisuals={}
 visualOutcome="uninitialised";restoreElapsed=0;restoreComplete=false
 SandboxScene:SetAmbientLight(Vector3(.38,.43,.47))
 -- Blocks are recreated on level changes; this one scene-manager-owned light is
 -- deliberately reused so repeated selection does not accumulate illumination.
 if not keyLight then keyLight=SandboxScene:CreateDirectionalLight(Vector3(-.45,-1,.65)) end
 keyLight:setDiffuseColour(ColourValue(.88,.87,.80))
 keyLight:setSpecularColour(ColourValue(.3,.28,.23))
 SandboxScene:ConfigureDirectionalShadows(keyLight,true,65)
 commonCourt(index)
 if index==1 then maintenance() elseif index==2 then coolantWorks() else relayInterlock() end
 SandboxScene:UpdateSceneGraph()
 local config=rcConfig(); SandboxNav:DefaultConfig(config)
 SandboxNav:ApplySettingConfig(config,0,.65,.2)
 config.minRegionArea=64; config.walkableSlopeAngle=45
 local nav=SandboxNav:CreateNavigationMesh(config,"default")
 if nav then nav:SetDebugVisible(false) end
 -- Visual planes never enter navigation or collide. The caller destroys them
 -- with clearAllObjects(MGR_OBJ_BLOCK,true) before creating the next court.
 local backdrop=SandboxObjects:CreateVisualPlane(180,180)
 backdrop:setMaterial("Crossfire/Slate"); backdrop:setPosition(Vector3(0,-1.05,0))
 surfaceDetails(index)
 Scene.SetOutcome(nil)
 print("[CrossfireScene] level="..index.." sector="..Encounters[index].id.." modules="..moduleCount.." nav="..tostring(nav~=nil))
 return nav
end
function Scene.Camera(zoom)
 -- Fit the actionable court and actor envelope to the agreed HUD-safe rectangle.
 -- Use the actual camera projection instead of assuming a FOV or Retina scale.
 local width,height=GameManager:getScreenWidth(),GameManager:getScreenHeight()
 if width<=80 or height<=260 then return end
 local camera=SandboxCamera:GetCamera()
 zoom=math.max(1,tonumber(zoom) or 1)
 if cameraCache and cameraCache.width==width and cameraCache.height==height and cameraCache.zoom==zoom then
  camera:setOrientation(cameraCache.orientation);camera:setPosition(cameraCache.position);return
 end
 local yaw,pitch=math.rad(178),math.rad(-42)
 local q=Quaternion(0,0,0)
 q.w=math.cos(yaw/2)*math.cos(pitch/2)
 q.x=math.cos(yaw/2)*math.sin(pitch/2)
 q.y=math.sin(yaw/2)*math.cos(pitch/2)
 q.z=-math.sin(yaw/2)*math.sin(pitch/2)
 camera:setOrientation(q)
 local back=Vector3(math.sin(yaw)*math.cos(pitch),-math.sin(pitch),math.cos(yaw)*math.cos(pitch))
 local up=Vector3(math.sin(yaw)*math.sin(pitch),math.cos(pitch),math.cos(yaw)*math.sin(pitch))
 local right=Vector3(math.cos(yaw),0,-math.sin(yaw))
 local target=Vector3(0,0,-.25)
 local left,top,rightEdge,bottom=28,108,width-28,height-132
 local centerX,centerY=(left+rightEdge)/2,(top+bottom)/2
 local points={}
 -- Ground is [-9,9] x [-11,10.5]; this extra envelope protects a complete unit
 -- near any accepted edge and its top, without changing geometry or physics.
 for _,x in ipairs({-9.65,9.65}) do for _,z in ipairs({-11.65,11.15}) do
  for _,y in ipairs({0,1.8}) do points[#points+1]=Vector3(x,y,z) end
 end end
 local function bounds()
  local minX,minY,maxX,maxY=math.huge,math.huge,-math.huge,-math.huge
  for _,point in ipairs(points) do
   local p=SandboxCamera:WorldToScreen(point)
   minX=math.min(minX,p.x);minY=math.min(minY,p.y);maxX=math.max(maxX,p.x);maxY=math.max(maxY,p.y)
  end
  return minX,minY,maxX,maxY
 end
 local function fitAt(distance)
  local position=target+back*distance
  for pass=1,4 do
   camera:setPosition(position)
   local x1,y1,x2,y2=bounds()
   local anchor=SandboxCamera:WorldToScreen(target)
   local step=SandboxCamera:WorldToScreen(target+up)
   local pixelsPerMetre=math.max(.001,anchor.y-step.y)
   position=position+right*(((x1+x2)/2-centerX)/pixelsPerMetre)+up*((centerY-(y1+y2)/2)/pixelsPerMetre)
  end
  camera:setPosition(position)
  local x1,y1,x2,y2=bounds()
  return position,x1>=left and x2<=rightEdge and y1>=top and y2<=bottom
 end
 local low,high=16,90
 for i=1,15 do
  local mid=(low+high)/2
  local _,fits=fitAt(mid)
  if fits then high=mid else low=mid end
 end
 local distance=(high+.08)*zoom
 local position=fitAt(distance)
 cameraCache={width=width,height=height,zoom=zoom,orientation=q,position=position,distance=distance}
 print(string.format("[CrossfireCamera] width=%d height=%d distance=%.3f yaw=178 pitch=-42 safe=28,108,%d,%d",width,height,distance,rightEdge,bottom))
end
function Scene.DrawShield(position,forward,flash)
 local yaw=math.atan2(forward.x,forward.z)
 local center=Vector3(position.x,.07,position.z)
 local color=flash and ColourValue(1,.79,.28,.95) or ColourValue(.92,.41,.16,.80)
 local priorInner,priorOuter=nil,nil
 -- Dense radial bars form a broad directional band, while the open rear stays
 -- unmistakable. This has no collider and uses the damage rule's 120-degree cone.
 for i=0,48 do
  local angle=yaw-math.pi/3+i*math.pi/72
  local direction=Vector3(math.sin(angle),0,math.cos(angle))
  local inner=center+direction*1.22
  local outer=center+direction*1.55
  DebugDrawer:drawLine(inner,outer,color)
  if priorInner then
   DebugDrawer:drawLine(priorInner,inner,color);DebugDrawer:drawLine(priorOuter,outer,color)
  end
  priorInner,priorOuter=inner,outer
 end
 local face=Vector3(forward.x,0,forward.z)
 local side=Vector3(forward.z,0,-forward.x)
 local tip=center+face*1.77
 DebugDrawer:drawLine(center+face*1.53+side*.19,tip,color)
 DebugDrawer:drawLine(center+face*1.53-side*.19,tip,color)
end
function Scene.Shadow()
 local a=SandboxObjects:CreateVisualPlane(1.5,1.2)
 a:setMaterial("Relay/ContactShadow"); a:setPosition(Vector3(0,-10,0)); return a
end
return Scene
