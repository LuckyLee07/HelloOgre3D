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

	threadpool:delay(3, function()
		print("[FGUI] input self test begin")
		print("[FGUI] input self test click buy:", FairyGuiManager:DebugInjectClick(503, 616, 0))
		threadpool:delay(1, function()
			print("[FGUI] input self test click reward:", FairyGuiManager:DebugInjectClick(745, 485, 0))
		end)
		threadpool:delay(2, function()
			print("[FGUI] input self test click close:", FairyGuiManager:DebugInjectClick(1154, 202, 0))
		end)
		threadpool:delay(3, function()
			print("[FGUI] input self test close top popup:", FairyGuiManager:CloseTopPopup())
			FairyGuiManager:DumpStacks()
		end)
	end)
end

local function tryRunFairyGuiKeySelfTest()
	if not isEnvEnabled("HELLO_FGUI_KEY_SELF_TEST") then
		return
	end

	threadpool:delay(4, function()
		print("[FGUI] key self test escape:", FairyGuiManager:HandleKeyPressed(1, 0))
		FairyGuiManager:DumpStacks()
	end)
end

local function tryRunFairyGuiTextInputSelfTest()
	if not isEnvEnabled("HELLO_FGUI_TEXT_INPUT_SELF_TEST") then
		return
	end

	threadpool:delay(9, function()
		local handle, inputHandle = FGUI_OpenTextInputProbe()
		local focused = inputHandle ~= nil and FairyGuiManager:Focus(inputHandle) or false
		local keyA = FairyGuiManager:DebugInjectKeyPressed(30, 65)
		local keyB = FairyGuiManager:DebugInjectKeyPressed(48, 66)
		local backspace = FairyGuiManager:DebugInjectKeyPressed(14, 0)
		local submit = FairyGuiManager:DebugInjectKeyPressed(28, 0)
		local text = inputHandle ~= nil and FairyGuiManager:GetText(inputHandle) or ""
		local focusHandle = FairyGuiManager:GetFocusedHandle()
		print("[FGUI] text input self test result:", handle ~= nil, inputHandle, focused, keyA, keyB, backspace, submit, text, focusHandle)
		FairyGuiManager:Close("TextInputProbe", true)
	end)
end

local function tryRunFairyGuiLifecycleSelfTest()
	if not isEnvEnabled("HELLO_FGUI_LIFECYCLE_SELF_TEST") then
		return
	end

	threadpool:delay(10, function()
		local handle, inputHandle = FGUI_OpenTextInputProbe()
		local directBinding = inputHandle ~= nil and FairyGuiManager:AddChanged(inputHandle, "", function()
			print("[FGUI] lifecycle direct child changed")
		end) or nil
		local focused = inputHandle ~= nil and FairyGuiManager:Focus(inputHandle) or false
		local timerId = handle ~= nil and FairyGuiManager:Delay("TextInputProbe", 30, function()
			print("[FGUI] lifecycle stale timer fired")
		end) or nil
		local snapshot = FairyGuiManager:CaptureCloseSnapshot("TextInputProbe")
		local closed = FairyGuiManager:Close("TextInputProbe", true)
		local clean = FairyGuiManager:ValidateClosedObject(snapshot, nil, "TextInputProbeLifecycleSelfTest", true)
		local stats = FairyGuiManager:GetDebugStats()
		print("[FGUI] lifecycle residue self test result:", handle ~= nil, inputHandle, directBinding, timerId, focused, closed, clean, stats.openUI, stats.binding, stats.timer, stats.childCache, stats.objectHandle, stats.view, stats.controller)
	end)
end

local function tryRunFairyGuiCleanupSelfTest()
	if not isEnvEnabled("HELLO_FGUI_CLEANUP_SELF_TEST") then
		return
	end

	threadpool:delay(5, function()
		print("[FGUI] cleanup self test close scene:", FairyGuiManager:CloseScene("Default", true))
		FairyGuiManager:Dump()
	end)
end

local LAYER_SAMPLE_UIS = {
	{ name = "LayerProbeNormal", layer = "Normal", title = "Layer Normal" },
	{ name = "LayerProbePopup", layer = "Popup", title = "Layer Popup + Modal" },
	{ name = "LayerProbeTop", layer = "Top", title = "Layer Top" },
	{ name = "LayerProbeToast", layer = "Toast", title = "Layer Toast" },
}

local function setLayerProbeText(handle, item)
	if handle == nil then
		return
	end

	local layerRoot = FairyGuiManager:GetLayerRoot(item.layer)
	FairyGuiManager:SetText(handle, "txt_title", item.title)
	FairyGuiManager:SetText(handle, "txt_time", string.format("handle=%s layerRoot=%s", tostring(handle), tostring(layerRoot)))
	FairyGuiManager:SetText(handle, "txt_desc", "LayerProbe: Close with FGUI_CloseLayerSample()")
end

local function getLayerProbeOpenCount()
	local count = 0
	for _, item in ipairs(LAYER_SAMPLE_UIS) do
		if FairyGuiManager:GetObjectInfo(item.name) ~= nil then
			count = count + 1
		end
	end
	return count
end

local function dumpLayerProbeCloseState(label)
	print("[FGUI] layer close state:", label, "probeCount=", getLayerProbeOpenCount())
	FairyGuiManager:DumpOpenUIs()
	FairyGuiManager:DumpStacks()
end

local function tryRunFairyGuiLayerSelfTest()
	if not isEnvEnabled("HELLO_FGUI_LAYER_SELF_TEST") then
		return
	end

	threadpool:delay(4, function()
		FGUI_OpenLayerSample()
		threadpool:delay(1, function()
			FGUI_DumpLayerSample()
		end)
	end)
end

local function tryRunFairyGuiLayerCloseSelfTest()
	if not isEnvEnabled("HELLO_FGUI_LAYER_CLOSE_SELF_TEST") then
		return
	end

	threadpool:delay(7, function()
		FGUI_RunLayerCloseSelfTest()
	end)
end

local function tryRunFairyGuiAct38SelfTest()
	if not isEnvEnabled("HELLO_FGUI_ACT38_SELF_TEST") then
		return
	end

	threadpool:delay(5, function()
		local ctrl = FGUI_OpenAct38Sample()
		local handle = ctrl and ctrl:GetHandle() or nil
		print("[FGUI] act38 self test result:", ctrl ~= nil, handle)
		local listApiOk = ctrl ~= nil and ctrl.RunListApiSelfTest ~= nil and ctrl:RunListApiSelfTest() or false
		print("[FGUI] act38 list api self test result:", listApiOk)
		threadpool:delay(1, function()
			FGUI_DumpAct38Sample()
		end)
	end)
end

local function tryRunFairyGuiWheelSelfTest()
	if not isEnvEnabled("HELLO_FGUI_WHEEL_SELF_TEST") then
		return
	end

	threadpool:delay(8, function()
		local ctrl = FairyGuiManager:GetController("Act38Test") or FGUI_OpenAct38Sample()
		local handle = ctrl and ctrl:GetHandle() or nil
		print("[FGUI] wheel self test open:", ctrl ~= nil, handle)
		threadpool:delay(1, function()
			local scrollDown = FairyGuiManager:DebugInjectMouseWheel(745, 485, -120)
			local scrollUp = FairyGuiManager:DebugInjectMouseWheel(745, 485, 120)
			print("[FGUI] wheel self test inject:", scrollDown, scrollUp)
			FairyGuiManager:DumpDebugStats()
		end)
	end)
end

local function tryRunFairyGuiMaskSelfTest()
	if not isEnvEnabled("HELLO_FGUI_MASK_SELF_TEST") then
		return
	end

	threadpool:delay(6, function()
		local handle = FGUI_OpenMaskProbe()
		print("[FGUI] mask probe self test result:", handle ~= nil, handle)
		FairyGuiManager:DumpRenderStats()
		threadpool:delay(1, function()
			local closed = FGUI_CloseMaskProbe()
			print("[FGUI] mask probe close self test:", closed)
			FairyGuiManager:DumpOpenUIs()
			FairyGuiManager:DumpStacks()
			FairyGuiManager:DumpDebugStats()
			FairyGuiManager:DumpRenderStats()
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
		tryRunFairyGuiKeySelfTest()
		tryRunFairyGuiTextInputSelfTest()
		tryRunFairyGuiLifecycleSelfTest()
		tryRunFairyGuiCleanupSelfTest()
		tryRunFairyGuiLayerSelfTest()
		tryRunFairyGuiLayerCloseSelfTest()
		tryRunFairyGuiAct38SelfTest()
		tryRunFairyGuiWheelSelfTest()
		tryRunFairyGuiMaskSelfTest()
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

function FGUI_OpenAct38Sample()
	FairyGuiManager:Close("Act38Test", true)
	local ctrl = FairyGuiManager:Open("Act38Test", {
		selectedTab = 0,
		dateText = "Act38 MVC Sample",
	})
	if ctrl == nil then
		print("[FGUI] open act_38_test failed")
		return nil
	end

	local handle = ctrl:GetHandle()
	print(
		"[FGUI] open act_38_test:",
		handle,
		"dayCount=", ctrl:GetListItemCount("m2_dayTaskList"),
		"spcCount=", ctrl:GetListItemCount("m2_spcTaskList"),
		"shopCount=", ctrl:GetListItemCount("m2_excShopList"))
	return ctrl
end

function FGUI_CloseAct38Sample()
	return FairyGuiManager:Close("Act38Test", true)
end

function FGUI_OpenMaskProbe()
	return FairyGuiManager:OpenMaskProbe({
		key = "MaskProbe",
		layer = "Top",
		group = "Sample",
		scene = "Default",
		popupMode = "stack",
		assets = {
			background = "res/assets/act_38/_imgs/board_task.png",
			content = "res/assets/act_38/_imgs/board_task.png",
			stripA = "res/assets/act_38/_imgs/board_prop_exchange.png",
			stripB = "res/assets/act_38/_imgs/btn_green.png",
			stripC = "res/assets/act_38/_imgs/button_close.png",
			mask = "res/assets/act_38/_imgs/img_mask.png",
		},
	})
end

function FGUI_CloseMaskProbe()
	return FairyGuiManager:Close("MaskProbe", true)
end

function FGUI_OpenTextInputProbe()
	return FairyGuiManager:OpenTextInputProbe({
		key = "TextInputProbe",
		layer = "Top",
		group = "Sample",
		scene = "Default",
		popupMode = "stack",
	})
end

function FGUI_CloseTextInputProbe()
	return FairyGuiManager:Close("TextInputProbe", true)
end

function FGUI_DumpAct38Sample()
	local objectInfo = FairyGuiManager:GetObjectInfo("Act38Test")
	if objectInfo == nil then
		print("[FGUI] act38 dump: closed")
		return false
	end
	print("[FGUI] act38 dump:", objectInfo.handle, objectInfo.packageName, objectInfo.objectName, objectInfo.layer, objectInfo.sortingOrder)
	print("[FGUI] act38 day count:", FairyGuiManager:GetListItemCount(objectInfo.handle, "m2_dayTaskList"))
	print("[FGUI] act38 spc count:", FairyGuiManager:GetListItemCount(objectInfo.handle, "m2_spcTaskList"))
	print("[FGUI] act38 shop count:", FairyGuiManager:GetListItemCount(objectInfo.handle, "m2_excShopList"))
	return true
end

function FGUI_Dump()
	return FairyGuiManager:Dump()
end

function FGUI_OpenLayerSample()
	local results = {}
	FairyGuiManager:CloseGroup("LayerProbe", true)
	for _, item in ipairs(LAYER_SAMPLE_UIS) do
		local handle = FairyGuiManager:Open(item.name)
		results[item.name] = handle
		setLayerProbeText(handle, item)

		local objectInfo = handle ~= nil and FairyGuiManager:GetObjectInfo(handle) or nil
		print(
			"[FGUI] layer sample open:",
			item.name,
			"handle=", handle,
			"layer=", item.layer,
			"layerRoot=", FairyGuiManager:GetLayerRoot(item.layer),
			"sorting=", objectInfo and objectInfo.sortingOrder)
	end
	FairyGuiManager:DumpLayerRoots()
	FairyGuiManager:DumpOpenUIs()
	FairyGuiManager:DumpStacks()
	return results
end

function FGUI_CloseLayerSample()
	return FairyGuiManager:CloseGroup("LayerProbe", true)
end

function FGUI_DumpLayerSample()
	FairyGuiManager:DumpLayerRoots()
	FairyGuiManager:DumpOpenUIs()
	FairyGuiManager:DumpStacks()
	return true
end

function FGUI_RunLayerCloseSelfTest()
	if getLayerProbeOpenCount() == 0 then
		FGUI_OpenLayerSample()
	end

	dumpLayerProbeCloseState("before")
	print("[FGUI] layer close top popup:", FairyGuiManager:CloseTopPopup(true))
	dumpLayerProbeCloseState("after CloseTopPopup")
	print("[FGUI] layer close Top:", FairyGuiManager:CloseLayer("Top", true))
	dumpLayerProbeCloseState("after CloseLayer Top")
	print("[FGUI] layer close group:", FairyGuiManager:CloseGroup("LayerProbe", true))
	dumpLayerProbeCloseState("after CloseGroup")
	return getLayerProbeOpenCount() == 0
end

function FGUI_DebugInjectClick(x, y, button)
	return FairyGuiManager:DebugInjectClick(x, y, button or 0)
end

function FGUI_DebugInjectKeyPressed(keyCode, keyText)
	return FairyGuiManager:DebugInjectKeyPressed(keyCode, keyText or 0)
end

function FGUI_DebugKeyPressed(keyCode, keyText)
	return FairyGuiManager:HandleKeyPressed(keyCode, keyText or 0)
end

function FGUI_CloseLayer(layerName, forceDestroy)
	return FairyGuiManager:CloseLayer(layerName, forceDestroy)
end

function FGUI_CloseGroup(groupName, forceDestroy)
	return FairyGuiManager:CloseGroup(groupName, forceDestroy)
end

function FGUI_CloseScene(sceneName, forceDestroy)
	return FairyGuiManager:CloseScene(sceneName, forceDestroy)
end

function FGUI_ChangeScene(sceneName, forceDestroy)
	return FairyGuiManager:ChangeScene(sceneName, forceDestroy)
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

