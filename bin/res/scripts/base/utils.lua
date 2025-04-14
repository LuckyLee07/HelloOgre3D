-----------------------一些实用小方法--------------------------
-- 去掉字符串首尾空格
function string.trim(strx)
   return (string.gsub(strx, "^%s*(.-)%s*$", "%1"))
end

function getFormatTime(timestamp)
	return  os.date("%Y-%m-%d %H:%M:%S", timestamp)
end

function reloadLua(moduleName)
    package.loaded[moduleName] = nil
    return require(moduleName)
end

-- 分割字符串
function string.split(str, mark)

	if string.len(str)==0 then return nil end

	local sIdx = 1
	local tsub = {}
	while true do
		local pos = string.find(str, mark, sIdx, true)
		if not pos then break end

	    table.insert(tsub, string.sub(str, sIdx, pos-1))
	    sIdx = pos + string.len(mark)
	end
	table.insert (tsub, string.sub(str, sIdx))

	return tsub
end

function table.size(table)
	local size = 0
	for k in pairs(table) do
		size = size + 1
	end
	return size
end

-------------table.tostring-------------
local function __tbl_tostring(tbl)
	local function sub_tostr(param)
		local result = tostring(param)
		if type(param) == 'table' then
			local pAttr = string.sub(result, -8)
			result = ('table: 0x%s'):format(pAttr)
		elseif type(param) == 'function' then
			local pAttr = string.sub(result, -8)
			result = ('function: 0x%s'):format(pAttr)
		end
		return result
	end

	local function get_key(key, level)
		if type(key) == "number" then
			key = ('  '):rep(level).. ('[%d]'):format(key)
        elseif type(key) == "string" then
			key = ('  '):rep(level).. ('[%q]'):format(key)
        else
			key = ('  '):rep(level).. ('[%s]'):format(sub_tostr(key))
		end
		return key
	end

	local function get_value(value)
		if type(value) == "number" then
			value = ('%d'):format(value)
        elseif type(value) == "string" then
            value = ('%q'):format(value)
        else
			value = ("%s"):format(sub_tostr(value))
		end
		return value
	end

	local ref_cache = {} -- 记录已存在的
    local function get_table(ptbl, level)
		local str_tbl = sub_tostr(ptbl)
        local str_format = ('{ --%s\n'):format(str_tbl)

		local index = 0
		local tbl_str = str_format
		local size = table.size(ptbl)
		ref_cache[ptbl] = true
        for key, value in pairs(ptbl) do
			index = index + 1
			key = get_key(key, level)
            if type(value) == "table" then
				if ref_cache[value] then
					tbl_str = tbl_str .. key .. (' : ref%q'):format(sub_tostr(value))
				else
					tbl_str = tbl_str .. key .. ' : ' .. get_table(value, level+1)
				end
            else
				tbl_str = tbl_str .. key .. " : " .. get_value(value)
            end
            tbl_str = tbl_str .. (index==size and "\n" or ",\n")
        end
        tbl_str = tbl_str .. ('  '):rep(level-1) .. "}"
        return tbl_str
    end
    return get_table(tbl, 1)
end
table.tostring =  __tbl_tostring
--print(__tbl_tostring({1, 2, 3}))

local __print__ = _G.print
_G.print = function( ... )
	local msg = ''
	local info = debug.getinfo(2)
    if info ~= nil then
		local file = info.source or ''
		-- source不一定是path，可能是str
		if #file >= 99 then
			file = 'loadstring'
		else
			local newfile = file:match("^.+/(.+)$")
			file = newfile or file
		end
		
		local line = info.currentline or 0
		msg = string.format('[%s:%d]', file, line)
		--__print__("Fxkk=========>>>", file)
    end

    local args = {...}
    for i, v in ipairs(args) do
		if type(v) == 'table' then
			local ss = table.tostring(v)
			if #ss > 5000 then ss = string.sub(ss, 1, 5000) end
			msg = msg .. ss .. ' '
		else
			local ss = tostring(v)
			if #ss > 5000 then ss = string.sub(ss, 1, 5000) end
			msg = msg .. ss .. ' '
		end
    end
    if #msg >= 8*1024 then ss = string.sub(ss, 1, 8*1024) end
    if #args <= 0 then m = 'nil' end
    --_print(msg)
    LuaInterface:log( msg )
end
