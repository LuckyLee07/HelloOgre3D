-- UTF-8 stays in Lua; Gorilla receives only printable ASCII and font switches.
-- The generated glyph banks share one cross-platform texture and font metrics.
local Data=require("res.scripts.samples.crossfire_hud_glyphs.lua")
local Text={}
local function chars(value)
 return tostring(value or ""):gmatch("[%z\1-\127\194-\244][\128-\191]*")
end
local function fontData(font) return Data.fonts[font or 9] or Data.fonts[9] end
local function width(char,info)
 local byte=char:byte()
 if byte==32 then return info.space end
 if byte<128 then return info.widths[byte] or 0 end
 return Data.index[char] and info.cjk or info.widths[63]
end
function Text.Plain(value)
 return tostring(value or ""):gsub("[\1-\8\11-\31\127]",""):gsub("\t"," ")
end
function Text.Width(value,font)
 local total,info=0,fontData(font)
 for char in chars(value) do total=total+width(char,info) end
 return total
end
function Text.Fit(value,limit,font)
 value=Text.Plain(value)
 if Text.Width(value,font)<=limit then return value end
 local result,total,info={},0,fontData(font)
 local suffix="..."; local available=math.max(0,limit-Text.Width(suffix,font))
 for char in chars(value) do
  local advance=width(char,info)
  if total+advance>available then break end
  result[#result+1]=char;total=total+advance
 end
 return table.concat(result)..suffix
end
function Text.Wrap(value,limit,font,maxLines)
 local lines,line,total={}, {},0
 local info=fontData(font)
 for char in chars(Text.Plain(value)) do
  local advance=width(char,info)
  if char=="\n" or (total+advance>limit and #line>0) then
   lines[#lines+1]=table.concat(line):gsub(" +$","");line={};total=0
  end
  if char~="\n" and (#line>0 or char~=" ") then line[#line+1]=char;total=total+advance end
 end
 if #line>0 then lines[#lines+1]=table.concat(line) end
 if #lines>maxLines then
  lines[maxLines]=Text.Fit(table.concat(lines,"",maxLines),limit,font)
  for i=#lines,maxLines+1,-1 do lines[i]=nil end
 end
 return table.concat(lines,"\n")
end
function Text.Markup(value,font)
 local info=fontData(font)
 local result,current={},nil
 for char in chars(Text.Plain(value)) do
  local target,code=info.ascii,char
  local index=Data.index[char]
  if index then target=info.bank+math.floor(index/94);code=string.char(33+index%94)
  elseif char:byte()>=128 then code="?" end
  if current~=target then result[#result+1]="%@"..target.."%";current=target end
  result[#result+1]=code=="%" and "%%" or code
 end
 return table.concat(result)
end
return Text
