-- Read-only snapshots after ordinary title selection/retry. No world mutations.
-- Mesh caches for authored files warm on first visits; compare like-for-like
-- only after all courts have been visited. Counts are not process RSS or VRAM.
local Test={}
Test.__index=Test
function Test.New()
 return setmetatable({seen={},baselines={},samples=0,comparisons=0,failed=0},Test)
end
function Test:Snapshot(level,reason)
 self.samples=self.samples+1
 self.seen[level]=true
 local dump=GameManager:buildRuntimeResourceDump(0)
 local total,loaded,manual,kb=string.match(dump,"%[ResourceDump%] type=Mesh total=(%d+) loaded=(%d+) manual=(%d+) loadedKB=(%d+)")
 local warm=self.seen[1] and self.seen[2] and self.seen[3]
 local values={tonumber(total),tonumber(loaded),tonumber(manual),tonumber(kb),ObjectManager:getObjectCount(),ObjectManager:getAiAgentCount()}
 local valid=total~=nil
 local baseline=self.baselines[level]
 local compared=warm and baseline~=nil
 if compared then
  self.comparisons=self.comparisons+1
  for i=1,6 do if values[i]~=baseline[i] then valid=false end end
 elseif warm and valid then self.baselines[level]=values end
 if not valid then self.failed=self.failed+1 end
 print(string.format("[CrossfireLifecycle] sample=%d level=%d reason=%s warm=%s compared=%s meshes=%s loaded=%s manual=%s meshKB=%s objects=%d agents=%d result=%s comparisons=%d failed=%d",
  self.samples,level,reason,tostring(warm==true),tostring(compared==true),tostring(total),tostring(loaded),tostring(manual),tostring(kb),values[5],values[6],valid and "PASS" or "FAIL",self.comparisons,self.failed))
end
return Test
