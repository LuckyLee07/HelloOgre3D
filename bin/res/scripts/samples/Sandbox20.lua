-- Crossfire: three authored encounters, real combat and simultaneous planned orders.
local Scene=require("res.scripts.samples.crossfire_scene.lua")
local Hud=require("res.scripts.samples.crossfire_hud.lua")
local Levels=require("res.scripts.samples.crossfire_encounters.lua")
local Feedback=require("res.scripts.samples.crossfire_feedback.lua")
local Profile=require("res.scripts.samples.crossfire_profile.lua")
local ids,planned,shadows,observed,effects={},{},{},{},{}
local hud,selected,paused,result,serial,restart= nil,1,true,nil,0,false
local hint="Assign VEGA and ROOK opposite routes. Orange arc = the protected front."
local profile=nil
local saveError=false
local level,screen,overlay=1,"title",nil
local transition=nil
local medal,medalText,newBest=0,"",false
local hoverX,hoverY=-1,-1
local hoverCache=nil
local hoverCacheKey=""
local muted=false
local silent=os.getenv("HELLO_AUDIO_SILENT")=="1"
local zoom=1
local started=nil
local physicsTest=nil
local heldKeys={}
local function now() return GameManager:getTimeInMillis() end
local function find(id)
 local all=ObjectManager:getAllAgents()
 for i=0,all:size()-1 do if all[i]:GetObjId()==id then return all[i] end end
end
local function board(a) return a:GetAIComponent():GetBlackboard() end
local function sound(name,pos,gain,priority)
 if silent or muted or not SandboxAudio then return end
 local pan=0
 if pos then local p=SandboxCamera:WorldToScreen(pos); pan=math.max(-.75,math.min(.75,(p.x/GameManager:getScreenWidth()-.5)*1.4)) end
 SandboxAudio:PlayLayer("res/audio/crossfire/"..name..".wav",(gain or .5)*(profile and profile.volume or .75),pan,priority or 1)
end
local function selectUnit(index)
 local a=find(ids[index]); if not a or a:GetHealth()<=0 then return end
 selected=index; sound("select",nil,.35,2)
 hint=(index==1 and "VEGA" or "ROOK").." 已选中 · 点地面移动，点哨卫指定攻击"
 print("[CrossfireSelect] slot="..index)
end
local function apply(index,order)
 local a=find(ids[index]); if not a or a:GetHealth()<=0 then return false end
 serial=serial+1
 local bb=board(a)
 if order.kind=="attack" then
  local target=find(order.target)
  if not target or target:GetHealth()<=0 or not a:GetAIComponent():CanSeeEnemy(order.target) then
   hint="目标被设备遮挡或已摧毁 · 先移动到开阔处"
   print("[CrossfireOrder] agent="..a:GetObjId().." kind=attack result=unavailable tick="..now())
   return false
  end
  local distance=target:GetPosition()-a:GetPosition(); distance.y=0
  -- Guard's authored firing radius is 18 m; visibility extends to 22 m.
  if distance:squaredLength()>18*18 then
   hint="目标超出射程 · 先靠近到开阔处"
   print("[CrossfireOrder] agent="..a:GetObjId().." kind=attack result=out_of_range tick="..now())
   return false
  end
 end
 bb:SetInt("command.serial",serial)
 bb:SetString("command.status","accepted")
 if order.kind=="move" then
  bb:SetVec3("movePos",order.pos); bb:SetInt("crossfire.target",-1)
 elseif order.kind=="attack" then
  a:GetAIComponent():ClearMovePosition(); bb:Remove("movePos")
  bb:SetInt("crossfire.target",order.target)
 else
  a:GetAIComponent():ClearMovePosition(); bb:Remove("movePos")
  bb:SetInt("crossfire.target",-1); bb:SetString("command.status","completed")
 end
 print("[CrossfireOrder] agent="..a:GetObjId().." kind="..order.kind.." result=accepted serial="..serial.." tick="..now())
 return true
end
local function order(index,value)
 local a=find(ids[index]); if not a or a:GetHealth()<=0 or result or screen~="battle" or overlay then return end
 if paused then
  planned[index]=value
  hint=(index==1 and "VEGA" or "ROOK").." · "..({move="移动",attack="指定攻击",hold="原地待命"})[value.kind].."已规划 · 按空格执行"
  print("[CrossfirePlan] slot="..index.." kind="..value.kind.." tick="..now())
 else
  if apply(index,value) then hint="指令已执行 · 按空格暂停，调整下一步" end
 end
 sound("order",value.pos,.4,2)
end
local function togglePause()
 if result or screen~="battle" or overlay then return end
 if paused then
  if started==nil and not planned[1] and not planned[2] and not physicsTest then
   hint="先点一处地面规划路线，再按空格执行"
   print("[CrossfireInput] execute=blocked reason=no_plan")
   return
  end
  -- All staged orders commit before the same next simulation tick.
  local accepted=true
  for i=1,2 do if planned[i] and not apply(i,planned[i]) then accepted=false end end
  planned={}; paused=false
  if started==nil then started=now() end
  sound("start",nil,.45,2)
  if accepted then hint="正在执行 · 抵达后自动开火，随时按空格暂停" end
 else paused=true; hint="时间已暂停 · 可调整两机路线或目标，再按空格同时执行" end
 GameManager:SetSimulationPaused(paused)
 print("[CrossfirePause] paused="..tostring(paused).." tick="..now())
end
local function spawn()
 GameManager:SetSimulationPaused(false)
 ids,planned,observed,effects={},{},{},{}
 selected,paused,result,serial,started=1,true,nil,0,nil
 medal,medalText,newBest=0,"",false
 hoverCache=nil;hoverCacheKey=""
 local encounter=Levels[level]
 for i=1,2+#encounter.enemies do
  local sentinel=i>2
  local config=sentinel and encounter.enemies[i-2] or encounter.allies[i]
  local a=SandboxObjects:CreateAgentWithProfile(AGENT_OBJ_NONE,sentinel and "crossfire_sentinel" or "crossfire_drone","res/scripts/agent/CrossfireAgent.lua")
  a:SetTeamId(sentinel and 2 or 1)
  if i==2 then a:initBody("models/crossfire/drone_b.mesh") end
  a:setPosition(Vector3(config.x,sentinel and .82 or .72,config.z))
  a:SetForward(Vector3(config.fx or 0,0,config.fz or 1))
  local hp=config.hp or 120
  a:GetAttribComponent():SetMaxHealth(hp); a:SetHealth(hp)
  local bb=board(a)
  bb:SetBool("weapon.actionOwnsFire",true)
  bb:SetInt("crossfire.clockMs",now())
  if sentinel then
   bb:SetFloat("crossfire.chargeMs",config.chargeMs or 1100)
   bb:SetFloat("crossfire.cooldownMs",config.cooldownMs or 1400)
  end
  ids[i]=a:GetObjId(); observed[ids[i]]={shot=0,blocked=0,hit=0,hp=hp,maxHp=hp}
 end
 Scene.Camera(zoom)
 if os.getenv("HELLO_CROSSFIRE_LAYOUT_PROBE")=="1" then
  local function point(key,x,y,z)
   local p=SandboxCamera:WorldToScreen(Vector3(x,y,z))
   print(string.format("[CrossfireLayout] key=%s screen=%.0f,%.0f",key,p.x,p.y))
  end
  for n,config in ipairs(Levels) do
   for i,p in ipairs(config.routes) do point("route"..n.."_"..i,p.x,0,p.z) end
   for i,p in ipairs(config.enemies) do point("enemy"..n.."_"..i,p.x,.82,p.z) end
  end
  point("front",-5,0,9)
  point("flank3",-7,0,10)
 end
 GameManager:SetSimulationPaused(true)
 hint=encounter.lesson
 print("[Crossfire] ready drones="..ids[1]..","..ids[2].." sentry="..ids[3].." paused=true level="..level.." enemies="..#encounter.enemies)
 for i,id in ipairs(ids) do
  local p=SandboxCamera:WorldToScreen(find(id):GetPosition())
  print(string.format("[CrossfireActorPoint] slot=%d screen=%.0f,%.0f level=%d",i,p.x,p.y,level))
 end
 for i,route in ipairs(encounter.routes) do
  local p=Vector3(route.x,0,route.z)
  local point=SandboxCamera:WorldToScreen(p)
  local path=std.vector_Ogre__Vector3_()
  local ok=SandboxNav:FindPath("default",find(ids[i]):GetPosition(),p,path)
  print(string.format("[CrossfireRoute] slot=%d screen=%.0f,%.0f reachable=%s points=%d level=%d",i,point.x,point.y,tostring(ok),path:size(),level))
 end
end
local function queueLevel(index,targetScreen,reason)
 paused=true; overlay=nil; GameManager:SetSimulationPaused(true)
 transition={level=index,screen=targetScreen,reason=reason}
end
local function openOverlay(name)
 if not paused then paused=true; GameManager:SetSimulationPaused(true) end
 overlay=name
 print("[CrossfireMenu] overlay="..name.." tick="..now())
end
local function saveSettings()
 profile.muted=muted
 saveError=not profile:Save()
 if saveError then hint="设置已生效，本次未能写入保存文件" end
 print(string.format("[CrossfireSettings] volume=%.2f muted=%s hints=%s",profile.volume,tostring(muted),tostring(profile.hints)))
end
local function action(name)
 if name=="pause" then togglePause()
 elseif name=="start" and screen=="title" then
  screen="battle"; overlay=nil; if transition then transition.screen="battle" end
  sound("start",nil,.4,2)
  print("[CrossfireScreen] screen=battle level="..level.." tick="..now())
 elseif name=="restart" then restart=true; overlay=nil
 elseif name=="next" and result=="VICTORY" then
  queueLevel(level<#Levels and level+1 or 1,level<#Levels and "battle" or "title","next")
 elseif name=="menu" then queueLevel(level,"title","menu")
 elseif name=="level1" or name=="level2" or name=="level3" then
  if screen=="title" then queueLevel(tonumber(name:sub(-1)),"title","preview") end
 elseif name=="settings" or name=="help" then openOverlay(name)
 elseif name=="close" then overlay=nil; print("[CrossfireMenu] overlay=closed tick="..now())
 elseif name=="mute" then muted=not muted; if muted then SandboxAudio:StopAll() end; saveSettings()
 elseif name=="volumeDown" or name=="volumeUp" then
  profile.volume=math.max(0,math.min(1,profile.volume+(name=="volumeUp" and .25 or -.25)))
  saveSettings(); sound("select",nil,.35,2)
 elseif name=="hints" then profile.hints=not profile.hints; saveSettings()
 elseif name=="select1" then selectUnit(1)
 elseif name=="select2" then selectUnit(2)
 elseif name=="quit" then GameManager:RequestQuit() end
end
local function drawLine(a,b,color) DebugDrawer:drawLine(a,b,color) end
local cyan=ColourValue(.24,.85,.82,.9)
local amber=ColourValue(1,.48,.15,.95)
local white=ColourValue(1,.91,.68,1)
local function pathPreview(a,p,color)
 local path=std.vector_Ogre__Vector3_()
 if SandboxNav:FindPath("default",a:GetPosition(),p,path) then
  for k=1,path:size()-1 do
   local u,v=path[k-1],path[k];u.y=.08;v.y=.08;drawLine(u,v,color)
  end
 end
 local dest=Vector3(p.x,.09,p.z)
 DebugDrawer:drawCircle(dest,.5,24,color,false)
 drawLine(dest+Vector3(-.18,0,0),dest+Vector3(.18,0,0),color)
 drawLine(dest+Vector3(0,0,-.18),dest+Vector3(0,0,.18),color)
end
local function presentation()
 local ctx={allies={},actors={},selected=selected,paused=paused,result=result,hint=hint,muted=muted,
  screen=screen,overlay=overlay,saveError=saveError,level=level,levels=Levels,records=profile.records,settings=profile,
  completedCount=profile:CompletedCount(),hoverX=hoverX,hoverY=hoverY}
 local living,damage,enemiesAlive,flankHits,blockedShots=0,0,0,0,0
 for i,id in ipairs(ids) do
  local a=find(id)
  if a then
   local bb=board(a); local pos=a:GetPosition(); local hp=a:GetHealth(); local old=observed[id]
   bb:SetInt("crossfire.clockMs",now())
   local shot,blocked,hit=bb:GetInt("crossfire.shotCount",0),bb:GetInt("crossfire.blockedCount",0),bb:GetInt("crossfire.hitCount",0)
   if shot>old.shot then
    sound("shot",pos,i>2 and .52 or .35,1)
    effects[#effects+1]={kind="muzzle",pos=pos+a:GetForward()*(i>2 and 1.12 or .9)+Vector3(0,i>2 and .20 or .23,0),at=now(),forward=a:GetForward()}
   end
   if blocked>old.blocked or hit>old.hit then
    sound(blocked>old.blocked and "shield" or "hit",pos,.55,2)
    effects[#effects+1]={kind=blocked>old.blocked and "shield" or "hit",pos=bb:GetVec3("crossfire.lastImpactPosition"),at=now()}
   end
   if hp<=0 and old.hp>0 then
    if i==selected and i<3 then
     local other=i==1 and 2 or 1;local survivor=find(ids[other])
     if survivor and survivor:GetHealth()>0 then selected=other end
    end
    sound("destroy",pos,.65,3)
    effects[#effects+1]={kind="destroy",pos=pos,at=now()}
    print("[CrossfireDestroyed] slot="..i.." id="..id.." tick="..now())
   end
   local status=bb:GetString("command.status")
   if i<3 and status=="failed" and old.status~="failed" then
    hint=(i==1 and "VEGA" or "ROOK").." 指令中断 · 重新选择落点或可见哨卫"
   end
   if status~="" and status~=old.status and i<3 then
    print("[CrossfireCommand] slot="..i.." status="..status.." target="..bb:GetInt("crossfire.target",-1).." level="..level)
   end
   old.status=status
   old.shot,old.blocked,old.hit,old.hp=shot,blocked,hit,hp
   shadows[i]:setPosition(Vector3(pos.x,.018,pos.z))
   if i<3 then damage=damage+math.max(0,old.maxHp-hp)
   else flankHits=flankHits+hit; blockedShots=blockedShots+blocked; if hp>0 then enemiesAlive=enemiesAlive+1 end end
   local front=a:GetForward()
   if hp>0 then
    if i<3 then
     living=living+1
     if i==selected and screen=="battle" and not overlay then DebugDrawer:drawCircle(Vector3(pos.x,.06,pos.z),.95,40,Feedback.colors[i],false) end
     local destination=nil
     if planned[i] then destination=planned[i].pos
     elseif bb:Has("movePos") then destination=bb:GetVec3("movePos") end
     if destination and screen=="battle" and not overlay then pathPreview(a,destination,Feedback.colors[i]); ctx["endpoint"..i]=destination end
    else
     -- This arc is the same 120-degree cone used by physical impact damage.
     local center=Vector3(pos.x,.10,pos.z)
     Scene.DrawShield(pos,front,bb:GetBool("crossfire.lastBlocked",false) and now()-bb:GetInt("crossfire.lastImpactMs",-10000)<350)
     local charge=bb:GetFloat("crossfire.charge",0)
     if charge>0 then
      local aim=bb:GetVec3("crossfire.aim");aim.y=.06
      drawLine(center,aim,amber)
      DebugDrawer:drawCircle(aim,.3+charge*.5,32,amber,false)
     end
    end
   end
   local item={pos=pos,hp=hp,maxHp=old.maxHp,enemy=i>2,name=Feedback.Name(id,ids),state=hp<=0 and "OFFLINE" or bb:GetString("crossfire.state"),
    damaged=not bb:GetBool("crossfire.lastBlocked",true) and now()-bb:GetInt("crossfire.lastImpactMs",-10000)<350,
    blocked=bb:GetBool("crossfire.lastBlocked",false) and now()-bb:GetInt("crossfire.lastImpactMs",-10000)<550}
   if i<3 then
    item.identityColor=Feedback.colors[i]
    local requested=planned[i] and planned[i].kind=="attack" and planned[i].target or bb:GetInt("crossfire.target",-1)
    if requested<=0 then requested=bb:GetInt("crossfire.lockId",-1) end
    local target=requested>0 and find(requested) or a:GetAIComponent():GetEnemy()
    local report=Feedback.Target(a,target,ids,find)
    item.fireState=report and report.text or "抵达后自动寻找目标"
    item.targetName=report and report.name or nil
    local moving=planned[i] and planned[i].kind=="move" or (not planned[i] and bb:Has("movePos"))
    if report and not moving then
     if report.state~="clear" then old.stalledAt=old.stalledAt or now() else old.stalledAt=nil end
     if screen=="battle" and not overlay and not result then Feedback.Draw(report,Feedback.colors[i]) end
     if i==selected then ctx.fireHint=(paused and "当前射界 · " or "")..report.name.."："..report.text end
     if old.stalledAt and now()-old.stalledAt>1800 and old.reportState~=report.state then
      print("[CrossfireObstruction] slot="..i.." target="..report.target.." first="..report.first.." reason="..report.state.." tick="..now())
      old.reportState=report.state
     elseif report.state=="clear" then old.reportState=nil end
    else
     old.stalledAt=nil
     if moving then item.fireState=paused and "按空格执行移动 · 抵达后自动开火" or "移动中 · 抵达后自动开火";item.targetName=nil end
    end
    item.order=planned[i] and (planned[i].kind=="move" and "MOVE / QUEUED" or (planned[i].kind=="attack" and "TARGET / QUEUED" or "HOLD / QUEUED")) or nil
    ctx.allies[#ctx.allies+1]=item
   end
   ctx.actors[#ctx.actors+1]=item
  end
 end
 for i=#effects,1,-1 do
  local e=effects[i];local t=(now()-e.at)/300
  if t>1 then table.remove(effects,i)
  elseif e.kind=="muzzle" then
   if t<.28 then
    drawLine(e.pos-e.forward*.12,e.pos+e.forward*.6,white)
    drawLine(e.pos+Vector3(-.16,0,0),e.pos+Vector3(.16,0,0),white)
   end
  else
   local radius=(e.kind=="destroy" and 1.4 or .65)*t
   local color=e.kind=="shield" and amber or white
   for k=1,7 do
    local angle=k*2.399
    local v=Vector3(math.cos(angle),.25+(k%3)*.3,math.sin(angle))
    drawLine(e.pos+v*radius,e.pos+v*(radius+.15*(1-t)),color)
   end
  end
 end
 if not result and not physicsTest and screen=="battle" then
  if enemiesAlive==0 then result="VICTORY"
  elseif living==0 then result="DEFEAT" end
  if result then
   paused=true;GameManager:SetSimulationPaused(true);planned={};effects={}
   sound(result=="VICTORY" and "win" or "lose",nil,.65,4)
   local elapsed=now()-(started or now())
   if result=="VICTORY" then
    local saved
    medal,newBest,saved=profile:Record(level,elapsed,damage,living,Levels[level].parMs)
    medalText=({"RELAY RESTORED","BOTH DRONES HOME","PRECISION TEAM"})[medal]
    saveError=not saved
    if saveError then hint="Record kept this session; this folder could not save it." end
   end
   print("[CrossfireMatch] result="..result.." elapsedMs="..elapsed.." natural=true level="..level.." living="..living.." damage="..damage.." medal="..medal.." flankHits="..flankHits.." blocked="..blockedShots)
   if result=="VICTORY" and profile:CompletedCount()==3 then print("[CrossfireCampaign] completed=3") end
  end
 end
 if screen=="battle" and not result then
  ctx.critical=level==3 and "联锁区：两机拉开后，分别点对侧哨卫；红线表示中途受阻，需再换位。" or (level==2 and "冷却区：设备会挡弹；先绕开设备，再从侧面开火。" or "橙色扇面是护盾正面；两机从不同方向接近，抵达后自动开火。")
  if not overlay and hoverX>=0 and not hud:Hit(hoverX,hoverY) then
   local actorIndex=hud:PickActor(hoverX,hoverY)
   local source=find(ids[selected])
   if actorIndex and actorIndex>2 then
    local report=Feedback.Target(source,find(ids[actorIndex]),ids,find)
    if report then Feedback.Draw(report,Feedback.colors[selected]);ctx.cursorHint="指定 "..report.name.." · "..report.text end
   elseif not actorIndex then
    local key=hoverX..":"..hoverY..":"..selected..":"..math.floor(now()/120)
    if key~=hoverCacheKey then hoverCache=Feedback.Ground(hoverX,hoverY,source);hoverCacheKey=key end
    Feedback.DrawGround(hoverCache,Feedback.colors[selected])
    ctx.cursorHint=hoverCache and hoverCache.text
   else ctx.cursorHint="点击选择 "..Feedback.Name(ids[actorIndex],ids) end
  end
 end
 ctx.result=result;ctx.paused=paused;ctx.hint=hint;ctx.selected=selected
 ctx.medal,ctx.medalText,ctx.newBest=medal,result=="DEFEAT" and "FIND A NEW ANGLE" or medalText,newBest
 ctx.completedCount=profile:CompletedCount();ctx.enemiesAlive=enemiesAlive;ctx.saveError=saveError
 ctx.nextLabel=level<#Levels and "ENTER / NEXT RELAY" or "ENTER / BACK TO OPERATIONS"
 ctx.resultDetail=string.format("%d 次有效侧击 · %d 次正面挡弹",flankHits,blockedShots)..(result=="DEFEAT" and "。检查红色射线，绕开护盾与残骸再攻击。" or (newBest and " · 新纪录" or ""))
 if profile.hints and not started and screen=="battle" and not result then
  local first,second=planned[1]~=nil,planned[2]~=nil
  ctx.tutorial={step=not first and 1 or (not second and 2 or 3),done1=first,done2=second,
   title=not first and "1 · 给 VEGA 规划路线" or (not second and "2 · 给 ROOK 另一条路线" or "3 · 两机同时执行"),
   detail=not first and "选中 VEGA，再点设备旁的空地。鼠标处会显示路线。" or (not second and "点 ROOK 或按 2，再点哨卫另一侧的空地。" or "按空格或点执行。无人机抵达后自动开火。")}
 end
 ctx.stats=string.format("%d / 2 台存活 · %.1f 秒 · 承受 %d 损伤",living,(now()-(started or now()))/1000,damage)
 hud:Update(ctx)
end
function Sandbox_Initialize()
 SandboxUI:SetBuildInfoVisible(false)
 _G.HELLO_SUPPRESS_AI_PATH_DRAW=true
 profile=Profile.Load(); muted=profile.muted
 print(string.format("[CrossfireSettings] loaded=true volume=%.2f muted=%s hints=%s completed=%d",profile.volume,tostring(muted),tostring(profile.hints),profile:CompletedCount()))
 -- Test launches opt into direct battle; ordinary play always gets the title.
 local initial=tonumber(os.getenv("HELLO_CROSSFIRE_LEVEL") or "1")
 if initial and initial==math.floor(initial) and initial>=1 and initial<=#Levels then level=initial end
 if os.getenv("HELLO_CROSSFIRE_PHYSICS_TEST")=="1" or os.getenv("HELLO_CROSSFIRE_QUERY_TEST")=="1" then level=1 end
 screen=(os.getenv("HELLO_CROSSFIRE_QUICKSTART")=="1" or os.getenv("HELLO_CROSSFIRE_PHYSICS_TEST")=="1" or os.getenv("HELLO_CROSSFIRE_QUERY_TEST")=="1") and "battle" or "title"
 Scene.Create(level)
 for i=1,4 do shadows[i]=Scene.Shadow() end
 hud=Hud.New()
 spawn()
 if os.getenv("HELLO_CROSSFIRE_PHYSICS_TEST")=="1" then
  physicsTest=require("res.scripts.samples.crossfire_physics_selftest.lua").New({ids=ids,find=find})
  paused=false;GameManager:SetSimulationPaused(false)
 end
 if os.getenv("HELLO_CROSSFIRE_QUERY_TEST")=="1" then
  physicsTest=require("res.scripts.samples.crossfire_query_selftest.lua").New({ids=ids,find=find})
  paused=false;GameManager:SetSimulationPaused(false)
 end
 print("[CrossfireScreen] screen="..screen.." level="..level.." completed="..profile:CompletedCount())
 presentation()
end
function Sandbox_Update(deltaMs)
 if not hud or #ids<3 then return end
 if restart or transition then
  local requested=transition
  restart=false;transition=nil;physicsTest=nil
  GameManager:SetSimulationPaused(false)
  SandboxAudio:StopAll();SandboxObjects:ClearProjectiles()
  -- Drop every Lua-held actor reference before destruction and nav replacement.
  ids,planned,observed,effects={},{},{},{}
  ObjectManager:clearAllObjects(MGR_OBJ_AGENT,true)
  if TeamBlackboard then TeamBlackboard:Reset() end
  if requested and requested.level~=level then
   shadows={}
   ObjectManager:clearAllObjects(MGR_OBJ_BLOCK,true)
   level=requested.level
   Scene.Create(level)
   for i=1,4 do shadows[i]=Scene.Shadow() end
  end
  for _,shadow in ipairs(shadows) do shadow:setPosition(Vector3(0,-10,0)) end
  if requested then screen=requested.screen end
  overlay=nil
  spawn()
  if requested then
   print("[CrossfireTransition] reason="..requested.reason.." level="..level.." screen="..screen.." agents="..ObjectManager:getAiAgentCount().." objects="..ObjectManager:getObjectCount())
  else print("[CrossfireRestart] cleared=true level="..level) end
 end
 if physicsTest then physicsTest:Update(deltaMs) end
 Scene.Camera(zoom)
 presentation()
end
function EventHandle_Keyboard(keycode,pressed)
 -- InputManager emits key-up for held keys on focus loss as well.
 if not pressed then heldKeys[keycode]=nil; return true end
 if heldKeys[keycode] then return true end
 heldKeys[keycode]=true
 if not hud then return true end
 if keycode==OIS.KC_M then action("mute");return true end
 if overlay then
  if keycode==OIS.KC_ESCAPE or keycode==OIS.KC_RETURN then action("close") end
  return true
 end
 if keycode==OIS.KC_ESCAPE then action("settings")
 elseif keycode==OIS.KC_H then action("help")
 elseif screen=="title" then
  if keycode==OIS.KC_RETURN then action("start")
  elseif keycode==OIS.KC_1 then action("level1")
  elseif keycode==OIS.KC_2 then action("level2")
  elseif keycode==OIS.KC_3 then action("level3") end
 elseif keycode==OIS.KC_SPACE then action("pause")
 elseif keycode==OIS.KC_1 then action("select1")
 elseif keycode==OIS.KC_2 then action("select2")
 elseif keycode==OIS.KC_R then action("restart")
 elseif keycode==OIS.KC_RETURN and result=="VICTORY" then action("next")
 elseif keycode==OIS.KC_X then order(selected,{kind="hold"}) end
 return true
end
function EventHandle_Mouse(ctype,x,y,button)
 hoverX,hoverY=x,y
 if ctype~=1 then return true end
 local hit=hud and hud:Hit(x,y)
 if hit then if button==0 then action(hit) end; return true end
 if result or screen~="battle" or overlay or not hud then return true end
 if button~=0 and button~=1 then return true end
 local nearest=hud:PickActor(x,y)
 if nearest and nearest<3 then
  selectUnit(nearest)
 else
  if nearest and nearest>2 then
   local a=find(ids[selected])
   if a and a:GetAIComponent():CanSeeEnemy(ids[nearest]) then order(selected,{kind="attack",target=ids[nearest]})
   else hint="目标被设备遮挡 · 先点地面移动到开阔处" end
  else
   local ground=Feedback.Ground(x,y,find(ids[selected]))
   if ground.ok then order(selected,{kind="move",pos=ground.pos})
   else hint=ground.text;print("[CrossfireInput] move=rejected reason=surface x="..x.." y="..y) end
  end
 end
 return true
end
function EventHandle_WindowResized(width,height) Scene.Camera(zoom) end
