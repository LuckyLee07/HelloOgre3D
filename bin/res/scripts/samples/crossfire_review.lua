-- Per-encounter, value-only diagnostics. Construct a new instance on each restart.
-- Update returns the current eligible alert, including while paused; the caller
-- decides where to display it. No engine access, callbacks or retained userdata.
-- Durations sum per-ally query intervals, not real projectile impacts. Two allies
-- can contribute during the same interval, so totals may exceed match elapsed time.
local Review={}
Review.__index=Review
local THRESHOLD_MS, UNKNOWN_GRACE_MS=1200,250
local priority={shield=1,obstructed=2,wreck=3}
local metricKey={shield="shieldMs",obstructed="obstructedMs",wreck="wreckMs"}
local titles={shield="正面护盾",obstructed="射界受阻",wreck="残骸挡住射界"}
local fallback={shield="向侧后方换位，再重新开火。",obstructed="向侧面换位，避开红色射线。",wreck="绕开残骸，再重新开火。"}

local function finite(v)
 return type(v)=="number" and v==v and v~=math.huge and v~=-math.huge
end
local function integer(v,minimum,maximum)
 return finite(v) and v>=minimum and v<=maximum and v==math.floor(v)
end
local function text(v,otherwise)
 return type(v)=="string" and v~="" and v or otherwise
end
local function point(value)
 if type(value)~="table" and type(value)~="userdata" then return nil end
 local ok,x,y,z=pcall(function() return value.x,value.y,value.z end)
 if not ok or not finite(x) or not finite(y) or not finite(z) then return nil end
 return {x=x,y=y,z=z}
end
local function reset(self,slot)
 self._allies[slot]=nil
end
local function unknown(self,slot,now)
 local old=self._allies[slot]
 if not old then return end
 -- Unknown snapshots never display a previous position, even when the clock is
 -- frozen. The grace only preserves already observed evidence, not unknown time.
 old.sampled=false
 if now-old.lastValidAt>UNKNOWN_GRACE_MS then reset(self,slot) end
end
local function alert(track)
 return {slot=track.slot,state=track.state,title=track.name.." · "..titles[track.state],
  detail=track.text,target=track.target,first=track.first,
  pos={x=track.pos.x,y=track.pos.y,z=track.pos.z}}
end
local function better(a,b)
 if not b then return true end
 if priority[a.state]~=priority[b.state] then return priority[a.state]>priority[b.state] end
 if a.durationMs~=b.durationMs then return a.durationMs>b.durationMs end
 return a.slot<b.slot
end
function Review.New()
 return setmetatable({_allies={},_now=nil,_active=false,
  _totals={shieldMs=0,obstructedMs=0,wreckMs=0}},Review)
end

function Review:Update(nowMs,active,allies)
 if not finite(nowMs) or nowMs<0 then
  self._allies={};self._now=nil;self._active=false
  return nil
 end
 local previous=self._now
 if previous and nowMs<previous then
  -- A changed clock cannot preserve a continuous firing episode. Keep prior
  -- measured totals, but establish a fresh baseline for the new clock segment.
  self._allies={};previous=nil
 end
 local dt=previous and nowMs-previous or 0
 local advancing=dt>0 and active==true and self._active
 self._now=nowMs;self._active=active==true
 local seen,candidate={},nil
 if type(allies)~="table" then allies={} end
 for _,a in ipairs(allies) do
  if type(a)=="table" and integer(a.slot,1,2) and not seen[a.slot] then
   local slot=a.slot
   seen[slot]=true
   local report=type(a.report)=="table" and a.report or nil
   local old=self._allies[slot]
   if a.moving or (finite(a.hp) and a.hp<=0) then
    reset(self,slot)
   else
    -- Target changes are decisive even when the rest of this snapshot is unknown.
    if old and report and integer(report.target,1,2147483647) and report.target~=old.target then
     reset(self,slot);old=nil
    end
    local state=report and report.state
    if report and type(state)=="string" and state~="unknown" and not priority[state] then
     reset(self,slot)
    else
     local hit=report and point(report.hit)
     local valid=priority[state] and finite(a.hp) and a.hp>0 and integer(a.shots,0,1000000000)
      and integer(report.target,1,2147483647) and integer(report.first,0,2147483647) and hit
      and (state=="shield" or (report.first>0 and report.first~=report.target))
     if not valid then unknown(self,slot,nowMs)
     else
      if old and (old.state~=state or old.first~=report.first or a.shots<old.lastShots
       or (not old.sampled and nowMs-old.lastValidAt>UNKNOWN_GRACE_MS)) then old=nil end
      if not old then
       old={slot=slot,state=state,target=report.target,first=report.first,durationMs=0,shots=0}
       self._allies[slot]=old
      elseif advancing and old.sampled then
       old.durationMs=old.durationMs+dt
       old.shots=old.shots+(a.shots-old.lastShots)
       local key=metricKey[state]
       self._totals[key]=self._totals[key]+dt
      end
      old.lastShots=a.shots;old.lastValidAt=nowMs;old.sampled=true
      old.name=text(a.name,slot==1 and "VEGA" or "ROOK")
      old.text=text(report.text,fallback[state]);old.pos=hit
      if old.durationMs>=THRESHOLD_MS and old.shots>=2 and better(old,candidate) then candidate=old end
     end
    end
   end
  end
 end
 for slot in pairs(self._allies) do if not seen[slot] then unknown(self,slot,nowMs) end end
 return candidate and alert(candidate) or nil
end

local function count(value,maximum)
 if not finite(value) then return 0 end
 return math.floor(math.max(0,math.min(maximum or 1000000000,value)))
end
function Review:Summary(outcome)
 outcome=type(outcome)=="table" and outcome or {}
 local metrics={shieldMs=self._totals.shieldMs,obstructedMs=self._totals.obstructedMs,
  wreckMs=self._totals.wreckMs,living=count(outcome.living,2),damage=count(outcome.damage),
  flankHits=count(outcome.flankHits),blockedShots=count(outcome.blockedShots),
  elapsedMs=count(outcome.elapsedMs,1000000000000),scope="per_ally_query_time"}
 metrics.entityMs=metrics.obstructedMs+metrics.wreckMs
 local title
 if metrics.living<2 then title="先保住双机，再寻找侧面"
 elseif metrics.entityMs>=THRESHOLD_MS and metrics.entityMs>=metrics.shieldMs then
  title=metrics.wreckMs>metrics.obstructedMs and "绕开残骸，再重新开火" or "先换站位，再重新开火"
 elseif metrics.shieldMs>=THRESHOLD_MS or metrics.blockedShots>=2 then title="绕过护盾，再交叉开火"
 elseif metrics.flankHits>0 then
  if outcome.result=="VICTORY" and metrics.damage>0 then title="保持侧击，再减少损伤"
  else title="保持侧面射界，双机配合" end
 else title="先拉开双机，再寻找侧面" end
 local detail=string.format("射界受阻合计：盾前 %.1f 秒 · 遮挡 %.1f 秒。\n两机分别累计；换位可减少无效交火。",
  metrics.shieldMs/1000,metrics.entityMs/1000)
 return {title=title,detail=detail,metrics=metrics}
end
return Review
