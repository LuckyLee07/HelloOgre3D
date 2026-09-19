-- Crossfire: three authored encounters, real combat and simultaneous planned orders.
local Scene=require("res.scripts.samples.crossfire_scene.lua")
local Hud=require("res.scripts.samples.crossfire_hud.lua")
local Levels=require("res.scripts.samples.crossfire_encounters.lua")
local Feedback=require("res.scripts.samples.crossfire_feedback.lua")
local Review=require("res.scripts.samples.crossfire_review.lua")
local Tactics=require("res.scripts.samples.crossfire_tactics.lua")
local Effects=require("res.scripts.samples.crossfire_effects.lua")
local Signals=require("res.scripts.samples.crossfire_signals.lua")
local Profile=require("res.scripts.samples.crossfire_profile.lua")
local ids,planned,shadows,observed={},{},{},{}
local effects,signals=nil,nil
local visualClock,outcomeAge,reportOpen=0,0,false
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
local stepUntil=nil
local review,alert,inspection,reviewSummary=nil,nil,nil,nil
local alertKey=""
local tacticKey=""
local function now() return GameManager:getTimeInMillis() end
local function find(id)
 local all=ObjectManager:getAllAgents()
 for i=0,all:size()-1 do if all[i]:GetObjId()==id then return all[i] end end
end
local function board(a) return a:GetAIComponent():GetBlackboard() end
local function logEffects(event)
 if not effects then return end
 local state=effects:Stats()
 print(string.format("[CrossfireEffects] event=%s capacity=%d active=%d peak=%d emitted=%d reused=%d evicted=%d visualMs=%d tick=%d",
  event,state.capacity,state.active,state.peak,state.emitted,state.reused,state.evicted,visualClock,now()))
end
local function applyVolume()
 if SandboxAudio then SandboxAudio:SetVolume((silent or muted) and 0 or .65*(profile and profile.volume or .75)) end
end
local function sound(name,pos,gain,priority)
 if silent or muted or not SandboxAudio then return end
 local pan=0
 if pos then local p=SandboxCamera:WorldToScreen(pos); pan=math.max(-.75,math.min(.75,(p.x/GameManager:getScreenWidth()-.5)*1.4)) end
 SandboxAudio:PlayLayer("res/audio/crossfire/"..name..".wav",gain or .5,pan,priority or 1)
end
local function selectUnit(index)
 local a=find(ids[index]); if not a or a:GetHealth()<=0 then return end
 selected=index; inspection=nil; sound("select",nil,.35,2)
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
 inspection=nil
 if paused then
  planned[index]=value
  hint=(index==1 and "VEGA" or "ROOK").." · "..({move="移动",attack="指定攻击",hold="原地待命"})[value.kind].."已规划 · 按空格执行"
  print("[CrossfirePlan] slot="..index.." kind="..value.kind.." tick="..now())
 else
  if not apply(index,value) then return end
  hint="指令已执行 · 按空格暂停，调整下一步"
 end
 sound("order",value.pos,.4,2)
end
local function togglePause()
 if result or screen~="battle" or overlay then return end
 stepUntil=nil;inspection=nil
 if paused then
  if started==nil and not planned[1] and not planned[2] and not physicsTest then
   hint="先点地面规划路线，再按空格执行或 E 推进"
   print("[CrossfireInput] execute=blocked reason=no_plan")
   return
  end
  -- All staged orders commit before the same next simulation tick.
  local accepted=true
  for i=1,2 do if planned[i] and not apply(i,planned[i]) then accepted=false end end
  planned={}; paused=false
  if started==nil then started=now() end
  sound("start",nil,.45,2)
  if accepted then hint="正在执行 · 随时按空格暂停" end
 else paused=true; hint="时间已暂停 · 可调整两机路线或目标，再按空格同时执行" end
 GameManager:SetSimulationPaused(paused)
 print("[CrossfirePause] paused="..tostring(paused).." tick="..now())
end
local function stepExecution()
 if result or screen~="battle" or overlay then return end
 if paused then togglePause() end
 if paused then return end
 stepUntil=now()+2000
 hint="推进 2 秒后自动暂停 · 随时按空格提前暂停"
 print("[CrossfireStep] event=started tick="..now().." until="..stepUntil.." level="..level)
end
local function spawn()
 GameManager:SetSimulationPaused(false)
 ids,planned,observed={},{},{}
 visualClock,outcomeAge,reportOpen=0,0,false
 if effects then effects:Reset();logEffects("reset") end
 if signals then signals:HideAll() end
 selected,paused,result,serial,started=1,true,nil,0,nil
 stepUntil=nil;review=Review.New();alert=nil;inspection=nil;reviewSummary=nil;alertKey="";tacticKey=""
 Scene.SetOutcome(nil)
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
 stepUntil=nil;inspection=nil
 paused=true; overlay=nil; GameManager:SetSimulationPaused(true)
 transition={level=index,screen=targetScreen,reason=reason}
end
local function openOverlay(name)
 stepUntil=nil;inspection=nil
 if not paused then paused=true; GameManager:SetSimulationPaused(true) end
 overlay=name
 print("[CrossfireMenu] overlay="..name.." tick="..now())
end
local function saveSettings()
 profile.muted=muted
 applyVolume()
 saveError=not profile:Save()
 if saveError then hint="设置已生效，本次未能写入保存文件" end
 print(string.format("[CrossfireSettings] volume=%.2f muted=%s hints=%s",profile.volume,tostring(muted),tostring(profile.hints)))
end
local function showReport()
 if not result or reportOpen then return end
 reportOpen=true
 logEffects("report")
 print("[CrossfireOutcome] phase=report result="..result.." elapsedUiMs="..outcomeAge.." tick="..now())
end
local function action(name)
 if name=="pause" then togglePause()
 elseif name=="step" then stepExecution()
 elseif name=="inspect" and alert and screen=="battle" and not result and not overlay then
  local inspected=alert
  stepUntil=nil;paused=true;GameManager:SetSimulationPaused(true)
  selectUnit(inspected.slot);inspection=inspected
  hint=inspected.detail
  print("[CrossfireInspect] slot="..inspected.slot.." reason="..inspected.state.." target="..inspected.target.." first="..inspected.first.." tick="..now())
 elseif name=="start" and screen=="title" then
  screen="battle"; overlay=nil; if transition then transition.screen="battle" end
  sound("start",nil,.4,2)
  print("[CrossfireScreen] screen=battle level="..level.." tick="..now())
 elseif name=="restart" then restart=true; overlay=nil;stepUntil=nil;inspection=nil
 elseif name=="report" then showReport()
 elseif name=="next" and result=="VICTORY" and reportOpen then
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
 local reviewActors={}
 for i,id in ipairs(ids) do
  local a=find(id)
  if a then
   local bb=board(a); local pos=a:GetPosition(); local hp=a:GetHealth(); local old=observed[id]
   bb:SetInt("crossfire.clockMs",now())
   local shot,blocked,hit=bb:GetInt("crossfire.shotCount",0),bb:GetInt("crossfire.blockedCount",0),bb:GetInt("crossfire.hitCount",0)
   if shot>old.shot then
    sound("shot",pos,i>2 and .52 or .35,1)
    effects:Emit("muzzle",a:GetWeaponComponent():GetMuzzlePosition(),a:GetForward(),visualClock)
   end
   if blocked>old.blocked or hit>old.hit then
    -- Counts may both change in one simulation step; kind and contact point
    -- must describe the same latest physical event.
    local impactKind=bb:GetBool("crossfire.lastBlocked",false) and "shield" or "hit"
    local impact=bb:GetVec3("crossfire.lastImpactPosition")
    sound(impactKind,impact,.55,2)
    effects:Emit(impactKind,impact,nil,visualClock)
   end
   if hp<=0 and old.hp>0 then
    if i==selected and i<3 then
     local other=i==1 and 2 or 1;local survivor=find(ids[other])
     if survivor and survivor:GetHealth()>0 then selected=other end
    end
    sound("destroy",pos,.65,3)
    effects:Emit("destroy",pos,nil,visualClock)
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
      -- Dashed ground warning is the recorded lock point, not a live projectile.
      local span=aim-center
      for segment=0,10,2 do drawLine(center+span*(segment/12),center+span*((segment+1)/12),amber) end
      DebugDrawer:drawCircle(aim,.55,32,amber,false)
      DebugDrawer:drawCircle(aim,.55*(1-charge)+.08,24,amber,false)
     end
    end
   end
   local item={id=id,lockId=bb:GetInt("crossfire.lockId",-1),pos=pos,hp=hp,maxHp=old.maxHp,enemy=i>2,name=Feedback.Name(id,ids),state=hp<=0 and "OFFLINE" or bb:GetString("crossfire.state"),
    charge=bb:GetFloat("crossfire.charge",0),phaseProgress=bb:GetFloat("crossfire.phaseProgress",0),
    phaseRemainingMs=bb:GetFloat("crossfire.phaseRemainingMs",0),
    damaged=not bb:GetBool("crossfire.lastBlocked",true) and now()-bb:GetInt("crossfire.lastImpactMs",-10000)<350,
    blocked=bb:GetBool("crossfire.lastBlocked",false) and now()-bb:GetInt("crossfire.lastImpactMs",-10000)<550}
   if i>2 then
    signals:Set(i-2,a:GetWeaponComponent():GetMuzzlePosition(),item.state,item.phaseProgress,
     hp>0 and screen=="battle" and not overlay and not result)
    if old.phase~=item.state then
     old.phase=item.state
     print(string.format("[CrossfirePhase] slot=%d state=%s remainingMs=%d progress=%.3f tick=%d",
      i,item.state,item.phaseRemainingMs,item.phaseProgress,now()))
    end
   end
   if i<3 then
    item.identityColor=Feedback.colors[i]
    local requested=planned[i] and planned[i].kind=="attack" and planned[i].target or bb:GetInt("crossfire.target",-1)
    if requested<=0 then requested=bb:GetInt("crossfire.lockId",-1) end
    local target=requested>0 and find(requested) or a:GetAIComponent():GetEnemy()
    local report=target and Feedback.Target(a,target,ids,find) or Feedback.Reference(a,ids,find)
    item.fireState=report and report.text or "抵达后自动寻找目标"
    item.targetName=report and not report.reference and report.name or nil
    local moving=planned[i] and planned[i].kind=="move" or (not planned[i] and bb:Has("movePos"))
    if report and not moving then
     if report.state~="clear" then old.stalledAt=old.stalledAt or now() else old.stalledAt=nil end
     if screen=="battle" and not overlay and not result and
      ((i==selected and (paused or report.state~="clear")) or (inspection and inspection.slot==i)) then
      Feedback.Draw(report,Feedback.colors[i])
     end
     if i==selected then ctx.fireHint=(report.reference and "附近射界 · " or (paused and "当前射界 · " or ""))..report.name.."："..report.text end
     if old.stalledAt and now()-old.stalledAt>1800 and old.reportState~=report.state then
      print("[CrossfireObstruction] slot="..i.." target="..report.target.." first="..report.first.." reason="..report.state.." reference="..tostring(report.reference==true).." tick="..now())
      old.reportState=report.state
     elseif report.state=="clear" then old.reportState=nil end
    else
     old.stalledAt=nil
     if moving then item.fireState=paused and "路线已规划 · 按空格执行" or "移动中";item.targetName=nil end
    end
    reviewActors[#reviewActors+1]={slot=i,name=item.name,hp=hp,moving=moving,shots=shot,report=report and not report.reference and report or nil}
    item.order=planned[i] and (planned[i].kind=="move" and "MOVE / QUEUED" or (planned[i].kind=="attack" and "TARGET / QUEUED" or "HOLD / QUEUED")) or nil
    ctx.allies[#ctx.allies+1]=item
   end
   ctx.actors[#ctx.actors+1]=item
  end
 end
 alert=review:Update(now(),screen=="battle" and not paused and not overlay and not result and not physicsTest,reviewActors)
 local nextAlertKey=alert and (alert.slot..":"..alert.state..":"..alert.target..":"..alert.first) or ""
 if nextAlertKey~=alertKey then
  alertKey=nextAlertKey
  if alert and screen=="battle" and not result and not physicsTest then
   print("[CrossfireAlert] slot="..alert.slot.." reason="..alert.state.." target="..alert.target.." first="..alert.first.." tick="..now())
  end
 end
 if inspection then
  local inspected=reviewActors[inspection.slot]
  if not inspected or inspected.hp<=0 or inspected.moving or not inspected.report
   or inspected.report.target~=inspection.target or inspected.report.first~=inspection.first
   or inspected.report.state~=inspection.state then inspection=nil end
 end
 if inspection and paused and screen=="battle" and not overlay and not result and inspection.pos then
  local p=inspection.pos;local point=Vector3(p.x,p.y,p.z)
  DebugDrawer:drawCircle(Vector3(p.x,.12,p.z),.75,36,amber,false)
  DebugDrawer:drawCircle(Vector3(p.x,.12,p.z),.48,28,amber,false)
  drawLine(Vector3(p.x,.12,p.z),point+Vector3(0,.55,0),amber)
 end
 if not result and not physicsTest and screen=="battle" then
  if enemiesAlive==0 then result="VICTORY"
  elseif living==0 then result="DEFEAT" end
  if result then
   paused=true;GameManager:SetSimulationPaused(true);planned={};stepUntil=nil;inspection=nil
   outcomeAge=0;reportOpen=false
   print("[CrossfireOutcome] phase=reveal result="..result.." tick="..now())
   signals:HideAll()
   Scene.SetOutcome(result)
   sound(result=="VICTORY" and "win" or "lose",nil,.65,4)
   local elapsed=now()-(started or now())
   reviewSummary=review:Summary({result=result,living=living,damage=damage,flankHits=flankHits,blockedShots=blockedShots,elapsedMs=elapsed})
   local metrics=reviewSummary.metrics
   local focus=metrics.focus
   print(string.format("[CrossfireReview] shieldMs=%d entityMs=%d focusSlot=%d focusState=%s focusQueryMs=%d emittedShots=%d scope=per_ally_query_time",
    metrics.shieldMs,metrics.entityMs,focus and focus.slot or 0,focus and focus.state or "none",
    focus and focus.queryMs or 0,focus and focus.emittedShots or 0))
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
 local tactics=Tactics.Read(ctx.actors,selected)
 if screen=="battle" and not overlay and not result then
  for i,a in ipairs(ctx.allies) do a.threat=tactics.byAlly[i] end
  local key=tactics.kind..":"..(tactics.sourceSlot or 0)..":"..(tactics.slot or 0)
  if key~=tacticKey then
   tacticKey=key
   print(string.format("[CrossfireTactic] kind=%s source=%d target=%d tick=%d",tactics.kind,tactics.sourceSlot or 0,tactics.slot or 0,now()))
  end
 end
 if screen=="battle" and not result then
  ctx.criticalColor=tactics.color
  ctx.critical=tactics.text or (level==3 and "两机拉开，分别点对侧哨卫；红线受阻时换位。" or (level==2 and "设备会挡弹；绕过设备，从侧面开火。" or "橙弧是护盾正面；两机分路，抵达后自动开火。"))
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
 ctx.result=result;ctx.resultReveal=result and not reportOpen;ctx.outcomeAge=outcomeAge;ctx.paused=paused;ctx.hint=hint;ctx.selected=selected
 ctx.stepRemaining=stepUntil and math.max(0,stepUntil-now()) or nil
 ctx.alert=not result and alert or nil;ctx.inspection=inspection;ctx.review=reviewSummary
 ctx.medal,ctx.medalText,ctx.newBest=medal,result=="DEFEAT" and "FIND A NEW ANGLE" or medalText,newBest
 ctx.completedCount=profile:CompletedCount();ctx.enemiesAlive=enemiesAlive;ctx.saveError=saveError
 ctx.nextLabel=level<#Levels and "ENTER / NEXT RELAY" or "ENTER / BACK TO OPERATIONS"
 ctx.resultDetail=string.format("%d 次有效侧击 · %d 次正面挡弹",flankHits,blockedShots)..(result=="DEFEAT" and "\n检查红色射线，绕开护盾与残骸再攻击。" or (newBest and " · 新纪录" or ""))
 if profile.hints and not started and screen=="battle" and not result then
  local first,second=planned[1]~=nil,planned[2]~=nil
  ctx.tutorial={step=not first and 1 or (not second and 2 or 3),done1=first,done2=second,
   title=not first and "1 · 给 VEGA 规划路线" or (not second and "2 · 给 ROOK 另一条路线" or "3 · 两机同时执行"),
   detail=not first and "选中 VEGA，再点设备旁的空地。鼠标处会显示路线。" or (not second and "点 ROOK 或按 2，再点哨卫另一侧的空地。" or "按空格持续执行，或按 E 推进 2 秒。")}
 end
 ctx.precision=Profile.Precision(now()-(started or now()),damage,living,Levels[level].parMs)
 ctx.stats=string.format("%d / 2 台存活 · %.1f 秒 · 承受 %d 损伤",living,(now()-(started or now()))/1000,damage)
 hud:Update(ctx)
end
function Sandbox_Initialize()
 SandboxUI:SetBuildInfoVisible(false)
 _G.HELLO_SUPPRESS_AI_PATH_DRAW=true
 profile=Profile.Load(); muted=profile.muted
 applyVolume()
 print(string.format("[CrossfireSettings] loaded=true volume=%.2f muted=%s hints=%s completed=%d",profile.volume,tostring(muted),tostring(profile.hints),profile:CompletedCount()))
 -- Test launches opt into direct battle; ordinary play always gets the title.
 local initial=tonumber(os.getenv("HELLO_CROSSFIRE_LEVEL") or "1")
 if initial and initial==math.floor(initial) and initial>=1 and initial<=#Levels then level=initial end
 if os.getenv("HELLO_CROSSFIRE_PHYSICS_TEST")=="1" or os.getenv("HELLO_CROSSFIRE_QUERY_TEST")=="1" then level=1 end
 screen=(os.getenv("HELLO_CROSSFIRE_QUICKSTART")=="1" or os.getenv("HELLO_CROSSFIRE_PHYSICS_TEST")=="1" or os.getenv("HELLO_CROSSFIRE_QUERY_TEST")=="1") and "battle" or "title"
 Scene.Create(level)
 for i=1,4 do shadows[i]=Scene.Shadow() end
 hud=Hud.New()
 effects=Effects.New()
 signals=Signals.New(#Levels[level].enemies)
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
function Sandbox_Update(deltaMs,uiDeltaMs)
 if not hud or #ids<3 then return end
 if restart or transition then
  local requested=transition
  restart=false;transition=nil;physicsTest=nil
  GameManager:SetSimulationPaused(false)
  SandboxAudio:StopAll();SandboxObjects:ClearProjectiles()
  -- Drop every Lua-held actor reference before destruction and nav replacement.
  ids,planned,observed={},{},{}
  ObjectManager:clearAllObjects(MGR_OBJ_AGENT,true)
  if TeamBlackboard then TeamBlackboard:Reset() end
  if requested and requested.level~=level then
   shadows={}
   effects:Release();effects=nil
   signals:Release();signals=nil
   ObjectManager:clearAllObjects(MGR_OBJ_BLOCK,true)
   level=requested.level
   Scene.Create(level)
   for i=1,4 do shadows[i]=Scene.Shadow() end
   effects=Effects.New()
   signals=Signals.New(#Levels[level].enemies)
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
 if stepUntil and not paused and now()>=stepUntil and not result then
  local deadline=stepUntil;stepUntil=nil;paused=true;GameManager:SetSimulationPaused(true)
  hint="2 秒已结束 · 调整计划，或按 E 继续推进"
  print("[CrossfireStep] event=finished tick="..now().." until="..deadline.." level="..level)
 end
 local uiMs=math.max(0,math.min(100,tonumber(uiDeltaMs) or deltaMs or 0))
 if result and not overlay then
  outcomeAge=outcomeAge+uiMs
  visualClock=visualClock+uiMs
  if outcomeAge>=900 then showReport() end
 elseif not result then visualClock=visualClock+math.max(0,deltaMs or 0) end
 Scene.UpdateVisuals(deltaMs,overlay and 0 or uiMs,overlay and "menu" or screen,paused)
 Scene.Camera(zoom)
 presentation()
 effects:Update(visualClock)
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
 elseif keycode==OIS.KC_E then action("step")
 elseif keycode==OIS.KC_1 then action("select1")
 elseif keycode==OIS.KC_2 then action("select2")
 elseif keycode==OIS.KC_R then action("restart")
 elseif keycode==OIS.KC_RETURN and result then
  if not reportOpen then action("report") elseif result=="VICTORY" then action("next") end
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
