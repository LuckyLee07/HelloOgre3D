local FairyGuiNativeApi = {}

local BACKEND_NAMES = {
	"FairyGuiRuntime",
}

local METHOD_ALIASES = {
	isFairyGuiAvailable = "IsAvailable",
	loadFairyGuiPackage = "LoadPackage",
	removeFairyGuiPackage = "RemovePackage",
	createFairyGuiObject = "CreateObject",
	createFairyGuiContainer = "CreateContainer",
	createFairyGuiChildContainer = "CreateChildContainer",
	createFairyGuiLoader = "CreateLoader",
	createFairyGuiText = "CreateText",
	createFairyGuiTextInput = "CreateTextInput",
	createFairyGuiGraphRect = "CreateGraphRect",
	createFairyGuiGraphRegularPolygon = "CreateGraphRegularPolygon",
	createFairyGuiModalMask = "CreateModalMask",
	getFairyGuiLastRenderCommandCount = "GetLastRenderCommandCount",
	getFairyGuiLastTriangleCount = "GetLastTriangleCount",
	getFairyGuiScreenWidth = "GetScreenWidth",
	getFairyGuiScreenHeight = "GetScreenHeight",
	getFairyGuiRuntimeObjectHandleCount = "GetRuntimeObjectHandleCount",
	getFairyGuiRuntimeListenerBindingCount = "GetRuntimeListenerBindingCount",
	getFairyGuiMaterialCount = "GetMaterialCount",
	getFairyGuiTextureCount = "GetTextureCount",
	getFairyGuiMaterialAliasCount = "GetMaterialAliasCount",
	getFairyGuiTextureAliasCount = "GetTextureAliasCount",
	getFairyGuiLastDrawCommandCount = "GetLastDrawCommandCount",
	getFairyGuiLastDrawTriangleCount = "GetLastDrawTriangleCount",
	getFairyGuiLastMaterialSwitchCount = "GetLastMaterialSwitchCount",
	getFairyGuiLastTextureSwitchCount = "GetLastTextureSwitchCount",
	getFairyGuiLastClippedCommandCount = "GetLastClippedCommandCount",
	getFairyGuiLastClippedTriangleCount = "GetLastClippedTriangleCount",
	getFairyGuiLastCulledCommandCount = "GetLastCulledCommandCount",
	getFairyGuiLastStencilCommandCount = "GetLastStencilCommandCount",
	getFairyGuiLastStencilTriangleCount = "GetLastStencilTriangleCount",
	getFairyGuiLastCpuClipSourceTriangleCount = "GetLastCpuClipSourceTriangleCount",
	getFairyGuiLastCpuClipOutputTriangleCount = "GetLastCpuClipOutputTriangleCount",
	getFairyGuiLastCpuClipFragmentCount = "GetLastCpuClipFragmentCount",
	getFairyGuiLastStencilClipScopeCount = "GetLastStencilClipScopeCount",
	getFairyGuiLastStencilClipPolygonCount = "GetLastStencilClipPolygonCount",
	getFairyGuiLastCustomCommandCount = "GetLastCustomCommandCount",
	getFairyGuiLastMaxBatchTriangles = "GetLastMaxBatchTriangles",
	getFairyGuiLastMaxBatchVertices = "GetLastMaxBatchVertices",
	isFairyGuiHardwareStencilSupported = "IsHardwareStencilSupported",
	getFairyGuiStencilBackendString = "GetStencilBackendString",
	getFairyGuiStencilBackendDetailString = "GetStencilBackendDetailString",
	getFairyGuiMaterialDetailString = "GetMaterialDetailString",
	getFairyGuiTextureDetailString = "GetTextureDetailString",
	getFairyGuiFrameRenderDetailString = "GetFrameRenderDetailString",
	plotFairyGuiServiceStats = "PlotServiceStats",
	getFairyGuiChild = "GetChild",
	getFairyGuiListItem = "GetListItem",
	getFairyGuiListItemCount = "GetListItemCount",
	addFairyGuiObjectToRoot = "AddObjectToRoot",
	addFairyGuiObjectToParent = "AddObjectToParent",
	setFairyGuiObjectPosition = "SetObjectPosition",
	setFairyGuiObjectSize = "SetObjectSize",
	setFairyGuiObjectVisible = "SetObjectVisible",
	setFairyGuiObjectAlpha = "SetObjectAlpha",
	setFairyGuiObjectTouchable = "SetObjectTouchable",
	setFairyGuiObjectMask = "SetObjectMask",
	setFairyGuiObjectSortingOrder = "SetObjectSortingOrder",
	setFairyGuiObjectText = "SetObjectText",
	getFairyGuiObjectText = "GetObjectText",
	setFairyGuiObjectIcon = "SetObjectIcon",
	setFairyGuiObjectLoaderUrl = "SetObjectLoaderUrl",
	setFairyGuiObjectControllerIndex = "SetObjectControllerIndex",
	getFairyGuiObjectControllerIndex = "GetObjectControllerIndex",
	setFairyGuiObjectControllerPage = "SetObjectControllerPage",
	getFairyGuiObjectControllerPage = "GetObjectControllerPage",
	getFairyGuiObjectControllerPageId = "GetObjectControllerPageId",
	getFairyGuiObjectControllerPageCount = "GetObjectControllerPageCount",
	getFairyGuiObjectControllerPageNameAt = "GetObjectControllerPageNameAt",
	getFairyGuiObjectControllerPageIdAt = "GetObjectControllerPageIdAt",
	setFairyGuiObjectValue = "SetObjectValue",
	getFairyGuiObjectValue = "GetObjectValue",
	setFairyGuiObjectMin = "SetObjectMin",
	getFairyGuiObjectMin = "GetObjectMin",
	setFairyGuiObjectMax = "SetObjectMax",
	getFairyGuiObjectMax = "GetObjectMax",
	setFairyGuiComboBoxSelectedIndex = "SetComboBoxSelectedIndex",
	getFairyGuiComboBoxSelectedIndex = "GetComboBoxSelectedIndex",
	setFairyGuiComboBoxValue = "SetComboBoxValue",
	getFairyGuiComboBoxValue = "GetComboBoxValue",
	playFairyGuiTransition = "PlayTransition",
	stopFairyGuiTransition = "StopTransition",
	focusFairyGuiObject = "FocusObject",
	clearFairyGuiFocus = "ClearFocus",
	getFairyGuiFocusedObject = "GetFocusedObject",
	setFairyGuiListItemCount = "SetListItemCount",
	setFairyGuiListSelectedIndex = "SetListSelectedIndex",
	getFairyGuiListSelectedIndex = "GetListSelectedIndex",
	setFairyGuiListVirtual = "SetListVirtual",
	refreshFairyGuiList = "RefreshList",
	scrollFairyGuiListToView = "ScrollListToView",
	centerFairyGuiObject = "CenterObject",
	injectFairyGuiMouseMove = "InjectLogicalMouseMove",
	injectFairyGuiMouseDown = "InjectLogicalMouseDown",
	injectFairyGuiMouseUp = "InjectLogicalMouseUp",
	injectFairyGuiMouseWheel = "InjectLogicalMouseWheel",
	injectFairyGuiClick = "InjectLogicalClick",
	injectFairyGuiKeyPressed = "InjectKeyPressed",
	injectFairyGuiKeyReleased = "InjectKeyReleased",
	injectFairyGuiImeCompositionText = "InjectImeCompositionText",
	injectFairyGuiImeCommitText = "InjectImeCommitText",
	clearFairyGuiImeComposition = "ClearImeComposition",
	getFairyGuiImeDebugString = "GetImeDebugString",
	addFairyGuiEventListener = "AddEventListener",
	addFairyGuiClickListener = "AddClickListener",
	addFairyGuiControllerChangedListener = "AddControllerChangedListener",
	removeFairyGuiListener = "RemoveListener",
	removeFairyGuiObject = "RemoveObject",
	clearFairyGuiObjects = "ClearObjects",
}

local function resolveBackend()
	for _, backendName in ipairs(BACKEND_NAMES) do
		local backend = _G[backendName]
		if backend ~= nil then
			return backend, backendName
		end
	end
	return nil, nil
end

local function getBackendMethod(backend, methodName)
	if backend == nil or type(methodName) ~= "string" then
		return nil
	end

	local method = backend[methodName]
	if type(method) == "function" then
		return method
	end

	local alias = METHOD_ALIASES[methodName]
	if alias ~= nil then
		method = backend[alias]
		if type(method) == "function" then
			return method
		end
	end

	return nil
end

local function resolveBackendMethod(methodName)
	for _, backendName in ipairs(BACKEND_NAMES) do
		local backend = _G[backendName]
		local method = getBackendMethod(backend, methodName)
		if method ~= nil then
			return backend, method, backendName
		end
	end
	return nil, nil, nil
end

function FairyGuiNativeApi:GetBackend()
	return resolveBackend()
end

function FairyGuiNativeApi:GetBackendName()
	local _, backendName = resolveBackend()
	return backendName or ""
end

function FairyGuiNativeApi:Has(methodName)
	local _, method = resolveBackendMethod(methodName)
	return method ~= nil
end

function FairyGuiNativeApi:Call(methodName, defaultValue, ...)
	local backend, method = resolveBackendMethod(methodName)
	if backend == nil or method == nil then
		return defaultValue
	end
	return method(backend, ...)
end

setmetatable(FairyGuiNativeApi, {
	__index = function(_, methodName)
		if type(methodName) ~= "string" then
			return nil
		end

		local _, method = resolveBackendMethod(methodName)
		if method == nil then
			return nil
		end

		return function(_, ...)
			local currentBackend, currentMethod = resolveBackendMethod(methodName)
			if currentBackend == nil or currentMethod == nil then
				return nil
			end
			return currentMethod(currentBackend, ...)
		end
	end,
})

return FairyGuiNativeApi
