--require("res.scripts.samples.Sandbox1")
--require("res.scripts.samples.Sandbox2")
--require("res.scripts.samples.Sandbox3")
--require("res.scripts.samples.Sandbox4")
--require("res.scripts.samples.Sandbox5")
--require("res.scripts.samples.Sandbox6")
--require("res.scripts.samples.Sandbox7")
require("res.scripts.samples.Sandbox8")

_G.LuaPluginMgr = ClassList.LuaPluginMgr:new()

require("res.scripts.samples.fgui_init")

local function isEnvEnabled(name)
	local value = os.getenv and os.getenv(name) or nil
	return value == "1" or value == "true" or value == "TRUE" or value == "yes" or value == "YES"
end

local FGUI_AUTOMATION_ENV_NAMES = {
	"HELLO_FGUI_SKIP_SANDBOX_SCENE",
	"HELLO_FGUI_DEBUG_PANEL_DEMO",
	"HELLO_FGUI_LONG_LOOP_SELF_TEST",
	"HELLO_FGUI_SELF_TEST_ALL",
	"HELLO_FGUI_INPUT_SELF_TEST",
	"HELLO_FGUI_KEY_SELF_TEST",
	"HELLO_FGUI_TEXT_INPUT_SELF_TEST",
	"HELLO_FGUI_LIFECYCLE_SELF_TEST",
	"HELLO_FGUI_CACHE_SELF_TEST",
	"HELLO_FGUI_COMMON_SERVICE_SELF_TEST",
	"HELLO_FGUI_COMMON_SERVICE_DEMO",
	"HELLO_FGUI_POPUP_RULE_SELF_TEST",
	"HELLO_FGUI_GUIDE_MASK_SELF_TEST",
	"HELLO_FGUI_EVENT_PAYLOAD_SELF_TEST",
	"HELLO_FGUI_SCREEN_ADAPT_SELF_TEST",
	"HELLO_FGUI_SCREEN_ADAPT_DEMO",
	"HELLO_FGUI_BUSINESS_FLOW_SELF_TEST",
	"HELLO_FGUI_BUSINESS_BENCHMARK_SELF_TEST",
	"HELLO_FGUI_RESOURCE_POLICY_SELF_TEST",
	"HELLO_FGUI_RESOURCE_FALLBACK_SELF_TEST",
	"HELLO_FGUI_TEXT_INPUT_POLICY_SELF_TEST",
	"HELLO_FGUI_PRESSURE_SELF_TEST",
	"HELLO_FGUI_LAYER_SELF_TEST",
	"HELLO_FGUI_LAYER_CLOSE_SELF_TEST",
	"HELLO_FGUI_LAYER_BOUNDARY_SELF_TEST",
	"HELLO_FGUI_ACT38_SELF_TEST",
	"HELLO_FGUI_WHEEL_SELF_TEST",
	"HELLO_FGUI_MASK_SELF_TEST",
	"HELLO_FGUI_CLEANUP_SELF_TEST",
}

local function isFairyGuiAutomationMode()
	for _, name in ipairs(FGUI_AUTOMATION_ENV_NAMES) do
		if isEnvEnabled(name) then
			return true
		end
	end
	return false
end

_G.HELLO_FGUI_AUTOMATION_MODE = isFairyGuiAutomationMode()

if _G.HELLO_FGUI_AUTOMATION_MODE then
	_G.Sandbox_Initialize = function()
		print("[FGUI] skip Sandbox_Initialize for automation")
	end
	_G.Sandbox_Update = function(deltaTimeInMillis)
	end
end

_G.__init__ = function(sec, msec)
	math.randomseed(os.time())
	threadpool:init(sec * 1000 + msec, 10)

	-- Init LuaPanda
	LuaPanda.start("127.0.0.1", 8818)

	-- Start periodic tolua GC
	threadpool:delay(5, function()
		_G.__gc__()
	end)

	FGUI_Init()
end

_G.__tick__ = function(detalMsec)
	threadpool:update(detalMsec)
end

_G.__gc__ = function()
	-- Configure Lua GC step
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
