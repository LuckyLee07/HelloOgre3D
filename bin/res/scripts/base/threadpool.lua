-- *threadpool* *threadpool.lua*
local IDLING = 0
local PROCESSING = 1
local ErrorCode = {OK = 0, TIMEOUT = 1000, NOT_IN_THREAD = 1006}

_G.delegate = function (func, ...) return func(...) end
 
_G.threadpool = {
    env = {
        free_thread_stack = {},
        free_thread_stack_len = 0,
        threadid2thread = {},
        working_thread = {},
        threadid_idx = 1,
        eid2tid = {},
        timer_list = {},
        msec = nil,
        seed = os.time(),
    },
    status = {
        IDLING = IDLING,
        PROCESSING = PROCESSING,
    },
}
local env = threadpool.env

local UINTMAX = math.pow(2, 32)

-- 生成唯一threadId
_G.gen_gid = function ()
    if env.seed < 0 or env.seed >= UINTMAX then env.seed = 1 end
    env.seed = env.seed + 1
    return env.seed
end

-- 创建新线程(协程)并初始化其状态 空闲时会被放回线程堆栈
local new_thread
new_thread = function(threadid)
    local co = coroutine.create(function(threadpool, thread)
        while true do
            delegate(coroutine.yield())
            threadpool.running = thread.parent
            thread.status = IDLING
            local l = #env.working_thread
            for i = l, 1, -1 do
                if env.working_thread[i] == thread then
                    table.remove(env.working_thread, i)
                    break
                end
            end
            thread._event_ = nil
            thread._timeout_ = nil
            env.free_thread_stack_len = env.free_thread_stack_len + 1
            env.free_thread_stack[env.free_thread_stack_len] = thread
        end
    end )

    local thread = {
        co = co,
        id = threadid,
        status = IDLING,
        call = function (self,func,...)
            self.status = PROCESSING
            table.insert(env.working_thread, self)
            if func and type(func) == "function" then
                return self:resume(func, ...)
            else
                self.status = IDLING
                local l = #env.working_thread
                for i = l, 1, -1 do
                     if env.working_thread[i] == self then
                         table.remove(env.working_thread, i)
                         break
                     end
                end
                self._event_ = nil
                self._timeout_ = nil
                env.free_thread_stack_len = env.free_thread_stack_len + 1
                env.free_thread_stack[env.free_thread_stack_len] = self
                return false, "param is nil or param is not a function"
            end
        end ,
        resume = function (self, ...)
            self.parent = threadpool.running
            threadpool.running = self
            local succ, msg = coroutine.resume(self.co, ...)
            if not succ then
                print('MessageBoxError Error In threadpool:', msg)
                new_thread(self.id)
            end
            return true
        end,
    }

    env.threadid2thread[threadid] = thread

    env.free_thread_stack_len = env.free_thread_stack_len + 1
    env.free_thread_stack[env.free_thread_stack_len] = thread
    coroutine.resume(co, threadpool, thread)
    return thread
end

-- 向线程池中添加指定数量的新线程
threadpool.add_thread = function (self, num)
    if num == nil then
        return false, "num cannot be nil"
    end
    for i = 1, num do
        new_thread(env.threadid_idx)
        env.threadid_idx = env.threadid_idx + 1
    end
    return true, "add thread ok, env.free_thread_stack len = " .. env.free_thread_stack_len
end

-- *threadpool:work*
-- 将工作分配给一个空闲线程执行，如果没有空闲线程，则添加更多线程
threadpool.work = function(self, f, ...)
    if env.free_thread_stack_len == 0 then
        return false, "no free thread .."
    end
    local tmp_thread = env.free_thread_stack[env.free_thread_stack_len]
    env.free_thread_stack_len = env.free_thread_stack_len  - 1
    
    if tmp_thread.status == IDLING then
        if type(f) == 'function' then
            local func = function(...)
                tmp_thread.ctx = {}
                local a = {f(...)}
                return unpack(a)
                --local a = {pcall(f, ...)}
                --if a[1] then return unpack(a, 2) end
                --print('MessageBoxError Error In threadpool:work', a[2])
            end
            return tmp_thread:call(func, ...)
        else
            return false, 'not a func'
        end
    else
        return false, "no free thread .."
    end
end

-- *threadpool:wait*
threadpool.wait = function(self, event, time)
    if not threadpool.running then
        return ErrorCode.NOT_IN_THREAD
    end
    if not env.msec then return end
    local Second = env.msec / 1000
    if time == nil then
        if type(event) == 'number' then
            time = event
            event = gen_gid()
        else
            time = 99999999
        end
    end
    env.eid2tid[event] = threadpool.running.id
    threadpool.running._event_ = event
    threadpool.running._timeout_ = Second + (time or 1)
    threadpool.running = threadpool.running.parent
    return coroutine.yield()
end

--每隔xx时间执行一次，总持续多长时间
threadpool.timer = function (self, duration, interval, tick_func, finish_func)
    if not duration or duration <= 0 then
        if tick_func then pcall(tick_func) end
        return
    end
    interval = (interval and interval > 0 and interval) or duration
    local Second = env.msec/1000
    local seq = gen_gid()
    table.insert(env.timer_list,{
        seq = seq,
        left_time = duration,
        interval = interval,
        tick_func = tick_func,
        finish_func = finish_func,
        next_timeout = Second,
        ts = Second,
        start = Second,
        tick = 0,
        total = math.ceil(duration/interval),
    })
    return seq
end

threadpool.delay = function (self, timeout, func)
    return threadpool:timer(timeout, timeout, nil, func)
end

threadpool.check_timeout = function(self)
    local second = env.msec/1000
    local thread
    local n = #env.working_thread
    for i = 1, n do
        thread = env.working_thread[i]
        if thread and  thread.status == PROCESSING and thread._timeout_ then
            if thread._timeout_ <= second then --timeout
                local event = thread._event_
                if event then
                    thread._event_ = nil
                    thread._timeout_ = nil
                    env.eid2tid[event] = nil

                    thread:resume(ErrorCode.TIMEOUT)
                end
            end
        end
    end

    for i = #env.timer_list, 1, -1 do
        local v = env.timer_list[i]
        if v then
            if second >= v.next_timeout and v.tick < v.total then
                v.next_timeout = second + v.interval
                v.tick = v.tick + 1
                local passed = second - v.ts
                passed = passed >= v.interval and v.interval or passed
                v.left_time = v.left_time - passed
                v.ts = second
                if v.tick_func then pcall(v.tick_func, v, v.tick) end
            end

            if second - v.ts >= v.left_time then
                v.left_time = 0
                v.ts = second
                if v.finish_func then pcall(v.finish_func, v) end
                table.remove(env.timer_list, i)
            end
        end
    end
end

threadpool.init = function(self, msec, num)
    print('init threadpool num = ', num)
    local ret, msg = self:add_thread(num)
    if not ret then error(msg) end
    env.msec = msec or os.time() * 1000
end

threadpool.update = function (self, local_msec)
    env.msec = env.msec + local_msec
    threadpool:check_timeout()
end

threadpool.free_count = function()
    return env.free_thread_stack_len
end

return threadpool
