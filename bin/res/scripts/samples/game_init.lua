local SANDBOX_SAMPLE_NAMES = {
	Sandbox1 = true,
	Sandbox2 = true,
	Sandbox3 = true,
	Sandbox4 = true,
	Sandbox5 = true,
	Sandbox6 = true,
	Sandbox7 = true,
	Sandbox8 = true,
}

local function getEnvValue(name)
	return os.getenv and os.getenv(name) or nil
end

local function isTruthyEnvValue(value)
	return value == "1" or value == "true" or value == "TRUE" or value == "True" or value == "yes" or value == "YES"
end

local function getSandboxSampleName()
	local sampleName = getEnvValue("HELLO_SANDBOX_SAMPLE")
	if sampleName == nil or sampleName == "" or sampleName == "Default" then
		return "Sandbox8"
	end
	if string.match(sampleName, "^%d+$") ~= nil then
		sampleName = "Sandbox" .. sampleName
	end
	if SANDBOX_SAMPLE_NAMES[sampleName] ~= true then
		error("[SandboxSmoke] unknown HELLO_SANDBOX_SAMPLE=" .. tostring(sampleName))
	end
	return sampleName
end

local sandboxSampleName = getSandboxSampleName()
local sandboxSmokeRunId = getEnvValue("HELLO_SANDBOX_SMOKE_RUN_ID")
_G.HELLO_SANDBOX_SAMPLE_NAME = sandboxSampleName
_G.HELLO_SANDBOX_SMOKE_MODE = isTruthyEnvValue(getEnvValue("HELLO_SANDBOX_SMOKE_TEST"))
_G.HELLO_SANDBOX_SMOKE_RUN_ID = sandboxSmokeRunId
require("res.scripts.samples." .. sandboxSampleName)
if sandboxSmokeRunId ~= nil and sandboxSmokeRunId ~= "" then
	print("[SandboxSmoke] run id:", sandboxSmokeRunId)
end
print("[SandboxSmoke] sample selected:", sandboxSampleName)

_G.LuaPluginMgr = ClassList.LuaPluginMgr:new()

require("res.scripts.samples.fgui_init")
local RuntimeDiagnostics = require("res.scripts.samples.runtime_diagnostics")

local function isEnvEnabled(name)
	return isTruthyEnvValue(getEnvValue(name))
end

local function getEnvNumber(name, defaultValue)
	local value = tonumber(getEnvValue(name))
	if value == nil then
		return defaultValue
	end
	return value
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
	"HELLO_FGUI_VIRTUAL_LIST_SELF_TEST",
	"HELLO_FGUI_TREE_SELF_TEST",
	"HELLO_FGUI_SUBMODULE_SELF_TEST",
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

local function tryRunRuntimeDiagnosticSelfTest()
	if not isEnvEnabled("HELLO_RUNTIME_DIAGNOSTIC_SELF_TEST") then
		return
	end

	local delaySeconds = tonumber(getEnvValue("HELLO_RUNTIME_DIAGNOSTIC_DELAY")) or 4
	threadpool:delay(delaySeconds, function()
		RuntimeDiagnostics.RunSelfTest()
	end)
end

local function tryConfigureAiScheduler()
	if ConfigManager ~= nil and ConfigManager.ConfigureAiScheduler ~= nil then
		ConfigManager:ConfigureAiScheduler(ObjectManager, _G.HELLO_SANDBOX_SAMPLE_NAME)
		return
	end

	if ObjectManager == nil or ObjectManager.configureAiScheduler == nil then
		return
	end
	local enabled = isEnvEnabled("HELLO_AI_SCHEDULER_ENABLE")
	local tickMs = getEnvNumber("HELLO_AI_SCHEDULER_TICK_MS", 50)
	local maxPerFrame = getEnvNumber("HELLO_AI_SCHEDULER_MAX_PER_FRAME", 8)
	ObjectManager:configureAiScheduler(enabled, tickMs, maxPerFrame)
	if enabled then
		print("[AIScheduler] configured", "enabled=", tostring(enabled), "tickMs=", tickMs, "maxPerFrame=", maxPerFrame)
	end
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
	if ConfigManager ~= nil and ConfigManager.ApplyStartupSeed ~= nil then
		ConfigManager:ApplyStartupSeed(_G.HELLO_SANDBOX_SAMPLE_NAME)
	else
		math.randomseed(os.time())
	end
	threadpool:init(sec * 1000 + msec, 10)

	-- Init LuaPanda
	LuaPanda.start("127.0.0.1", 8818)

	-- Start periodic tolua GC
	threadpool:delay(5, function()
		_G.__gc__()
	end)

	FGUI_Init()
	tryConfigureAiScheduler()
	tryRunRuntimeDiagnosticSelfTest()
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
