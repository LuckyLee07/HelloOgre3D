local LuaPluginMgr = Class("LuaPluginMgr")

--function LuaPluginMgr:constructor()
LuaPluginMgr.Init = function(self)
	_G.SandboxLua = {
		localGt = {}, -- 存储每个对象对应的局部环境
        filecache = {}, -- 存储文件对应的代码Code
	}
end

-- 获取指定对象的局部环境
--function LuaPluginMgr:GetLocalGt(obj)
LuaPluginMgr.GetLocalGt = function(self, obj)
	if not obj then return end

	local localgt = _G.SandboxLua.localGt[obj]
	if not localgt then
		localgt = {}
		_G.SandboxLua.localGt[obj] = localgt
	end
	return localgt
end

-- 清除指定对象的局部环境
--function LuaPluginMgr:RemLocalGt(obj)
LuaPluginMgr.RemLocalGt = function(self, obj)
	if not obj then return end
	_G.SandboxLua.localGt[obj] = nil
end

-- 绑定 Lua 插件
--function LuaPluginMgr:Bind(object, codefunc)
LuaPluginMgr.Bind = function(self, object, codefunc)
    -- 定义全局环境（_G）和一个独立的本地环境
    local globalEnv = _G

    -- 获取该对象的局部环境 用于存储插件中产生的变量
	local localEnv = self:GetLocalGt(object) or {}

    -- 构造插件的执行环境，插件可以访问：
    --   1. 我们预先定义好的 API（script）
    --   2. 全局变量（globalEnv）
    --   3. 本地环境（localEnv，用于存储新定义的变量）
    -- 这里还把绑定对象以 owner 字段传入，方便脚本中使用
    local pluginEnv = setmetatable({}, {
        __index = function(t, key)
            -- 优先从 localEnv 查找
            if localEnv[key] ~= nil then
                return localEnv[key]
            end
            -- 否则从全局环境中查找
            return globalEnv[key]
        end,
        __newindex = function(t, key, value)
            -- 插件中定义的新变量存放到 localEnv 中
            localEnv[key] = value
        end,
    })

    -- 将插件环境存储到对象的字段中，便于后续调用插件中的函数
    object:setPluginEnv(pluginEnv)

    -- 设置 codefunc 的执行环境为我们构造的 pluginEnv
    setfenv(codefunc, pluginEnv)

    -- 执行插件代码，并捕获错误
    local ok, err = pcall(codefunc)
    if not ok then
        print("插件执行错误: " .. tostring(err))
        return false
    end

    return true
end

LuaPluginMgr.LoadScript = function(self, luascript)
    local ok, func = pcall(loadstring, luascript)
    if not ok then
        print("Error loading script: " .. string.sub(luascript, 0, 10) .. ", errorcode = " .. tostring(func))
        return nil
    end

    return func
end

-- 封装的接口，加载文件到Cache
--function LuaPluginMgr:LoadLuaFile(filepath)
LuaPluginMgr.LoadLuaFile = function(self, filepath)
    local script = _G.SandboxLua.filecache[filepath]
    if script ~= nil then
        return self:LoadScript(script)
    end

    local file = io.open(filepath, "r")
    if not file then
        print("Error loading Lua file:", filePath)
        return nil
    end
    script = file:read("*a")
    file:close()
    _G.SandboxLua.filecache[filepath] = script;

    return self:LoadScript(script)
end

-- 封装的接口，通过文件路径加载并绑定插件到对象上
--function LuaPluginMgr:BindByLuaFile(object, luafile)
LuaPluginMgr.BindByLuaFile = function(self, object, luafile)
	local func = self:LoadLuaFile(luafile)
	if not func then
		return false
	end
	return self:Bind(object, func)
end
