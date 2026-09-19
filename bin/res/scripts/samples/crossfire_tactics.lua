-- Value-only reading of the real sentry cycle. No decisions, timers or engine refs.
local Tactics={}
local function alive(a) return a and type(a.hp)=="number" and a.hp>0 end
local function remaining(a)
 local n=tonumber(a.phaseRemainingMs) or 0
 return n==n and math.max(0,math.min(10000,n)) or 0
end
function Tactics.Read(actors,selected)
 local byId,byAlly={},{}
 for i,a in ipairs(actors or {}) do if alive(a) and a.id then byId[a.id]={slot=i,actor=a} end end
 local imminent,cooling=nil,nil
 for i,a in ipairs(actors or {}) do
  if a.enemy and alive(a) then
   if a.state=="LOCKING" or a.state=="FIRING" then
    local target=byId[a.lockId]
    if target and not target.actor.enemy then
     local event={sourceSlot=i,sourceName=a.name or "哨卫",slot=target.slot,
      name=target.actor.name or (target.slot==1 and "VEGA" or "ROOK"),
      state=a.state,remainingMs=remaining(a)}
     local old=byAlly[event.slot]
     if not old or event.remainingMs<old.remainingMs then byAlly[event.slot]=event end
     if not imminent or event.remainingMs<imminent.remainingMs or
      (event.remainingMs==imminent.remainingMs and event.slot==selected and imminent.slot~=selected) then imminent=event end
    end
   elseif a.state=="COOLING" then
    cooling=cooling or a
   end
  end
 end
 if imminent then
  local firing=imminent.state=="FIRING"
  return {kind=firing and "burst" or "lock",slot=imminent.slot,sourceSlot=imminent.sourceSlot,
   byAlly=byAlly,color="amber",text=imminent.sourceName..(firing and " 正在连发 · " or " 锁定 ")..
    imminent.name..(firing and "，离开橙色瞄点。" or " · 趁蓄力换位，离开橙色瞄点。")}
 end
 if cooling then
  return {kind="cooling",byAlly=byAlly,color="blue",text=(cooling.name or "哨卫").." 冷却 · 可以调整站位，橙弧护盾仍有效。"}
 end
 return {kind="neutral",byAlly=byAlly}
end
return Tactics
