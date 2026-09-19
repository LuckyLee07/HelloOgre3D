-- Run from repository root with Lua 5.1: lua tools/test_crossfire_commands.lua
-- Real Sandbox20 closures and value-only engine fakes. Never opens a window.
local function up(fn,name,value,set)
 for i=1,80 do
  local key,old=debug.getupvalue(fn,i)
  if not key then break end
  if key==name then if set then debug.setupvalue(fn,i,value) end;return old end
 end
 error("missing production upvalue "..name)
end
local function eq(a,b,message) assert(a==b,(message or "value")..": expected "..tostring(b)..", got "..tostring(a)) end
local function contains(s,part) assert(s and s:find(part,1,true),tostring(s).." missing "..part) end
local function noop() end
local vector={}
local v
v=function(x,y,z)
 return setmetatable({x=x,y=y,z=z},{__index=vector,
  __add=function(a,b) return v(a.x+b.x,a.y+b.y,a.z+b.z) end,
  __sub=function(a,b) return v(a.x-b.x,a.y-b.y,a.z-b.z) end,
  __mul=function(a,n) return v(a.x*n,a.y*n,a.z*n) end})
end
function vector:squaredLength() return self.x*self.x+self.y*self.y+self.z*self.z end
local function agent(id,x,z)
 local a={id=id,hp=120,pos=v(x,1,z),visible=true,clears=0,bb={values={},writes=0}}
 local bb=a.bb
 function bb:SetInt(k,value) self.values[k]=value;self.writes=self.writes+1 end
 bb.SetString=bb.SetInt;bb.SetVec3=bb.SetInt
 function bb:Remove(k) self.values[k]=nil;self.writes=self.writes+1 end
 function bb:GetInt(k,default) local value=self.values[k];if value==nil then return default end;return value end
 bb.GetFloat=bb.GetInt;bb.GetBool=bb.GetInt
 function bb:GetString(k) return self.values[k] or "" end
 function bb:GetVec3(k) return self.values[k] end
 function bb:Has(k) return self.values[k]~=nil end
 a.ai={GetBlackboard=function() return bb end,CanSeeEnemy=function() return a.visible end,
  GetEnemy=function() return a.enemy end,ClearMovePosition=function() a.clears=a.clears+1 end}
 function a:GetHealth() return self.hp end
 function a:GetObjId() return self.id end
 function a:GetPosition() return self.pos end
 function a:GetForward() return v(0,0,1) end
 function a:GetAIComponent() return self.ai end
 function a:GetWeaponComponent() return {GetMuzzlePosition=function() return self.pos end} end
 return a
end
local function harness()
 local h={logs={},pauseCalls={},projectileClears=0,recordCalls=0,clock=1000,width=960,height=720}
 h.agents={agent(1,0,0),agent(2,1,0),agent(3,0,10)}
 local list={[0]=h.agents[1],[1]=h.agents[2],[2]=h.agents[3],size=function() return 3 end}
 local feedback={colors={{},{}},Name=function(id) return ({"VEGA","ROOK","哨卫 01"})[id] end,
  Target=function(_,target) return {state="clear",text="射界畅通",name="哨卫 01",target=target.id,first=0} end,
  Reference=function() return {state="clear",text="附近射界",name="哨卫 01",target=3,first=0,reference=true} end,
  Draw=noop,DrawGround=noop,Ground=function() return {ok=false,text="设备表面不可通行"} end}
 local scene={DrawShield=noop,SetOutcome=noop,UpdateVisuals=noop,Camera=noop}
 local modules={crossfire_scene=scene,crossfire_feedback=feedback,crossfire_encounters={{parMs=22000}},
  crossfire_tactics={Read=function() return {kind="neutral",byAlly={}} end},
  crossfire_profile={Precision=function() return {} end}}
 local env=setmetatable({require=function(name) return modules[name:match("samples%.(.-)%.lua")] or {} end,
  ColourValue=function(...) return {...} end,Vector3=v,print=function(line) h.logs[#h.logs+1]=line end,
  os={getenv=function() return nil end},ObjectManager={getAllAgents=function() return list end},
  GameManager={getTimeInMillis=function() return h.clock end,getScreenWidth=function() return h.width end,
   SetSimulationPaused=function(_,state) h.pauseCalls[#h.pauseCalls+1]=state end},
  SandboxObjects={ClearProjectiles=function() h.projectileClears=h.projectileClears+1 end},
  DebugDrawer={drawLine=noop,drawCircle=noop},SandboxCamera={WorldToScreen=function(_,p) return {x=p.x,y=p.z} end},
  std={vector_Ogre__Vector3_=function() return {size=function() return 0 end} end},SandboxNav={FindPath=function() return false end}}, {__index=_G})
 local chunk=assert(loadfile("bin/res/scripts/samples/Sandbox20.lua"));setfenv(chunk,env);chunk()
 h.env=env;h.feedbackModule=feedback
 h.action=up(env.EventHandle_Keyboard,"action");h.order=up(env.EventHandle_Mouse,"order")
 h.toggle=up(h.action,"togglePause");h.step=up(h.action,"stepExecution")
 h.apply=up(h.order,"apply");h.validate=up(h.apply,"validate");h.commit=up(h.apply,"commit")
 h.setFeedback=up(h.order,"setFeedback");h.advance=up(env.Sandbox_Update,"advanceFeedback")
 h.present=up(env.Sandbox_Update,"presentation")
 h.tutorial=up(h.present,"tutorialFor");h.view=up(h.present,"commandView")
 function h:notice() return up(self.advance,"feedback") end
 function h:remaining() return up(self.advance,"feedbackRemainingMs") end
 function h:plans() return up(self.order,"planned") end
 function h:serial() return up(self.commit,"serial") end
 function h:paused() return up(self.toggle,"paused") end
 function h:started() return up(self.toggle,"started") end
 up(h.toggle,"screen","battle",true);up(h.validate,"ids",{1,2,3},true)
 local observed={}
 for i=1,3 do observed[i]={hp=120,maxHp=120,shot=0,blocked=0,hit=0} end
 up(h.present,"observed",observed,true)
 up(h.present,"shadows",{{setPosition=noop},{setPosition=noop},{setPosition=noop}},true)
 h.profile={records={},hints=true,CompletedCount=function() return 0 end,
  Record=function() h.recordCalls=h.recordCalls+1;return 2,true,true end}
 up(h.present,"profile",h.profile,true)
 up(h.present,"review",{Update=function() return nil end,Summary=function() return {metrics={shieldMs=0,entityMs=0}} end},true)
 up(h.present,"signals",{Set=noop,HideAll=noop},true)
 up(h.present,"effects",{Emit=noop,Update=noop},true)
 h.hud={Hit=function() return nil end,PickActor=function() return h.pick end,Update=function(_,ctx) h.ctx=ctx end}
 up(h.present,"hud",h.hud,true)
 return h
end
local count=0
local function test(name,fn) fn();count=count+1;print("PASS "..name) end
local function move(x,z) return {kind="move",pos=v(x,0,z)} end
local function attack() return {kind="attack",target=3} end
local function assertUntouched(h,p1,p2)
 eq(h:plans()[1],p1,"plan 1 retained");eq(h:plans()[2],p2,"plan 2 retained")
 eq(h:serial(),0,"serial unchanged");eq(h:started(),nil,"start unchanged");eq(h:paused(),true)
 eq(#h.pauseCalls,0,"native pause not touched")
 for _,a in ipairs(h.agents) do eq(a.bb.writes,0,"no partial BB writes");eq(a.clears,0,"no partial path clear") end
end
test("order entry rejects invalid attacks and positions without replacing the existing plan",function()
 local h=harness();local old=move(3,5);assert(h.order(1,old))
 h.agents[3].pos=v(0,1,18.01);eq(h.order(1,attack()),false);eq(h:plans()[1],old)
 h.agents[3].pos=v(0,1,18);eq(h.order(1,attack()),true,"18 m inclusive")
 old=h:plans()[1]
 h.agents[1].visible=false;eq(h.order(1,attack()),false);eq(h:plans()[1],old)
 h.agents[1].visible=true;h.agents[3].hp=0;eq(h.order(1,attack()),false)
 for _,value in ipairs({{kind="move",pos=v(0/0,0,0)},{kind="move",pos=v(math.huge,0,0)},{kind="move"},{kind="bogus"}}) do
  eq(h.order(1,value),false);eq(h:plans()[1],old)
 end
 eq(h:serial(),0);eq(h.agents[1].bb.writes,0);eq(h:notice().kind,"error")
end)
test("batch prevalidation is atomic for hidden, dead, distant targets and lost allies",function()
 for _,invalid in ipairs({"hidden","dead","distant","ally"}) do
  local h=harness();local p1,p2=move(-5,3),attack();assert(h.order(1,p1));assert(h.order(2,p2))
  if invalid=="hidden" then h.agents[2].visible=false
  elseif invalid=="dead" then h.agents[3].hp=0
  elseif invalid=="distant" then h.agents[3].pos=v(0,1,19)
  else h.agents[2].hp=0 end
  eq(h.toggle(),false);assertUntouched(h,p1,p2)
  contains(h.logs[#h.logs],"reason=invalid_plan slot=2")
  local notice=h:notice().text
  eq(h.step(),false);eq(up(h.step,"stepUntil"),nil);eq(h:notice().text,notice,"E retains rejection")
  assertUntouched(h,p1,p2)
 end
end)
test("successful batch commits both, then E and empty existing-match resume work",function()
 local h=harness();assert(h.order(1,move(-3,5)));assert(h.order(2,attack()));assert(h.step())
 eq(h:paused(),false);eq(h:started(),1000);eq(h:serial(),2);eq(next(h:plans()),nil)
 eq(h.agents[1].bb.values["command.serial"],1);eq(h.agents[2].bb.values["command.serial"],2)
 eq(h.agents[2].bb.values["crossfire.target"],3);eq(up(h.step,"stepUntil"),3000)
 assert(h.toggle());eq(h:paused(),true);eq(up(h.step,"stepUntil"),nil)
 assert(h.toggle());eq(h:paused(),false);eq(h:serial(),2);eq(h:started(),1000)
end)
test("initial no-plan E remains paused and visible, runtime apply revalidates",function()
 local h=harness();eq(h.step(),false);assertUntouched(h,nil,nil);eq(up(h.step,"stepUntil"),nil)
 eq(h:notice().kind,"error");contains(h:notice().text,"先点地面")
 assert(h.order(1,move(3,4)));assert(h.toggle())
 local old=h.agents[1].bb.values.movePos;local serial=h:serial();local writes=h.agents[1].bb.writes
 h.agents[3].pos=v(0,1,30);eq(h.order(1,attack()),false)
 eq(h.agents[1].bb.values.movePos,old);eq(h:serial(),serial);eq(h.agents[1].bb.writes,writes)
end)
test("feedback lifetime uses UI delta while paused, freezes in overlays, resets on retry/menu",function()
 local h=harness();h.step();eq(h:remaining(),4000)
 h.env.Sandbox_Update(0,100);eq(h:remaining(),3900,"UI delta advances during sim pause")
 h.action("help");h.env.Sandbox_Update(0,100);eq(h:remaining(),3900,"overlay freezes notice")
 h.action("close");h.advance(3899);assert(h:notice());h.advance(1);eq(h:notice(),nil)
 h.order(1,move(2,4));eq(h:remaining(),2000);h.advance(1999);assert(h:notice());h.advance(1);eq(h:notice(),nil)
 h.step();h.action("restart");eq(h:notice(),nil)
 local other=harness();other.step();other.action("menu");eq(other:notice(),nil)
end)
test("mouse rejection survives hover and tutorial, valid operation replaces it",function()
 local h=harness();local old=move(2,4);h.order(1,old)
 h.env.EventHandle_Mouse(1,500,300,0);h.present()
 eq(h:plans()[1],old);eq(h.ctx.feedback.kind,"error");contains(h.ctx.feedback.text,"设备表面")
 assert(h.ctx.tutorial);assert(h.ctx.cursorHint,"hover still computed independently")
 h.pick=3;h.agents[1].visible=false;h.env.EventHandle_Mouse(1,500,300,0);h.present()
 eq(h:plans()[1],old);eq(h.ctx.feedback.kind,"error");contains(h.ctx.feedback.text,"目标")
 h.pick=1;h.env.EventHandle_Mouse(1,500,300,0);h.present();eq(h.ctx.feedback.kind,"success")
end)
test("old paused movement and interrupted commands reach HUD with truthful state",function()
 local h=harness();h.agents[1].bb.values.movePos=v(3,0,4);h.agents[1].bb.values["crossfire.state"]="MOVING"
 h.present();eq(h.ctx.allies[1].orderState,"suspended");contains(h.ctx.allies[1].fireState,"移动已暂停")
 assert(not h.ctx.allies[1].fireState:find("已规划",1,true))
 h.agents[1].bb.values["command.status"]="failed";h.present()
 eq(h.ctx.feedback.kind,"error");contains(h.ctx.feedback.text,"VEGA 指令中断")
 h.advance(1000);h.present();eq(h:remaining(),3000,"same failed state does not restart lifetime")
 h.order(1,{kind="hold"});h.present();eq(h.ctx.allies[1].orderState,"queued");contains(h.ctx.allies[1].orderText,"待命")
 local state,text=h.view(nil,false,true,true,"FIRING");eq(state,"suspended");contains(text,"交火")
 state,text=h.view(nil,false,true,false,"COOLING");eq(state,"attack");eq(text,"冷却中")
end)
local glyphs=assert(loadfile("bin/res/scripts/samples/crossfire_hud_glyphs.lua"))()
local function textWidth(text)
 local f=glyphs.fonts[9];local total=0
 for c in text:gmatch("[%z\1-\127\194-\244][\128-\191]*") do
  total=total+(c:byte()>=128 and f.cjk or (c==" " and f.space or f.widths[c:byte()] or 0))
 end
 return total
end
test("tutorial uses actual command kinds and all real detail strings fit 960 px card",function()
 local h=harness();local maximum=0
 local function check(t)
  maximum=math.max(maximum,textWidth(t.detail));assert(textWidth(t.detail)<=518,t.detail.." too wide")
  assert(textWidth(t.title)<=518,t.title.." too wide")
 end
 check(h.tutorial({}))
 for _,kind in ipairs({"move","attack","hold"}) do
  for _,slot in ipairs({1,2}) do
   local t=h.tutorial({[slot]={kind=kind}});check(t)
   if kind=="move" then contains(t.detail,"选择另一侧空地")
   else assert(not t.detail:find("路线",1,true));contains(t.detail,kind=="hold" and "待命" or "攻击") end
  end
  for _,other in ipairs({"move","attack","hold"}) do
   local t=h.tutorial({{kind=kind},{kind=other}});check(t);eq(t.done1,true);eq(t.done2,true)
   contains(t.detail,kind=="hold" and "VEGA 待命" or (kind=="attack" and "VEGA 攻击" or "VEGA 移动"))
  end
 end
 print("  actual tutorial maximum="..maximum.." px / 518 px at 960 x 720")
end)
test("victory and defeat clear projectiles exactly once without changing event counters",function()
 for _,result in ipairs({"VICTORY","DEFEAT"}) do
  local h=harness()
  if result=="VICTORY" then h.agents[3].hp=0 else h.agents[1].hp=0;h.agents[2].hp=0 end
  h.agents[3].bb.values["crossfire.hitCount"]=4;h.agents[3].bb.values["crossfire.blockedCount"]=2
  h.agents[3].bb.values["crossfire.lastImpactPosition"]=v(0,1,9)
  h.present();eq(h.ctx.result,result);eq(h.projectileClears,1);eq(h:paused(),true)
  h.present();eq(h.projectileClears,1);eq(h.recordCalls,result=="VICTORY" and 1 or 0)
  eq(h.agents[3].bb.values["crossfire.hitCount"],4);eq(h.agents[3].bb.values["crossfire.blockedCount"],2)
 end
end)
print("PASS crossfire command regression: "..count.." groups; production closures, no engine/window")
