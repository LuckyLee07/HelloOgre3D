_G.FairyGuiManager = ClassList.FairyGuiManager:GetInst()
FairyGuiManager:RegisterUIRegistry(require("res.scripts.ui.UIRegistry"))

local function isEnvEnabled(name)
	local value = os.getenv and os.getenv(name) or nil
	return value == "1" or value == "true" or value == "TRUE" or value == "True"
end

local function isFairyGuiAutomationMode()
	return _G.HELLO_FGUI_AUTOMATION_MODE == true or isEnvEnabled("HELLO_FGUI_SKIP_SANDBOX_SCENE")
end

local function mergeFairyGuiParams(defaults, overrides)
	local result = {}
	if type(defaults) == "table" then
		for key, value in pairs(defaults) do
			result[key] = value
		end
	end
	if type(overrides) == "table" then
		for key, value in pairs(overrides) do
			result[key] = value
		end
	end
	return result
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
		local left = FairyGuiManager:DebugInjectKeyPressed(203, 0)
		local keyC = FairyGuiManager:DebugInjectKeyPressed(46, 67)
		local delete = FairyGuiManager:DebugInjectKeyPressed(211, 0)
		local backspace = FairyGuiManager:DebugInjectKeyPressed(14, 0)
		local submit = FairyGuiManager:DebugInjectKeyPressed(28, 0)
		local text = inputHandle ~= nil and FairyGuiManager:GetText(inputHandle) or ""
		local focusHandle = FairyGuiManager:GetFocusedHandle()
		print("[FGUI] text input self test result:", handle ~= nil, inputHandle, focused, keyA, keyB, left, keyC, delete, backspace, submit, text, focusHandle)
		FairyGuiManager:Close("TextInputProbe", true)
	end)
end

local function tryRunFairyGuiImeSelfTest()
	if not isEnvEnabled("HELLO_FGUI_IME_SELF_TEST") then
		return
	end

	threadpool:delay(9, function()
		local handle, inputHandle = FGUI_OpenTextInputProbe()
		local focused = inputHandle ~= nil and FairyGuiManager:Focus(inputHandle) or false
		local compose = FairyGuiManager:DebugInjectImeCompositionText("zhong")
		local composeText = inputHandle ~= nil and FairyGuiManager:GetText(inputHandle) or ""
		local composeDebug = FairyGuiManager:GetImeDebugString()
		local commit = FairyGuiManager:DebugInjectImeCommitText("\228\184\173")
		local text = inputHandle ~= nil and FairyGuiManager:GetText(inputHandle) or ""
		local commitDebug = FairyGuiManager:GetImeDebugString()
		local clear = FairyGuiManager:DebugClearImeComposition()
		local clearDebug = FairyGuiManager:GetImeDebugString()
		local composeOk = compose == true and composeText == "" and string.find(composeDebug, "active=1", 1, true) ~= nil
		local commitOk = commit == true and text == "\228\184\173" and string.find(commitDebug, "commits=1", 1, true) ~= nil
		local clearOk = clear == true and string.find(clearDebug, "active=0", 1, true) ~= nil
		print("[FGUI] ime self test result:", handle ~= nil, inputHandle, focused, composeOk, commitOk, clearOk, text, composeDebug, commitDebug, clearDebug)
		FairyGuiManager:Close("TextInputProbe", true)
	end)
end

local function tryRunFairyGuiComplexControlsSelfTest()
	if not isEnvEnabled("HELLO_FGUI_COMPLEX_CONTROLS_SELF_TEST") then
		return
	end

	threadpool:delay(9, function()
		print("[FGUI] complex controls self test result:", FGUI_RunComplexControlsSelfTest())
		FairyGuiManager:DumpEventStats()
		FairyGuiManager:DumpRenderStats()
	end)
end

local function tryRunFairyGuiVirtualListSelfTest()
	if not isEnvEnabled("HELLO_FGUI_VIRTUAL_LIST_SELF_TEST") then
		return
	end

	threadpool:delay(9, function()
		print("[FGUI] virtual list self test result:", FGUI_RunVirtualListSelfTest())
		FairyGuiManager:DumpHealth(true)
	end)
end

local function tryRunFairyGuiTreeSelfTest()
	if not isEnvEnabled("HELLO_FGUI_TREE_SELF_TEST") then
		return
	end

	threadpool:delay(9, function()
		print("[FGUI] tree self test result:", FGUI_RunTreeSelfTest())
		FairyGuiManager:DumpHealth(true)
	end)
end

local function tryRunFairyGuiSubModuleSelfTest()
	if not isEnvEnabled("HELLO_FGUI_SUBMODULE_SELF_TEST") then
		return
	end

	threadpool:delay(9, function()
		print("[FGUI] submodule self test result:", FGUI_RunSubModuleSelfTest())
		FairyGuiManager:DumpHealth(true)
	end)
end

local function tryRunFairyGuiDebugPanelSelfTest()
	if not isEnvEnabled("HELLO_FGUI_DEBUG_PANEL_SELF_TEST") then
		return
	end

	threadpool:delay(9, function()
		print("[FGUI] debug panel self test result:", FGUI_RunDebugPanelSelfTest())
		FairyGuiManager:DumpHealth(true)
	end)
end

local function tryRunFairyGuiAiDebugPanelSelfTest()
	if not isEnvEnabled("HELLO_FGUI_AI_DEBUG_PANEL_SELF_TEST") then
		return
	end

	threadpool:delay(9, function()
		print("[FGUI] ai debug panel self test result:", FGUI_RunAiDebugPanelSelfTest())
		FairyGuiManager:DumpHealth(true)
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

local function tryRunFairyGuiPopupRuleSelfTest()
	if not isEnvEnabled("HELLO_FGUI_POPUP_RULE_SELF_TEST") then
		return
	end

	threadpool:delay(13, function()
		print("[FGUI] popup rule self test result:", FGUI_RunPopupRuleSelfTest())
		FairyGuiManager:DumpStacks()
		FairyGuiManager:DumpHealth(true)
	end)
end

local function tryRunFairyGuiGuideMaskSelfTest()
	if not isEnvEnabled("HELLO_FGUI_GUIDE_MASK_SELF_TEST") then
		return
	end

	threadpool:delay(14, function()
		print("[FGUI] guide mask self test result:", FGUI_RunGuideMaskSelfTest())
		FairyGuiManager:DumpHealth(true)
	end)
end

local function tryRunFairyGuiEventPayloadSelfTest()
	if not isEnvEnabled("HELLO_FGUI_EVENT_PAYLOAD_SELF_TEST") then
		return
	end

	threadpool:delay(15, function()
		print("[FGUI] event payload self test result:", FGUI_RunEventPayloadSelfTest())
		FairyGuiManager:DumpEventStats()
		FairyGuiManager:DumpHealth(true)
	end)
end

local function tryRunFairyGuiScreenAdaptSelfTest()
	if not isEnvEnabled("HELLO_FGUI_SCREEN_ADAPT_SELF_TEST") then
		return
	end

	threadpool:delay(16, function()
		print("[FGUI] screen adapt self test result:", FGUI_RunScreenAdaptSelfTest())
		FairyGuiManager:DumpHealth(true)
	end)
end

local function tryRunFairyGuiBusinessFlowSelfTest()
	if not isEnvEnabled("HELLO_FGUI_BUSINESS_FLOW_SELF_TEST") then
		return
	end

	threadpool:delay(17, function()
		print("[FGUI] business flow self test result:", FGUI_RunBusinessFlowSelfTest())
		FairyGuiManager:DumpHealth(true)
	end)
end

local function tryRunFairyGuiBusinessBenchmarkSelfTest()
	if not isEnvEnabled("HELLO_FGUI_BUSINESS_BENCHMARK_SELF_TEST") then
		return
	end

	threadpool:delay(18, function()
		print("[FGUI] business benchmark self test result:", FGUI_RunBusinessBenchmarkSelfTest())
		FairyGuiManager:DumpPerfStats()
		FairyGuiManager:DumpHealth(true)
	end)
end

local function tryRunFairyGuiResourcePolicySelfTest()
	if not isEnvEnabled("HELLO_FGUI_RESOURCE_POLICY_SELF_TEST") then
		return
	end

	threadpool:delay(18, function()
		print("[FGUI] resource policy self test result:", FGUI_RunResourcePolicySelfTest())
		FairyGuiManager:DumpResourceRefs()
		FairyGuiManager:DumpHealth(true)
	end)
end

local function tryRunFairyGuiResourceFallbackSelfTest()
	if not isEnvEnabled("HELLO_FGUI_RESOURCE_FALLBACK_SELF_TEST") then
		return
	end

	threadpool:delay(19, function()
		print("[FGUI] resource fallback self test result:", FGUI_RunResourceFallbackSelfTest())
		FairyGuiManager:DumpResourceFallbacks()
		FairyGuiManager:DumpHealth(true)
	end)
end

local function tryRunFairyGuiTextInputPolicySelfTest()
	if not isEnvEnabled("HELLO_FGUI_TEXT_INPUT_POLICY_SELF_TEST") then
		return
	end

	threadpool:delay(20, function()
		print("[FGUI] text input policy self test result:", FGUI_RunTextInputPolicySelfTest())
		FairyGuiManager:DumpHealth(true)
	end)
end

local function tryRunFairyGuiPressureSelfTest()
	if not isEnvEnabled("HELLO_FGUI_PRESSURE_SELF_TEST") then
		return
	end

	threadpool:delay(6, function()
		local count = tonumber(os.getenv and os.getenv("HELLO_FGUI_PRESSURE_COUNT") or nil)
		local listCount = tonumber(os.getenv and os.getenv("HELLO_FGUI_PRESSURE_LIST_COUNT") or nil)
		local started = FGUI_RunPressureSelfTest({
			maxPopupCount = count,
			listItemCount = listCount,
		})
		if started ~= true then
			print("[FGUI] pressure self test result:", false)
		end
	end)
end

local function tryRunFairyGuiScreenAdaptDemo()
	if not isEnvEnabled("HELLO_FGUI_SCREEN_ADAPT_DEMO") then
		return
	end

	threadpool:delay(6, function()
		print("[FGUI] screen adapt demo result:", FGUI_RunScreenAdaptDemo())
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

local function tryRunFairyGuiDebugPanelDemo()
	if not isEnvEnabled("HELLO_FGUI_DEBUG_PANEL_DEMO") then
		return
	end

	threadpool:delay(5, function()
		local handle = FGUI_ShowDebugPanel({
			key = "__DebugPanelDemo",
			title = "FGUI Debug Demo",
			duration = 20,
		})
		print("[FGUI] debug panel demo open:", handle)
		FairyGuiManager:DumpHealth(true)
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

local function tryRunFairyGuiLayerBoundarySelfTest()
	if not isEnvEnabled("HELLO_FGUI_LAYER_BOUNDARY_SELF_TEST") then
		return
	end

	threadpool:delay(8, function()
		print("[FGUI] layer boundary self test result:", FGUI_RunLayerBoundarySelfTest())
		FairyGuiManager:DumpLayerRoots()
		FairyGuiManager:DumpStacks()
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
		local graphHandle = FGUI_OpenGraphMaskProbe()
		local nestedHandle = FGUI_OpenNestedGraphMaskProbe()
		print("[FGUI] mask probe open self test:", handle ~= nil, graphHandle ~= nil, nestedHandle ~= nil, handle, graphHandle, nestedHandle)
		FairyGuiManager:DumpRenderStats()
		threadpool:delay(1, function()
			local render = FairyGuiManager:GetRenderStats()
			local clipOk = render ~= nil and (render.cpuClipSourceTriangleCount or 0) > 0 and (render.stencilClipPolygonCount or 0) > 0
			local renderOk = render ~= nil and (render.drawCommandCount or 0) > 0 and (render.stencilCommandCount or 0) > 0 and clipOk == true
			local closed = FGUI_CloseMaskProbe()
			local graphClosed = FGUI_CloseGraphMaskProbe()
			local nestedClosed = FGUI_CloseNestedGraphMaskProbe()
			print("[FGUI] mask probe self test result:", handle ~= nil and graphHandle ~= nil and nestedHandle ~= nil and renderOk == true and closed == true and graphClosed == true and nestedClosed == true, "draw=", render and render.drawCommandCount, "stencil=", render and render.stencilCommandCount, "cpuClip=", render and tostring(render.cpuClipSourceTriangleCount) .. "/" .. tostring(render.cpuClipOutputTriangleCount) .. "/" .. tostring(render.cpuClipFragmentCount), "stencilClip=", render and tostring(render.stencilClipScopeCount) .. "/" .. tostring(render.stencilClipPolygonCount), "backend=", render and render.stencilBackend)
			print("[FGUI] mask probe close self test:", closed, graphClosed, nestedClosed)
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

	local startDelay = isEnvEnabled("HELLO_FGUI_SELF_TEST_ALL") and 15 or 6
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

	local function openSample()
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
		tryRunFairyGuiImeSelfTest()
		tryRunFairyGuiComplexControlsSelfTest()
		tryRunFairyGuiVirtualListSelfTest()
		tryRunFairyGuiTreeSelfTest()
		tryRunFairyGuiSubModuleSelfTest()
		tryRunFairyGuiDebugPanelSelfTest()
		tryRunFairyGuiAiDebugPanelSelfTest()
		tryRunFairyGuiLifecycleSelfTest()
		tryRunFairyGuiCacheSelfTest()
		tryRunFairyGuiCommonServiceSelfTest()
		tryRunFairyGuiPopupRuleSelfTest()
		tryRunFairyGuiGuideMaskSelfTest()
		tryRunFairyGuiEventPayloadSelfTest()
		tryRunFairyGuiScreenAdaptSelfTest()
		tryRunFairyGuiBusinessFlowSelfTest()
		tryRunFairyGuiBusinessBenchmarkSelfTest()
		tryRunFairyGuiResourcePolicySelfTest()
		tryRunFairyGuiResourceFallbackSelfTest()
		tryRunFairyGuiTextInputPolicySelfTest()
		tryRunFairyGuiPressureSelfTest()
		tryRunFairyGuiScreenAdaptDemo()
		tryRunFairyGuiCommonServiceDemo()
		tryRunFairyGuiDebugPanelDemo()
		tryRunFairyGuiCleanupSelfTest()
		tryRunFairyGuiLayerSelfTest()
		tryRunFairyGuiLayerCloseSelfTest()
		tryRunFairyGuiLayerBoundarySelfTest()
		tryRunFairyGuiAct38SelfTest()
		tryRunFairyGuiWheelSelfTest()
		tryRunFairyGuiMaskSelfTest()
		tryRunFairyGuiSelfTestSuite()
		tryRunFairyGuiLongLoopSelfTest()
	end

	if isFairyGuiAutomationMode() then
		openSample()
	else
		threadpool:delay(1, openSample)
	end
end

function FGUI_ReopenAct37Sample(param)
	return FairyGuiManager:Open("Act37TestMvc", mergeFairyGuiParams({
		roleId = 2001,
		source = "ManualReopen",
		showRed = true,
	}, param))
end

function FGUI_CloseAct37Sample()
	return FairyGuiManager:Close("Act37TestMvc")
end

function FGUI_DestroyAct37Sample()
	return FairyGuiManager:Destroy("Act37TestMvc")
end

function FGUI_OpenAct38Sample(param)
	FairyGuiManager:Close("Act38Test", true)
	local ctrl = FairyGuiManager:Open("Act38Test", mergeFairyGuiParams({
		selectedTab = 0,
		dateText = "Act38 MVC Sample",
	}, param))
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

function FGUI_RunComplexControlsSelfTest()
	FairyGuiManager:Close("Act37TestMvc", true, "complexControlsReset")
	FairyGuiManager:Close("Act38Test", true, "complexControlsReset")
	local ctrl = FGUI_OpenAct38Sample({
		scene = "ComplexControls",
		group = "ComplexControls",
		dateText = "Complex Controls Self Test",
	})
	local complexOk = ctrl ~= nil and ctrl.RunComplexControlSelfTest ~= nil and ctrl:RunComplexControlSelfTest() or false
	FairyGuiManager:Close("Act38Test", true, "complexControlsCleanup")
	FairyGuiManager:Close("Act37TestMvc", true, "complexControlsCleanup")
	local health = FairyGuiManager:GetHealthStats()
	local finalClean = health.openUI == 0 and health.binding == 0 and health.transitionCallback == 0
	print("[FGUI] complex controls self test detail:", complexOk, finalClean, "openUI=", health.openUI, "binding=", health.binding, "transition=", health.transitionCallback)
	return complexOk == true and finalClean == true
end

function FGUI_RunVirtualListSelfTest()
	FairyGuiManager:Close("Act37TestMvc", true, "virtualListReset")
	FairyGuiManager:Close("Act38Test", true, "virtualListReset")
	local ctrl = FGUI_OpenAct38Sample({
		scene = "VirtualList",
		group = "VirtualList",
		dateText = "Virtual List Self Test",
	})
	local listOk = ctrl ~= nil and ctrl.RunVirtualListSelfTest ~= nil and ctrl:RunVirtualListSelfTest() or false
	local stats = ctrl ~= nil and ctrl:GetListDebugStats("m2_dayTaskList") or {}
	FairyGuiManager:Close("Act38Test", true, "virtualListCleanup")
	FairyGuiManager:Close("Act37TestMvc", true, "virtualListCleanup")
	local health = FairyGuiManager:GetHealthStats()
	local finalClean = health.openUI == 0 and health.binding == 0 and health.transitionCallback == 0
	print("[FGUI] virtual list self test detail:", listOk, finalClean, "render=", stats.renderCount, "items=", stats.itemHandleCount, "reuse=", stats.reuseCount, "openUI=", health.openUI, "binding=", health.binding, "transition=", health.transitionCallback)
	return listOk == true and finalClean == true
end

function FGUI_RunTreeSelfTest()
	FairyGuiManager:Close("Act37TestMvc", true, "treeReset")
	FairyGuiManager:Close("Act38Test", true, "treeReset")
	local ctrl = FGUI_OpenAct38Sample({
		scene = "Tree",
		group = "Tree",
		dateText = "Tree Self Test",
	})
	local treeOk = ctrl ~= nil and ctrl.RunTreeSelfTest ~= nil and ctrl:RunTreeSelfTest() or false
	FairyGuiManager:Close("Act38Test", true, "treeCleanup")
	FairyGuiManager:Close("Act37TestMvc", true, "treeCleanup")
	local health = FairyGuiManager:GetHealthStats()
	local finalClean = health.openUI == 0 and health.binding == 0 and health.transitionCallback == 0
	print("[FGUI] tree self test detail:", treeOk, finalClean, "openUI=", health.openUI, "binding=", health.binding, "transition=", health.transitionCallback)
	return treeOk == true and finalClean == true
end

function FGUI_RunSubModuleSelfTest()
	FairyGuiManager:Close("Act37TestMvc", true, "subModuleReset")
	FairyGuiManager:Close("Act38Test", true, "subModuleReset")
	FairyGuiManager:Close("__SubModuleChildAct37", true, "subModuleReset")
	local parentCtrl = FGUI_OpenAct38Sample({
		scene = "SubModule",
		group = "SubModule",
		dateText = "SubModule Self Test",
	})
	if parentCtrl == nil then
		return false
	end

	local childCtrl = parentCtrl:OpenChild("Act37TestMvc", {
		key = "__SubModuleChildAct37",
		x = 18,
		y = 18,
		stackMode = "None",
		closeOnSceneChange = false,
	})
	local childInfo = FairyGuiManager:GetObjectInfo("__SubModuleChildAct37")
	local childKeys = parentCtrl:GetChildUIKeys()
	local childLinked = childCtrl ~= nil
		and childInfo ~= nil
		and childInfo.parentKey == "Act38Test"
		and #childKeys == 1
		and childKeys[1] == "__SubModuleChildAct37"
	local timerId = childInfo ~= nil and FairyGuiManager:Delay("__SubModuleChildAct37", 60, function()
		print("[FGUI] stale submodule timer fired")
	end) or nil
	local bindingId = childInfo ~= nil and FairyGuiManager:AddClick(childInfo.handle, "", function(evt)
		print("[FGUI] submodule root click:", evt.rootHandle)
	end) or nil
	local healthBefore = FairyGuiManager:GetHealthStats()
	local parentClosed = FairyGuiManager:Close("Act38Test", true, "subModuleParentClose")
	local childClosed = FairyGuiManager:GetObjectInfo("__SubModuleChildAct37") == nil
	local parentClosedClean = FairyGuiManager:GetObjectInfo("Act38Test") == nil
	local childKeysAfter = FairyGuiManager:GetChildUIKeys("Act38Test")
	local healthAfter = FairyGuiManager:GetHealthStats()
	local clean = healthAfter.openUI == 0
		and healthAfter.binding == 0
		and healthAfter.timer == 0
		and (healthAfter.childUI or 0) == 0
		and healthAfter.view == 0
		and healthAfter.controller == 0

	print(
		"[FGUI] submodule self test detail:",
		"linked=", childLinked,
		"timer/binding=", timerId, bindingId,
		"before=", healthBefore.openUI, healthBefore.binding, healthBefore.timer, healthBefore.childUI,
		"closed=", parentClosed, childClosed, parentClosedClean,
		"after=", healthAfter.openUI, healthAfter.binding, healthAfter.timer, healthAfter.childUI, healthAfter.view, healthAfter.controller,
		"childKeysAfter=", #childKeysAfter)
	return childLinked == true
		and timerId ~= nil
		and bindingId ~= nil
		and parentClosed == true
		and childClosed == true
		and parentClosedClean == true
		and #childKeysAfter == 0
		and clean == true
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

function FGUI_OpenGraphMaskProbe()
	return FairyGuiManager:OpenMaskProbe({
		key = "GraphMaskProbe",
		layer = "Top",
		group = "Sample",
		scene = "Default",
		popupMode = "stack",
		graphMask = true,
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

function FGUI_OpenNestedGraphMaskProbe()
	return FairyGuiManager:OpenMaskProbe({
		key = "NestedGraphMaskProbe",
		layer = "Top",
		group = "Sample",
		scene = "Default",
		popupMode = "stack",
		graphMask = true,
		nestedMask = true,
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

function FGUI_CloseGraphMaskProbe()
	return FairyGuiManager:Close("GraphMaskProbe", true)
end

function FGUI_CloseNestedGraphMaskProbe()
	return FairyGuiManager:Close("NestedGraphMaskProbe", true)
end

function FGUI_OpenTextInputProbe(param)
	return FairyGuiManager:OpenTextInputProbe(mergeFairyGuiParams({
		key = "TextInputProbe",
		layer = "Top",
		group = "Sample",
		scene = "Default",
		popupMode = "stack",
	}, param))
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

function FGUI_DumpResourceFallbacks()
	return FairyGuiManager:DumpResourceFallbacks()
end

function FGUI_Dump()
	return FairyGuiManager:Dump()
end

function FGUI_ShowDebugPanel(param)
	return FairyGuiManager:ShowDebugPanel(param)
end

function FGUI_DumpDebugTarget(target)
	return FairyGuiManager:DumpDebugTarget(target)
end

function FGUI_CloseDebugTarget(target, forceDestroy)
	return FairyGuiManager:CloseDebugTarget(target, forceDestroy)
end

function FGUI_HideDebugPanel(key)
	return FairyGuiManager:HideDebugPanel(key)
end

function FGUI_RefreshDebugPanel(key)
	return FairyGuiManager:RefreshDebugPanel(key)
end

function FGUI_ShowAiDebugPanel(param)
	return FairyGuiManager:ShowAiDebugPanel(param)
end

function FGUI_HideAiDebugPanel(key)
	return FairyGuiManager:HideAiDebugPanel(key)
end

function FGUI_RefreshAiDebugPanel(key)
	return FairyGuiManager:RefreshAiDebugPanel(key)
end

function FGUI_RunDebugPanelSelfTest()
	FairyGuiManager:Close("Act37TestMvc", true, "debugPanelReset")
	FairyGuiManager:Close("Act38Test", true, "debugPanelReset")
	FairyGuiManager:HideDebugPanel("__DebugPanelSelfTest")
	local ctrl = FGUI_OpenAct38Sample({
		scene = "DebugPanelSelfTest",
		group = "DebugPanelSelfTest",
	})
	if ctrl == nil then
		return false
	end

	local wheelOk = FairyGuiManager:DebugInjectMouseWheel(745, 485, -120)
	local debugHandle = FairyGuiManager:ShowDebugPanel({
		key = "__DebugPanelSelfTest",
		scene = "DebugPanelSelfTest",
		title = "Debug Panel Self Test",
		autoRefresh = false,
		lineCount = 18,
		debugTarget = "Act38Test",
	})
	local refreshOk = debugHandle ~= nil and FairyGuiManager:RefreshDebugPanel("__DebugPanelSelfTest") == true
	local lines = FairyGuiManager:BuildDebugPanelLines({ lineCount = 24 })
	local snapshot = FairyGuiManager:GetDebugPanelSnapshot({ maxPackages = 5, maxBindings = 6 })
	local targetObjectInfo = FairyGuiManager:GetObjectInfo("Act38Test")
	local packageKey = targetObjectInfo ~= nil and (targetObjectInfo.packageName or targetObjectInfo.packagePath) or nil
	local hasTop = snapshot ~= nil and snapshot.topUI ~= nil
	local hasLayer = snapshot ~= nil and type(snapshot.layerSummary) == "table" and #snapshot.layerSummary > 0
	local hasPackage = snapshot ~= nil and type(snapshot.packageSummary) == "table" and #snapshot.packageSummary > 0
	local hasEvent = snapshot ~= nil and snapshot.eventStats ~= nil and (snapshot.eventStats.total or 0) > 0
	local hasBindings = snapshot ~= nil and type(snapshot.bindingSummary) == "table" and #snapshot.bindingSummary > 0
	local hasFallbackSnapshot = snapshot ~= nil and type(snapshot.resourceFallbacks) == "table"
	local hasLines = type(lines) == "table" and #lines >= 12
	local hasRenderLine = false
	local hasDrawLine = false
	local hasStencilLine = false
	local hasOpenLine = false
	local hasFallbackLine = false
	for _, line in ipairs(lines or {}) do
		if string.find(line, "Render", 1, true) ~= nil then
			hasRenderLine = true
		end
		if string.find(line, "Draw ", 1, true) ~= nil then
			hasDrawLine = true
		end
		if string.find(line, "Stencil ", 1, true) ~= nil then
			hasStencilLine = true
		end
		if string.find(line, "Open ", 1, true) ~= nil then
			hasOpenLine = true
		end
		if string.find(line, "Fallbacks ", 1, true) ~= nil then
			hasFallbackLine = true
		end
	end
	local hasRenderDetail = snapshot ~= nil
		and snapshot.render ~= nil
		and snapshot.render.drawCommandCount ~= nil
		and snapshot.render.materialSwitchCount ~= nil
		and snapshot.render.clippedCommandCount ~= nil
		and snapshot.render.stencilCommandCount ~= nil
		and snapshot.render.cpuClipSourceTriangleCount ~= nil
		and snapshot.render.stencilClipPolygonCount ~= nil
		and snapshot.render.stencilBackend ~= nil
	local dumpTargetOk = FairyGuiManager:DumpDebugTarget("Act38Test") == true
	local dumpPackageOk = packageKey ~= nil and FairyGuiManager:DumpPackageRef(packageKey) == true
	local closeTargetOk = FairyGuiManager:CloseDebugTarget("Act38Test", true) == true
	local targetClosed = FairyGuiManager:GetObjectInfo("Act38Test") == nil

	FairyGuiManager:HideDebugPanel("__DebugPanelSelfTest")
	FairyGuiManager:Close("Act38Test", true, "debugPanelCleanup")
	local health = FairyGuiManager:GetHealthStats()
	local finalClean = health.openUI == 0 and health.binding == 0 and health.timer == 0
	print(
		"[FGUI] debug panel self test detail:",
		"debug=", debugHandle,
		"refresh=", refreshOk,
		"wheel=", wheelOk,
		"lines=", hasLines, #lines,
		"top=", hasTop,
		"layer=", hasLayer,
		"package=", hasPackage,
		"event=", hasEvent,
		"binding=", hasBindings,
		"fallbackSnapshot=", hasFallbackSnapshot,
		"fallbackLine=", hasFallbackLine,
		"renderLine=", hasRenderLine,
		"drawLine=", hasDrawLine,
		"stencilLine=", hasStencilLine,
		"renderDetail=", hasRenderDetail,
		"openLine=", hasOpenLine,
		"dumpTarget=", dumpTargetOk,
		"dumpPackage=", dumpPackageOk,
		"closeTarget=", closeTargetOk,
		"targetClosed=", targetClosed,
		"clean=", finalClean,
		"openUI=", health.openUI,
		"binding=", health.binding,
		"timer=", health.timer)
	return debugHandle ~= nil
		and refreshOk == true
		and wheelOk == true
		and hasLines == true
		and hasTop == true
		and hasLayer == true
		and hasPackage == true
		and hasEvent == true
		and hasBindings == true
		and hasFallbackSnapshot == true
		and hasFallbackLine == true
		and hasRenderLine == true
		and hasDrawLine == true
		and hasStencilLine == true
		and hasRenderDetail == true
		and hasOpenLine == true
		and dumpTargetOk == true
		and dumpPackageOk == true
		and closeTargetOk == true
		and targetClosed == true
		and finalClean == true
end

function FGUI_RunAiDebugPanelSelfTest()
	FairyGuiManager:HideAiDebugPanel("__AiDebugPanelSelfTest")
	local debugHandle = FairyGuiManager:ShowAiDebugPanel({
		key = "__AiDebugPanelSelfTest",
		scene = "AiDebugPanelSelfTest",
		title = "AI Debug Panel Self Test",
		autoRefresh = false,
		lineCount = 10,
		maxAgents = 8,
	})
	local refreshOk = debugHandle ~= nil and FairyGuiManager:RefreshAiDebugPanel("__AiDebugPanelSelfTest") == true
	local snapshot = FairyGuiManager:GetAiDebugSnapshot({ maxAgents = 8 })
	local lines = FairyGuiManager:BuildAiDebugPanelLines({ lineCount = 10, maxAgents = 8 })
	local hasSnapshot = snapshot ~= nil and type(snapshot.lines) == "table" and #snapshot.lines >= 1
	local hasHeader = false
	for _, line in ipairs(lines or {}) do
		if string.find(line, "AI agents=", 1, true) ~= nil then
			hasHeader = true
			break
		end
	end
	FairyGuiManager:HideAiDebugPanel("__AiDebugPanelSelfTest")
	local closed = FairyGuiManager:GetObjectInfo("__AiDebugPanelSelfTest") == nil
	print(
		"[FGUI] ai debug panel self test detail:",
		"debug=", debugHandle,
		"refresh=", refreshOk,
		"snapshot=", hasSnapshot,
		"lines=", type(lines) == "table" and #lines or 0,
		"header=", hasHeader,
		"closed=", closed,
		"agents=", snapshot and snapshot.agentCount or nil,
		"soldiers=", snapshot and snapshot.soldierCount or nil)
	return debugHandle ~= nil and refreshOk == true and hasSnapshot == true and hasHeader == true and closed == true
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
			highlightRect = { x = 420, y = 240, width = 260, height = 140 },
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
	local serviceMeta = serviceStats.__meta or {}
	local serviceStatsOk = (serviceMeta.serviceOpenTotal or 0) >= 6
		and (serviceMeta.serviceKindCount or 0) >= 6
		and (serviceMeta.createdTotal or 0) >= 6
		and (serviceMeta.peakOpen or 0) >= 6
		and (serviceMeta.toastQueuedTotal or 0) >= 1
		and (serviceMeta.toastDedupeIgnoredTotal or 0) >= 1
		and (serviceMeta.loadingPeakRefTotal or 0) >= 2
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
		and serviceStatsOk == true

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
	local cleanupMeta = cleanupStats.__meta or {}
	local cleaned = cleanupStats.Toast == nil
		and cleanupStats.Tip == nil
		and cleanupStats.Loading == nil
		and cleanupStats.GuideMask == nil
		and cleanupStats.MessageBox == nil
		and cleanupStats.PopupMenu == nil
		and cleanupMeta.loadingRefTotal == 0
		and cleanupMeta.toastQueue == 0
		and cleanupMeta.serviceOpenTotal == 0
		and (cleanupMeta.closedTotal or 0) >= 6

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
		"serviceStatsOk=", serviceStatsOk,
		"service=", tostring(serviceMeta.serviceOpenTotal or 0) .. "/" .. tostring(serviceMeta.serviceKindCount or 0) .. "/" .. tostring(serviceMeta.peakOpen or 0),
		"serviceTotals=", tostring(cleanupMeta.createdTotal or 0) .. "/" .. tostring(cleanupMeta.closedTotal or 0) .. "/" .. tostring(cleanupMeta.failedTotal or 0),
		"opened=", opened,
		"cleaned=", cleaned)
	return opened == true and loadingHideOnce == true and loadingStillOpen == true and loadingHideTwice == true and loadingClosed == true and cleaned == true
end

function FGUI_RunPopupRuleSelfTest()
	FairyGuiManager:Close("PopupRuleHigh", true, "popupRuleSelfTestReset")
	FairyGuiManager:Close("PopupRuleLow", true, "popupRuleSelfTestReset")

	local highHandle = FairyGuiManager:ShowMessageBox("Priority High", "High priority popup should stay on top.", { "OK" }, nil, {
		key = "PopupRuleHigh",
		popupGroup = "PopupRule",
		popupMode = "stack",
		priority = 5,
		closeOnMaskClick = true,
	})
	local lowHandle = FairyGuiManager:ShowMessageBox("Priority Low", "Low priority popup opens later but stays below.", { "OK" }, nil, {
		key = "PopupRuleLow",
		popupGroup = "PopupRule",
		popupMode = "stack",
		priority = 0,
		closeOnMaskClick = true,
	})
	local topPopup = FairyGuiManager:GetTopStackObject(FairyGuiManager.popupStack)
	local priorityOk = topPopup ~= nil and topPopup.key == "PopupRuleHigh"
	local maskClick = FairyGuiManager:DebugInjectClick(20, 20, 0)
	local highClosed = FairyGuiManager:GetObjectInfo("PopupRuleHigh") == nil
	local lowAlive = FairyGuiManager:GetObjectInfo("PopupRuleLow") ~= nil
	FairyGuiManager:Close("PopupRuleLow", true, "popupRuleSelfTestCleanup")
	print("[FGUI] popup rule self test detail:", highHandle, lowHandle, "top=", topPopup and topPopup.key or nil, "maskClick=", maskClick, "highClosed=", highClosed, "lowAlive=", lowAlive)
	return highHandle ~= nil and lowHandle ~= nil and priorityOk == true and maskClick == true and highClosed == true and lowAlive == true
end

function FGUI_RunGuideMaskSelfTest()
	FairyGuiManager:HideGuideMask("guideMaskSelfTestReset")
	local handle = FairyGuiManager:ShowGuideMask({
		text = "GuideMask self test",
		textX = 80,
		textY = 120,
		highlightRect = { x = 160, y = 160, width = 240, height = 150 },
		closeOnMaskClick = true,
	})
	local objectInfo = FairyGuiManager:GetObjectInfo("__GuideMask")
	local maskCount = objectInfo ~= nil and type(objectInfo.guideMaskHandles) == "table" and #objectInfo.guideMaskHandles or 0
	local insideClick = FairyGuiManager:DebugInjectClick(220, 220, 0)
	local stillOpen = FairyGuiManager:GetObjectInfo("__GuideMask") ~= nil
	local outsideClick = FairyGuiManager:DebugInjectClick(20, 20, 0)
	local closed = FairyGuiManager:GetObjectInfo("__GuideMask") == nil
	FairyGuiManager:HideGuideMask("guideMaskSelfTestCleanup")
	print("[FGUI] guide mask self test detail:", handle, "maskCount=", maskCount, "insideClick=", insideClick, "stillOpen=", stillOpen, "outsideClick=", outsideClick, "closed=", closed)
	return handle ~= nil and maskCount > 0 and stillOpen == true and outsideClick == true and closed == true
end

function FGUI_RunEventPayloadSelfTest()
	FairyGuiManager:Close("__EventPayloadProbe", true, "eventPayloadSelfTestReset")
	local objectInfo = FairyGuiManager:OpenServiceContainer("__EventPayloadProbe", {
		layer = "Top",
		fullScreen = true,
		modal = false,
		touchable = true,
		stackMode = "None",
		priority = 20,
		serviceType = "EventPayloadProbe",
	})
	if objectInfo == nil then
		return false
	end

	local hitHandle = FairyGuiManager:CreateModalMask(objectInfo, {
		modal = true,
		modalAlpha = 0.01,
	})
	if hitHandle == nil then
		hitHandle = objectInfo.handle
	end

	local payload = nil
	local beginPayload = nil
	local endPayload = nil
	local beginBindingId = FairyGuiManager:AddEvent(hitHandle, "", "TouchBegin", function(event)
		beginPayload = event
	end)
	local bindingId = FairyGuiManager:AddEvent(hitHandle, "", "DragMove", function(event)
		payload = event
	end)
	local endBindingId = FairyGuiManager:AddEvent(hitHandle, "", "TouchEnd", function(event)
		endPayload = event
	end)
	local down = FairyGuiManager:DebugInjectMouseDown(64, 64, 0)
	local move = FairyGuiManager:DebugInjectMouseMove(96, 112)
	local up = FairyGuiManager:DebugInjectMouseUp(96, 112, 0)
	FairyGuiManager:Close("__EventPayloadProbe", true, "eventPayloadSelfTestCleanup")
	payload = payload or endPayload or beginPayload
	local payloadOk = payload ~= nil
		and payload.rootHandle == hitHandle
		and payload.senderHandle ~= nil
		and payload.x ~= nil
		and payload.y ~= nil
		and payload.button ~= nil
		and payload.touchId ~= nil
	print("[FGUI] event payload self test detail:", "hit=", hitHandle, "binding=", beginBindingId, bindingId, endBindingId, "inject=", down, move, up, "payloadOk=", payloadOk, "begin=", beginPayload ~= nil, "end=", endPayload ~= nil, "delta=", payload and payload.dragDeltaX or nil, payload and payload.dragDeltaY or nil)
	return beginBindingId ~= nil and bindingId ~= nil and endBindingId ~= nil and down == true and move == true and up == true and payloadOk == true and beginPayload ~= nil and endPayload ~= nil
end

function FGUI_RunScreenAdaptSelfTest()
	FairyGuiManager:ClearToastQueue()
	FairyGuiManager:Close("__Toast", true, "screenAdaptSelfTestReset")
	FairyGuiManager:Close("__GuideMask", true, "screenAdaptSelfTestReset")
	FairyGuiManager:Close("ScreenAdaptMessage", true, "screenAdaptSelfTestReset")
	FairyGuiManager:Close("ScreenAdaptPopup", true, "screenAdaptSelfTestReset")

	local screenWidth = FairyGuiManager:GetScreenWidth()
	local screenHeight = FairyGuiManager:GetScreenHeight()
	local messageHandle = FairyGuiManager:ShowMessageBox("Screen Adapt", "Centered popup should stay centered after layout refresh.", { "OK" }, nil, {
		key = "ScreenAdaptMessage",
		center = true,
		closeOnMaskClick = true,
	})
	local messageInfo = FairyGuiManager:GetObjectInfo("ScreenAdaptMessage")
	local messageRect = messageInfo and messageInfo.lastLayoutRect or nil
	local messageCenterOk = messageRect ~= nil
		and math.abs(messageRect.x - math.max((screenWidth - messageRect.width) * 0.5, 0)) <= 2
		and math.abs(messageRect.y - math.max((screenHeight - messageRect.height) * 0.5, 0)) <= 2

	local popupHandle = FairyGuiManager:ShowPopupMenu({ "Alpha", "Beta", "Gamma" }, screenWidth + 40, screenHeight + 40, nil, {
		key = "ScreenAdaptPopup",
		fitInScreen = true,
	})
	local popupInfo = FairyGuiManager:GetObjectInfo("ScreenAdaptPopup")
	local popupRect = popupInfo and popupInfo.lastLayoutRect or nil
	local popupClampOk = popupRect ~= nil
		and popupRect.x >= 0
		and popupRect.y >= 0
		and popupRect.x + popupRect.width <= screenWidth + 1
		and popupRect.y + popupRect.height <= screenHeight + 1

	local guideHandle = FairyGuiManager:ShowGuideMask({
		text = "Screen adapt guide",
		highlightRect = { x = 640, y = 360, width = 180, height = 120 },
		designWidth = 1280,
		designHeight = 720,
		scaleMode = "stretch",
		closeOnMaskClick = true,
	})
	local guideInfo = FairyGuiManager:GetObjectInfo("__GuideMask")
	local guideRect = guideInfo and guideInfo.guideMaskRect or nil
	local guideOk = guideRect ~= nil
		and guideRect.x >= 0
		and guideRect.y >= 0
		and guideRect.x + guideRect.width <= screenWidth + 1
		and guideRect.y + guideRect.height <= screenHeight + 1

	local toastHandle = FairyGuiManager:ShowToast("Screen adapt toast", 0, { bottom = 100 })
	local toastInfo = FairyGuiManager:GetObjectInfo("__Toast")
	local toastRect = toastInfo and toastInfo.toastLayoutRect or nil
	local toastOk = toastRect ~= nil
		and toastRect.x >= 0
		and toastRect.x + toastRect.width <= screenWidth + 1
		and toastRect.y >= 0
		and toastRect.y + toastRect.height <= screenHeight + 1

	print(
		"[FGUI] screen adapt self test detail:",
		"screen=", screenWidth, screenHeight,
		"message=", messageHandle, messageCenterOk,
		"popup=", popupHandle, popupClampOk,
		"guide=", guideHandle, guideOk,
		"toast=", toastHandle, toastOk)

	FairyGuiManager:Close("__Toast", true, "screenAdaptSelfTestCleanup")
	FairyGuiManager:Close("__GuideMask", true, "screenAdaptSelfTestCleanup")
	FairyGuiManager:Close("ScreenAdaptMessage", true, "screenAdaptSelfTestCleanup")
	FairyGuiManager:Close("ScreenAdaptPopup", true, "screenAdaptSelfTestCleanup")
	return messageCenterOk == true and popupClampOk == true and guideOk == true and toastOk == true
end

function FGUI_RunScreenAdaptDemo()
	if threadpool == nil or threadpool.delay == nil then
		print("[FGUI] screen adapt demo failed: threadpool unavailable")
		return false
	end

	FairyGuiManager:ClearToastQueue()
	FairyGuiManager:Close("__Toast", true, "screenAdaptDemoReset")
	FairyGuiManager:Close("__GuideMask", true, "screenAdaptDemoReset")
	FairyGuiManager:Close("ScreenAdaptMessage", true, "screenAdaptDemoReset")
	FairyGuiManager:Close("ScreenAdaptPopup", true, "screenAdaptDemoReset")

	local screenWidth = FairyGuiManager:GetScreenWidth()
	local screenHeight = FairyGuiManager:GetScreenHeight()
	FairyGuiManager:ShowMessageBox("Screen Adapt Demo", "Centered popup, clamped menu, toast area and scaled guide mask are visible together.", { "OK" }, nil, {
		key = "ScreenAdaptMessage",
		center = true,
		closeOnMaskClick = true,
		priority = 4,
	})
	FairyGuiManager:ShowPopupMenu({ "Bottom Right", "Clamped", "Menu" }, screenWidth - 24, screenHeight - 24, function(index, item)
		print("[FGUI] screen adapt demo popup select:", index, item)
	end, {
		key = "ScreenAdaptPopup",
		fitInScreen = true,
		priority = 5,
	})
	FairyGuiManager:ShowGuideMask({
		text = "Guide rect uses 1280x720 design coordinates",
		textX = 80,
		textY = 160,
		highlightRect = { x = 470, y = 240, width = 260, height = 150 },
		designWidth = 1280,
		designHeight = 720,
		scaleMode = "stretch",
		closeOnMaskClick = true,
	})
	FairyGuiManager:ShowToast("Toast keeps its screen area on resize", 6, { bottom = 110, dedupeKey = "ScreenAdaptDemoToast" })
	threadpool:delay(8, function()
		FairyGuiManager:Close("__Toast", true, "screenAdaptDemoCleanup")
		FairyGuiManager:Close("__GuideMask", true, "screenAdaptDemoCleanup")
		FairyGuiManager:Close("ScreenAdaptMessage", true, "screenAdaptDemoCleanup")
		FairyGuiManager:Close("ScreenAdaptPopup", true, "screenAdaptDemoCleanup")
		FairyGuiManager:DumpHealth(true)
		print("[FGUI] screen adapt demo end")
	end)
	return true
end

local closeFairyGuiBusinessFlowObjects = nil

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
	schedule(0.6, "ComplexControls", function()
		return FGUI_RunComplexControlsSelfTest()
	end)
	schedule(0.6, "VirtualList", function()
		return FGUI_RunVirtualListSelfTest()
	end)
	schedule(0.6, "Tree", function()
		return FGUI_RunTreeSelfTest()
	end)
	schedule(0.6, "SubModule", function()
		return FGUI_RunSubModuleSelfTest()
	end)
	schedule(0.6, "DebugPanel", function()
		return FGUI_RunDebugPanelSelfTest()
	end)
	schedule(0.6, "AiDebugPanel", function()
		return FGUI_RunAiDebugPanelSelfTest()
	end)
	schedule(0.6, "MouseWheel", function()
		local ctrl = FairyGuiManager:GetController("Act38Test") or FGUI_OpenAct38Sample()
		local scrollDown = FairyGuiManager:DebugInjectMouseWheel(745, 485, -120)
		local scrollUp = FairyGuiManager:DebugInjectMouseWheel(745, 485, 120)
		return ctrl ~= nil and scrollDown and scrollUp, tostring(ctrl ~= nil) .. "," .. tostring(scrollDown) .. "," .. tostring(scrollUp)
	end)
	schedule(0.6, "EventPayload", function()
		FairyGuiManager:Close("Act38Test", true, "selfTestEventPayloadReset")
		FairyGuiManager:Close("Act37TestMvc", true, "selfTestEventPayloadReset")
		return FGUI_RunEventPayloadSelfTest()
	end)
	schedule(0.6, "TextInput", function()
		local handle, inputHandle = FGUI_OpenTextInputProbe()
		local focused = inputHandle ~= nil and FairyGuiManager:Focus(inputHandle) or false
		local keyA = FairyGuiManager:DebugInjectKeyPressed(30, 65)
		local keyB = FairyGuiManager:DebugInjectKeyPressed(48, 66)
		local left = FairyGuiManager:DebugInjectKeyPressed(203, 0)
		local keyC = FairyGuiManager:DebugInjectKeyPressed(46, 67)
		local delete = FairyGuiManager:DebugInjectKeyPressed(211, 0)
		local backspace = FairyGuiManager:DebugInjectKeyPressed(14, 0)
		local submit = FairyGuiManager:DebugInjectKeyPressed(28, 0)
		local text = inputHandle ~= nil and FairyGuiManager:GetText(inputHandle) or ""
		FairyGuiManager:Close("TextInputProbe", true)
		return handle ~= nil and focused and keyA and keyB and left and keyC and delete and backspace and submit and text == "A", text
	end)
	schedule(0.6, "ImeCommit", function()
		local handle, inputHandle = FGUI_OpenTextInputProbe()
		local focused = inputHandle ~= nil and FairyGuiManager:Focus(inputHandle) or false
		local compose = FairyGuiManager:DebugInjectImeCompositionText("zhong")
		local composeText = inputHandle ~= nil and FairyGuiManager:GetText(inputHandle) or ""
		local composeDebug = FairyGuiManager:GetImeDebugString()
		local commit = FairyGuiManager:DebugInjectImeCommitText("\228\184\173")
		local text = inputHandle ~= nil and FairyGuiManager:GetText(inputHandle) or ""
		local commitDebug = FairyGuiManager:GetImeDebugString()
		local clear = FairyGuiManager:DebugClearImeComposition()
		FairyGuiManager:Close("TextInputProbe", true)
		local composeOk = compose == true and composeText == "" and string.find(composeDebug, "active=1", 1, true) ~= nil
		local commitOk = commit == true and text == "\228\184\173" and string.find(commitDebug, "commits=", 1, true) ~= nil
		return handle ~= nil and focused and composeOk and commitOk and clear == true, text
	end)
	schedule(0.6, "LayerClose", function()
		return FGUI_RunLayerCloseSelfTest()
	end)
	schedule(0.6, "LayerBoundary", function()
		return FGUI_RunLayerBoundarySelfTest()
	end)
	schedule(0.6, "MaskProbe", function()
		local handle = FGUI_OpenMaskProbe()
		local closed = FGUI_CloseMaskProbe()
		return handle ~= nil and closed == true, handle
	end)
	schedule(0.6, "GraphMaskProbe", function()
		local handle = FGUI_OpenGraphMaskProbe()
		local closed = FGUI_CloseGraphMaskProbe()
		return handle ~= nil and closed == true, handle
	end)
	schedule(0.6, "NestedGraphMaskProbe", function()
		local handle = FGUI_OpenNestedGraphMaskProbe()
		local closed = FGUI_CloseNestedGraphMaskProbe()
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
	schedule(0.6, "PopupRules", function()
		return FGUI_RunPopupRuleSelfTest()
	end)
	schedule(0.6, "GuideMask", function()
		return FGUI_RunGuideMaskSelfTest()
	end)
	schedule(0.6, "ScreenAdapt", function()
		return FGUI_RunScreenAdaptSelfTest()
	end)
	schedule(0.6, "BusinessFlow", function()
		return FGUI_RunBusinessFlowSelfTest()
	end)
	schedule(0.6, "BusinessBenchmark", function()
		return FGUI_RunBusinessBenchmarkSelfTest({ iterations = 2 })
	end)
	schedule(0.6, "ResourcePolicy", function()
		return FGUI_RunResourcePolicySelfTest()
	end)
	schedule(0.6, "ResourceFallback", function()
		return FGUI_RunResourceFallbackSelfTest()
	end)
	schedule(0.6, "TextInputPolicy", function()
		return FGUI_RunTextInputPolicySelfTest()
	end)
	schedule(0.6, "Cleanup", function()
		FairyGuiManager:Close("Act37TestMvc", true)
		FairyGuiManager:Close("Act38Test", true)
		FairyGuiManager:CloseGroup("LayerProbe", true)
		FairyGuiManager:Close("MaskProbe", true)
		FairyGuiManager:Close("GraphMaskProbe", true)
		FairyGuiManager:Close("NestedGraphMaskProbe", true)
		FairyGuiManager:Close("TextInputProbe", true)
		FairyGuiManager:Close("BusinessFlowTextInput", true)
		FairyGuiManager:Close("__EventPayloadProbe", true)
		FairyGuiManager:HideAiDebugPanel("__AiDebugPanelSelfTest")
		FairyGuiManager:Close("ScreenAdaptMessage", true)
		FairyGuiManager:Close("ScreenAdaptPopup", true)
		FairyGuiManager:ClearResourceFallbacks()
		if closeFairyGuiBusinessFlowObjects ~= nil then
			closeFairyGuiBusinessFlowObjects("selfTestSuiteCleanup")
		end
		FairyGuiManager:UnloadPackageGroup("Sample")
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
		FairyGuiManager:DumpPerfStats()
	end)
	return true
end

closeFairyGuiBusinessFlowObjects = function(reason)
	reason = reason or "businessFlowCleanup"
	FairyGuiManager:ClearToastQueue()
	FairyGuiManager:CloseScene("BusinessFlow", true)
	FairyGuiManager:Close("Act37TestMvc", true, reason)
	FairyGuiManager:Close("Act38Test", true, reason)
	FairyGuiManager:Close("BusinessFlowTextInput", true, reason)
	FairyGuiManager:Close("__EventPayloadProbe", true, reason)
	FairyGuiManager:Close("__Toast", true, reason)
	FairyGuiManager:Close("__Tip", true, reason)
	FairyGuiManager:HideLoading({ force = true, reason = reason })
	FairyGuiManager:Close("__GuideMask", true, reason)
	FairyGuiManager:Close("__MessageBox", true, reason)
	FairyGuiManager:Close("__PopupMenu", true, reason)
	FairyGuiManager:Close("BusinessFlowMessage", true, reason)
	FairyGuiManager:Close("BusinessFlowPopup", true, reason)
	FairyGuiManager:HideDebugPanel("BusinessFlowDebug")
	FairyGuiManager:HideAiDebugPanel("__AiDebugPanelSelfTest")
end

local function closeFairyGuiPressureObjects(reason)
	reason = reason or "pressureCleanup"
	FairyGuiManager:CloseScene("Pressure", true)
	FairyGuiManager:CloseGroup("Pressure", true)
	FairyGuiManager:Close("__PressureAct38", true, reason)
	FairyGuiManager:Close("__PressureDebug", true, reason)
	for index = 1, 80 do
		FairyGuiManager:Close("__PressurePopup" .. tostring(index), true, reason)
	end
end

local function closeFairyGuiLongLoopObjects()
	FairyGuiManager:Close("Act37TestMvc", true)
	FairyGuiManager:Close("Act38Test", true)
	FairyGuiManager:CloseGroup("LayerProbe", true)
	FairyGuiManager:Close("MaskProbe", true)
	FairyGuiManager:Close("GraphMaskProbe", true)
	FairyGuiManager:Close("NestedGraphMaskProbe", true)
	FairyGuiManager:Close("TextInputProbe", true)
	FairyGuiManager:Close("BusinessFlowTextInput", true)
	FairyGuiManager:Close("__EventPayloadProbe", true)
	FairyGuiManager:Close("ScreenAdaptMessage", true)
	FairyGuiManager:Close("ScreenAdaptPopup", true)
	FairyGuiManager:HideAiDebugPanel("__AiDebugPanelSelfTest")
	if closeFairyGuiBusinessFlowObjects ~= nil then
		closeFairyGuiBusinessFlowObjects("longLoopCleanup")
	end
	closeFairyGuiPressureObjects("longLoopCleanup")
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

local function countFairyGuiPackagesByGroup(refsByPackage, groupName)
	local count = 0
	for _, ref in pairs(refsByPackage or {}) do
		for _, group in ipairs(ref.groups or {}) do
			if group == groupName then
				count = count + 1
				break
			end
		end
	end
	return count
end

local function buildFairyGuiPressureCounts(maxPopupCount)
	maxPopupCount = tonumber(maxPopupCount) or 50
	maxPopupCount = math.max(1, math.floor(maxPopupCount))

	local candidates = { 1, 5, 20, maxPopupCount }
	local seen = {}
	local counts = {}
	for _, count in ipairs(candidates) do
		count = math.min(math.max(1, count), maxPopupCount)
		if seen[count] ~= true then
			seen[count] = true
			table.insert(counts, count)
		end
	end
	table.sort(counts)
	return counts
end

local function createFairyGuiPressureTask(index)
	local stateIndex = index % 3
	return {
		desc = "Pressure task " .. tostring(index),
		num = tostring(index % 7) .. "/7",
		state = stateIndex == 0 and "done" or stateIndex == 1 and "go" or "get",
	}
end

local function createFairyGuiPressureShopItem(index)
	local stateIndex = index % 3
	return {
		cost = tostring(index * 10),
		reward = tostring(index),
		state = stateIndex == 0 and "finish" or stateIndex == 1 and "exchange" or "owned",
	}
end

local function buildFairyGuiPressureStats(label, popupCount, listItemCount)
	local health = FairyGuiManager:GetHealthStats()
	local render = FairyGuiManager:GetRenderStats()
	local perf = FairyGuiManager:GetPerfStats()
	local packageRefCount, packageDetail = getFairyGuiPackageRefCount()
	local warnings = FairyGuiManager:GetResourceWarnings()
	print(
		"[FGUI] pressure stats:",
		label or "",
		"popup=", popupCount or 0,
		"list=", listItemCount or 0,
		"openUI=", health.openUI,
		"hiddenUI=", health.hiddenUI,
		"binding=", health.binding,
		"timer=", health.timer,
		"objectHandle=", health.objectHandle,
		"childCache=", health.childCache,
		"packageRef=", packageRefCount,
		"warnings=", #warnings,
		"command=", render.commandCount,
		"triangle=", render.triangleCount,
		"draw=", tostring(render.drawCommandCount) .. "/" .. tostring(render.drawTriangleCount),
		"switch=", tostring(render.materialSwitchCount) .. "/" .. tostring(render.textureSwitchCount),
		"clip=", tostring(render.clippedCommandCount) .. "/" .. tostring(render.clippedTriangleCount),
		"cpuClip=", tostring(render.cpuClipSourceTriangleCount) .. "/" .. tostring(render.cpuClipOutputTriangleCount) .. "/" .. tostring(render.cpuClipFragmentCount),
		"stencil=", tostring(render.stencilCommandCount) .. "/" .. tostring(render.stencilTriangleCount),
		"stencilClip=", tostring(render.stencilClipScopeCount) .. "/" .. tostring(render.stencilClipPolygonCount),
		"backend=", render.stencilBackend,
		"maxBatch=", tostring(render.maxBatchTriangles) .. "/" .. tostring(render.maxBatchVertices),
		"material=", render.materialCount,
		"texture=", render.textureCount,
		"materialAlias=", render.materialAliasCount,
		"textureAlias=", render.textureAliasCount,
		"openPerf=", perf.open and perf.open.count or 0,
		"closePerf=", perf.close and perf.close.count or 0,
		packageDetail)
	return {
		health = health,
		render = render,
		perf = perf,
		packageRefCount = packageRefCount,
		warningCount = #warnings,
	}
end

local function openFairyGuiPressurePopup(index, popupCount)
	local width = 220
	local height = 112
	local columns = math.max(1, math.ceil(math.sqrt(popupCount)))
	local column = (index - 1) % columns
	local row = math.floor((index - 1) / columns)
	local x = 24 + column * 34
	local y = 36 + row * 26
	local key = "__PressurePopup" .. tostring(index)
	local objectInfo = FairyGuiManager:OpenServiceContainer(key, {
		key = key,
		name = "PressurePopup",
		scene = "Pressure",
		group = "Pressure",
		layer = "Popup",
		stackMode = "Popup",
		popupMode = "stack",
		popupGroup = "Pressure",
		closeOnSceneChange = true,
		modal = false,
		touchable = true,
		x = x,
		y = y,
		width = width,
		height = height,
		priority = index,
		serviceType = "PressurePopup",
	})
	if objectInfo == nil then
		return nil
	end

	FairyGuiManager:AddServiceImage(objectInfo, "pressure_bg", "res/assets/act_38/_imgs/board_task.png", 0, 0, width, height, 0.86)
	FairyGuiManager:AddServiceText(objectInfo, "pressure_title", "Pressure " .. tostring(index), 10, 10, width - 20, 26, 18, 255, 236, 180)
	FairyGuiManager:AddServiceText(objectInfo, "pressure_body", "popup=" .. tostring(index) .. "/" .. tostring(popupCount), 10, 40, width - 20, 24, 15, 210, 235, 255)
	FairyGuiManager:AddServiceButton(objectInfo, "pressure_button", "OK", width - 72, height - 38, 58, 28, function(evt)
		print("[FGUI] pressure popup click:", index, evt and evt.bindingId)
	end)
	return objectInfo.handle
end

local function openFairyGuiPressurePopups(popupCount)
	local handles = {}
	for index = 1, popupCount do
		local handle = openFairyGuiPressurePopup(index, popupCount)
		if handle ~= nil then
			handles[#handles + 1] = handle
		end
	end
	return handles
end

local function openFairyGuiPressureList(listItemCount)
	listItemCount = math.max(1, tonumber(listItemCount) or 50)
	local ctrl = FGUI_OpenAct38Sample({
		key = "__PressureAct38",
		scene = "Pressure",
		group = "Pressure",
		popupGroup = "Pressure",
		popupMode = "stack",
		modal = false,
		fullScreen = true,
		dateText = "Pressure List x" .. tostring(listItemCount),
	})
	if ctrl == nil then
		return nil, false
	end

	local dayTasks = {}
	local spcTasks = {}
	local shopItems = {}
	for index = 1, listItemCount do
		dayTasks[index] = createFairyGuiPressureTask(index)
		if index <= math.max(1, math.floor(listItemCount * 0.5)) then
			spcTasks[index] = createFairyGuiPressureTask(index + 1000)
		end
		if index <= math.max(1, math.floor(listItemCount * 0.4)) then
			shopItems[index] = createFairyGuiPressureShopItem(index)
		end
	end

	local dayOk = ctrl:SetListData("m2_dayTaskList", dayTasks, function(item, data, index)
		ctrl:RenderTaskItem(item, data, index)
	end)
	local spcOk = ctrl:SetListData("m2_spcTaskList", spcTasks, function(item, data, index)
		ctrl:RenderTaskItem(item, data, index)
	end)
	local shopOk = ctrl:SetListData("m2_excShopList", shopItems, function(item, data, index)
		ctrl:RenderShopItem(item, data, index)
	end)
	return ctrl, dayOk == true and spcOk == true and shopOk == true
end

function FGUI_RunBusinessFlowSelfTest()
	closeFairyGuiBusinessFlowObjects("businessFlowReset")

	local perfBefore = FairyGuiManager:GetPerfStats()
	local openBefore = perfBefore.open and perfBefore.open.count or 0
	local ctrl38 = FGUI_OpenAct38Sample({
		scene = "BusinessFlow",
		group = "BusinessFlow",
		dateText = "Business Flow Sample",
	})
	local pageOpen = ctrl38 ~= nil and FairyGuiManager:GetObjectInfo("Act38Test") ~= nil
	local listOk = ctrl38 ~= nil and ctrl38.RunListApiSelfTest ~= nil and ctrl38:RunListApiSelfTest() or false

	local dragOk = FGUI_RunEventPayloadSelfTest()

	local textHandle, inputHandle = FGUI_OpenTextInputProbe({
		key = "BusinessFlowTextInput",
		scene = "BusinessFlow",
		group = "BusinessFlow",
	})
	local focused = inputHandle ~= nil and FairyGuiManager:Focus(inputHandle) or false
	local keyA = FairyGuiManager:DebugInjectKeyPressed(30, 65)
	local submit = FairyGuiManager:DebugInjectKeyPressed(28, 0)
	local text = inputHandle ~= nil and FairyGuiManager:GetText(inputHandle) or ""
	local textOk = textHandle ~= nil and inputHandle ~= nil and focused == true and keyA == true and submit == true and text == "A"

	local toastHandle = FairyGuiManager:ShowToast("Business flow toast", 0, {
		scene = "BusinessFlow",
		queue = false,
		dedupeKey = "BusinessFlowToast",
	})
	local loadingHandle = FairyGuiManager:ShowLoading("Business flow loading", {
		scene = "BusinessFlow",
		refKey = "BusinessFlow",
	})
	local guideHandle = FairyGuiManager:ShowGuideMask({
		scene = "BusinessFlow",
		text = "Business flow guide",
		textX = 80,
		textY = 120,
		highlightRect = { x = 240, y = 180, width = 240, height = 150 },
		closeOnMaskClick = false,
	})
	local messageHandle = FairyGuiManager:ShowMessageBox("Business Flow", "MessageBox with modal popup.", { "OK", "Cancel" }, nil, {
		key = "BusinessFlowMessage",
		scene = "BusinessFlow",
		closeOnMaskClick = true,
	})
	local popupHandle = FairyGuiManager:ShowPopupMenu({ "Inspect", "Use", "Cancel" }, 100, 140, nil, {
		key = "BusinessFlowPopup",
		scene = "BusinessFlow",
	})
	local debugHandle = FairyGuiManager:ShowDebugPanel({
		key = "BusinessFlowDebug",
		scene = "BusinessFlow",
		title = "Business Flow Debug",
		autoRefresh = false,
	})
	local debugOk = debugHandle ~= nil and FairyGuiManager:RefreshDebugPanel("BusinessFlowDebug") == true

	local serviceStats = FairyGuiManager:GetServiceStats()
	local servicesOk = toastHandle ~= nil
		and loadingHandle ~= nil
		and guideHandle ~= nil
		and messageHandle ~= nil
		and popupHandle ~= nil
		and debugHandle ~= nil
		and serviceStats.Toast ~= nil
		and serviceStats.Loading ~= nil
		and serviceStats.GuideMask ~= nil
		and serviceStats.MessageBox ~= nil
		and serviceStats.PopupMenu ~= nil
		and serviceStats.DebugPanel ~= nil

	local healthDuring = FairyGuiManager:GetHealthStats()
	local perfAfter = FairyGuiManager:GetPerfStats()
	local perfOk = perfAfter.open ~= nil and perfAfter.open.count > openBefore
	local sceneClosed = FairyGuiManager:CloseScene("BusinessFlow", true)
	closeFairyGuiBusinessFlowObjects("businessFlowFinalCleanup")
	local finalClean, finalDetail = checkFairyGuiLongLoopClean("business flow final")

	print(
		"[FGUI] business flow self test detail:",
		"page=", pageOpen,
		"list=", listOk,
		"drag=", dragOk,
		"text=", textOk,
		"services=", servicesOk,
		"debug=", debugOk,
		"perf=", perfOk,
		"sceneClosed=", sceneClosed,
		"duringOpenUI=", healthDuring.openUI,
		finalDetail)

	return pageOpen == true
		and listOk == true
		and dragOk == true
		and textOk == true
		and servicesOk == true
		and debugOk == true
		and perfOk == true
		and sceneClosed > 0
		and finalClean == true
end

function FGUI_RunResourcePolicySelfTest()
	FairyGuiManager:Close("Act37TestMvc", true, "resourcePolicyReset")
	FairyGuiManager:Close("Act38Test", true, "resourcePolicyReset")
	FairyGuiManager:CloseGroup("LayerProbe", true)
	FairyGuiManager:Close("TextInputProbe", true, "resourcePolicyReset")
	FairyGuiManager:UnloadPackageGroup("Sample")
	FairyGuiManager:UnloadPackageGroup("LayerProbe")

	local preloadStats = FairyGuiManager:PreloadScene("Default", { groupName = "Sample" })
	local refsAfterPreload = FairyGuiManager:GetPackageRefs()
	local samplePackageCount = countFairyGuiPackagesByGroup(refsAfterPreload, "Sample")
	local preloadOk = preloadStats.loaded >= 2 and samplePackageCount >= 2

	local tagUnloadStats = FairyGuiManager:UnloadPackageTag("act38")
	local tagUnloadOk = tagUnloadStats.unloaded >= 1

	FairyGuiManager:PreloadScene("Default", { groupName = "Sample" })
	local ctrl38 = FGUI_OpenAct38Sample({
		scene = "ResourcePolicy",
		group = "ResourcePolicy",
	})
	local openOk = ctrl38 ~= nil
	local inUseUnloadStats = FairyGuiManager:UnloadPackageGroup("Sample")
	local skipInUseOk = inUseUnloadStats.skipped >= 1

	FairyGuiManager:Close("Act38Test", true, "resourcePolicyCloseAct38")
	local finalUnloadStats = FairyGuiManager:UnloadPackageGroup("Sample")
	local finalUnloadOk = finalUnloadStats.unloaded >= 1

	local oldCachePolicy = FairyGuiManager:GetCachePolicy()
	FairyGuiManager:SetCachePolicy({ maxHiddenTotal = 1, maxHiddenPerPackage = 4, warnHiddenSeconds = 60 })
	local cache37 = FGUI_ReopenAct37Sample({
		key = "ResourcePolicyCache37",
		cache = true,
		group = "ResourcePolicy",
	})
	local cache38 = FGUI_OpenAct38Sample({
		key = "ResourcePolicyCache38",
		cache = true,
		group = "ResourcePolicy",
	})
	local hide37 = cache37 ~= nil and FairyGuiManager:Close("ResourcePolicyCache37", false, "resourcePolicyCacheHide") or false
	local hide38 = cache38 ~= nil and FairyGuiManager:Close("ResourcePolicyCache38", false, "resourcePolicyCacheHide") or false
	local cacheWarnings = FairyGuiManager:GetResourceWarnings()
	local cacheWarningOk = false
	for _, warning in ipairs(cacheWarnings) do
		if warning.kind == "hiddenCacheOverTotalBudget" then
			cacheWarningOk = true
			break
		end
	end
	FairyGuiManager:Close("ResourcePolicyCache37", true, "resourcePolicyCacheCleanup")
	FairyGuiManager:Close("ResourcePolicyCache38", true, "resourcePolicyCacheCleanup")
	FairyGuiManager:SetCachePolicy(oldCachePolicy)
	local finalClean, finalDetail = checkFairyGuiLongLoopClean("resource policy final")

	print(
		"[FGUI] resource policy self test detail:",
		"preload=", preloadStats.loaded, samplePackageCount, preloadOk,
		"tagUnload=", tagUnloadStats.unloaded, tagUnloadOk,
		"inUse=", inUseUnloadStats.matched, inUseUnloadStats.unloaded, inUseUnloadStats.skipped, skipInUseOk,
		"finalUnload=", finalUnloadStats.unloaded, finalUnloadOk,
		"cachePolicy=", hide37, hide38, cacheWarningOk, #cacheWarnings,
		"open=", openOk,
		finalDetail)

	return preloadOk == true
		and tagUnloadOk == true
		and openOk == true
		and skipInUseOk == true
		and finalUnloadOk == true
		and hide37 == true
		and hide38 == true
		and cacheWarningOk == true
		and finalClean == true
end

function FGUI_RunResourceFallbackSelfTest()
	FairyGuiManager:Close("Act37TestMvc", true, "resourceFallbackReset")
	FairyGuiManager:Close("Act38Test", true, "resourceFallbackReset")
	FairyGuiManager:ClearResourceFallbacks()
	FairyGuiManager:UnloadPackageGroup("ResourceFallback", true)
	local fallbackPolicy = FairyGuiManager:GetResourceFallbackPolicy()
	FairyGuiManager:SetResourceFallbackPolicy({ recordMissingChild = true })

	local packageFallback = FairyGuiManager:RecordResourceFallback("missingPackage", {
		packagePath = "res/fuires/__missing_fgui_package__",
		packageName = "__missing_fgui_package__",
		uiName = "ResourceFallbackSelfTest",
		group = "ResourceFallback",
		scene = "ResourceFallback",
	}, "self test simulated missing package")
	local componentFallback = FairyGuiManager:RecordResourceFallback("missingComponent", {
		packageName = "act_38_test",
		objectName = "__MissingComponent__",
		uiName = "ResourceFallbackSelfTest",
		group = "ResourceFallback",
		scene = "ResourceFallback",
	}, "self test simulated missing component")

	local ctrl = FGUI_OpenAct38Sample({
		scene = "ResourceFallback",
		group = "ResourceFallback",
	})
	local handle = ctrl ~= nil and ctrl:GetHandle() or nil
	local missingChild = handle ~= nil and FairyGuiManager:GetChild(handle, "__missing_child__") or nil
	local missingEvent = handle ~= nil and FairyGuiManager:AddClick(handle, "__missing_button__", function()
	end) or nil

	local fallbacks = FairyGuiManager:GetResourceFallbacks()
	local kindCount = {}
	for _, item in ipairs(fallbacks or {}) do
		kindCount[item.kind] = (kindCount[item.kind] or 0) + 1
	end
	local lines = FairyGuiManager:BuildDebugPanelLines({ lineCount = 24 })
	local hasFallbackLine = false
	for _, line in ipairs(lines or {}) do
		if string.find(line, "Fallbacks ", 1, true) ~= nil then
			hasFallbackLine = true
			break
		end
	end
	local dumpCount = FairyGuiManager:DumpResourceFallbacks()

	FairyGuiManager:Close("Act38Test", true, "resourceFallbackCleanup")
	FairyGuiManager:UnloadPackageGroup("ResourceFallback", true)
	FairyGuiManager:SetResourceFallbackPolicy(fallbackPolicy)
	local finalClean, finalDetail = checkFairyGuiLongLoopClean("resource fallback final")
	local ok = packageFallback ~= nil
		and componentFallback ~= nil
		and missingChild == nil
		and missingEvent == nil
		and (kindCount.missingPackage or 0) >= 1
		and (kindCount.missingComponent or 0) >= 1
		and (kindCount.missingChild or 0) >= 1
		and (kindCount.missingEventTarget or 0) >= 1
		and hasFallbackLine == true
		and dumpCount >= 4
		and finalClean == true

	print(
		"[FGUI] resource fallback self test detail:",
		"missingPackage=", kindCount.missingPackage or 0,
		"missingComponent=", kindCount.missingComponent or 0,
		"missingChild=", kindCount.missingChild or 0,
		"missingEventTarget=", kindCount.missingEventTarget or 0,
		"fallbackLine=", hasFallbackLine,
		"dump=", dumpCount,
		finalDetail)
	return ok
end

function FGUI_RunTextInputPolicySelfTest()
	FairyGuiManager:Close("TextInputProbe", true, "textInputPolicyReset")

	local handle, inputHandle = FGUI_OpenTextInputProbe({
		textInputPolicy = {
			inputType = "integer",
			maxLength = 3,
		},
	})
	local opened = handle ~= nil and inputHandle ~= nil
	local policy = inputHandle ~= nil and FairyGuiManager:GetTextInputPolicy(inputHandle) or nil
	local policyOk = policy ~= nil and policy.inputType == "integer" and tonumber(policy.maxLength) == 3
	local setText = inputHandle ~= nil and FairyGuiManager:SetText(inputHandle, nil, "a1b2\228\184\1733") or false
	local applied = inputHandle ~= nil and FairyGuiManager:ApplyTextInputPolicy(inputHandle, nil) or false
	local text = inputHandle ~= nil and FairyGuiManager:GetText(inputHandle) or ""
	local cleared = inputHandle ~= nil and FairyGuiManager:SetTextInputPolicy(inputHandle, nil, nil) or false
	local clearedPolicy = inputHandle ~= nil and FairyGuiManager:GetTextInputPolicy(inputHandle) or nil
	local snapshot = FairyGuiManager:CaptureCloseSnapshot("TextInputProbe")
	local closed = FairyGuiManager:Close("TextInputProbe", true, "textInputPolicyCleanup")
	local clean = FairyGuiManager:ValidateClosedObject(snapshot, nil, "TextInputPolicySelfTest", true)

	print(
		"[FGUI] text input policy self test detail:",
		"opened=", opened,
		"policy=", policyOk,
		"setText=", setText,
		"applied=", applied,
		"text=", text,
		"cleared=", cleared,
		"clearedPolicy=", clearedPolicy == nil,
		"closed=", closed,
		"clean=", clean)
	return opened == true
		and policyOk == true
		and setText == true
		and applied == true
		and text == "123"
		and cleared == true
		and clearedPolicy == nil
		and closed == true
		and clean == true
end

function FGUI_RunBusinessBenchmarkSelfTest(config)
	config = config or {}
	local iterations = tonumber(config.iterations) or tonumber(os.getenv and os.getenv("HELLO_FGUI_BUSINESS_BENCHMARK_COUNT") or nil) or 3
	iterations = math.max(1, math.min(tonumber(iterations) or 3, 20))

	closeFairyGuiBusinessFlowObjects("businessBenchmarkReset")
	FairyGuiManager:Close("BusinessBenchmarkAct38", true, "businessBenchmarkReset")
	FairyGuiManager:Close("BusinessBenchmarkTextInput", true, "businessBenchmarkReset")
	FairyGuiManager:HideDebugPanel("BusinessBenchmarkDebug")

	local perfBefore = FairyGuiManager:GetPerfStats()
	local openBefore = perfBefore.open and perfBefore.open.count or 0
	local passCount = 0
	local failDetail = ""
	for index = 1, iterations do
		FairyGuiManager:Close("BusinessBenchmarkAct38", true, "businessBenchmarkIterationReset")
		FairyGuiManager:Close("BusinessBenchmarkTextInput", true, "businessBenchmarkIterationReset")
		FairyGuiManager:HideDebugPanel("BusinessBenchmarkDebug")

		local ctrl = FGUI_OpenAct38Sample({
			key = "BusinessBenchmarkAct38",
			scene = "BusinessBenchmark",
			group = "BusinessBenchmark",
			dateText = "Business Benchmark " .. tostring(index),
		})
		local listOk = ctrl ~= nil and ctrl.RunListApiSelfTest ~= nil and ctrl:RunListApiSelfTest() or false
		local debugHandle = FairyGuiManager:ShowDebugPanel({
			key = "BusinessBenchmarkDebug",
			scene = "BusinessBenchmark",
			title = "Business Benchmark",
			autoRefresh = false,
			lineCount = 16,
			debugTarget = "BusinessBenchmarkAct38",
		})
		local debugOk = debugHandle ~= nil and FairyGuiManager:RefreshDebugPanel("BusinessBenchmarkDebug") == true
		local textHandle, inputHandle = FGUI_OpenTextInputProbe({
			key = "BusinessBenchmarkTextInput",
			scene = "BusinessBenchmark",
			group = "BusinessBenchmark",
			textInputPolicy = {
				inputType = "integer",
				maxLength = 4,
			},
		})
		local textOk = false
		if inputHandle ~= nil then
			FairyGuiManager:SetText(inputHandle, nil, "A12B34")
			FairyGuiManager:ApplyTextInputPolicy(inputHandle, nil)
			textOk = FairyGuiManager:GetText(inputHandle) == "1234"
		end
		local iterationOk = ctrl ~= nil and listOk == true and debugOk == true and textHandle ~= nil and textOk == true
		if iterationOk then
			passCount = passCount + 1
		else
			failDetail = failDetail .. string.format("[#%s ctrl=%s list=%s debug=%s text=%s]", tostring(index), tostring(ctrl ~= nil), tostring(listOk), tostring(debugOk), tostring(textOk))
		end

		FairyGuiManager:Close("BusinessBenchmarkTextInput", true, "businessBenchmarkIterationCleanup")
		FairyGuiManager:HideDebugPanel("BusinessBenchmarkDebug")
		FairyGuiManager:Close("BusinessBenchmarkAct38", true, "businessBenchmarkIterationCleanup")
	end

	FairyGuiManager:CloseScene("BusinessBenchmark", true)
	FairyGuiManager:CloseGroup("BusinessBenchmark", true)
	local perfAfter = FairyGuiManager:GetPerfStats()
	local render = FairyGuiManager:GetRenderStats()
	local finalClean, finalDetail = checkFairyGuiLongLoopClean("business benchmark final")
	local perfOk = perfAfter.open ~= nil and perfAfter.open.count > openBefore

	print(
		"[FGUI] business benchmark self test detail:",
		"pass=", passCount,
		"/", iterations,
		"perf=", perfOk,
		"openCount=", perfAfter.open and perfAfter.open.count or 0,
		"openAvgMs=", perfAfter.open and perfAfter.open.avgMs or 0,
		"draw=", tostring(render.drawCommandCount) .. "/" .. tostring(render.drawTriangleCount),
		"fail=", failDetail,
		finalDetail)
	return passCount == iterations
		and perfOk == true
		and finalClean == true
end

function FGUI_RunPressureSelfTest(config)
	if threadpool == nil or threadpool.delay == nil then
		print("[FGUI] pressure self test failed: threadpool unavailable")
		return false
	end

	config = config or {}
	local maxPopupCount = tonumber(config.maxPopupCount) or 50
	local listItemCount = tonumber(config.listItemCount) or math.max(maxPopupCount, 50)
	local interval = tonumber(config.interval) or 0.5
	local holdSeconds = tonumber(config.holdSeconds) or 0.8
	local counts = buildFairyGuiPressureCounts(maxPopupCount)
	local results = {}
	local failCount = 0
	local delay = 0

	local function record(count, ok, detail)
		if ok ~= true then
			failCount = failCount + 1
		end
		results[#results + 1] = {
			count = count,
			ok = ok == true,
			detail = detail,
		}
		print("[FGUI] pressure case:", count, ok == true and "PASS" or "FAIL", detail or "")
	end

	local function runOpenCase(popupCount)
		closeFairyGuiLongLoopObjects()

		local handles = openFairyGuiPressurePopups(popupCount)
		local ctrl, listOk = openFairyGuiPressureList(listItemCount)
		local health = FairyGuiManager:GetHealthStats()
		local openOk = #handles == popupCount
			and ctrl ~= nil
			and listOk == true
			and health.openUI >= popupCount + 1
		buildFairyGuiPressureStats("open", popupCount, listItemCount)
		return openOk, "handles=" .. tostring(#handles) .. " openUI=" .. tostring(health.openUI) .. " listOk=" .. tostring(listOk)
	end

	local function runCloseCase(popupCount)
		buildFairyGuiPressureStats("rendered", popupCount, listItemCount)
		closeFairyGuiPressureObjects("pressureCaseClose")
		local clean, detail = checkFairyGuiLongLoopClean("pressure " .. tostring(popupCount))
		return clean == true, detail
	end

	print("[FGUI] pressure self test begin:", table.concat(counts, ","), "list=", listItemCount)
	closeFairyGuiLongLoopObjects()
	for _, popupCount in ipairs(counts) do
		delay = delay + interval
		threadpool:delay(delay, function()
			local ok, result, detail = pcall(runOpenCase, popupCount)
			if not ok then
				record(popupCount, false, result)
			elseif result ~= true then
				record(popupCount, false, detail)
			end
		end)

		delay = delay + holdSeconds
		threadpool:delay(delay, function()
			local ok, result, detail = pcall(runCloseCase, popupCount)
			if not ok then
				record(popupCount, false, result)
			else
				record(popupCount, result == true, detail)
			end
		end)
	end

	threadpool:delay(delay + interval, function()
		local passCount = 0
		for _, result in ipairs(results) do
			if result.ok then
				passCount = passCount + 1
			end
		end
		local expected = #counts
		local finalClean, finalDetail = checkFairyGuiLongLoopClean("pressure final")
		local allOk = passCount == expected and failCount == 0 and #results == expected and finalClean == true
		print("[FGUI] pressure self test end:", passCount, "/", expected, "finalClean=", finalClean, finalDetail)
		print("[FGUI] pressure self test result:", allOk)
		FairyGuiManager:DumpPerfStats()
	end)
	return true
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
			local graphMaskHandle = FGUI_OpenGraphMaskProbe()
			local graphMaskClosed = FGUI_CloseGraphMaskProbe()
			if graphMaskHandle == nil or graphMaskClosed ~= true then
				return false, "Graph mask probe failed"
			end
			local nestedMaskHandle = FGUI_OpenNestedGraphMaskProbe()
			local nestedMaskClosed = FGUI_CloseNestedGraphMaskProbe()
			if nestedMaskHandle == nil or nestedMaskClosed ~= true then
				return false, "Nested graph mask probe failed"
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
			FairyGuiManager:DumpPerfStats()
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

function FGUI_RunLayerBoundarySelfTest()
	local function near(a, b)
		return math.abs((tonumber(a) or 0) - (tonumber(b) or 0)) < 0.01
	end

	local oldSafeArea = FairyGuiManager:GetSafeArea()
	FairyGuiManager:SetSafeArea({ left = 8, top = 10, right = 12, bottom = 14 })
	local safeArea = FairyGuiManager:GetSafeArea()
	local screenWidth = FairyGuiManager:GetScreenWidth()
	local screenHeight = FairyGuiManager:GetScreenHeight()
	local safeRect = FairyGuiManager:GetLayoutRect({
		width = 100,
		height = 50,
		alignX = "right",
		alignY = "bottom",
		useSafeArea = true,
		fitInScreen = true,
	})
	local safeAreaOk = safeArea.left == 8
		and safeArea.top == 10
		and safeArea.right == 12
		and safeArea.bottom == 14
		and safeRect ~= nil
		and near(safeRect.x, screenWidth - 12 - 100)
		and near(safeRect.y, screenHeight - 14 - 50)

	local topPolicy = FairyGuiManager:GetLayerPolicy("Top")
	local toastPolicy = FairyGuiManager:GetLayerPolicy("Toast")
	local policyOk = topPolicy.popup == true
		and topPolicy.closeOnEscape == true
		and toastPolicy.popup ~= true
		and toastPolicy.closeOnEscape ~= true
		and FairyGuiManager:IsPopupLayer("Toast") == false

	local escHandle = FairyGuiManager:ShowMessageBox("Boundary", "ESC should close this popup.", { "OK" }, nil, {
		key = "LayerBoundaryEsc",
		popupGroup = "LayerBoundary",
		popupMode = "stack",
	})
	local escObject = FairyGuiManager:GetObjectInfo("LayerBoundaryEsc")
	local escAllowed = FairyGuiManager:CanClosePopupByEscape(escObject)
	local escClosed = FairyGuiManager:HandleKeyPressed(1, 0)
	local escOk = escHandle ~= nil and escAllowed == true and escClosed == true and FairyGuiManager:GetObjectInfo("LayerBoundaryEsc") == nil

	local parentRoot = FairyGuiManager:EnsureLayerRoot("Top")
	local handleA, inputA = FGUI_OpenTextInputProbe({
		key = "LayerBoundaryInputA",
		parentHandle = parentRoot,
		x = 20,
		y = 80,
		center = false,
	})
	local handleB, inputB = FGUI_OpenTextInputProbe({
		key = "LayerBoundaryInputB",
		layer = "Top",
		x = 20,
		y = 260,
		center = false,
	})
	local objectA = FairyGuiManager:GetObjectInfo("LayerBoundaryInputA")
	local parentOk = parentRoot ~= nil and objectA ~= nil and objectA.parentHandle == parentRoot
	local focusA = inputA ~= nil and FairyGuiManager:Focus(inputA) or false
	local tabHandled = FairyGuiManager:HandleKeyPressed(15, 0)
	local focusB = inputB ~= nil and FairyGuiManager:GetFocusedHandle() == inputB
	local tabOk = handleA ~= nil and handleB ~= nil and focusA == true and tabHandled == true and focusB == true

	FairyGuiManager:Close("LayerBoundaryInputA", true, "layerBoundaryCleanup")
	FairyGuiManager:Close("LayerBoundaryInputB", true, "layerBoundaryCleanup")
	FairyGuiManager:Close("LayerBoundaryEsc", true, "layerBoundaryCleanup")
	FairyGuiManager:ClearFocus()
	FairyGuiManager:SetSafeArea(oldSafeArea)

	print("[FGUI] layer boundary self test detail:",
		"safeAreaOk=", safeAreaOk,
		"policyOk=", policyOk,
		"escOk=", escOk,
		"parentOk=", parentOk,
		"tabOk=", tabOk,
		"focusTarget=", tostring(inputB))
	return safeAreaOk == true and policyOk == true and escOk == true and parentOk == true and tabOk == true
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
