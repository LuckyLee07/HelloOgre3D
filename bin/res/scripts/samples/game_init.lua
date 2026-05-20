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

local function isEnvEnabled(name)
	local value = os.getenv and os.getenv(name) or nil
	return value == "1" or value == "true" or value == "TRUE" or value == "True"
end

local function tryRunFairyGuiInputSelfTest()
	if not isEnvEnabled("HELLO_FGUI_INPUT_SELF_TEST") then
		return
	end

	threadpool:delay(4, function()
		print("[FGUI] input self test begin")
		print("[FGUI] input self test click buy:", FairyGuiManager:DebugInjectClick(503, 616, 0))
		threadpool:delay(1, function()
			print("[FGUI] input self test click reward:", FairyGuiManager:DebugInjectClick(745, 485, 0))
		end)
		threadpool:delay(2, function()
			print("[FGUI] input self test click close:", FairyGuiManager:DebugInjectClick(1154, 202, 0))
		end)
	end)
end

local function tryOpenFairyGuiSample()
	local file = io.open("res/fuires/act_37_test.fui", "rb")
	if file == nil then
		return
	end
	file:close()

	threadpool:delay(1, function()
		local mvcCtrl = FairyGuiManager:Open("Act37TestMvc", {
			roleId = 1001,
			source = "InitialOpen",
			showRed = true,
		})
		local mvcHandle = mvcCtrl and mvcCtrl:GetHandle() or nil
		print("[FGUI] open act_37_test mvc:", mvcHandle)

		threadpool:delay(2, function()
			local reopenCtrl = FairyGuiManager:Open("Act37TestMvc", {
				roleId = 1002,
				source = "ReopenSample",
				showRed = false,
			})
			local reopenHandle = reopenCtrl and reopenCtrl:GetHandle() or nil
			print("[FGUI] reopen act_37_test mvc:", reopenHandle)
		end)

		tryRunFairyGuiInputSelfTest()
	end)
end

function FGUI_ReopenAct37Sample()
	return FairyGuiManager:Open("Act37TestMvc", {
		roleId = 2001,
		source = "ManualReopen",
		showRed = true,
	})
end

function FGUI_CloseAct37Sample()
	return FairyGuiManager:Close("Act37TestMvc")
end

function FGUI_DestroyAct37Sample()
	return FairyGuiManager:Destroy("Act37TestMvc")
end

function FGUI_Dump()
	return FairyGuiManager:Dump()
end

function FGUI_DebugInjectClick(x, y, button)
	return FairyGuiManager:DebugInjectClick(x, y, button or 0)
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

