--require("res.scripts.samples.Sandbox1")
--require("res.scripts.samples.Sandbox2")
--require("res.scripts.samples.Sandbox3")
--require("res.scripts.samples.Sandbox4")
--require("res.scripts.samples.Sandbox5")
--require("res.scripts.samples.Sandbox6")
--require("res.scripts.samples.Sandbox7")
require("res.scripts.samples.Sandbox8")

_G.LuaPluginMgr = ClassList.LuaPluginMgr:new()
_G.FairyGuiManager = ClassList.FairyGuiManager:GetInst()
FairyGuiManager:RegisterUIRegistry(require("res.scripts.ui.UIRegistry"))

local function tryOpenFairyGuiSample()
	local file = io.open("res/fuires/act_37_test.fui", "rb")
	if file == nil then
		return
	end
	file:close()

	threadpool:delay(1, function()
		local view = FairyGuiManager:Open("Act37TestView")
		local handle = view and view:GetHandle() or nil
		print("[FGUI] open act_37_test:", handle)

		local mvcCtrl = FairyGuiManager:Open("Act37TestMvc", {
			roleId = 1001,
			source = "Registry"
		})
		local mvcHandle = mvcCtrl and mvcCtrl:GetHandle() or nil
		print("[FGUI] open act_37_test mvc:", mvcHandle)
	end)
end

_G.__init__ = function(sec, msec)
	math.randomseed(os.time())
	threadpool:init(sec * 1000 + msec, 10)

	-- 初始化LuaPanda
	LuaPanda.start("127.0.0.1", 8818)

	-- 启动GC释放tolua的内存
	threadpool:delay(5, function()
		_G.__gc__()
	end)

	tryOpenFairyGuiSample()
end

_G.__tick__ = function(detalMsec)
	threadpool:update(detalMsec)
end

_G.__gc__ = function()
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
			--print('__gc__+++++++++++++++++++>>>', getFormatTime(os.time()))
			--print('gc step mem:', math.floor(count/1024), ' step finish?', ok)
			threadpool:wait(3)
		end
	end)
end

