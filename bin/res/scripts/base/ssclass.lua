--[[
    用于支持定义类
]]

_G.SandboxClass = {}
SandboxClass.__index = SandboxClass


--[[
    -- 类名
    classname = string,
    -- 注册缓存
    {
        [classname] = table(class), -- 注册的类
    }
    -- 基类
    super = table(class)
]]

------------------------------------------------------

-- 写入默认方法
local function InitDefault(class)
    -- 创建实例
    function class:new(...)
        local ins = {
            _constructor_super = {},
        }
        setmetatable(ins, {__index = self})

        -- 有构造函数，需要执行构造函数
        self.constructor(ins, ...)

        return ins
    end

    -- 删除实例
    function class:delete()
        -- 如果有析构函数
        self:destructor()
    end
end

------------------------------------------------------

-- 创建类
function SandboxClass:Class(classname, super)
    if self[classname] then
        return self[classname]
    end

    -- 新的类
    local class = {
        _classname = classname, -- 类名
        _super = {}, -- 父类
        _superList = {}
    }
    InitDefault(class)

    -- 继承，父类
    if super then
        local supertype = type(super)
        if supertype == "string" then
            class._superList[super] = self[super]
            super = self[super]
            supertype = type(super)
        end
        if supertype == "table" then
            class._super = setmetatable({}, {
                __index = function(t, k)
                    local v = super[k]
                    if not v then
                        return nil
                    end

                    if k == "constructor" then -- 构造标记
                        return function(u, ...)
                            u._constructor_super = u._constructor_super or {}
                            u._constructor_super[classname] = true -- 初始化了
                            v(u, ...)
                        end
                    elseif k == "destructor" then -- 析构标记
                        return function(u)
                            u._constructor_super = u._constructor_super or {}
                            if u._constructor_super[classname] then -- 有构造标记才会析构
                                u._constructor_super[classname] = false
                                v(u)
                            end
                        end
                    end

                    return v
                end,
                __newindex = function(t, k, v)
                    -- 不允许新增
                end,
            })
            for key, value in pairs(super._superList or {}) do
                class._superList[key] = value
            end
        end
    end

    -- 定制构造和析构
    local classinfo = {
        constructor = function(u, ...)
            local super1 = class._super
            local constructor = super1 and super1.constructor
            if constructor then
                constructor(u, ...)
            end
        end,
        destructor = function(u)
            local super1 = class._super
            local destructor = super1 and super1.destructor
            if destructor then
                destructor(u)
            end
        end,
    }
    setmetatable(class, {
        __index = function(t, k)
            -- 类成员
            local v = classinfo[k]
            if v ~= nil then
                return v
            end

            return class._super[k] -- 父类
        end,
        __newindex = function(t, k, v)
            -- 重载 析构
            if k == "destructor" then
                classinfo.destructor = function(u)
                    v(u)
                    
                    -- 需要额外析构父类
                    local super1 = class._super
                    local destructor = super1 and super1.destructor
                    if destructor then
                        destructor(u)
                    end
                end
            else
                classinfo[k] = v
            end
        end,
    })

    self[classname] = class
    return class
end

-- 释放类
function SandboxClass:Release(classname)
    self[classname] = nil
end