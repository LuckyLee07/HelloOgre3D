local FairyGuiServiceLoading = {}

local function isBlank(value)
	return value == nil or value == ""
end

local function copyTable(source, target)
	target = target or {}
	if type(source) ~= "table" then
		return target
	end

	for name, value in pairs(source) do
		target[name] = value
	end
	return target
end

local function getColor(style, key, red, green, blue)
	local color = style ~= nil and style[key] or nil
	if type(color) == "table" then
		return tonumber(color.r or color.red or color[1]) or red,
			tonumber(color.g or color.green or color[2]) or green,
			tonumber(color.b or color.blue or color[3]) or blue
	end
	return red, green, blue
end

local function ensureServiceStats(service)
	local state = service:GetStore():GetServiceState()
	if state.serviceStats == nil then
		state.serviceStats = {
			createdTotal = 0,
			closedTotal = 0,
			failedTotal = 0,
			peakOpen = 0,
			toastQueuedTotal = 0,
			toastDedupeIgnoredTotal = 0,
			toastShownTotal = 0,
			toastTimeoutTotal = 0,
			loadingShowTotal = 0,
			loadingHideTotal = 0,
			loadingForceHideTotal = 0,
			loadingPeakRefTotal = 0,
			byType = {},
		}
	end
	service.serviceStats = state.serviceStats
	return state.serviceStats
end

local function syncServiceState(service)
	local state = service:GetStore():GetServiceState()
	state.toastQueue = service.toastQueue or {}
	state.toastActive = service.toastActive
	state.toastSerial = service.toastSerial or 0
	state.toastDedupe = service.toastDedupe or {}
	state.loadingRefs = service.loadingRefs or {}
	state.loadingRefTotal = service.loadingRefTotal or 0
	state.serviceStats = service.serviceStats or state.serviceStats
	state.serviceSkins = service.serviceSkins or state.serviceSkins
	service.toastQueue = state.toastQueue
	service.toastActive = state.toastActive
	service.toastSerial = state.toastSerial
	service.toastDedupe = state.toastDedupe
	service.loadingRefs = state.loadingRefs
	service.loadingRefTotal = state.loadingRefTotal
	service.serviceStats = state.serviceStats
	service.serviceSkins = state.serviceSkins
	service.serviceState = state
end

function FairyGuiServiceLoading.ShowLoading(service, text, param)
	if service == nil or service.owner == nil then
		return nil
	end

	param = copyTable(param)
	local refKey = param.refKey or "Default"
	service.loadingRefs[refKey] = (service.loadingRefs[refKey] or 0) + 1
	service.loadingRefTotal = (service.loadingRefTotal or 0) + 1
	syncServiceState(service)
	local stats = ensureServiceStats(service)
	stats.loadingShowTotal = (stats.loadingShowTotal or 0) + 1
	stats.loadingPeakRefTotal = math.max(stats.loadingPeakRefTotal or 0, service.loadingRefTotal or 0)

	local existing = service:GetObjectInfo("__Loading")
	if existing ~= nil then
		existing.loadingText = text or existing.loadingText
		if not isBlank(text) then
			service:SetText(existing.handle, "loading_text", text)
		end
		service:ApplyServiceLayout(existing)
		return existing.handle
	end

	param.key = param.key or "__Loading"
	param.layer = param.layer or "Top"
	param.stackMode = param.stackMode or "Popup"
	param.popupGroup = param.popupGroup or "Loading"
	param.popupMode = param.popupMode or "replace"
	param.fullScreen = true
	param.modal = param.modal ~= false
	local skin = service:ResolveServiceSkin("Loading", param)
	param.resolvedServiceSkin = skin
	param.serviceSkinName = param.serviceSkinName or skin.name
	param.modalAlpha = param.modalAlpha or skin.modalAlpha
	param.closeOnMaskClick = false
	param.touchable = false
	param.serviceType = "Loading"

	local objectInfo = service:OpenServiceContainer(param.key, param)
	if objectInfo == nil then
		service.loadingRefs[refKey] = math.max((service.loadingRefs[refKey] or 1) - 1, 0)
		service.loadingRefTotal = math.max((service.loadingRefTotal or 1) - 1, 0)
		syncServiceState(service)
		return nil
	end
	objectInfo.loadingText = text or "Loading..."
	local red, green, blue = getColor(objectInfo.serviceSkin or skin, "textColor", 255, 255, 255)
	objectInfo.loadingTextHandle = service:AddServiceText(objectInfo, "loading_text", text or "Loading...", 0, 0, param.width or skin.width or 280, param.height or skin.height or 36, param.fontSize or skin.fontSize or 24, red, green, blue)
	service:ApplyServiceLayout(objectInfo)
	local timeout = tonumber(param.timeout)
	if timeout ~= nil and timeout > 0 then
		service:Delay(objectInfo.key, timeout, function()
			service:HideLoading({ force = true, reason = "loadingTimeout" })
		end)
	end
	return objectInfo.handle
end

function FairyGuiServiceLoading.GetLoadingRefCount(service)
	if service == nil or service.owner == nil then
		return 0
	end
	return service.loadingRefTotal or 0
end

function FairyGuiServiceLoading.HideLoading(service, paramOrReason)
	if service == nil or service.owner == nil then
		return false
	end

	local param = type(paramOrReason) == "table" and paramOrReason or { reason = paramOrReason }
	local refKey = param.refKey or "Default"
	local force = param.force == true
	local reason = param.reason or "hideLoading"
	local stats = ensureServiceStats(service)
	stats.loadingHideTotal = (stats.loadingHideTotal or 0) + 1
	if force then
		stats.loadingForceHideTotal = (stats.loadingForceHideTotal or 0) + 1
	end

	if not force then
		local refCount = service.loadingRefs[refKey] or 0
		if refCount > 0 then
			service.loadingRefs[refKey] = refCount - 1
			service.loadingRefTotal = math.max((service.loadingRefTotal or 1) - 1, 0)
			if service.loadingRefs[refKey] <= 0 then
				service.loadingRefs[refKey] = nil
			end
			syncServiceState(service)
		end
		if (service.loadingRefTotal or 0) > 0 then
			return true
		end
	end

	service.loadingRefs = {}
	service.loadingRefTotal = 0
	syncServiceState(service)
	return service:Close("__Loading", true, reason)
end

return FairyGuiServiceLoading
