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

local function tryRunFairyGuiCacheSelfTest()
	if not isEnvEnabled("HELLO_FGUI_CACHE_SELF_TEST") then
		return
	end

	threadpool:delay(11, function()
		print("[FGUI] cache self test result:", FGUI_RunCacheSelfTest())
		FairyGuiManager:DumpHealth(true)
	end)
end

local function tryRunFairyGuiCommonServiceSelfTest()
	if not isEnvEnabled("HELLO_FGUI_COMMON_SERVICE_SELF_TEST") then
		return
	end

	threadpool:delay(12, function()
		print("[FGUI] common service self test result:", FGUI_RunCommonServiceSelfTest())
		FairyGuiManager:DumpServices()
		FairyGuiManager:DumpHealth(true)
	end)
end

local function tryRunFairyGuiCommonServiceDemo()
	if not isEnvEnabled("HELLO_FGUI_COMMON_SERVICE_DEMO") then
		return
	end

	threadpool:delay(5, function()
		print("[FGUI] common service demo result:", FGUI_RunCommonServiceDemo())
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

local function tryRunFairyGuiSelfTestSuite()
	if not isEnvEnabled("HELLO_FGUI_SELF_TEST_ALL") then
		return
	end

	threadpool:delay(4, function()
		FGUI_RunSelfTestSuite()
	end)
end

local function tryRunFairyGuiLongLoopSelfTest()
	if not isEnvEnabled("HELLO_FGUI_LONG_LOOP_SELF_TEST") and not isEnvEnabled("HELLO_FGUI_SELF_TEST_ALL") then
		return
	end

	local startDelay = isEnvEnabled("HELLO_FGUI_SELF_TEST_ALL") and 12 or 6
	threadpool:delay(startDelay, function()
		local count = tonumber(os.getenv and os.getenv("HELLO_FGUI_LONG_LOOP_COUNT") or nil)
		FGUI_RunLongLoopSelfTest({
			iterations = count or (isEnvEnabled("HELLO_FGUI_SELF_TEST_ALL") and 5 or 12),
		})
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
		tryRunFairyGuiCacheSelfTest()
		tryRunFairyGuiCommonServiceSelfTest()
		tryRunFairyGuiCommonServiceDemo()
		tryRunFairyGuiCleanupSelfTest()
		tryRunFairyGuiLayerSelfTest()
		tryRunFairyGuiLayerCloseSelfTest()
		tryRunFairyGuiAct38SelfTest()
		tryRunFairyGuiWheelSelfTest()
		tryRunFairyGuiMaskSelfTest()
		tryRunFairyGuiSelfTestSuite()
		tryRunFairyGuiLongLoopSelfTest()
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

function FGUI_DumpHealth(verbose)
	return FairyGuiManager:DumpHealth(verbose == true)
end

function FGUI_DumpResourceRefs()
	return FairyGuiManager:DumpResourceRefs()
end

function FGUI_DumpResourceWarnings()
	return FairyGuiManager:DumpResourceWarnings()
end

function FGUI_Dump()
	return FairyGuiManager:Dump()
end

function FGUI_ShowToast(text, duration)
	return FairyGuiManager:ShowToast(text, duration)
end

function FGUI_ShowLoading(text)
	return FairyGuiManager:ShowLoading(text)
end

function FGUI_HideLoading()
	return FairyGuiManager:HideLoading()
end

function FGUI_ShowMessageBox(title, message)
	return FairyGuiManager:ShowMessageBox(title, message, { "OK", "Cancel" }, function(index, label)
		print("[FGUI] message box select:", index, label)
	end)
end

function FGUI_ShowDialog(title, message)
	return FGUI_ShowMessageBox(title, message)
end

function FGUI_ShowTip(text, x, y, duration)
	return FairyGuiManager:ShowTip(text, x, y, duration)
end

function FGUI_ShowGuideMask(text)
	return FairyGuiManager:ShowGuideMask({
		text = text or "GuideMask",
		closeOnMaskClick = true,
	})
end

function FGUI_HideGuideMask()
	return FairyGuiManager:HideGuideMask()
end

function FGUI_ShowPopupMenu()
	return FairyGuiManager:ShowPopupMenu({ "Inspect", "Close", "Cancel" }, 80, 120, function(index, item)
		print("[FGUI] popup menu select:", index, item)
	end)
end

function FGUI_RunCommonServiceDemo()
	if threadpool == nil or threadpool.delay == nil then
		print("[FGUI] common service demo failed: threadpool unavailable")
		return false
	end

	FairyGuiManager:ClearToastQueue()
	FairyGuiManager:Close("__Toast", true, "commonServiceDemoReset")
	FairyGuiManager:Close("__Tip", true, "commonServiceDemoReset")
	FairyGuiManager:HideLoading({ force = true, reason = "commonServiceDemoReset" })
	FairyGuiManager:Close("__GuideMask", true, "commonServiceDemoReset")
	FairyGuiManager:Close("__MessageBox", true, "commonServiceDemoReset")
	FairyGuiManager:Close("__PopupMenu", true, "commonServiceDemoReset")

	local delay = 0
	local function schedule(stepDelay, name, func)
		delay = delay + stepDelay
		threadpool:delay(delay, function()
			print("[FGUI] common service demo step:", name)
			local ok, err = pcall(func)
			if not ok then
				print("[FGUI] common service demo step error:", name, err)
			end
		end)
	end

	print("[FGUI] common service demo begin")
	schedule(0.1, "ToastQueue", function()
		FairyGuiManager:ShowToast("Toast 1/3: first message", 2, { dedupeKey = "CommonDemoToast1" })
		FairyGuiManager:ShowToast("Toast 1/3: first message", 2, { dedupeKey = "CommonDemoToast1" })
		FairyGuiManager:ShowToast("Toast 2/3: queued message", 2, { dedupeKey = "CommonDemoToast2" })
		FairyGuiManager:ShowToast("Toast 3/3: queue finished", 2, { dedupeKey = "CommonDemoToast3" })
	end)
	schedule(0.8, "LoadingOpenTwice", function()
		FairyGuiManager:ShowLoading("Loading ref 1/2", { refKey = "CommonDemo", timeout = 10 })
		FairyGuiManager:ShowLoading("Loading ref 2/2: first hide keeps me", { refKey = "CommonDemo", timeout = 10 })
	end)
	schedule(2.0, "LoadingHideOnce", function()
		FairyGuiManager:HideLoading({ refKey = "CommonDemo", reason = "commonServiceDemoHideOnce" })
		FairyGuiManager:ShowToast("HideLoading once: loading stays", 2, { dedupeKey = "CommonDemoLoadingOnce" })
	end)
	schedule(2.0, "LoadingHideTwice", function()
		FairyGuiManager:HideLoading({ refKey = "CommonDemo", reason = "commonServiceDemoHideTwice" })
		FairyGuiManager:ShowToast("HideLoading twice: loading closed", 2, { dedupeKey = "CommonDemoLoadingTwice" })
	end)
	schedule(1.0, "Tip", function()
		FairyGuiManager:ShowTip("Tip demo: fixed screen position", 72, 96, 4)
	end)
	schedule(1.2, "MessageBox", function()
		FairyGuiManager:ShowMessageBox("Common Service Demo", "MessageBox stays for a moment, then closes automatically.", { "OK", "Later" }, function(index, label)
			print("[FGUI] common service demo message select:", index, label)
		end)
	end)
	schedule(2.2, "CloseMessageBox", function()
		FairyGuiManager:Close("__MessageBox", true, "commonServiceDemoAutoClose")
	end)
	schedule(0.4, "PopupMenu", function()
		FairyGuiManager:ShowPopupMenu({ "Inspect", "Use", "Cancel" }, 96, 156, function(index, item)
			print("[FGUI] common service demo popup select:", index, item)
		end)
	end)
	schedule(2.0, "ClosePopupMenu", function()
		FairyGuiManager:Close("__PopupMenu", true, "commonServiceDemoAutoClose")
	end)
	schedule(0.4, "GuideMask", function()
		FairyGuiManager:ShowGuideMask({
			text = "GuideMask demo: click mask or wait",
			textX = 80,
			textY = 120,
			closeOnMaskClick = true,
		})
	end)
	schedule(3.0, "Cleanup", function()
		FairyGuiManager:CloseToast("commonServiceDemoAutoClose")
		FairyGuiManager:ClearToastQueue()
		FairyGuiManager:HideGuideMask("commonServiceDemoAutoClose")
		FairyGuiManager:Close("__Tip", true, "commonServiceDemoAutoClose")
		FairyGuiManager:HideLoading({ force = true, reason = "commonServiceDemoCleanup" })
		FairyGuiManager:DumpServices()
		FairyGuiManager:DumpHealth(true)
		print("[FGUI] common service demo end")
	end)
	return true
end

function FGUI_RunCommonServiceSelfTest()
	FairyGuiManager:ClearToastQueue()
	FairyGuiManager:Close("__Toast", true, "serviceSelfTestReset")
	FairyGuiManager:Close("__Tip", true, "serviceSelfTestReset")
	FairyGuiManager:HideLoading({ force = true, reason = "serviceSelfTestReset" })
	FairyGuiManager:Close("__GuideMask", true, "serviceSelfTestReset")
	FairyGuiManager:Close("__MessageBox", true, "serviceSelfTestReset")
	FairyGuiManager:Close("__PopupMenu", true, "serviceSelfTestReset")

	local toastHandle = FairyGuiManager:ShowToast("Toast service", 0)
	local toastDedupeHandle = FairyGuiManager:ShowToast("Toast service", 0)
	local toastQueuedId = FairyGuiManager:ShowToast("Toast service queued", 0)
	local toastQueueOk = toastDedupeHandle == toastHandle and FairyGuiManager:GetToastQueueCount() == 1 and toastQueuedId ~= nil
	local tipHandle = FairyGuiManager:ShowTip("Tip service", 40, 80, 0)
	local loadingHandle = FairyGuiManager:ShowLoading("Loading service", { refKey = "SelfTest" })
	local loadingHandle2 = FairyGuiManager:ShowLoading("Loading service updated", { refKey = "SelfTest" })
	local loadingRefOk = loadingHandle ~= nil and loadingHandle2 == loadingHandle and FairyGuiManager:GetLoadingRefCount() == 2
	local guideHandle = FairyGuiManager:ShowGuideMask({ text = "Guide service" })
	local messageHandle = FairyGuiManager:ShowMessageBox("Message", "Common service probe", { "OK", "Cancel" })
	local popupHandle = FairyGuiManager:ShowPopupMenu({ "One", "Two" }, 100, 120)
	local serviceStats = FairyGuiManager:GetServiceStats()
	local opened = toastHandle ~= nil
		and tipHandle ~= nil
		and loadingHandle ~= nil
		and guideHandle ~= nil
		and messageHandle ~= nil
		and popupHandle ~= nil
		and serviceStats.Toast ~= nil
		and serviceStats.Tip ~= nil
		and serviceStats.Loading ~= nil
		and serviceStats.GuideMask ~= nil
		and serviceStats.MessageBox ~= nil
		and serviceStats.PopupMenu ~= nil
		and toastQueueOk == true
		and loadingRefOk == true

	local loadingHideOnce = FairyGuiManager:HideLoading({ refKey = "SelfTest", reason = "serviceSelfTestHideOnce" })
	local loadingStillOpen = FairyGuiManager:GetObjectInfo("__Loading") ~= nil and FairyGuiManager:GetLoadingRefCount() == 1
	local loadingHideTwice = FairyGuiManager:HideLoading({ refKey = "SelfTest", reason = "serviceSelfTestHideTwice" })
	local loadingClosed = FairyGuiManager:GetObjectInfo("__Loading") == nil and FairyGuiManager:GetLoadingRefCount() == 0

	FairyGuiManager:Close("__Toast", true, "serviceSelfTestCleanup")
	FairyGuiManager:Close("__Tip", true, "serviceSelfTestCleanup")
	FairyGuiManager:HideLoading({ force = true, reason = "serviceSelfTestCleanup" })
	FairyGuiManager:Close("__GuideMask", true, "serviceSelfTestCleanup")
	FairyGuiManager:Close("__MessageBox", true, "serviceSelfTestCleanup")
	FairyGuiManager:Close("__PopupMenu", true, "serviceSelfTestCleanup")
	local cleanupStats = FairyGuiManager:GetServiceStats()
	local cleaned = cleanupStats.Toast == nil
		and cleanupStats.Tip == nil
		and cleanupStats.Loading == nil
		and cleanupStats.GuideMask == nil
		and cleanupStats.MessageBox == nil
		and cleanupStats.PopupMenu == nil
		and cleanupStats.__meta.loadingRefTotal == 0
		and cleanupStats.__meta.toastQueue == 0

	print(
		"[FGUI] common service self test detail:",
		"toast=", toastHandle,
		"toastQueueOk=", toastQueueOk,
		"tip=", tipHandle,
		"loading=", loadingHandle,
		"loadingRefOk=", loadingRefOk,
		"loadingHide=", loadingHideOnce, loadingStillOpen, loadingHideTwice, loadingClosed,
		"guide=", guideHandle,
		"message=", messageHandle,
		"popup=", popupHandle,
		"opened=", opened,
		"cleaned=", cleaned)
	return opened == true and loadingHideOnce == true and loadingStillOpen == true and loadingHideTwice == true and loadingClosed == true and cleaned == true
end

function FGUI_RunSelfTestSuite()
	if threadpool == nil or threadpool.delay == nil then
		print("[FGUI] self test suite failed: threadpool unavailable")
		return false
	end

	local results = {}
	local delay = 0
	local function record(name, ok, detail)
		results[#results + 1] = {
			name = name,
			ok = ok == true,
			detail = detail,
		}
		print("[FGUI] self test case:", name, ok == true and "PASS" or "FAIL", detail or "")
	end
	local function schedule(stepDelay, name, func)
		delay = delay + stepDelay
		threadpool:delay(delay, function()
			local ok, result, detail = pcall(func)
			if not ok then
				record(name, false, result)
			else
				record(name, result == true, detail)
			end
		end)
	end

	print("[FGUI] self test suite begin")
	schedule(0.2, "Act37Open", function()
		local ctrl = FGUI_ReopenAct37Sample()
		return ctrl ~= nil, ctrl and ctrl:GetHandle() or nil
	end)
	schedule(0.6, "Act38ListApi", function()
		local ctrl = FGUI_OpenAct38Sample()
		local ok = ctrl ~= nil and ctrl.RunListApiSelfTest ~= nil and ctrl:RunListApiSelfTest() or false
		return ok, ctrl and ctrl:GetHandle() or nil
	end)
	schedule(0.6, "MouseWheel", function()
		local scrollDown = FairyGuiManager:DebugInjectMouseWheel(745, 485, -120)
		local scrollUp = FairyGuiManager:DebugInjectMouseWheel(745, 485, 120)
		return scrollDown and scrollUp, tostring(scrollDown) .. "," .. tostring(scrollUp)
	end)
	schedule(0.6, "TextInput", function()
		local handle, inputHandle = FGUI_OpenTextInputProbe()
		local focused = inputHandle ~= nil and FairyGuiManager:Focus(inputHandle) or false
		local keyA = FairyGuiManager:DebugInjectKeyPressed(30, 65)
		local keyB = FairyGuiManager:DebugInjectKeyPressed(48, 66)
		local backspace = FairyGuiManager:DebugInjectKeyPressed(14, 0)
		local submit = FairyGuiManager:DebugInjectKeyPressed(28, 0)
		local text = inputHandle ~= nil and FairyGuiManager:GetText(inputHandle) or ""
		FairyGuiManager:Close("TextInputProbe", true)
		return handle ~= nil and focused and keyA and keyB and backspace and submit and text == "A", text
	end)
	schedule(0.6, "LayerClose", function()
		return FGUI_RunLayerCloseSelfTest()
	end)
	schedule(0.6, "MaskProbe", function()
		local handle = FGUI_OpenMaskProbe()
		local closed = FGUI_CloseMaskProbe()
		return handle ~= nil and closed == true, handle
	end)
	schedule(0.6, "LifecycleResidue", function()
		local handle, inputHandle = FGUI_OpenTextInputProbe()
		if handle == nil or inputHandle == nil then
			return false
		end
		FairyGuiManager:AddChanged(inputHandle, "", function()
			print("[FGUI] self test direct child changed")
		end)
		FairyGuiManager:Focus(inputHandle)
		FairyGuiManager:Delay("TextInputProbe", 30, function()
			print("[FGUI] self test stale timer fired")
		end)
		local snapshot = FairyGuiManager:CaptureCloseSnapshot("TextInputProbe")
		local closed = FairyGuiManager:Close("TextInputProbe", true)
		local clean = FairyGuiManager:ValidateClosedObject(snapshot, nil, "SelfTestSuite", true)
		return closed == true and clean == true
	end)
	schedule(0.6, "CacheHideReopen", function()
		return FGUI_RunCacheSelfTest()
	end)
	schedule(0.6, "CommonServices", function()
		return FGUI_RunCommonServiceSelfTest()
	end)
	schedule(0.6, "Cleanup", function()
		FairyGuiManager:Close("Act37TestMvc", true)
		FairyGuiManager:Close("Act38Test", true)
		FairyGuiManager:CloseGroup("LayerProbe", true)
		FairyGuiManager:Close("MaskProbe", true)
		FairyGuiManager:Close("TextInputProbe", true)
		local stats = FairyGuiManager:DumpHealth(true)
		local resourceWarnings = FairyGuiManager:GetResourceWarnings()
		return stats.openUI == 0 and stats.binding == 0 and stats.timer == 0 and #resourceWarnings == 0, "openUI=" .. tostring(stats.openUI)
	end)
	threadpool:delay(delay + 0.6, function()
		local passCount = 0
		for _, result in ipairs(results) do
			if result.ok then
				passCount = passCount + 1
			end
		end
		print("[FGUI] self test suite end:", passCount, "/", #results)
	end)
	return true
end

local function closeFairyGuiLongLoopObjects()
	FairyGuiManager:Close("Act37TestMvc", true)
	FairyGuiManager:Close("Act38Test", true)
	FairyGuiManager:CloseGroup("LayerProbe", true)
	FairyGuiManager:Close("MaskProbe", true)
	FairyGuiManager:Close("TextInputProbe", true)
end

local function getFairyGuiPackageRefCount()
	local refCount = 0
	local details = {}
	local refsByPackage = FairyGuiManager:GetPackageRefs()
	for packageKey, ref in pairs(refsByPackage) do
		local packageRefCount = ref.refCount or 0
		refCount = refCount + packageRefCount
		if packageRefCount > 0 or (ref.openCount or 0) > 0 then
			details[#details + 1] = tostring(packageKey) .. ":ref=" .. tostring(packageRefCount) .. ",open=" .. tostring(ref.openCount or 0)
		end
	end
	return refCount, table.concat(details, "|")
end

local function checkFairyGuiLongLoopClean(label)
	local stats = FairyGuiManager:GetHealthStats()
	local warnings = FairyGuiManager:GetResourceWarnings()
	local packageRefCount, packageDetail = getFairyGuiPackageRefCount()
	local clean = stats.openUI == 0
		and stats.hiddenUI == 0
		and stats.binding == 0
		and stats.timer == 0
		and stats.objectHandle == 0
		and stats.childCache == 0
		and stats.view == 0
		and stats.controller == 0
		and packageRefCount == 0
		and #warnings == 0
	local detail = "openUI=" .. tostring(stats.openUI)
		.. " hiddenUI=" .. tostring(stats.hiddenUI)
		.. " binding=" .. tostring(stats.binding)
		.. " timer=" .. tostring(stats.timer)
		.. " objectHandle=" .. tostring(stats.objectHandle)
		.. " childCache=" .. tostring(stats.childCache)
		.. " view=" .. tostring(stats.view)
		.. " controller=" .. tostring(stats.controller)
		.. " packageRef=" .. tostring(packageRefCount)
		.. " warnings=" .. tostring(#warnings)
	if packageDetail ~= "" then
		detail = detail .. " packages=" .. packageDetail
	end
	if not clean then
		print("[FGUI] long loop dirty:", label or "", detail)
		for _, warning in ipairs(warnings) do
			print("[FGUI] long loop warning:", warning.kind, "package=", warning.packageKey, warning.detail or "")
		end
		FairyGuiManager:DumpHealth(true)
	end
	return clean, detail
end

function FGUI_RunLongLoopSelfTest(config)
	if threadpool == nil or threadpool.delay == nil then
		print("[FGUI] long loop self test failed: threadpool unavailable")
		return false
	end

	config = config or {}
	local iterations = tonumber(config.iterations) or 12
	local interval = tonumber(config.interval) or 0.2
	local results = {
		pass = 0,
		fail = 0,
	}

	if iterations < 1 then
		iterations = 1
	end
	if interval < 0 then
		interval = 0
	end

	local function record(iteration, ok, detail)
		if ok == true then
			results.pass = results.pass + 1
		else
			results.fail = results.fail + 1
		end
		print("[FGUI] long loop iteration:", iteration, ok == true and "PASS" or "FAIL", detail or "")
	end

	local function runIteration(iteration)
		local ok, result, detail = pcall(function()
			closeFairyGuiLongLoopObjects()

			local ctrl37 = FGUI_ReopenAct37Sample()
			if ctrl37 == nil then
				return false, "Act37 open failed"
			end
			FairyGuiManager:Close("Act37TestMvc", true)

			local ctrl38 = FGUI_OpenAct38Sample()
			if ctrl38 == nil then
				return false, "Act38 open failed"
			end
			local listOk = ctrl38.RunListApiSelfTest ~= nil and ctrl38:RunListApiSelfTest() or false
			FairyGuiManager:Close("Act38Test", true)
			if listOk ~= true then
				return false, "Act38 list api failed"
			end

			if FGUI_RunLayerCloseSelfTest() ~= true then
				return false, "Layer close failed"
			end

			local maskHandle = FGUI_OpenMaskProbe()
			local maskClosed = FGUI_CloseMaskProbe()
			if maskHandle == nil or maskClosed ~= true then
				return false, "Mask probe failed"
			end

			local textHandle, inputHandle = FGUI_OpenTextInputProbe()
			local focused = inputHandle ~= nil and FairyGuiManager:Focus(inputHandle) or false
			local textClosed = FGUI_CloseTextInputProbe()
			if textHandle == nil or inputHandle == nil or focused ~= true or textClosed ~= true then
				return false, "TextInput probe failed"
			end

			closeFairyGuiLongLoopObjects()
			return checkFairyGuiLongLoopClean("iteration " .. tostring(iteration))
		end)

		if not ok then
			record(iteration, false, result)
		else
			record(iteration, result == true, detail)
		end

		if iteration >= iterations or results.fail > 0 then
			closeFairyGuiLongLoopObjects()
			local finalClean, finalDetail = checkFairyGuiLongLoopClean("final")
			print("[FGUI] long loop self test end:", results.pass, "/", iterations, "fail=", results.fail, "finalClean=", finalClean, finalDetail)
			return
		end

		threadpool:delay(interval, function()
			runIteration(iteration + 1)
		end)
	end

	print("[FGUI] long loop self test begin:", iterations, "interval=", interval)
	closeFairyGuiLongLoopObjects()
	threadpool:delay(interval, function()
		runIteration(1)
	end)
	return true
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

function FGUI_RunCacheSelfTest()
	FairyGuiManager:Close("Act37TestMvc", true, "cacheSelfTestReset")
	local ctrl = FGUI_ReopenAct37Sample()
	local handle = ctrl and ctrl:GetHandle() or nil
	if handle == nil then
		return false
	end

	local timerId = FairyGuiManager:Delay("Act37TestMvc", 30, function()
		print("[FGUI] cache self test stale timer fired")
	end)
	local hidden = FairyGuiManager:Close("Act37TestMvc", false, "cacheSelfTestHide")
	local hiddenStats = FairyGuiManager:GetDebugStats()
	local reopenedCtrl = FGUI_ReopenAct37Sample()
	local reopenedHandle = reopenedCtrl and reopenedCtrl:GetHandle() or nil
	local reopenedStats = FairyGuiManager:GetDebugStats()
	local destroyed = FairyGuiManager:Close("Act37TestMvc", true, "cacheSelfTestDestroy")
	local finalStats = FairyGuiManager:GetDebugStats()

	print(
		"[FGUI] cache self test detail:",
		"handle=", handle,
		"timer=", timerId,
		"hidden=", hidden,
		"hiddenUI=", hiddenStats.hiddenUI,
		"timerAfterHide=", hiddenStats.timer,
		"reopenedHandle=", reopenedHandle,
		"openAfterReopen=", reopenedStats.openUI,
		"hiddenAfterReopen=", reopenedStats.hiddenUI,
		"destroyed=", destroyed,
		"finalOpen=", finalStats.openUI,
		"finalTimer=", finalStats.timer)

	return hidden == true
		and hiddenStats.hiddenUI == 1
		and hiddenStats.timer == 0
		and reopenedHandle == handle
		and reopenedStats.hiddenUI == 0
		and destroyed == true
		and finalStats.openUI == 0
		and finalStats.timer == 0
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

function FGUI_Init()
	tryOpenFairyGuiSample()
end
