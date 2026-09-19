-- Manager-owned Gorilla primitives; stable keys survive every encounter/retry.
-- All inputs are plain values. Hit returns an action or "block" for UI surfaces.
local Text=require("res.scripts.samples.crossfire_hud_text.lua")
local Hud={}; Hud.__index=Hud
local colors={
 clear=ColourValue(0,0,0,0), white=ColourValue(1,1,1,1), panel=ColourValue(.045,.075,.089,.96),
 solid=ColourValue(.035,.061,.073,.99), selected=ColourValue(.075,.205,.214,.98),
 hover=ColourValue(.12,.28,.29,.99), cyan=ColourValue(.259,.788,.706,1), blue=ColourValue(.392,.596,.855,1),
 amber=ColourValue(.98,.57,.29,1), track=ColourValue(.10,.16,.18,1),
 rule=ColourValue(.18,.28,.30,1), shade=ColourValue(.014,.026,.035,.63),
 resultShade=ColourValue(.014,.026,.035,.43), disabled=ColourValue(.045,.069,.079,.95)
}
local function number(value,default)
 local n=tonumber(value)
 if not n or n~=n or n==math.huge or n==-math.huge then return default or 0 end
 return n
end
local function clamp(value,a,b) return math.max(a,math.min(b,number(value,a))) end
local plain=Text.Plain
local function clock(ms)
 local seconds=math.floor(math.max(0,number(ms))/1000)
 return string.format("%02d:%02d",math.floor(seconds/60),seconds%60)
end
local fit=Text.Fit
local function inside(x,y,b)
 return x>=b.x and y>=b.y and x<b.x+b.w and y<b.y+b.h
end
local function levelInfo(ctx)
 local index=math.floor(clamp(ctx.level or 1,1,3))
 return (ctx.levels or {})[index] or {},index
end
local function collection(ctx)
 if ctx.completedCount~=nil then return math.floor(clamp(ctx.completedCount,0,3)) end
 local count=0
 for i=1,3 do if (ctx.records or {})[i] then count=count+1 end end
 return count
end
function Hud.New()
 return setmetatable({frames={},hits={},hitCount=0,wrapCache={},actorHits={},actorCount=0,labelRects={}},Hud)
end
function Hud:Frame(key,x,y,w,h,color,text,font,layer,muted)
 local node=self.frames[key]
 if not node then
  local object=SandboxUI:CreateUIFrame(layer or 4)
  if not object then return end
  object:setTextMargin(0,0)
  node={object=object,visible=false}; self.frames[key]=node
 end
 local f=node.object
 x,y,w,h=math.floor(x),math.floor(y),math.max(1,math.floor(w)),math.max(1,math.floor(h))
 if node.x~=x or node.y~=y then f:setPosition(Vector2(x,y)); node.x,node.y=x,y end
 if node.w~=w or node.h~=h then f:setDimension(Vector2(w,h)); node.w,node.h=w,h end
 color=color or "clear"
 if node.color~=color then f:setBackgroundColor(colors[color] or color); node.color=color end
 if node.raw~=text or node.font~=font or node.muted~=muted then
  local markup=text and ((muted and "%8" or "%0")..Text.Markup(text,font or 9)) or ""
  if node.text~=markup then f:setMarkupText(markup);node.text=markup end
  node.raw,node.font,node.muted=text,font,muted
 end
 if not node.visible then f:setVisible(true); node.visible=true end
 node.used=true
end
function Hud:Text(key,x,y,w,h,text,font,layer,muted)
 self:Frame(key,x,y,w,h,"clear",text,font or 9,layer or 5,muted)
end
function Hud:Wrap(key,value,width,font,maxLines)
 local text=plain(value)
 local cached=self.wrapCache[key]
 width=math.max(8,math.floor(width)-2);font=font or 9;maxLines=maxLines or 2
 if cached and cached.text==text and cached.width==width and cached.font==font and cached.maxLines==maxLines then return cached.value end
 local valueText=Text.Wrap(text,width,font,maxLines)
 self.wrapCache[key]={text=text,width=width,font=font,maxLines=maxLines,value=valueText}
 return valueText
end
function Hud:Region(x,y,w,h,action)
 self.hitCount=self.hitCount+1
 local hit=self.hits[self.hitCount]
 if not hit then hit={}; self.hits[self.hitCount]=hit end
 hit.x,hit.y,hit.w,hit.h,hit.action=x,y,w,h,action or "block"
end
function Hud:Hovered(x,y,w,h,layer)
 return (layer or 4)>=self.hoverLayer and self.hoverX>=x and self.hoverY>=y and self.hoverX<x+w and self.hoverY<y+h
end
function Hud:Button(key,x,y,w,h,label,action,primary,layer,enabled)
 layer=layer or 4
 local active=enabled~=false
 local hovered=active and self:Hovered(x,y,w,h,layer)
 local color=not active and "disabled" or (hovered and "hover" or (primary and "selected" or "panel"))
 self:Frame(key,x,y,w,h,color,nil,nil,layer)
 self:Frame(key.."_edge",x,y+h-2,w,2,primary and "cyan" or (hovered and "cyan" or "rule"),nil,nil,layer)
 local font=h>=40 and 14 or 9
 local pad=w<70 and 8 or 14
 local text=self:Wrap(key.."_label",label,w-pad*2,font,1)
 self:Text(key.."_text",x+pad,y+math.floor((h-(font==14 and 26 or 20))/2),w-pad*2,h-4,text,font,layer+1,not active)
 self:Region(x,y,w,h,active and action or "block")
end
function Hud:Medal(key,x,y,value,layer)
 for i=1,3 do self:Frame(key..i,x+(i-1)*16,y,11,5,i<=number(value) and "amber" or "track",nil,nil,layer) end
end
function Hud:Header(ctx,title)
 local w=self.width
 local level,index=levelInfo(ctx)
 self:Text("brand",20,8,260,30,"CROSS / FIRE",14)
 self:Text("edition",20,35,290,22,title and "双机协同 / 三处中继站" or (string.format("%02d / 03  ",index)..(level.name or "维修庭院")),9)
 self:Button("settings",w-282,14,124,34,"ESC  设置","settings")
 self:Button("retry",w-148,14,124,34,title and "退出游戏" or "R  重试",title and "quit" or "restart")
 self:Button("help",w-282,58,124,30,"H  操作说明","help")
 local zeroVolume=ctx.settings and number(ctx.settings.volume,.65)<=0
 self:Button("mute",w-148,58,124,30,ctx.muted and "M  已静音" or (zeroVolume and "M  音量为零" or "M  声音开"),"mute")
 if not title then
  self:Button("phase",w/2-146,10,150,38,ctx.result and "本关已结束" or (ctx.paused and "空格  执行" or "空格  暂停"),"pause",ctx.paused,4,not ctx.result)
  self:Button("step",w/2+12,10,132,38,ctx.stepRemaining and string.format("%.1f 秒后停",ctx.stepRemaining/1000) or "E  推进 2 秒","step",false,4,not ctx.result)
  local critical=ctx.critical or "两机分路，从没有护盾的侧面进攻。"
  self:Frame("critical_background",20,58,w-328,42,"panel")
  self:Frame("critical_edge",20,63,3,32,"amber")
  self:Text("critical",30,59,w-338,40,self:Wrap("critical",critical,w-338,9,2),9)
  for i=1,3 do self:Frame("progress"..i,20+(i-1)*23,98,17,2,i==index and "cyan" or ((ctx.records or {})[i] and "amber" or "track")) end
 end
end
function Hud:Title(ctx)
 local level,index=levelInfo(ctx)
 local x,y,w=24,math.max(142,math.floor(self.height*.20)),372
 local h=478
 self:Frame("title_panel",x,y,w,h,"panel",nil,nil,6)
 self:Frame("title_accent",x,y,3,h,"cyan",nil,nil,6)
 self:Region(x,y,w,h,"block")
 self:Text("title_heading",x+24,y+22,w-48,32,collection(ctx)==3 and "三处中继站已收复" or "选择中继站",14,7)
 self:Text("title_detail",x+24,y+59,w-48,20,ctx.saveError and "本次进度暂时无法保存。" or (collection(ctx)==3 and "三关已完成，可以重访并改进路线。" or "三场遭遇，自由选择，从侧翼打开局面。"),9,7)
 for i=1,3 do
  local entry=(ctx.levels or {})[i] or {}
  local record=(ctx.records or {})[i]
  local cy=y+94+(i-1)*82
  local key="level"..i
  local hovered=self:Hovered(x+18,cy,w-36,70,6)
  self:Frame(key,x+18,cy,w-36,70,hovered and "hover" or (i==index and "selected" or "solid"),nil,nil,6)
  self:Frame(key.."_accent",x+18,cy,3,70,i==index and "cyan" or "rule",nil,nil,6)
  self:Text(key.."_number",x+31,cy+13,34,29,string.format("%02d",i),14,7)
  self:Text(key.."_name",x+78,cy+12,w-119,26,self:Wrap(key.."_name",entry.name or ("中继站 "..i),w-119,14,1),14,7)
  local detail=record and ("最佳 "..clock(record.timeMs).." / "..math.floor(number(record.damage)).." 损伤") or (entry.subtitle or "准备行动")
  self:Text(key.."_record",x+78,cy+43,w-149,20,self:Wrap(key.."_record",detail,w-149,9,1),9,7,not record)
  self:Medal(key.."_medal",x+w-78,cy+55,record and record.medal or 0,7)
  self:Region(x+18,cy,w-36,70,"level"..i)
 end
 self:Text("title_lesson",x+24,y+348,w-48,44,self:Wrap("title_lesson",level.lesson or "两机分路，寻找没有护盾的侧面。",w-48,9,2),9,7)
 self:Button("start",x+24,y+407,w-48,48,"ENTER  /  开始行动 "..(level.id or string.format("%02d",index)),"start",true,6)
 local captionX=math.max(x+w+44,self.width-350)
 local captionW=self.width-captionX-24
 self:Frame("scene_caption_panel",captionX-12,self.height-178,captionW+24,146,"panel",nil,nil,4)
 self:Text("scene_kicker",captionX,self.height-165,captionW,22,"中继站 "..(level.id or "07").." / "..string.format("第 %02d / 03 关",index),9,5)
 self:Text("scene_name",captionX,self.height-135,captionW,34,self:Wrap("scene_name",level.name or "维修庭院",captionW,14,1),14,5)
 self:Text("scene_description",captionX,self.height-92,captionW,42,self:Wrap("scene_description",level.description or "一座哨卫，两条路线，寻找侧翼。",captionW,9,2),9,5)
 self:Text("title_footer",24,self.height-36,420,22,"1 / 2 / 3 选关   ENTER 开始   "..collection(ctx).." / 3 已收复",9,7)
end
-- Stable actor names stay visible; status is a separate line, never a rename.
function Hud:Actors(ctx)
 local top,bottom=104,self.height-130
 local count=0
 for i,a in ipairs(ctx.actors or {}) do
  if a.pos and number(a.hp)>0 then
   local p=SandboxCamera:WorldToScreen(a.pos+Vector3(0,1.15,0))
   local body=SandboxCamera:WorldToScreen(a.pos)
   self.actorCount=self.actorCount+1
   local hit=self.actorHits[self.actorCount] or {};self.actorHits[self.actorCount]=hit
   hit.slot,hit.x,hit.y,hit.label=i,body.x,body.y,nil
   hit.radius=a.enemy and 24 or 20
   if p.x>0 and p.x<self.width and p.y>top-40 and p.y<bottom+40 then
    local lw,lh=a.enemy and 130 or 100,a.enemy and 42 or 25
    local x=clamp(p.x-lw/2,20,self.width-lw-20)
    local y=clamp(a.enemy and (p.y-lh-10) or (body.y+18),top,bottom-lh)
    -- Four actors at most: try fixed nearby lanes without allocating native UI.
    for attempt=0,7 do
     local clash=false
     for j=1,count do
      local other=self.labelRects[j]
      if x<other.x+other.w+6 and x+lw+6>other.x and y<other.y+other.h+5 and y+lh+5>other.y then clash=true;break end
     end
     if not clash then break end
     if attempt<3 then y=clamp(y-lh-7,top,bottom-lh)
     elseif attempt<6 then x=clamp(p.x+24,20,self.width-lw-20);y=clamp(p.y+(attempt-4)*(lh+7),top,bottom-lh)
     else x=clamp(p.x-lw-30,20,self.width-lw-20) end
    end
    count=count+1
    local rect=self.labelRects[count] or {};self.labelRects[count]=rect
    rect.x,rect.y,rect.w,rect.h=x,y,lw,lh;hit.label=rect
    local key="actor"..i
    local identity=a.enemy and "amber" or (a.identityColor or (i==2 and "blue" or "cyan"))
    local name=a.name or (a.enemy and "哨卫" or (i==2 and "ROOK" or "VEGA"))
    if not a.enemy then name=(i==2 and "02  " or "01  ")..name end
    self:Frame(key.."_label",x,y,lw,lh,"panel",nil,nil,3)
    self:Frame(key.."_identity",x,y,3,lh,identity,nil,nil,3)
    self:Text(key.."_name",x+8,y+1,lw-14,22,self:Wrap(key.."_name",name,lw-14,9,1),9,4)
    if a.enemy then
     self:Text(key.."_state",x+8,y+21,lw-14,21,a.blocked and "护盾挡弹" or "橙弧 / 护盾正面",9,4,true)
    end
    self:Frame(key.."_track",x,y+lh,lw,3,"track",nil,nil,3)
    self:Frame(key.."_hp",x,y+lh,lw*clamp(number(a.hp)/math.max(1,number(a.maxHp,120)),0,1),3,a.damaged and "white" or identity,nil,nil,3)
   end
  end
 end
end
local states={OFFLINE="已失联",READY="待命",MOVING="移动中",MOVE="移动中",ATTACK="交战中",FIRING="开火中",IDLE="待命",CHARGING="蓄力中",TRACKING="瞄准中",LOCKING="蓄力中",COOLING="冷却中",COVERING="寻找射界",["OUT OF RANGE"]="超出射程",
 ["MOVE / QUEUED"]="移动路线已规划",["TARGET / QUEUED"]="攻击目标已规划",["HOLD / QUEUED"]="原地待命已规划"}
function Hud:Battle(ctx)
 local y=self.height-120
 local cardW=self.width>=1100 and 208 or 184
 for i=1,2 do
  local a=(ctx.allies or {})[i] or {}
  local hp=math.max(0,number(a.hp));local alive=hp>0
  local x=20+(i-1)*(cardW+12)
  local key="ally"..i;local selected=ctx.selected==i and alive
  local identity=a.identityColor or (i==1 and "cyan" or "blue")
  local hovered=alive and self:Hovered(x,y,cardW,100,4)
  self:Frame(key,x,y,cardW,100,hovered and "hover" or (selected and "selected" or "panel"))
  self:Frame(key.."_accent",x,y,4,100,identity)
  self:Frame(key.."_selected",x+4,y,cardW-4,selected and 3 or 1,selected and identity or "rule")
  self:Text(key.."_name",x+14,y+8,cardW-22,28,i==1 and "01  VEGA" or "02  ROOK",14,5,not alive)
  local state=not alive and "已失联" or (a.order or a.state or "READY")
  state=states[state] or state
  self:Text(key.."_status",x+14,y+37,cardW-24,22,self:Wrap(key.."_status",state,cardW-24,9,1),9,5,not alive)
  local detail=not alive and "本关无法继续指挥" or (a.targetName and ("目标 / "..a.targetName) or (selected and "已选中 · 点地面下令" or "点击 / 按 "..i.." 选择"))
  self:Text(key.."_detail",x+14,y+60,cardW-24,22,self:Wrap(key.."_detail",detail,cardW-24,9,1),9,5,not alive)
  self:Frame(key.."_track",x+14,y+88,cardW-28,4,"track")
  if alive then self:Frame(key.."_hp",x+14,y+88,(cardW-28)*clamp(hp/math.max(1,number(a.maxHp,120)),0,1),4,a.damaged and "white" or (hp>40 and identity or "amber")) end
  self:Region(x,y,cardW,100,alive and ("select"..i) or "block")
 end
 local x=20+2*(cardW+12);local w=self.width-x-20
 self:Frame("command_panel",x,y,w,100,"panel")
 self:Region(x,y,w,100,"block")
 local selected=(ctx.allies or {})[ctx.selected or 1] or {}
 local fire=ctx.fireHint or selected.fireState or "抵达后自动寻找目标"
 local fireLines=ctx.fireHint and 2 or 1
 self:Text("fire_reason",x+14,y+5,w-28,fireLines*20,self:Wrap("fire_reason",fire,w-28,9,fireLines),9)
 local tutorial=(not ctx.settings or ctx.settings.hints~=false) and ctx.tutorial or nil
 local detail=tutorial and ((tutorial.title or "规划路线").." · "..(tutorial.detail or "")) or ctx.hint or "点地面移动，点哨卫指定攻击。"
 local detailLines=tutorial and not ctx.alert and 2 or 1
 if not ctx.alert then
  self:Text("feedback",x+14,y+9+fireLines*20,w-28,detailLines*20,self:Wrap("feedback",detail,w-28,9,detailLines),9)
 end
 if ctx.alert then
  self:Frame("alert_accent",x+14,y+65,3,29,"amber")
  self:Text("alert_title",x+23,y+68,w-173,22,self:Wrap("alert_title",ctx.alert.title,w-173,9,1),9)
  self:Button("inspect",x+w-136,y+63,122,32,ctx.inspection and "已定位受阻" or "查看受阻","inspect",false,4)
 elseif fireLines+detailLines<4 then
  self:Text("cursor_hint",x+14,y+77,w-28,22,self:Wrap("cursor_hint",ctx.cursorHint or "1 / 2 选机   空格 暂停 / 执行   E 推进 2 秒",w-28,9,1),9,5,true)
 end
end
function Hud:Endpoints(ctx)
 for i=1,2 do
  local point=ctx["endpoint"..i]
  local actor=(ctx.allies or {})[i]
  local nearby=point and actor and actor.pos and (actor.pos-point):squaredLength()<4
  if point and not nearby then
   local p=SandboxCamera:WorldToScreen(point)
   local x,y=clamp(p.x+13,20,self.width-48),clamp(p.y-12,104,self.height-157)
   local identity=i==1 and "cyan" or "blue"
   -- Endpoint numbers use the same identity stripe as cards and actor labels.
   self:Frame("endpoint"..i,x,y,30,24,"panel",nil,nil,2)
   self:Frame("endpoint_edge"..i,x,y,3,24,identity,nil,nil,2)
   self:Text("endpoint_text"..i,x+6,y+1,25,22,i==1 and "01" or "02",9,3)
  end
 end
end
function Hud:Outcome(ctx)
 local victory=ctx.result=="VICTORY"
 local w=584;local x=math.floor((self.width-w)/2);local y=self.height-122
 self:Region(0,0,self.width,self.height,"block")
 self:Frame("outcome_panel",x,y,w,102,"panel",nil,nil,8)
 self:Frame("outcome_edge",x,y,w,3,victory and "cyan" or "amber",nil,nil,9)
 self:Text("outcome_title",x+22,y+8,w-44,42,victory and "中继站已收复" or "小队已失联",24,10)
 self:Text("outcome_detail",x+22,y+57,w-224,22,victory and "防线已解除 · 正在恢复供电" or "行动结束 · 调整路线后再试",9,10)
 self:Button("outcome_report",x+w-184,y+53,162,32,"ENTER 查看报告","report",false,10)
end
function Hud:Result(ctx)
 local level,index=levelInfo(ctx)
 local victory=ctx.result=="VICTORY"
 local complete=victory and index==3 and collection(ctx)==3
 local w,h=584,500
 local x,y=math.floor((self.width-w)/2),math.floor((self.height-h)/2)
 self:Frame("result_shade",0,0,self.width,self.height,"resultShade",nil,nil,8)
 self:Region(0,0,self.width,self.height,"block")
 self:Frame("result_panel",x,y,w,h,"solid",nil,nil,9)
 self:Frame("result_accent",x,y,w,3,victory and "cyan" or "amber",nil,nil,10)
 self:Text("result_kicker",x+30,y+24,w-60,24,"中继站 "..(level.id or "07").." / 行动报告",9,11)
 self:Text("result_title",x+30,y+58,w-60,46,complete and "三站联通" or (victory and "中继站已收复" or "小队已失联"),24,11)
 local detail=ctx.resultDetail or (complete and "三处中继站已收复，重新挑战可以改进你的路线。" or (victory and "从侧面打开了局面。" or "两机分路，趁哨卫蓄力时换位。"))
 self:Text("result_detail",x+30,y+118,w-60,42,self:Wrap("result_detail",detail,w-60,9,2),9,11)
 self:Frame("result_rule",x+30,y+169,w-60,1,"rule",nil,nil,10)
 self:Text("result_medal_label",x+30,y+191,300,21,victory and (ctx.newBest and "刷新个人纪录" or "本关表现") or "下一次尝试",9,11)
 self:Text("result_medal",x+30,y+216,w-135,32,victory and (({[1]="中继站已收复",[2]="配合默契",[3]="精确协同"})[ctx.medal or 1]) or "寻找新的进攻角度",14,11)
 self:Medal("result_medal_pip",x+w-88,y+224,victory and ctx.medal or 0,11)
 self:Text("result_stats",x+30,y+260,w-60,24,self:Wrap("result_stats",ctx.stats or "",w-60,9,1),9,11)
 self:Frame("review_rule",x+30,y+290,w-60,1,"rule",nil,nil,10)
 local review=ctx.review or {}
 self:Text("review_title",x+30,y+305,w-60,24,self:Wrap("review_title",review.title or "行动复盘",w-60,9,1),9,11)
 self:Text("review_detail",x+30,y+331,w-60,43,self:Wrap("review_detail",review.detail or "两机分路，绕到侧面攻击。",w-60,9,2),9,11)
 self:Text("result_collection",x+30,y+382,w-60,23,"收复进度  "..collection(ctx).." / 3",9,11)
 for i=1,3 do
  local record=(ctx.records or {})[i]
  self:Frame("result_progress"..i,x+w-150+(i-1)*40,y+387,30,5,record and "cyan" or "track",nil,nil,11)
 end
 self:Button("result_next",x+30,y+412,w-60,40,victory and (index<3 and "ENTER  /  前往下一关" or "ENTER  /  返回选关") or "R  /  重试本关",victory and "next" or "restart",true,10)
 self:Button("result_retry",x+30,y+460,(w-72)/2,30,victory and "R  重玩本关" or "H  操作说明",victory and "restart" or "help",false,10)
 self:Button("result_menu",x+42+(w-72)/2,y+460,(w-72)/2,30,"返回选关","menu",false,10)
end
function Hud:Modal(ctx)
 local help=ctx.overlay=="help"
 local w,h=help and 596 or 520,help and 510 or 446
 local x,y=math.floor((self.width-w)/2),math.floor((self.height-h)/2)
 self:Frame("overlay_shade",0,0,self.width,self.height,"shade",nil,nil,12)
 self:Region(0,0,self.width,self.height,"block")
 self:Frame("overlay_panel",x,y,w,h,"solid",nil,nil,13)
 self:Frame("overlay_accent",x,y,w,3,"cyan",nil,nil,14)
 self:Text("overlay_kicker",x+30,y+22,w-130,22,help and "两台无人机 / 同步行动" or (ctx.screen=="title" and "偏好设置" or "任务已暂停"),9,15)
 self:Button("overlay_close",x+w-80,y+18,50,30,"ESC","close",false,14)
 self:Text("overlay_title",x+30,y+54,w-60,44,help and "操作说明" or "设置",24,15)
 if help then
  local guidance={
   {"先规划，再同步执行","点无人机或底部卡片选择，再点亮色边线内的地面规划路线。按空格持续执行，或按 E 推进 2 秒后停下观察。"},
   {"绕过正面，留意射线","橙色扇面保护哨卫正面。两机从不同方向接近，抵达后自动开火。点哨卫指定攻击；红色射线表示设备或残骸挡住弹丸。"},
   {"趁蓄力换位，必要时换目标","橙色长线是哨卫的瞄准方向，及时离开。点击“查看受阻”会暂停并标出挡弹处；换位后再指定目标。"}
  }
  for i,entry in ipairs(guidance) do
   local ry=y+119+(i-1)*91
   self:Text("guide_number"..i,x+30,ry,34,28,string.format("%02d",i),14,15)
   self:Text("guide_title"..i,x+77,ry,w-107,29,entry[1],14,15)
   self:Text("guide_detail"..i,x+77,ry+34,w-107,44,self:Wrap("guide_detail"..i,entry[2],w-107,9,2),9,15)
  end
  self:Frame("guide_rule",x+30,y+395,w-60,1,"rule",nil,nil,14)
  self:Text("guide_controls",x+30,y+409,w-60,42,"空格 暂停 / 执行   E 推进 2 秒   R 重试\n1 / 2 选机   X 原地待命   鼠标点击 移动 / 目标",9,15)
  self:Button("guide_return",x+30,y+459,w-60,36,ctx.screen=="title" and "关闭说明 / 返回选关" or "关闭说明 / 返回规划","close",true,14)
 else
  local settings=ctx.settings or {}
  local volume=clamp(settings.volume or .65,0,1)
  self:Text("settings_detail",x+30,y+106,w-60,22,ctx.saveError and "设置本次有效，当前目录无法保存。" or (ctx.screen=="title" and "偏好设置会在下次启动时保留。" or "关闭后保持暂停，按空格继续执行。"),9,15)
  self:Text("volume_label",x+30,y+146,272,29,"音量  "..math.floor(volume*100+.5).."%",14,15)
  self:Button("volume_mute",x+w-157,y+141,127,32,ctx.muted and "M  恢复声音" or "M  静音","mute",false,14)
  self:Button("volume_down",x+30,y+186,44,34,"-","volumeDown",false,14,volume>0)
  self:Frame("volume_track",x+90,y+201,w-180,5,"track",nil,nil,14)
  if volume>0 then self:Frame("volume_fill",x+90,y+201,(w-180)*volume,5,"cyan",nil,nil,14) end
  self:Button("volume_up",x+w-74,y+186,44,34,"+","volumeUp",false,14,volume<1)
  self:Text("volume_state",x+30,y+226,w-60,22,volume==0 and "音量为零 / 点击 + 恢复音量。" or (ctx.muted and "已静音 / 按 M 恢复声音。" or "M 切换静音，不改变音量设置。"),9,15)
  self:Text("hints_label",x+30,y+253,250,29,"步骤引导",14,15)
  self:Text("hints_detail",x+30,y+284,w-190,22,"显示初次路线规划提示。",9,15)
  self:Button("hints",x+w-154,y+252,124,40,settings.hints==false and "关闭" or "开启","hints",false,14)
  self:Frame("settings_rule",x+30,y+315,w-60,1,"rule",nil,nil,14)
  self:Button("settings_return",x+30,y+335,w-60,42,ctx.screen=="title" and "返回选关" or "返回规划","close",true,14)
  self:Button("settings_menu",x+30,y+396,(w-72)/2,32,"返回选关","menu",false,14)
  self:Button("settings_quit",x+42+(w-72)/2,y+396,(w-72)/2,32,"退出游戏","quit",false,14)
 end
end
function Hud:Update(ctx)
 self.width,self.height=GameManager:getScreenWidth(),GameManager:getScreenHeight()
 self.hoverX,self.hoverY=number(ctx.hoverX,-1),number(ctx.hoverY,-1)
 self.hoverLayer=ctx.overlay and 14 or (ctx.result and 10 or 0)
 self.hitCount=0; self.actorCount=0
 for _,node in pairs(self.frames) do node.used=false end
 local title=ctx.screen=="title"
 if title then self:Region(0,0,self.width,self.height,"block") end
 self:Header(ctx,title)
 if title then self:Title(ctx)
 elseif not ctx.result then self:Endpoints(ctx); self:Actors(ctx); self:Battle(ctx) end
 if ctx.result and not title then
  if ctx.resultReveal then self:Outcome(ctx) else self:Result(ctx) end
 end
 if ctx.overlay then self:Modal(ctx) end
 for _,node in pairs(self.frames) do
  if not node.used and node.visible then node.object:setVisible(false); node.visible=false end
 end
end
function Hud:PickActor(x,y)
 local nearest,distance=nil,math.huge
 for i=1,self.actorCount do
  local hit=self.actorHits[i]
  local d=(x-hit.x)^2+(y-hit.y)^2
  if d<(hit.radius or 20)^2 and d<distance then nearest,distance=hit.slot,d end
 end
 if nearest then return nearest end
 for i=1,self.actorCount do
  local hit=self.actorHits[i]
  if hit.label and inside(x,y,hit.label) then return hit.slot end
 end
end
function Hud:Hit(x,y)
 for i=self.hitCount,1,-1 do if inside(x,y,self.hits[i]) then return self.hits[i].action end end
end
return Hud
