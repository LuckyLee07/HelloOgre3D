
require("res.scripts.base.gui")

--require("res.scripts.samples.Sandbox1")
require("res.scripts.samples.Sandbox2")
--require("res.scripts.samples.Sandbox3")
--require("res.scripts.samples.Sandbox4")

_G.__init__ = function(sec, msec)
	math.randomseed(os.time())
	threadpool:init(sec * 1000 + msec, 10)

	-- 初始化LuaPanda
	LuaPanda.start("127.0.0.1", 8818)

	-- 设置LuaGC垃圾回收
	threadpool:work(function()
		while true do 
			local count = collectgarbage('count')
			local step = 500
			if count >= 3 * 1024 then
				collectgarbage('setstepmul', step * 2)
			elseif count >= 2 * 1024 then
				collectgarbage('setstepmul', step * 1)
			elseif count >= 1 * 1024 then
				collectgarbage('setstepmul', step * 0.5)
			else
				collectgarbage('setstepmul', step * 0.1)
			end
			local stepsize = 256
			local ok = collectgarbage('step', stepsize)
			--print('gc step mem:', math.floor(count/1024), '->', math.floor(collectgarbage('count')/1024), ' step finish?', ok)
			print('Fxll========>>>333', getFormatTime(os.time()))
			threadpool:wait(100)
		end
	end)
end

_G.__tick__ = function(sec, msec)
	threadpool:update(sec * 1000 + msec)
end