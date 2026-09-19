-- Crossfire-only preferences and three encounter records. Never execute file data.
-- Public settings: volume [0,1], muted, hints. records[level] contains only
-- {medal,timeMs,damage}. Record returns nil,false,false for invalid inputs.
local Profile = {}
Profile.__index = Profile
local MAX_BYTES, MAX_TIME_MS, MAX_DAMAGE = 4096, 86400000, 1000000
local WINDOWS = package.config:sub(1,1) == "\\"

local function finite(value,minimum,maximum,integer)
 return type(value)=="number" and value==value and value~=math.huge and value~=-math.huge
  and value>=minimum and value<=maximum and (not integer or value==math.floor(value))
end
local function validRecord(record)
 return type(record)=="table" and finite(record.medal,1,3,true)
  and finite(record.timeMs,0,MAX_TIME_MS) and finite(record.damage,0,MAX_DAMAGE)
end
local function exists(path)
 local file=io.open(path,"rb")
 if not file then return false end
 file:close();return true
end
local function readBounded(path)
 local file=io.open(path,"rb")
 if not file then return nil end
 -- Inspect the size before reading; an oversized or non-seekable file is ignored.
 local size=file:seek("end")
 if not size or size>MAX_BYTES or not file:seek("set",0) then file:close();return nil end
 local data=file:read(size)
 file:close()
 return data
end
local function numberText(value) return string.format("%.17g",value) end
local function replaceFile(path,temporary)
 if os.rename(temporary,path) then return true end
 if not WINDOWS or not exists(path) then return false end
 -- Windows C rename does not overwrite. Move the old file aside only after the
 -- replacement is complete, retain it on failure, and never overwrite a backup.
 local backup=path..".bak"
 if exists(backup) or not os.rename(path,backup) then return false end
 if os.rename(temporary,path) then
  os.remove(backup)
  return true
 end
 -- If rollback itself fails, Load can recover the untouched .bak file.
 os.rename(backup,path)
 return false
end
local function writeComplete(path,data)
 local file=io.open(path,"wb")
 if not file then return false end
 local wrote,writeResult=pcall(file.write,file,data)
 local flushed,flushResult=false,false
 if wrote and writeResult then flushed,flushResult=pcall(file.flush,file) end
 local closed,closeResult=pcall(file.close,file)
 return wrote and writeResult~=nil and flushed and flushResult~=nil and closed and closeResult~=nil
end

function Profile.Load()
 local path=os.getenv("HELLO_CROSSFIRE_PROFILE")
 if not path or path=="" then path="crossfire_settings.cfg" end
 local self=setmetatable({volume=.75,muted=false,hints=true,records={},_path=path},Profile)
 local data=readBounded(path)
 if not data and not exists(path) then
  data=readBounded(path..".bak")
  self._recoveryBackup=data~=nil
 end
 if not data then return self end
 for line in (data.."\n"):gmatch("([^\n]*)\n") do
  local key,value=line:match("^%s*([%w_.]+)%s*=%s*(.-)%s*$")
  if key=="volume" then
   local number=tonumber(value)
   if finite(number,0,1) then self.volume=number end
  elseif key=="muted" or key=="hints" then
   local number=tonumber(value)
   if number==0 or number==1 then self[key]=number==1 end
  elseif key then
   local level=tonumber(key:match("^record%.([123])$"))
   if level then
    local medal,timeMs,damage=value:match("^([^,]+),([^,]+),([^,]+)$")
    local record={medal=tonumber(medal),timeMs=tonumber(timeMs),damage=tonumber(damage)}
    if validRecord(record) then self.records[level]=record end
   end
  end
 end
 return self
end

function Profile:Save()
 if not finite(self.volume,0,1) or type(self.muted)~="boolean"
  or type(self.hints)~="boolean" or type(self.records)~="table" then return false end
 local lines={"version=1","volume="..numberText(self.volume),
  "muted="..(self.muted and "1" or "0"),"hints="..(self.hints and "1" or "0")}
 for level=1,3 do
  local record=self.records[level]
  if record then
   if not validRecord(record) then return false end
   lines[#lines+1]="record."..level.."="..record.medal..","..numberText(record.timeMs)..","..numberText(record.damage)
  end
 end
 local temporary=self._path..".tmp"
 if not writeComplete(temporary,table.concat(lines,"\r\n").."\r\n") then
  os.remove(temporary);return false
 end
 local saved=replaceFile(self._path,temporary)
 if not saved then os.remove(temporary)
 elseif self._recoveryBackup then
  -- A previous interrupted Windows replacement was recovered into memory.
  -- The new main file now contains that data, so the old backup can be retired.
  if os.remove(self._path..".bak") then self._recoveryBackup=false end
 end
 return saved
end

function Profile:Record(level,timeMs,damage,living,parMs)
 if not finite(level,1,3,true) or not finite(timeMs,0,MAX_TIME_MS)
  or not finite(damage,0,MAX_DAMAGE) or not finite(living,0,2,true)
  or not finite(parMs,0,MAX_TIME_MS) then return nil,false,false end
 local medal=1+(living==2 and 1 or 0)
 if living==2 and damage<=60 and timeMs<=parMs then medal=medal+1 end
 local current=self.records[level]
 local better=not validRecord(current) or medal>current.medal
  or (medal==current.medal and (timeMs<current.timeMs or (timeMs==current.timeMs and damage<current.damage)))
 if better then self.records[level]={medal=medal,timeMs=timeMs,damage=damage} end
 return medal,better,self:Save()
end
function Profile:CompletedCount()
 local count=0
 for level=1,3 do if validRecord(self.records[level]) then count=count+1 end end
 return count
end
return Profile
