--生成类的方法
ClassList = {}

function Class(className,super)
    local curClass = {}
    curClass.className = className
    curClass.type = "class"
    curClass.super = super
	
	local _info = debug.getinfo(2, "S");
    local _path = string.sub(_info.source, 2, -1);
	curClass.___classPath___ = _path;
	
    local function getInstance(param)
        local instance = {}
        setmetatable(instance,{__index = curClass})

        local function create(tempClass,param)
            if tempClass.super then 
                setmetatable(tempClass,{__index = tempClass.super})
                create(tempClass.super,param)
            end 
            if tempClass.Init then 
                tempClass.Init(instance,param)
            end 
        end 

        create(curClass,param)

        return instance
    end

    curClass.new = function(param)
        if curClass.GetInst then
            print("单例类不能通过NEW的方式创建")
            return nil
        end 
        return getInstance(param)
    end

    curClass.instance = function(param)
        return getInstance(param)
    end

    ClassList[curClass.className] = curClass

    return curClass
end